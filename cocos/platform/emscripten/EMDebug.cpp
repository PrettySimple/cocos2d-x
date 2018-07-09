//
// EMDebug.cpp
//

/*
For now, always enabled in debug mode, despite the realtime overhead (which reduced a lot now that we switched to std::unordered_map for tracking...)
*/

#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)

#    include "EMDebug.h"

/*

Rudimentary memory sanitizer for Emscripten.

Attempts to detect memory heap overflow/underflow and use-after-free (well, doesn't detect, but corrupts the memory bad enough to hopefully make the program
crash or misbehave ASAP).

References:

    http://www.flipcode.com/archives/How_To_Find_Memory_Leaks.shtml
    http://en.cppreference.com/w/cpp/memory/new/operator_new
    http://en.cppreference.com/w/cpp/memory/new/operator_delete


TODO:

    [-] Support for __FILE__ and __LINE__
    [-] Introduce back the new/new[] vs delete/delete[] checks
    [+] Build our own allocator_traits for internal structures
    [-] Check if there is a way to intercept memory access/reads ?
    [+] Dedicated map for freed pointers, will speed up some of the iterations.
    [+] Find out if printf() can me implemented using a custom allocator, in order to get rid of workarounds
    [-] Handle stack pointers in check() and friends
    [+] Persist settings, so that they start as soon as the program launches
    [-] Get rid of retain %, but instead always retain (when requested), and forcibly free when malloc() fails for internal structures or safe mode (requires
non-throwing malloc)
    [-] Use JS setInterval() instead of Node for scheduling "frame" checks.

    [-] Assert we're not in a throwing malloc!
    [-] Rename __in_safe_mode => __do_not_track

*/

#    include <cassert>
#    include <cstdint>
//#include <new>
//#include <stdexcept>
#    include <array>
#    include <emscripten.h>
#    include <emscripten/bind.h>
#    include <map>
#    include <random>
#    include <string.h>
#    include <unordered_map>

#    define printf printf_is_not_safe

extern "C"
{
    extern void* emscripten_builtin_malloc(size_t size);
    extern void emscripten_builtin_free(void* ptr);
    // Emscripten doesn't alias realloc(), hence our realloc() must always allocate a new pointer & copy data
    // extern void	*emscripten_builtin_realloc(void *ptr, size_t size);
}

/*
    Untracked wrappers DEFINITION.

    Untracked wrappers, as their name suggests, don't track allocations. However, they do release retained tracked allocations under memory pressure (when
   emscripten_builtin_malloc() fails)

    Untracked allocations are used:

    * For internal tracking structures
    * For calls we make from our wrappers (eg. printf) and that trigger allocation requests
*/

static void* __untracked_malloc(size_t size);
static void __untracked_free(void* ptr);

// Makes sure the builtin allocator/deallocator is triggered in some internal functions (printf(), EM_ASM() macros...)
// Without this, we had to implement a lot of workarounds to prevent those functions from altering our iterators when invoked from loops
static uint64_t __in_safe_mode = 0;

static uint32_t __oom_counter = 0;

#    define __assert(expr)      \
        do                      \
        {                       \
            ++__in_safe_mode;   \
            {                   \
                assert((expr)); \
            }                   \
            --__in_safe_mode;   \
        } while (0)

// Allocator that bypasses our overloads, used to store the allocations tracking data
// Shamelessly stolen from https://github.com/vincetse/allocator/blob/master/include/lazy/memory/buffer_allocator.h

template <typename T>
class sanitizer_allocator
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;

    /*
        typedef std::ptrdiff_t difference_type;
        typedef T& reference;
        typedef const T& const_reference;

        template <typename U>
        struct rebind
        {
            typedef sanitizer_allocator<U> other;
        };
    */

    sanitizer_allocator() noexcept {}

    sanitizer_allocator(const sanitizer_allocator&) noexcept {}

    // "Rebind" constructor
    template <typename U>
    sanitizer_allocator(const sanitizer_allocator<U>& alloc) noexcept
    {
        (void)alloc;
    }

    ~sanitizer_allocator() noexcept {}

    pointer allocate(std::size_t n, const_pointer cp = 0) throw()
    {
        (void)cp;
        return reinterpret_cast<pointer>(__untracked_malloc(n * sizeof(T)));
    }

    void deallocate(pointer ptr, std::size_t n)
    {
        (void)n;
        __untracked_free(reinterpret_cast<void*>(ptr));
    }
};

// In multiple of sizeof(uintptr_t).
#    define SANITIZE_SIZE 4
#    define SANITIZE_PATTERN 0xaa
#    define ALLOCATED_PATTERN 0xee
#    define DELETE_PATTERN 0xff
// Beware, this will consume RETAIN_CAPACITY * sizeof(retain_t) bytes
#    define RETAIN_CAPACITY 5000000

struct alloc_t
{
    uintptr_t ptr;
    size_t size;

    bool underflow_reported;
    bool overflow_reported;

    alloc_t(uintptr_t in_ptr, size_t in_size)
    : ptr(in_ptr)
    , size(in_size)
    , underflow_reported(false)
    , overflow_reported(false)
    {
    }
};

// Separating the tracking of retained (aka free but not really) blocks for faster iterations
struct retain_t
{
    uintptr_t ptr;
    size_t size;

    bool write_after_free_reported;
    bool dbl_free_reported;

    retain_t(uintptr_t in_ptr = 0, size_t in_size = 0) // defaults required for the std::array<retain_t> constructor
    : ptr(in_ptr)
    , size(in_size)
    , write_after_free_reported(false)
    , dbl_free_reported(false)
    {
    }
};

// A bit of stats. We're mostly interested by the peak usage
static uint64_t __alloc_entries_peak = 0;
static uint64_t __alloc_bytes_total = 0;
static uint64_t __alloc_bytes_peak = 0;

static uint64_t __retain_bytes_total = 0;

// In retain mode, free() attempts to retain the memory instead of actually free()'ing it
static bool __alloc_retain = false;

using alloc_map_type =
    std::unordered_map<uintptr_t, alloc_t, std::hash<uintptr_t>, std::equal_to<uintptr_t>, sanitizer_allocator<std::pair<const uintptr_t, alloc_t>>>;

static alloc_map_type& __alloc_list()
{
    static alloc_map_type alloc_list;
    return alloc_list;
}

/*

Switching (temporarily?) to using a pre-allocated array for retain.
The issue with the map was:

    * free() would need to insert a new entry into __retain_list(), triggering __untracked_malloc()
    * __untracked_malloc() might attempt to release retained entries under memory pressure, triggering __retain_list().erase()
    * This would make __retain_list() reentrant, and I doubt std::unordered_map() would cope with it gracefully.

Meanwhile, we:
    * preallocate the retain list, thus effectively limiting it's capacity,
    * effectively limit the retain list capacity to a predefined value.
    * (as a consequence) make the lookup slower (although rare, only in assert_ptr() on invalid ptr)

*/

struct retain_list_type
{
    std::array<retain_t, RETAIN_CAPACITY> array;
    size_t count;
    size_t peak;
    uint64_t oos_counter; // out-of-space counter

    retain_list_type()
    : count(0)
    , peak(0)
    , oos_counter(0)
    {
    }

    struct entry_t
    {
        size_t index;
        retain_t& entry;

        retain_t* operator->() { return &entry; }
    };

    bool insert(const alloc_t& entry)
    {
        if (count == RETAIN_CAPACITY)
        {
            ++oos_counter;
            return false;
        }

        array[count] = {entry.ptr, entry.size};

        if (++count > peak)
            peak = count;

        return true;
    }

    retain_t* lookup(uintptr_t ptr)
    {
        for (size_t i = 0; i < count; ++i)
        {
            if (array[i].ptr == ptr)
                return &array[i];
        }

        return nullptr;
    }

    entry_t random()
    {
        __assert(count);

        auto index = static_cast<size_t>(count != 1 ? (rand() % count) : 0);

        return {index, array[index]};
    }

    void remove(const entry_t& random) { remove(random.index); }

    void remove(size_t index)
    {
        __assert(count);
        __assert(index < count);

        if (index != --count)
            array[index] = array[count];
    }
};

static retain_list_type& __retain_list()
{
    static retain_list_type retain_list;
    return retain_list;
}

/*
static uint64_t	__total_memory()
{
    static uint64_t	total_memory = 0;

    if(total_memory == 0)
    {
        ++__in_safe_mode;
        {
            total_memory = static_cast<uint64_t>(EM_ASM_INT({return TOTAL_MEMORY;}));
        }
        --__in_safe_mode;
    }

    return total_memory;
}
*/

/*
    Reporting / aborting
*/

// TODO: I definetly need to find a way to output messages without malloc() being invoked from the primitives...
// v[f]printf() does invoke malloc()! vsprintf() might do the job?

static void __info(const char* fmt, ...)
{
    ++__in_safe_mode;
    {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
    --__in_safe_mode;
}

static void __error(const char* fmt, ...)
{
    ++__in_safe_mode;
    {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }
    --__in_safe_mode;
}

/*
static void	__abort()
{
    // It has been verified that __abort() invokes malloc()...
    ++__in_safe_mode;
    abort();
    --__in_safe_mode;
}
*/

/*
    Watermarking and checking
    Each of these receive the size and ptr that were actually requested/returned in malloc().
*/

static void __watermark_allocated(uintptr_t ptr, size_t size)
{
    // Sanitize before
    memset(reinterpret_cast<void*>(ptr - (sizeof(uintptr_t) * SANITIZE_SIZE)), SANITIZE_PATTERN, sizeof(uintptr_t) * SANITIZE_SIZE);

    // Write ALLOCATED_PATTERN to the actually allocated memory, to hopefully trigger issues when using uninitialized memory
    memset(reinterpret_cast<void*>(ptr), ALLOCATED_PATTERN, size);

    // Sanitize after
    memset(reinterpret_cast<void*>(ptr + size), SANITIZE_PATTERN, sizeof(uintptr_t) * SANITIZE_SIZE);
}

static void __watermark_retained(uintptr_t ptr, size_t size)
{
    memset(reinterpret_cast<void*>(ptr - (sizeof(uintptr_t) * SANITIZE_SIZE)), SANITIZE_PATTERN, size + (2 * sizeof(uintptr_t) * SANITIZE_SIZE));
}

static void __watermark_freed(uintptr_t ptr, size_t size)
{
    memset(reinterpret_cast<void*>(ptr - (sizeof(uintptr_t) * SANITIZE_SIZE)), DELETE_PATTERN, size + (2 * sizeof(uintptr_t) * SANITIZE_SIZE));
}

static bool __check_underflow(const alloc_t& entry)
{
    // SANITIZE_SIZE x sizeof(uintptr_t) bytes before the pointer provided to the application are expected to be filled with SANITIZE_PATTERN

    for (uint8_t* tmp = reinterpret_cast<uint8_t*>(entry.ptr - (sizeof(uintptr_t) * SANITIZE_SIZE)); tmp < reinterpret_cast<uint8_t*>(entry.ptr); ++tmp)
    {
        if (*tmp != SANITIZE_PATTERN)
            return true;
    }

    return false;
}

static bool __check_overflow(const alloc_t& entry)
{
    // SANITIZE_SIZE x sizeof(uintptr_t) bytes after the allocated zone are expected to be filled with SANITIZE_PATTERN

    for (uint8_t* tmp = reinterpret_cast<uint8_t*>(entry.ptr) + entry.size;
         tmp < reinterpret_cast<uint8_t*>(entry.ptr + entry.size + (sizeof(uintptr_t) * SANITIZE_SIZE)); ++tmp)
    {
        if (*tmp != SANITIZE_PATTERN)
            return true;
    }

    return false;
}

static bool __check_retained(const retain_t& entry)
{
    // SANITIZE_SIZE x sizeof(uintptr_t) bytes before, after, and the allocated zone itself, are expected to be filled with SANITIZE_PATTERN

    for (uint8_t* tmp = reinterpret_cast<uint8_t*>(entry.ptr - (sizeof(uintptr_t) * SANITIZE_SIZE));
         tmp < reinterpret_cast<uint8_t*>(entry.ptr + entry.size + (sizeof(uintptr_t) * SANITIZE_SIZE)); ++tmp)
    {
        if (*tmp != SANITIZE_PATTERN)
            return true;
    }

    return false;
}

static inline void __unretain_all()
{
    for (size_t i = 0; i < __retain_list().count; ++i)
    {
        auto& entry = __retain_list().array[i];

        if (!entry.write_after_free_reported && __check_retained(entry))
            __error("*** [EMS] (%p): memory was written to after free()\n", reinterpret_cast<void*>(entry.ptr));

        __watermark_freed(entry.ptr, entry.size);
        __untracked_free(reinterpret_cast<void*>(entry.ptr - (sizeof(uintptr_t) * SANITIZE_SIZE)));

        __assert(__retain_bytes_total >= entry.size);
        __retain_bytes_total -= entry.size;
    }

    __retain_list().count = 0;
    __retain_list().peak = 0;
    __retain_list().oos_counter = 0;
}

static inline bool __unretain_some(size_t number)
{
    // Invoked under memory pressure, releases (and verifies) up to number retained allocation at a time
    // Returns true if at least one was unretained.

    __assert(number);

    if (!__retain_list().count)
        return false;

    while (number-- && __retain_list().count)
    {
        // Random approach
        auto random = __retain_list().random();

        if (!random->write_after_free_reported && __check_retained(random.entry))
            __error("*** [EMS] (%p): memory was written to after free()\n", reinterpret_cast<void*>(random->ptr));

        __watermark_freed(random->ptr, random->size);
        __untracked_free(reinterpret_cast<void*>(random->ptr - (sizeof(uintptr_t) * SANITIZE_SIZE)));

        __assert(__retain_bytes_total >= random->size);
        __retain_bytes_total -= random->size;

        // Remove from __retain_list()
        __retain_list().remove(random);
    }

    return true;
}

/*
    Untracked wrappers IMPLEMENTATION
*/

void* __untracked_malloc(size_t size)
{
    void* result;

    while ((result = emscripten_builtin_malloc(size)) == nullptr && __unretain_some(16))
        ;

    if (result)
        return result;

    ++__oom_counter;
    return nullptr;
}

void __untracked_free(void* ptr)
{
    emscripten_builtin_free(ptr);
}

/*
    The actual malloc/free/realloc wrappers that will be linked-in into the project
*/

void* __attribute__((noinline)) malloc(size_t size)
{
    static bool initialized = false;
    static bool reentered = false;

    if (__in_safe_mode)
        return __untracked_malloc(size);

    // safeguard - make sure we aren't reentrant
    __assert(!reentered);
    reentered = true;

    if (!initialized)
    {
        ++__in_safe_mode;

        // Read the persistent settings (only the retain setting for now...)
        {
            __alloc_retain = EM_ASM_INT({
                try
                {
                    return window.localStorage.getItem('__EMS_retain') == = '1';
                }
                catch (e)
                {
                    return 0;
                }
            });

            if (__alloc_retain)
                __info("*** [EMS] Using retain (persisted)\n");
        }

        // TODO: consider filling up the memory with dummy allocs (that get instantly freed), effectively retaining all/most of the allocations

        --__in_safe_mode;
        initialized = true;
    }

    // Compute the size we actually need to allocate (taking SANITIZE_SIZE into account)
    size_t actual_size = size + (2 * sizeof(uintptr_t) * SANITIZE_SIZE);

    // Then allocate from... untracked malloc (so that we don't duplicate the memory pressure release code)
    uintptr_t actual_ptr = reinterpret_cast<uintptr_t>(__untracked_malloc(actual_size));

    // Don't attempt logging here... it's likely worthless. Something will crash shortafter, and EM will provide a stack trace.
    // Moreover, __untracked_malloc() will have incremented __oom_counter which can be queried from JS

    if (actual_ptr == 0)
    {
        reentered = false;
        return nullptr;
    }

    auto result_ptr = actual_ptr + (sizeof(uintptr_t) * SANITIZE_SIZE);

    __watermark_allocated(result_ptr, size);

    // Finally, record to the allocation list!
    if (!__alloc_list().emplace(std::piecewise_construct, std::forward_as_tuple(result_ptr), std::forward_as_tuple(result_ptr, size)).second)
    {
        __untracked_free(reinterpret_cast<void*>(actual_ptr));
        __error("*** [EMS] failed tracking allocation entry!\n");
        return nullptr;
    }

    if (__alloc_list().size() > __alloc_entries_peak)
        __alloc_entries_peak = __alloc_list().size();

    __alloc_bytes_total += size;

    if (__alloc_bytes_total > __alloc_bytes_peak)
        __alloc_bytes_peak = __alloc_bytes_total;

    reentered = false;

    return reinterpret_cast<void*>(result_ptr);
}

void __attribute__((noinline)) free(void* void_ptr)
{
    if (void_ptr == nullptr)
        return;

    if (__in_safe_mode)
    {
        __untracked_free(void_ptr);
        return;
    }

    auto ptr = reinterpret_cast<uintptr_t>(void_ptr);

    // Lookup __alloc_list
    {
        auto entry = __alloc_list().find(ptr);

        if (entry != __alloc_list().end())
        {
            if (!entry->second.underflow_reported && __check_underflow(entry->second))
                __error("*** [EMS] free(%p): underflow detected\n", void_ptr);

            if (!entry->second.overflow_reported && __check_overflow(entry->second))
                __error("*** [EMS] free(%p): overflow detected\n", void_ptr);

            // Attempt (!) retaining. It might fail as the retain list is a fixed capacity one.

            if (__alloc_retain && __retain_list().insert(entry->second))
            {
                __watermark_retained(entry->second.ptr, entry->second.size);
                __retain_bytes_total += entry->second.size;
            }
            else
            {
                __watermark_freed(entry->second.ptr, entry->second.size);
                __untracked_free(reinterpret_cast<void*>(entry->second.ptr - (sizeof(uintptr_t) * SANITIZE_SIZE)));
            }

            // Untrack

            __assert(__alloc_bytes_total >= entry->second.size);
            __alloc_bytes_total -= entry->second.size;

            __alloc_list().erase(entry);

            return;
        }
    }

    // Not allocated, yet attempt to find it in __retain_list(), to promptly report a double-free
    {
        auto entry = __retain_list().lookup(ptr);

        if (entry)
        {
            if (!entry->dbl_free_reported)
            {
                entry->dbl_free_reported = true;
                __error("*** [EMS] free(%p): pointer already free'd\n", void_ptr);
            }
            return;
        }
    }

    __error("*** [EMS] free(%p): pointer not allocated here (or double-free'd)\n", void_ptr);
}

void* __attribute__((noinline)) realloc(void* void_ptr, size_t size)
{
    // Due to emscripten not aliasing emscripten_builtin_realloc(), we have to always allocate a new pointer, and copy!

    if (void_ptr != nullptr)
    {
        auto ptr = reinterpret_cast<uintptr_t>(void_ptr);
        const auto& entry = __alloc_list().find(ptr);

        if (entry != __alloc_list().end())
        {
            // We don't attempt to intercept the case where the new allocation is smaller - we still want to sanitize at the exact byte boundary!

            auto new_ptr = malloc(size);

            // We're not meant to free(void_ptr) if the new allocation fails
            if (new_ptr == nullptr)
                return nullptr;

            // dumbly copy the data

            size_t copy = entry->second.size < size ? entry->second.size : size;

            if (copy)
                memcpy(new_ptr, void_ptr, copy);

            // and dumbly free the previous pointer
            free(void_ptr);

            return new_ptr;
        }
        else
        {
            __error("*** [EMS] realloc(%p, %zu): pointer not allocated here\n", void_ptr, size);
            return nullptr;
        }
    }
    else
        return malloc(size);
}

void* __attribute__((noinline)) calloc(size_t n_elements, size_t elem_size)
{
    // Loosy implementation that doesn't check whether n_elements * elem_size overflows...

    auto size = n_elements * elem_size;
    auto ptr_void = malloc(size);

    if (ptr_void != nullptr)
        memset(ptr_void, 0, size);

    return ptr_void;
}

/*********************************************************************************************************************/
/* EmscriptenMemorySanitizer */

///////////////////////////////////////////////////////////////////////
// CPP API

void EmscriptenMemorySanitizer::assert_ptr(void* void_ptr, const char* file, int line) noexcept
{
    const auto ptr = reinterpret_cast<uintptr_t>(void_ptr);
    const auto& entry = __alloc_list().find(ptr);

    if (entry == __alloc_list().end())
    {
        auto retained_entry = __retain_list().lookup(ptr);

        if (retained_entry)
            __error("*** [EMS] assert_ptr(%p) is a pointer to free'd memory [%s:%d]\n", void_ptr, file, line);
        else
            __error("*** [EMS] assert_ptr(%p) is not a pointer to allocated memory [%s:%d]\n", void_ptr, file, line);
    }
    else
    {
        // Test corruptions (regardless of whether they were already reported)
        if (__check_underflow(entry->second))
            __error("*** [EMS] assert_ptr(%p): underflow detected [%s:%d]\n", void_ptr, file, line);

        if (__check_overflow(entry->second))
            __error("*** [EMS] assert_ptr(%p): overflow detected [%s:%d]\n", void_ptr, file, line);
    }
}

///////////////////////////////////////////////////////////////////////
// JS API

void EmscriptenMemorySanitizer::js2cpp_stats()
{
    size_t bytes_allocated = 0;

    for (const auto& it : __alloc_list())
        bytes_allocated += it.second.size;

    assert(bytes_allocated == __alloc_bytes_total);

    __info("*** [EMS] Allocated entries/peak: %zu / %llu\n", __alloc_list().size(), __alloc_entries_peak);
    __info("*** [EMS] Allocated memory total/peak: %llu / %llu\n", __alloc_bytes_total, __alloc_bytes_peak);

    if (__retain_list().count)
    {
        size_t bytes_retained = 0;

        for (size_t i = 0; i < __retain_list().count; ++i)
            bytes_retained += __retain_list().array[i].size;

        assert(bytes_retained == __retain_bytes_total);

        __info("*** [EMS] Retained entries/peak: %zu / %zu\n", __retain_list().count, __retain_list().peak);
        __info("*** [EMS] Retained memory: %llu\n", __retain_bytes_total);
    }

    // TODO: oom & ooc events

    if (__oom_counter)
        __info("*** [EMS] %zu out-of-memory events\n", __oom_counter);

    if (__retain_list().oos_counter)
        __info("*** [EMS] %zu retain out-of-space events\n", __retain_list().oos_counter);
}

void EmscriptenMemorySanitizer::js2cpp_ptr_info(uintptr_t ptr)
{
    // Lookup __alloc_list()
    {
        const auto& info = __alloc_list().find(ptr);

        if (info != __alloc_list().end())
        {
            __info("*** [EMS] %p allocated (%zu bytes)\n", reinterpret_cast<void*>(ptr), info->second.size);
            return;
        }
    }

    // Lookup __retain_list()
    {
        auto info = __retain_list().lookup(ptr);

        if (info)
        {
            __info("*** [EMS] %p retained (%zu bytes)\n", reinterpret_cast<void*>(ptr), info->size);
            return;
        }
    }

    __info("*** [EMS] %p not allocated nor retained\n", reinterpret_cast<void*>(ptr));
}

void EmscriptenMemorySanitizer::js2cpp_size_stats()
{
    std::map<size_t, size_t, std::less<size_t>, sanitizer_allocator<std::pair<const size_t, size_t>>> size_stats;

    __info("*** [EMS] Allocated memory by size [ size : number ]\n");

    for (const auto& it : __alloc_list())
    {
        if (size_stats.count(it.second.size))
            ++size_stats[it.second.size];
        else
            size_stats[it.second.size] = 1;
    }

    for (const auto& it : size_stats)
        __info("*** [EMS] %lu: %lu\n", it.first, it.second);
}

bool EmscriptenMemorySanitizer::js2cpp_check()
{
    bool ok = true;

    // Don't use __error() here, as we don't need the stack trace

    // Check __alloc_list
    for (auto& entry : __alloc_list())
    {
        if (!entry.second.underflow_reported && __check_underflow(entry.second))
        {
            entry.second.underflow_reported = true;
            __info("*** [EMS] check(%p): underflow detected\n", reinterpret_cast<void*>(entry.second.ptr));
            ok = false;
        }

        if (!entry.second.overflow_reported && __check_overflow(entry.second))
        {
            entry.second.overflow_reported = true;
            __info("*** [EMS] check(%p): overflow detected\n", reinterpret_cast<void*>(entry.second.ptr));
            ok = false;
        }
    }

    // Check __retain_list

    for (size_t i = 0; i < __retain_list().count; ++i)
    {
        auto& entry = __retain_list().array[i];

        if (!entry.write_after_free_reported && __check_retained(entry))
        {
            entry.write_after_free_reported = true;
            __info("*** [EMS] check(%p): memory was written to after free()\n", reinterpret_cast<void*>(entry.ptr));
            ok = false;
        }
    }

    return ok;
}

bool EmscriptenMemorySanitizer::js2cpp_retain(bool retain)
{
    if (__alloc_retain && !retain)
        __unretain_all();

    __alloc_retain = retain;

    // Persist the setting. Failures are silently ignored

    ++__in_safe_mode;
    {
        EM_ASM(
            {
                try
                {
                    window.localStorage.setItem('__EMS_retain', $0);
                }
                catch (e)
                {
                }
            },
            __alloc_retain ? 1 : 0);
    }
    --__in_safe_mode;

    return retain;
}

/*
    JS Bindings

    Usage:

        Module.EMS.stats()

        Module.EMS.ptr_info(ptr)

        Module.EMS.check()

        Module.EMS.retain(bool)

*/

#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wglobal-constructors"

EMSCRIPTEN_BINDINGS(EmscriptenMemorySanitizer_binding)
{
    emscripten::class_<EmscriptenMemorySanitizer>("EMS")
        .class_function("stats", &EmscriptenMemorySanitizer::js2cpp_stats)
        .class_function("size_stats", &EmscriptenMemorySanitizer::js2cpp_size_stats)
        .class_function("ptr_info", &EmscriptenMemorySanitizer::js2cpp_ptr_info)
        .class_function("check", &EmscriptenMemorySanitizer::js2cpp_check)
        .class_function("retain", &EmscriptenMemorySanitizer::js2cpp_retain);
}

#    pragma clang diagnostic pop

#endif // #if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)

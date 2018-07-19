/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef CC_BASE_SCHEDULER_H
#define CC_BASE_SCHEDULER_H

#include "base/CCRef.h"
#include "platform/CCPlatformMacros.h"
#include "platform/CCPlatformDefine.h"

#include <cstdint>
#include <mutex>
#include <chrono>
#include <cstddef>
#include <functional>
#include <iosfwd>
#include <limits>
#include <list>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

NS_CC_BEGIN

class Scheduler;

using ccSchedulerFunc = std::function<void(float)>;

class CC_DLL Timer : public Ref
{
public:
    enum struct Type : std::uint8_t
    {
        UNKNOWN = 0,
        DEFAULT,
        SELECTOR,
        CALLBACK
    };

private:
    Type _type = Type::DEFAULT;

protected:
    Scheduler* _scheduler = nullptr; // weak ref
    std::chrono::milliseconds _elapsed = std::chrono::milliseconds::max();
    bool _runForever = false;
    bool _useDelay = false;
    std::uint32_t _timesExecuted = 0;
    std::uint32_t _repeat = 0; // 0 = once, 1 is 2 x executed
    std::chrono::milliseconds _delay = std::chrono::milliseconds::zero();
    std::chrono::milliseconds _interval = std::chrono::milliseconds::zero();

protected:
    Timer() = default;
    inline explicit Timer(Type type)
    : _type(type)
    {
    }
    Timer(Timer const&) = delete;
    Timer& operator=(Timer const&) = delete;
    Timer(Timer&&) noexcept = delete;
    Timer& operator=(Timer&&) noexcept = delete;
    ~Timer() override;

public:
    /** get interval in seconds */
    inline std::chrono::milliseconds getInterval() const noexcept { return _interval; }
    /** set interval in seconds */
    inline void setInterval(std::chrono::milliseconds interval) noexcept { _interval = interval; }

    inline Type getType() const noexcept { return _type; }

    void setupTimerWithInterval(std::chrono::milliseconds seconds, std::uint32_t repeat, std::chrono::milliseconds delay);

    virtual void trigger(float dt) = 0;
    virtual void cancel() = 0;

    /** triggers the timer */
    void update(float dt);
};

class CC_DLL TimerTargetSelector final : public Timer
{
    Ref* _target = nullptr;
    SEL_SCHEDULE _selector = nullptr;

public:
    TimerTargetSelector();
    TimerTargetSelector(TimerTargetSelector const&) = delete;
    TimerTargetSelector& operator=(TimerTargetSelector const&) = delete;
    TimerTargetSelector(TimerTargetSelector&&) noexcept = delete;
    TimerTargetSelector& operator=(TimerTargetSelector&&) noexcept = delete;
    ~TimerTargetSelector() final = default;

    /** Initializes a timer with a target, a selector and an interval in seconds, repeat in number of times to repeat, delay in seconds. */
    bool initWithSelector(Scheduler* scheduler, SEL_SCHEDULE selector, Ref* target, std::chrono::milliseconds seconds, std::uint32_t repeat,
                          std::chrono::milliseconds delay);

    inline SEL_SCHEDULE getSelector() const noexcept { return _selector; }

    void trigger(float dt) final;
    void cancel() final;
};

class CC_DLL TimerTargetCallback final : public Timer
{
    void* _target = nullptr;
    ccSchedulerFunc _callback = nullptr;
    std::string _key;

public:
    TimerTargetCallback();
    TimerTargetCallback(TimerTargetCallback const&) = delete;
    TimerTargetCallback& operator=(TimerTargetCallback const&) = delete;
    TimerTargetCallback(TimerTargetCallback&&) noexcept = delete;
    TimerTargetCallback& operator=(TimerTargetCallback&&) noexcept = delete;
    ~TimerTargetCallback() final = default;

    // Initializes a timer with a target, a lambda and an interval in seconds, repeat in number of times to repeat, delay in seconds.
    bool initWithCallback(Scheduler* scheduler, ccSchedulerFunc const& callback, void* target, std::string const& key, std::chrono::milliseconds seconds,
                          std::uint32_t repeat, std::chrono::milliseconds delay);

    inline ccSchedulerFunc getCallback() const noexcept { return _callback; }
    inline std::string getKey() const noexcept { return _key; }

    void trigger(float dt) final;
    void cancel() final;
};

class Scheduler;

class UpdateData
{
public:
    struct element final
    {
        ccSchedulerFunc callback = nullptr;
        void* target = nullptr;
        std::int32_t priority = 0;
        bool paused = false;
        std::uint64_t index = 0;

        element() = default;
        explicit element(ccSchedulerFunc const& c, void* t, std::int32_t pr, bool pa, std::uint64_t i);
        element(element const&) = delete;
        element& operator=(element const&) = delete;
        element(element&&) noexcept = delete;
        element& operator=(element&&) noexcept = delete;
        ~element() = default;
    };

private:
    struct element_less final
    {
        constexpr bool operator()(element const& a, element const& b) const
        {
            if (a.priority < b.priority)
            {
                return true;
            }
            return a.priority == b.priority && a.index < b.index;
        }
    };

    std::set<element, element_less> _data;
    std::unordered_map<void*, decltype(_data)::key_type const&> _targets;
    std::uint64_t _index = 0;

public:
    void pause_target(void* target);
    void resume_target(void* target);
    bool is_pause_target(void* target) const noexcept;
    void pause_all_targets();
    std::unordered_set<void*> pause_all_updates_with_min_priority(std::int32_t min_priority);

    void add_update(ccSchedulerFunc const& c, void* t, std::int32_t pr, bool pa);

    void remove_update(void* t);
    void remove_all_updates_with_min_priority(std::int32_t min_priority);

    inline std::size_t count(void* t) const noexcept { return _targets.count(t); }
    inline decltype(_data)::iterator begin() { return _data.begin(); }
    inline decltype(_data)::iterator end() { return _data.end(); }
    inline decltype(_data)::const_iterator begin() const noexcept { return _data.begin(); }
    inline decltype(_data)::const_iterator end() const noexcept { return _data.end(); }
    inline bool empty() const noexcept { return _data.empty(); }
    inline std::size_t size() const noexcept { return _data.size(); }

    decltype(_data)::key_type const& get_element_from_target(void* t) const noexcept;
    void remove_element(decltype(_data)::key_type const& key);
};

class TimerData
{
public:
    struct Key final
    {
        std::string name;
        void* target = nullptr;

        Key() = default;
        explicit Key(std::string const& n, void* t)
        : name(n)
        , target(t)
        {
        }
        Key(Key const&) = delete;
        Key& operator=(Key const&) = delete;
        Key(Key&&) noexcept = default;
        Key& operator=(Key&&) noexcept = delete;
        ~Key() = default;
    };

private:
    struct element final
    {
        Timer* timer = nullptr;
        void* target = nullptr;
        std::uint64_t index = 0;
        bool paused = false;
        std::string name;

        element() = default;
        explicit element(Timer* ti, void* ta, std::uint64_t i, bool p, std::string const& n);
        element(element const&) = delete;
        element& operator=(element const&) = delete;
        element(element&&) noexcept = delete;
        element& operator=(element&&) noexcept = delete;
        ~element();
    };

    struct element_less final
    {
        constexpr bool operator()(element const& a, element const& b) const { return a.index < b.index; }
    };

    struct key_hasher final
    {
        std::size_t operator()(Key const& k) const noexcept;
    };

    struct key_equal_to final
    {
        bool operator()(Key const& lhs, Key const& rhs) const noexcept { return lhs.name == rhs.name && lhs.target == rhs.target; }
    };

    std::set<element, element_less> _data;
    std::unordered_multimap<void*, decltype(_data)::key_type const&> _targets;
    std::unordered_map<Key, decltype(_data)::key_type const&, key_hasher, key_equal_to> _timers;
    std::size_t _index = 0;

public:
    void pause_target(void* target);
    void resume_target(void* target);
    bool is_pause_target(void* target) const noexcept;
    std::unordered_set<void*> pause_all_targets();

    void add_timer(Key const& k, Timer* timer, bool paused);

    void remove_timer(Key const& k);
    void remove_timer(SEL_SCHEDULE selector, Ref* target);
    void remove_all_timers();
    void remove_all_timers_from_target(void* target);

    inline std::size_t count(Key const& k) const noexcept { return _timers.count(k); }
    inline std::size_t count(void* target) const noexcept { return _targets.count(target); }
    inline decltype(_data)::iterator begin() { return _data.begin(); }
    inline decltype(_data)::iterator end() { return _data.end(); }
    inline decltype(_data)::const_iterator begin() const noexcept { return _data.begin(); }
    inline decltype(_data)::const_iterator end() const noexcept { return _data.end(); }
    inline bool empty() const noexcept { return _data.empty(); }
    inline std::size_t size() const noexcept { return _data.size(); }

    decltype(_data)::key_type const& get_element_from_target(Key const& k) const noexcept;
    void remove_element(decltype(_data)::key_type const& key);
};

/** @brief Scheduler is responsible for triggering the scheduled callbacks.
 You should not use system timer for your game logic. Instead, use this class.

 There are 2 different types of callbacks (selectors):

 - update selector: the 'update' selector will be called every frame. You can customize the priority.
 - custom selector: A custom selector will be called every frame, or with a custom interval of time

 The 'custom selectors' should be avoided when possible. It is faster, and consumes less memory to use the 'update selector'.

 */
class CC_DLL Scheduler final : public Ref
{
    float _timeScale = 1.f;

    std::list<void*> _updates_to_process;
    std::int32_t _updates_to_process_priority = std::numeric_limits<std::int32_t>::min();
    UpdateData _updates;
    std::list<TimerData::Key> _timers_to_process;
    TimerData _timers;

    std::vector<std::function<void()>> _functionsToPerform;
    std::mutex _performMutex;

public:
    /** Priority level reserved for system services.
     */
    static constexpr std::int32_t const PRIORITY_SYSTEM = std::numeric_limits<std::int32_t>::min();

    /** Minimum priority level for user scheduling.
     * Priority level of user scheduling should bigger then this value.
     */
    static constexpr std::int32_t const PRIORITY_NON_SYSTEM_MIN = PRIORITY_SYSTEM + 1;

    Scheduler() = default;
    Scheduler(Scheduler const&) = delete;
    Scheduler& operator=(Scheduler const&) = delete;
    Scheduler(Scheduler&&) noexcept = delete;
    Scheduler& operator=(Scheduler&&) noexcept = delete;
    ~Scheduler() final;

    /**
     * Gets the time scale of schedule callbacks.
     * @see Scheduler::setTimeScale()
     */
    inline float getTimeScale() const noexcept { return _timeScale; }
    /** Modifies the time of all scheduled callbacks.
     You can use this property to create a 'slow motion' or 'fast forward' effect.
     Default is 1.0. To create a 'slow motion' effect, use values below 1.0.
     To create a 'fast forward' effect, use values higher than 1.0.
     @since v0.8
     @warning It will affect EVERY scheduled selector / action.
     */
    inline void setTimeScale(float timeScale) noexcept { _timeScale = timeScale; }

    /** 'update' the scheduler.
     * You should NEVER call this method, unless you know what you are doing.
     * @lua NA
     */
    void update(float dt);

    /////////////////////////////////////

    // schedule

    /** The scheduled method will be called every 'interval' seconds.
     If paused is true, then it won't be called until it is resumed.
     If 'interval' is 0, it will be called every frame, but if so, it's recommended to use 'scheduleUpdate' instead.
     If the 'callback' is already scheduled, then only the interval parameter will be updated without re-scheduling it again.
     repeat let the action be repeated repeat + 1 times, use CC_REPEAT_FOREVER to let the action run continuously
     delay is the amount of time the action will wait before it'll start.
     @param callback The callback function.
     @param target The target of the callback function.
     @param interval The interval to schedule the callback. If the value is 0, then the callback will be scheduled every frame.
     @param repeat repeat+1 times to schedule the callback.
     @param delay Schedule call back after `delay` seconds. If the value is not 0, the first schedule will happen after `delay` seconds.
     But it will only affect first schedule. After first schedule, the delay time is determined by `interval`.
     @param paused Whether or not to pause the schedule.
     @param key The key to identify the callback function, because there is not way to identify a std::function<>.
     @since v3.0
     */
    void schedule(ccSchedulerFunc const& callback, void* target, std::chrono::milliseconds interval, std::uint32_t repeat, std::chrono::milliseconds delay,
                  bool paused, std::string const& key);

    /** The scheduled method will be called every 'interval' seconds for ever.
     @param callback The callback function.
     @param target The target of the callback function.
     @param interval The interval to schedule the callback. If the value is 0, then the callback will be scheduled every frame.
     @param paused Whether or not to pause the schedule.
     @param key The key to identify the callback function, because there is not way to identify a std::function<>.
     @since v3.0
     */
    void schedule(ccSchedulerFunc const& callback, void* target, std::chrono::milliseconds interval, bool paused, std::string const& key);

    /** The scheduled method will be called every `interval` seconds.
     If paused is true, then it won't be called until it is resumed.
     If 'interval' is 0, it will be called every frame, but if so, it's recommended to use 'scheduleUpdate' instead.
     If the selector is already scheduled, then only the interval parameter will be updated without re-scheduling it again.
     repeat let the action be repeated repeat + 1 times, use CC_REPEAT_FOREVER to let the action run continuously
     delay is the amount of time the action will wait before it'll start

     @param selector The callback function.
     @param target The target of the callback function.
     @param interval The interval to schedule the callback. If the value is 0, then the callback will be scheduled every frame.
     @param repeat repeat+1 times to schedule the callback.
     @param delay Schedule call back after `delay` seconds. If the value is not 0, the first schedule will happen after `delay` seconds.
     But it will only affect first schedule. After first schedule, the delay time is determined by `interval`.
     @param paused Whether or not to pause the schedule.
     @since v3.0
     */
    void schedule(SEL_SCHEDULE selector, Ref* target, std::chrono::milliseconds interval, std::uint32_t repeat, std::chrono::milliseconds delay, bool paused);

    /** The scheduled method will be called every `interval` seconds for ever.
     @param selector The callback function.
     @param target The target of the callback function.
     @param interval The interval to schedule the callback. If the value is 0, then the callback will be scheduled every frame.
     @param paused Whether or not to pause the schedule.
     */
    void schedule(SEL_SCHEDULE selector, Ref* target, std::chrono::milliseconds interval, bool paused);

    /** Schedules the 'update' selector for a given target with a given priority.
     The 'update' selector will be called every frame.
     The lower the priority, the earlier it is called.
     @since v3.0
     @lua NA
     */
    template <typename T>
    void scheduleUpdate(T* target, std::int32_t priority, bool paused)
    {
        schedulePerFrame([target](float dt) { target->update(dt); }, target, priority, paused);
    }

    /////////////////////////////////////

    // unschedule

    /** Unschedules a callback for a key and a given target.
     If you want to unschedule the 'callbackPerFrame', use unscheduleUpdate.
     @param key The key to identify the callback function, because there is not way to identify a std::function<>.
     @param target The target to be unscheduled.
     @since v3.0
     */
    void unschedule(std::string const& key, void* target);

    /** Unschedules a selector for a given target.
     If you want to unschedule the "update", use `unscheudleUpdate()`.
     @param selector The selector that is unscheduled.
     @param target The target of the unscheduled selector.
     @since v3.0
     */
    void unschedule(SEL_SCHEDULE selector, Ref* target);

    /** Unschedules the update selector for a given target
     @param target The target to be unscheduled.
     @since v0.99.3
     */
    void unscheduleUpdate(void* target);

    /** Unschedules all selectors for a given target.
     This also includes the "update" selector.
     @param target The target to be unscheduled.
     @since v0.99.3
     @lua NA
     */
    void unscheduleAllForTarget(void* target);

    /** Unschedules all selectors from all targets.
     You should NEVER call this method, unless you know what you are doing.
     @since v0.99.3
     */
    void unscheduleAll();

    /** Unschedules all selectors from all targets with a minimum priority.
     You should only call this with `PRIORITY_NON_SYSTEM_MIN` or higher.
     @param minPriority The minimum priority of selector to be unscheduled. Which means, all selectors which
     priority is higher than minPriority will be unscheduled.
     @since v2.0.0
     */
    void unscheduleAllWithMinPriority(std::int32_t minPriority);

    /////////////////////////////////////

    // isScheduled

    /** Checks whether a callback associated with 'key' and 'target' is scheduled.
     @param key The key to identify the callback function, because there is not way to identify a std::function<>.
     @param target The target of the callback.
     @return True if the specified callback is invoked, false if not.
     @since v3.0.0
     */
    bool isScheduled(std::string const& key, void* target) const noexcept;

    /** Checks whether a selector for a given target is scheduled.
     @param selector The selector to be checked.
     @param target The target of the callback.
     @return True if the specified selector is invoked, false if not.
     @since v3.0
     */
    bool isScheduled(SEL_SCHEDULE selector, Ref* target) const noexcept;

    /////////////////////////////////////

    /** Pauses the target.
     All scheduled selectors/update for a given target won't be 'ticked' until the target is resumed.
     If the target is not present, nothing happens.
     @param target The target to be paused.
     @since v0.99.3
     */
    void pauseTarget(void* target);

    /** Resumes the target.
     The 'target' will be unpaused, so all schedule selectors/update will be 'ticked' again.
     If the target is not present, nothing happens.
     @param target The target to be resumed.
     @since v0.99.3
     */
    void resumeTarget(void* target);

    /** Returns whether or not the target is paused.
     * @param target The target to be checked.
     * @return True if the target is paused, false if not.
     * @since v1.0.0
     * @lua NA
     */
    bool isTargetPaused(void* target);

    /** Pause all selectors from all targets.
     You should NEVER call this method, unless you know what you are doing.
     @since v2.0.0
     */
    std::unordered_set<void*> pauseAllTargets();

    /** Pause all selectors from all targets with a minimum priority.
     You should only call this with PRIORITY_NON_SYSTEM_MIN or higher.
     @param minPriority The minimum priority of selector to be paused. Which means, all selectors which
     priority is higher than minPriority will be paused.
     @since v2.0.0
     */
    std::unordered_set<void*> pauseAllTargetsWithMinPriority(std::int32_t minPriority);

    /** Resume selectors on a set of targets.
     This can be useful for undoing a call to pauseAllSelectors.
     @param targetsToResume The set of targets to be resumed.
     @since v2.0.0
     */
    void resumeTargets(std::unordered_set<void*> const& targetsToResume);

    /** Calls a function on the cocos2d thread. Useful when you need to call a cocos2d function from another thread.
     This function is thread safe.
     @param function The function to be run in cocos2d thread.
     @since v3.0
     @js NA
     */
    void performFunctionInCocosThread(std::function<void()> const& function);

protected:
    /** Schedules the 'callback' function for a given target with a given priority.
     The 'callback' selector will be called every frame.
     The lower the priority, the earlier it is called.
     @note This method is only for internal use.
     @since v3.0
     @js _schedulePerFrame
     */
    void schedulePerFrame(ccSchedulerFunc const& callback, void* target, std::int32_t priority, bool paused);
};

NS_CC_END

#endif // CC_BASE_SCHEDULER_H

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

#include "base/CCScheduler.h"

#include "base/CCDirector.h"
#include "base/CCScriptSupport.h"
#include "base/ccCArray.h"
#include "base/ccMacros.h"
#include "base/iterator_pair.h"
#include "base/utlist.h"

#include <algorithm>
#include <memory>

NS_CC_BEGIN

// Timer

void Timer::setupTimerWithInterval(std::chrono::milliseconds seconds, unsigned int repeat, std::chrono::milliseconds delay)
{
    _elapsed = std::chrono::milliseconds::max();
    _interval = seconds;
    _delay = delay;
    _useDelay = (_delay > std::chrono::milliseconds::zero()) ? true : false;
    _repeat = repeat;
    _runForever = (_repeat == CC_REPEAT_FOREVER) ? true : false;
}

void Timer::update(float dt)
{
    if (_elapsed == std::chrono::milliseconds::max())
    {
        _elapsed = std::chrono::milliseconds::zero();
        _timesExecuted = 0;
        return;
    }

    // accumulate elapsed time
    _elapsed += std::chrono::milliseconds(static_cast<std::size_t>(1000.f * dt));

    // deal with delay
    if (_useDelay)
    {
        if (_elapsed < _delay)
        {
            return;
        }
        trigger(static_cast<float>(_delay.count()) / 1000.f);
        _elapsed = _elapsed - _delay;
        _timesExecuted += 1;
        _useDelay = false;
        // after delay, the rest time should compare with interval
        if (!_runForever && _timesExecuted > _repeat)
        { // unschedule timer
            cancel();
            return;
        }
    }

    // if _interval == 0, should trigger once every frame
    auto interval = (_interval > std::chrono::milliseconds::zero()) ? _interval : _elapsed;
    while (_elapsed >= interval)
    {
        trigger(static_cast<float>(interval.count()) / 1000.f);
        _elapsed -= interval;
        _timesExecuted += 1;

        if (!_runForever && _timesExecuted > _repeat)
        {
            cancel();
            break;
        }

        if (_elapsed <= std::chrono::milliseconds::zero())
        {
            break;
        }
    }
}

// TimerTargetSelector

TimerTargetSelector::TimerTargetSelector()
: Timer(Timer::Type::SELECTOR)
{
}

bool TimerTargetSelector::initWithSelector(Scheduler* scheduler, SEL_SCHEDULE selector, Ref* target, std::chrono::milliseconds seconds, unsigned int repeat,
                                           std::chrono::milliseconds delay)
{
    _scheduler = scheduler;
    _target = target;
    _selector = selector;
    setupTimerWithInterval(seconds, repeat, delay);
    return true;
}

void TimerTargetSelector::trigger(float dt)
{
    if (_target != nullptr && _selector != nullptr)
    {
        (_target->*_selector)(dt);
    }
}

void TimerTargetSelector::cancel()
{
    _scheduler->unschedule(_selector, _target);
}

// TimerTargetCallback

TimerTargetCallback::TimerTargetCallback()
: Timer(Timer::Type::CALLBACK)
{
}

bool TimerTargetCallback::initWithCallback(Scheduler* scheduler, ccSchedulerFunc const& callback, void* target, std::string const& key,
                                           std::chrono::milliseconds seconds, unsigned int repeat, std::chrono::milliseconds delay)
{
    _scheduler = scheduler;
    _target = target;
    _callback = callback;
    _key = key;
    setupTimerWithInterval(seconds, repeat, delay);
    return true;
}

void TimerTargetCallback::trigger(float dt)
{
    if (_callback != nullptr)
    {
        _callback(dt);
    }
}

void TimerTargetCallback::cancel()
{
    _scheduler->unschedule(_key, _target);
}

// UpdateData

UpdateData::element::element(ccSchedulerFunc const& c, void* t, int pr, bool pa, std::uint64_t i)
: callback(c)
, target(t)
, priority(pr)
, paused(pa)
, index(i)
{
}

void UpdateData::pause_target(void* target)
{
    if (auto search_target = _targets.find(target); search_target != _targets.end())
    {
        const_cast<UpdateData::element&>(search_target->second).paused = true;
    }
}

void UpdateData::resume_target(void* target)
{
    if (auto search_target = _targets.find(target); search_target != _targets.end())
    {
        const_cast<UpdateData::element&>(search_target->second).paused = false;
    }
}

bool UpdateData::is_pause_target(void* target) const noexcept
{
    if (auto search_target = _targets.find(target); search_target != _targets.end())
    {
        return search_target->second.paused;
    }
    return false;
}

std::unordered_set<void*> UpdateData::pause_all_updates_with_min_priority(int min_priority)
{
    std::unordered_set<void*> ret;
    ret.reserve(_data.size());
    for (auto const& ele : _data)
    {
        if (ele.priority >= min_priority)
        {
            const_cast<UpdateData::element&>(ele).paused = true;
            ret.emplace(ele.target);
        }
    }
    return ret;
}

void UpdateData::pause_all_targets()
{
    for (auto& ele : _targets)
    {
        const_cast<UpdateData::element&>(ele.second).paused = true;
    }
}

void UpdateData::add_update(ccSchedulerFunc const& c, void* t, int pr, bool pa)
{
    CCASSERT(_targets.count(t) == 0, "update has already been added!");
    CCASSERT(t != nullptr, "Argument target must be non-nullptr");

    if (_data.empty())
    {
        _index = 0;
    }

    auto const it = _data.emplace(c, t, pr, pa, _index++);
    CC_ASSERT(it.second);
    _targets.emplace(t, *it.first);
}

void UpdateData::remove_update(void* t)
{
    if (auto search_update = _targets.find(t); search_update != _targets.end())
    {
        remove_element(search_update->second);
    }
}

void UpdateData::remove_all_updates_with_min_priority(int min_priority)
{
    std::vector<std::reference_wrapper<UpdateData::element const>> tmp;
    tmp.reserve(_data.size());
    for (auto const& ele : _data)
    {
        if (ele.priority >= min_priority)
        {
            tmp.emplace_back(std::cref(ele));
        }
    }
    for (auto const& ele : tmp)
    {
        remove_element(ele);
    }
}

decltype(UpdateData::_data)::key_type const& UpdateData::get_element_from_target(void* t) const noexcept
{
    static decltype(UpdateData::_data)::key_type const empty;
    auto search = _targets.find(t);
    if (search != _targets.end())
    {
        return search->second;
    }
    return empty;
}

void UpdateData::remove_element(decltype(UpdateData::_data)::key_type const& key)
{
    CCASSERT(key.target != nullptr, "Argument target must be non-nullptr");
    if (_data.count(key) > 0)
    {
        auto search_target = _targets.find(key.target);
        if (search_target != _targets.end())
        {
            _targets.erase(search_target);
        }

        _data.erase(key);
    }
}

// TimerData

template <class T>
inline constexpr void hash_combine(std::size_t& seed, T const& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

std::size_t TimerData::key_hasher::operator()(Key const& k) const noexcept
{
    std::size_t seed = 0;
    hash_combine(seed, k.name);
    hash_combine(seed, reinterpret_cast<uintptr_t>(k.target));
    return seed;
}

TimerData::element::element(Timer* ti, void* ta, std::uint64_t i, bool p, std::string const& n)
: timer(ti)
, target(ta)
, index(i)
, paused(p)
, name(n)
{
    CC_SAFE_RETAIN(timer);
}

TimerData::element::~element()
{
    CC_SAFE_RELEASE(timer);
}

void TimerData::pause_target(void* target)
{
    auto search_target = _targets.equal_range(target);
    for (auto& ele : make_iterator_pair(search_target))
    {
        const_cast<TimerData::element&>(ele.second).paused = true;
    }
}

void TimerData::resume_target(void* target)
{
    auto search_target = _targets.equal_range(target);
    for (auto& ele : make_iterator_pair(search_target))
    {
        const_cast<TimerData::element&>(ele.second).paused = false;
    }
}

bool TimerData::is_pause_target(void* target) const noexcept
{
    auto search_target = _targets.equal_range(target);
    for (auto const& ele : make_iterator_pair(search_target))
    {
        if (ele.second.paused == false)
        {
            return false;
        }
    }
    return true;
}

std::unordered_set<void*> TimerData::pause_all_targets()
{
    std::unordered_set<void*> ret;
    ret.reserve(_targets.size());
    for (auto& ele : _targets)
    {
        const_cast<TimerData::element&>(ele.second).paused = true;
        ret.emplace(ele.second.target);
    }
    return ret;
}

void TimerData::add_timer(Key const& k, Timer* timer, bool paused)
{
    CCASSERT(_timers.count(k) == 0, "timer has already been added!");

    if (_data.empty())
    {
        _index = 0;
    }

    auto const& it = _data.emplace_hint(_data.end(), timer, k.target, _index++, paused, std::string(k.name));
    _targets.emplace(k.target, *it);
    _timers.emplace(TimerData::Key{it->name, it->target}, *it);
}

void TimerData::remove_timer(SEL_SCHEDULE selector, Ref* target)
{
    auto search_target = _targets.equal_range(target);
    for (auto const& ele : make_iterator_pair(search_target))
    {
        if (ele.second.timer->getType() == Timer::Type::SELECTOR && selector == static_cast<TimerTargetSelector*>(ele.second.timer)->getSelector())
        {
            remove_element(ele.second);
            break;
        }
    }
}

void TimerData::remove_timer(Key const& k)
{
    if (auto search_timer = _timers.find(k); search_timer != _timers.end())
    {
        remove_element(search_timer->second);
    }
}

void TimerData::remove_all_timers()
{
    std::vector<std::reference_wrapper<TimerData::element const>> tmp;
    tmp.reserve(_data.size());
    for (auto const& ele : _data)
    {
        tmp.emplace_back(std::cref(ele));
    }

    for (auto const& ele : tmp)
    {
        remove_element(ele);
    }
}

void TimerData::remove_all_timers_from_target(void* target)
{
    std::vector<std::reference_wrapper<TimerData::element const>> tmp;
    tmp.reserve(_data.size());

    auto search_target = _targets.equal_range(target);
    for (auto const& ele : make_iterator_pair(search_target))
    {
        if (ele.second.target == target)
        {
            tmp.emplace_back(std::cref(ele.second));
        }
    }
    for (auto const& ele : tmp)
    {
        remove_element(ele);
    }
}

decltype(TimerData::_data)::key_type const& TimerData::get_element_from_target(Key const& k) const noexcept
{
    static decltype(_data)::key_type const empty;
    auto search = _timers.find(k);
    if (search != _timers.end())
    {
        return search->second;
    }
    return empty;
}

void TimerData::remove_element(decltype(_data)::key_type const& key)
{
    if (_data.count(key) > 0)
    {
        auto search_target = _targets.equal_range(key.target);
        for (auto it = search_target.first; it != search_target.second;)
        {
            if (std::addressof(it->second) == std::addressof(key))
            {
                it = _targets.erase(it);
            }
            else
            {
                ++it;
            }
        }

        if (auto search_timer = _timers.find(TimerData::Key{key.name, key.target}); search_timer != _timers.end())
        {
            _timers.erase(search_timer);
        }

        _data.erase(key);
    }
}

// Scheduler

Scheduler::~Scheduler()
{
    unscheduleAll();
}

void Scheduler::schedule(ccSchedulerFunc const& callback, void* target, std::chrono::milliseconds interval, bool paused, std::string const& key)
{
    schedule(callback, target, interval, CC_REPEAT_FOREVER, std::chrono::milliseconds::zero(), paused, key);
}

void Scheduler::schedule(ccSchedulerFunc const& callback, void* target, std::chrono::milliseconds interval, unsigned int repeat,
                         std::chrono::milliseconds delay, bool paused, std::string const& key)
{
    CCASSERT(target != nullptr, "Argument target must be non-nullptr");
    CCASSERT(!key.empty(), "key should not be empty!");

    TimerData::Key const k(key, target);
    auto& element = _timers.get_element_from_target(k);
    if (element.target == nullptr && element.name.empty())
    {
        TimerTargetCallback* timer = new (std::nothrow) TimerTargetCallback();
        timer->initWithCallback(this, callback, target, key, interval, repeat, delay);
        _timers.add_timer(k, timer, paused);
        _timers_to_process.emplace_back(key, target);
        timer->release();
    }
    else
    {
        CCASSERT(element.paused == paused, "element's paused should be paused!");
        if (element.timer->getType() == Timer::Type::CALLBACK)
        {
            CCLOG("CCScheduler#scheduleSelector. Selector already scheduled. Updating interval from: %.4f to %.4f", element.timer->getInterval(), interval);
            element.timer->setInterval(interval);
        }
    }
}

void Scheduler::unschedule(std::string const& key, void* target)
{
    // explicit handle nil arguments when removing an object
    if (target == nullptr || key.empty())
    {
        return;
    }

    TimerData::Key const k(key, target);
    _timers.remove_timer(k);
}

void Scheduler::schedulePerFrame(ccSchedulerFunc const& callback, void* target, int priority, bool paused)
{
    CCASSERT(target != nullptr, "Argument target must be non-nullptr");

    auto& element = _updates.get_element_from_target(target);
    if (element.target == nullptr)
    {
        _updates.add_update(callback, target, priority, paused);
        if (priority >= _updates_to_process_priority)
        {
            _updates_to_process.emplace_back(target);
        }
    }
    else
    {
        if (element.priority != priority)
        {
            _updates.remove_update(target);
            _updates.add_update(callback, target, priority, paused);

            if (priority >= _updates_to_process_priority && std::find(_updates_to_process.begin(), _updates_to_process.end(), target) == _updates_to_process.end())
            {
                _updates_to_process.emplace_back(target);
            }
        }
        else
        {
            const_cast<UpdateData::element&>(element).paused = paused;
            const_cast<UpdateData::element&>(element).callback = callback;
        }
    }
}

bool Scheduler::isScheduled(std::string const& key, void* target) const noexcept
{
    CCASSERT(!key.empty(), "Argument key must not be empty");
    CCASSERT(target != nullptr, "Argument target must be non-nullptr");

    TimerData::Key const k(key, target);
    if (_timers.count(k) > 0)
    {
        return true;
    }
    return false;
}

void Scheduler::unscheduleUpdate(void* target)
{
    if (target == nullptr)
    {
        return;
    }

    _updates.remove_update(target);
}

void Scheduler::unscheduleAll()
{
    unscheduleAllWithMinPriority(PRIORITY_SYSTEM);
}

void Scheduler::unscheduleAllWithMinPriority(int minPriority)
{
    _timers.remove_all_timers();
    _updates.remove_all_updates_with_min_priority(minPriority);
}

void Scheduler::unscheduleAllForTarget(void* target)
{
    if (target == nullptr)
    {
        return;
    }

    _timers.remove_all_timers_from_target(target);
    _updates.remove_update(target);
}

void Scheduler::resumeTarget(void* target)
{
    CCASSERT(target != nullptr, "target can't be nullptr!");
    _timers.resume_target(target);
    _updates.resume_target(target);
}

void Scheduler::pauseTarget(void* target)
{
    CCASSERT(target != nullptr, "target can't be nullptr!");
    _timers.pause_target(target);
    _updates.pause_target(target);
}

bool Scheduler::isTargetPaused(void* target)
{
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if (_timers.count(target) > 0)
    {
        return _timers.is_pause_target(target);
    }
    return _updates.is_pause_target(target);
}

std::unordered_set<void*> Scheduler::pauseAllTargets()
{
    return pauseAllTargetsWithMinPriority(PRIORITY_SYSTEM);
}

std::unordered_set<void*> Scheduler::pauseAllTargetsWithMinPriority(int minPriority)
{
    std::unordered_set<void*> idsWithSelectors;

    auto paused_timers = _timers.pause_all_targets();
    idsWithSelectors.insert(paused_timers.begin(), paused_timers.end());

    auto paused_updates = _updates.pause_all_updates_with_min_priority(minPriority);
    idsWithSelectors.insert(paused_updates.begin(), paused_updates.end());

    return idsWithSelectors;
}

void Scheduler::resumeTargets(std::unordered_set<void*> const& targetsToResume)
{
    for (auto const& obj : targetsToResume)
    {
        resumeTarget(obj);
    }
}

void Scheduler::performFunctionInCocosThread(std::function<void()> const& function)
{
    std::lock_guard<std::mutex> lock(_performMutex);
    _functionsToPerform.emplace_back(function);
}

// main loop
void Scheduler::update(float dt)
{
    if (std::abs(_timeScale - 1.f) >= std::numeric_limits<float>::epsilon())
    {
        dt *= _timeScale;
    }

    // update selectors
    if (!_updates.empty())
    {
        _updates_to_process.clear();
        for (auto const& ele : _updates)
        {
            if (!ele.paused)
            {
                _updates_to_process.emplace_back(ele.target);
            }
        }
        while (!_updates_to_process.empty())
        {
            auto target = _updates_to_process.front();
            _updates_to_process.pop_front();

            auto const& ele = _updates.get_element_from_target(target);
            if (ele.target != nullptr && !ele.paused)
            {
                _updates_to_process_priority = ele.priority;
                ele.callback(dt);
            }
        }
        _updates_to_process_priority = std::numeric_limits<int>::min();
    }

    // custom selectors
    if (!_timers.empty())
    {
        _timers_to_process.clear();
        for (auto const& ele : _timers)
        {
            if (!ele.paused)
            {
                _timers_to_process.emplace_back(ele.name, ele.target);
            }
        }
        while (!_timers_to_process.empty())
        {
            auto const& key = _timers_to_process.front();
            _timers_to_process.pop_front();

            auto const& ele = _timers.get_element_from_target(key);
            if (ele.target != nullptr && !ele.paused)
            {
                auto timer = ele.timer;
                timer->retain(); // Need to make sure that the timer won't be deleted during the execution of update()
                timer->update(dt);
                timer->release();
            }
        }
    }

    //
    // Functions allocated from another thread
    //
    decltype(_functionsToPerform) cpy;
    {
        std::lock_guard<std::mutex> lock(_performMutex);
        if (!_functionsToPerform.empty())
        {
            cpy = _functionsToPerform;
            _functionsToPerform.clear();
        }
    }
    for (auto const& function : cpy)
    {
        function();
    }
}

void Scheduler::schedule(SEL_SCHEDULE selector, Ref* target, std::chrono::milliseconds interval, unsigned int repeat, std::chrono::milliseconds delay, bool paused)
{
    CCASSERT(target != nullptr, "Argument target must be non-nullptr");

    TimerData::Key const k("", target);
    auto& element = _timers.get_element_from_target(k);
    if (element.target == nullptr)
    {
        TimerTargetSelector* timer = new (std::nothrow) TimerTargetSelector();
        timer->initWithSelector(this, selector, target, interval, repeat, delay);
        _timers.add_timer(k, timer, paused);
        _timers_to_process.emplace_back("", target);
        timer->release();
    }
    else
    {
        CCASSERT(element.paused == paused, "element's paused should be paused!");
        if (element.timer->getType() == Timer::Type::SELECTOR)
        {
            CCLOG("CCScheduler#scheduleSelector. Selector already scheduled. Updating interval from: %.4f to %.4f", element.timer->getInterval(), interval);
            element.timer->setInterval(interval);
        }
    }
}

void Scheduler::schedule(SEL_SCHEDULE selector, Ref* target, std::chrono::milliseconds interval, bool paused)
{
    this->schedule(selector, target, interval, CC_REPEAT_FOREVER, std::chrono::milliseconds::zero(), paused);
}

bool Scheduler::isScheduled(SEL_SCHEDULE selector, Ref* target) const noexcept
{
    CCASSERT(selector != nullptr, "Argument selector must be non-nullptr");
    CCASSERT(target != nullptr, "Argument target must be non-nullptr");

    auto const& element = _timers.get_element_from_target(TimerData::Key{"", target});
    if (element.target != nullptr && element.timer->getType() == Timer::Type::SELECTOR &&
        selector == static_cast<TimerTargetSelector*>(element.timer)->getSelector())
    {
        return true;
    }
    return false;
}

void Scheduler::unschedule(SEL_SCHEDULE selector, Ref* target)
{
    // explicit handle nil arguments when removing an object
    if (target == nullptr || selector == nullptr)
    {
        return;
    }

    _timers.remove_timer(selector, target);
}

NS_CC_END

//
//  CCActionManagerData.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 12/04/2018.
//

#include <CCActionManagerData.h>

#include "base/ccMacros.h"
#include "iterator_pair.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_set>

NS_CC_BEGIN

ActionManagerData::Element::Element(Node* t, Action* a, bool p, std::size_t i, ActionManagerData* m) : manager(m)
, target(t)
, action(a)
, paused(p)
, index(i)
{
    CC_SAFE_RETAIN(target);
    CC_SAFE_RETAIN(action);
}

ActionManagerData::Element::Element(ActionManagerData::Element&& other) noexcept
{
    manager = other.manager;
    target = other.target;
    action = other.action;
    paused = other.paused;
    index = other.index;

    other.manager = nullptr;
    other.target = nullptr;
    other.action = nullptr;
}

ActionManagerData::Element& ActionManagerData::Element::operator=(ActionManagerData::Element&& other) noexcept
{
    manager = other.manager;
    target = other.target;
    action = other.action;
    paused = other.paused;
    index = other.index;

    other.manager = nullptr;
    other.target = nullptr;
    other.action = nullptr;

    return *this;
}

void ActionManagerData::Element::destroy() const
{
    CCASSERT(manager != nullptr, "manager can't be nullptr!");
    manager->_actions.erase(action);

    auto search_target = manager->_targets.equal_range(target);
    auto search_action = std::find_if(search_target.first, search_target.second, [this](typename target_t::value_type const& element) {
        return std::addressof(element.second) == std::addressof(*this);
    });
    if (search_action != search_target.second)
    {
        manager->_targets.erase(search_action);
    }

    action->stop();

    CC_SAFE_RELEASE(target);
    CC_SAFE_RELEASE(action);
}

void ActionManagerData::pause_target(Node* target)
{
    auto search_target = _targets.equal_range(target);
    for (auto& ele : make_iterator_pair(search_target))
    {
        const_cast<Element&>(ele.second).paused = true;
    }
}

void ActionManagerData::resume_target(Node* target)
{
    auto search_target = _targets.equal_range(target);
    for (auto& ele : make_iterator_pair(search_target))
    {
        const_cast<Element&>(ele.second).paused = false;
    }
}

void ActionManagerData::pause_all_targets()
{
    for (auto& ele : _actions)
    {
        const_cast<Element&>(ele.second).paused = true;
    }
}

void ActionManagerData::add_action(Node* target, Action* action, bool paused)
{
    CCASSERT(_actions.count(action) == 0, "action already be added!");

    if (_data.empty())
    {
        _index = 0;
    }

    action->startWithTarget(target);
    auto const data = _data.emplace_hint(_data.end(), target, action, paused, _index++, this);
    _actions.emplace(action, *data);
    _targets.emplace(target, *data);
}

void ActionManagerData::remove_all_actions_from_target(Node* target)
{
    auto search_target = _targets.equal_range(target);
    auto const size = std::distance(search_target.first, search_target.second);
    if (size > 0)
    {
        std::vector<std::reference_wrapper<Element const>> tmp;
        tmp.reserve(size);
        for (auto const& ele : make_iterator_pair(search_target))
        {
            tmp.emplace_back(std::cref(ele.second));
        }

        for (auto const& ele : tmp)
        {
            remove_element(ele);
        }
    }
}

void ActionManagerData::remove_all_actions()
{
    std::vector<std::reference_wrapper<Element const>> tmp;
    tmp.reserve(_data.size());
    for (auto const& ele :_data)
    {
        tmp.emplace_back(std::cref(ele));
    }

    for (auto const& ele :tmp)
    {
        remove_element(ele);
    }
}

void ActionManagerData::remove_action(Action* action)
{
    auto search = _actions.find(action);
    if (search != _actions.end())
    {
        remove_element(search->second);
    }
}

void ActionManagerData::remove_action_from_target_by_tag(Node* target, int tag)
{
    auto search_target = _targets.equal_range(target);
    auto search_action = std::find_if(search_target.first, search_target.second, [tag](typename target_t::value_type const& ele) {
        return ele.second.action->getTag() == tag;
    });
    if (search_action != search_target.second)
    {
        remove_element(search_action->second);
    }
}

void ActionManagerData::remove_all_actions_from_target_by_tag(Node* target, int tag)
{
    auto search_target = _targets.equal_range(target);
    auto const size = std::distance(search_target.first, search_target.second);
    if (size > 0)
    {
        std::vector<std::reference_wrapper<Element const>> tmp;
        tmp.reserve(size);
        for (auto const& ele : make_iterator_pair(search_target))
        {
            if (ele.second.action->getTag() == tag)
            {
                tmp.emplace_back(std::cref(ele.second));
            }
        }

        for (auto const& ele : tmp)
        {
            remove_element(ele);
        }
    }
}

void ActionManagerData::remove_all_actions_from_target_by_flag(Node* target, unsigned int flag)
{
    auto search_target = _targets.equal_range(target);
    auto const size = std::distance(search_target.first, search_target.second);
    if (size > 0)
    {
        std::vector<std::reference_wrapper<Element const>> tmp;
        tmp.reserve(size);
        for (auto const& ele : make_iterator_pair(search_target))
        {
            if ((ele.second.action->getFlags() & flag) == flag)
            {
                tmp.emplace_back(std::cref(ele.second));
            }
        }

        for (auto const& ele : tmp)
        {
            remove_element(ele);
        }
    }
}

Action* ActionManagerData::get_action_from_target_by_tag(Node* target, int tag) const noexcept
{
    auto search_target = _targets.equal_range(target);
    auto search_action = std::find_if(search_target.first, search_target.second, [tag](typename target_t::value_type const& ele) {
        return ele.second.action->getTag() == tag;
    });
    if (search_action != search_target.second)
    {
        return search_action->second.action;
    }
    return nullptr;
}

std::size_t ActionManagerData::get_number_of_running_action_from_target(Node* target) const noexcept
{
    auto search_target = _targets.equal_range(target);
    return std::distance(search_target.first, search_target.second);
}

std::vector<Action*> ActionManagerData::get_all_actions_from_target(Node* target) const
{
    std::vector<Action*> ret;

    auto search_target = _targets.equal_range(target);
    auto const size = std::distance(search_target.first, search_target.second);
    if (size > 0)
    {
        ret.reserve(size);
        for (auto& ele : make_iterator_pair(search_target))
        {
            ret.emplace_back(ele.second.action);
        }
    }

    return ret;
}

std::vector<Node*> ActionManagerData::get_all_targets() const
{
    std::unordered_set<Node*> tmp;

    tmp.reserve(_targets.size());
    for (auto const& it : _targets)
    {
        tmp.emplace(it.first);
    }

    return std::vector<Node*>{tmp.begin(), tmp.end()};
}

typename ActionManagerData::data_t::key_type const& ActionManagerData::get_element_from_action(Action* action) const noexcept
{
    static typename ActionManagerData::data_t::key_type const empty;
    auto search = _actions.find(action);
    if (search != _actions.end())
    {
        return search->second;
    }
    return empty;
}

void ActionManagerData::remove_element(typename data_t::key_type const& key)
{
    if (_data.count(key) > 0)
    {
        key.destroy();
        _data.erase(key);
    }
}

NS_CC_END

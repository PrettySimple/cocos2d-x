//
//  CCActionManagerData.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#include "CCActionManagerData.h"

#include "2d/CCAction.h"
#include "2d/CCNode.h"
#include "base/ccMacros.h"
#include <algorithm>
#include <functional>
#include <vector>
#include <unordered_set>

NS_CC_BEGIN

template <typename T>
struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename PairIter>
class iterator_pair
{
public:
    using first_type_t = typename remove_cvref_t<PairIter>::first_type;
    using second_type_t = typename remove_cvref_t<PairIter>::second_type;

    inline explicit iterator_pair (PairIter&& p) : _p (std::forward<PairIter>(p)) {}
    inline first_type_t begin() { return _p.first; }
    inline second_type_t end() { return _p.second; }
    inline first_type_t begin() const noexcept { return _p.first; }
    inline second_type_t end() const noexcept { return _p.second; }

private:
    PairIter _p;
};

template <typename PairIter>
inline iterator_pair<PairIter> make_iterator_pair(PairIter&& p)
{
    return iterator_pair<PairIter>(std::forward<PairIter>(p));
}

ActionManagerData::Element::Element(Node * t, Action * a, bool p, std::size_t i, ActionManagerData* m) : manager(m)
, target(t)
, action(a)
, paused(p)
, index(i)
{
    if (manager->_targets.count(target) == 0)
    {
        CC_SAFE_RETAIN(target);
    }
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

ActionManagerData::Element::~Element()
{
    if (manager != nullptr)
    {
        manager->_actions.erase(action);

        auto search_target = manager->_targets.equal_range(target);
        auto search_action = std::find_if(search_target.first, search_target.second, [this](target_t::value_type const& element) {
            return action == element.second.action;
        });
        if (search_action != manager->_targets.end())
        {
            manager->_targets.erase(search_action);
        }

        if (manager->_targets.count(target) == 0)
        {
            CC_SAFE_RELEASE(target);
        }
        CC_SAFE_RELEASE(action);
    }
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
            _data.erase(ele);
        }
    }
}

void ActionManagerData::remove_all_actions()
{
    _data.clear();
}

void ActionManagerData::remove_action(Action* action)
{
    auto search = _actions.find(action);
    if (search != _actions.end())
    {
        _data.erase(search->second);
    }
}

void ActionManagerData::remove_action_from_target_by_tag(Node* target, int tag)
{
    auto search_target = _targets.equal_range(target);
    auto search_action = std::find_if(search_target.first, search_target.second, [tag](target_t::value_type const& ele) {
        return ele.second.action->getTag() == tag;
    });
    if (search_action != _targets.end())
    {
        _data.erase(search_action->second);
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
            if (ele.first->getTag() == tag)
            {
                tmp.emplace_back(std::cref(ele.second));
            }
        }

        for (auto const& ele : tmp)
        {
            _data.erase(ele);
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
            _data.erase(ele);
        }
    }
}

Action* ActionManagerData::get_action_from_target_by_tag(Node* target, int tag) const noexcept
{
    auto search_target = _targets.equal_range(target);
    auto search_action = std::find_if(search_target.first, search_target.second, [tag](target_t::value_type const& ele) {
        return ele.first->getTag() == tag;
    });
    if (search_action != _targets.end())
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

ActionManagerData::data_t::key_type const& ActionManagerData::get_element_from_action(Action* action) const noexcept
{
    static ActionManagerData::data_t::key_type const empty;
    auto search = _actions.find(action);
    if (search != _actions.end())
    {
        return search->second;
    }
    return empty;
}

NS_CC_END

//
//  CCActionManagerData.h
//  cocos2d
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#ifndef ACTION_CCACTION_MANAGERDATA_H
#define ACTION_CCACTION_MANAGERDATA_H

#include "2d/CCAction.h"
#include "2d/CCNode.h"
#include "base/ccMacros.h"
#include "iterator_pair.h"
#include "platform/CCPlatformMacros.h"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

NS_CC_BEGIN

template <bool DryRun>
class ActionManagerData
{
    struct Element
    {
        ActionManagerData<DryRun>* manager = nullptr; // weak ref
        Node* target = nullptr; // weak ref
        Action* action = nullptr; // weak ref
        bool paused = false;
        mutable bool on_remove_once = true;
        std::size_t index = 0;

        Element() = default;
        explicit Element(Node* t, Action* a, bool p, std::size_t i, ActionManagerData<DryRun>* m) : manager(m)
        , target(t)
        , action(a)
        , paused(p)
        , index(i)
        {
            if constexpr(!DryRun)
            {
                if (manager->_targets.count(target) == 0)
                {
                    CC_SAFE_RETAIN(target);
                }
                CC_SAFE_RETAIN(action);
            }
        }
        Element(Element const&) = delete;
        Element& operator=(Element const&) = delete;
        Element(Element&& other) noexcept
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
        Element& operator=(Element&& other) noexcept
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
        ~Element()
        {
            if (manager != nullptr)
            {
                manager->_actions.erase(action);

                auto search_target = manager->_targets.equal_range(target);
                auto search_action = std::find_if(search_target.first, search_target.second, [this](typename target_t::value_type const& element) {
                    return std::addressof(element.second) == std::addressof(*this);
                });
                if (search_action != search_target.second)
                {
                    manager->_targets.erase(search_action);
                }

                on_remove();

                if constexpr(!DryRun)
                {
                    if (manager->_targets.count(target) == 0)
                    {
                        CC_SAFE_RELEASE(target);
                    }
                    CC_SAFE_RELEASE(action);
                }
            }
        }

        inline void on_remove() const
        {
            if constexpr(!DryRun)
            {
                if (on_remove_once)
                {
                    on_remove_once = false;
                    action->stop();
                }
            }
        }
    };

    struct element_less
    {
        constexpr bool operator()(Element const& a, Element const& b) const
        {
            return a.index < b.index;
        }
    };

    struct element_hash
    {
        std::size_t operator()(Element const& s) const noexcept
        {
            return s.index;
        }
    };

    struct element_equal_to
    {
        constexpr bool operator()(Element const& lhs, Element const& rhs ) const
        {
            return lhs.index == rhs.index;
        }
    };
    
    using data_t = typename std::conditional<DryRun, std::unordered_set<Element, element_hash, element_equal_to>, std::set<Element, element_less>>::type;
    using action_t = std::unordered_map<Action*, typename data_t::key_type const&>;
    using target_t = std::unordered_multimap<Node*, typename data_t::key_type const&>;

    data_t _data;
    action_t _actions; // weak ref
    target_t _targets; // weak ref
    std::size_t _index = 0;
public:
    void pause_target(Node* target)
    {
        auto search_target = _targets.equal_range(target);
        for (auto& ele : make_iterator_pair(search_target))
        {
            const_cast<Element&>(ele.second).paused = true;
        }
    }
    void resume_target(Node* target)
    {
        auto search_target = _targets.equal_range(target);
        for (auto& ele : make_iterator_pair(search_target))
        {
            const_cast<Element&>(ele.second).paused = false;
        }
    }
    void pause_all_targets()
    {
        for (auto& ele : _actions)
        {
            const_cast<Element&>(ele.second).paused = true;
        }
    }

    void add_action(Node* target, Action* action, bool paused)
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

    void remove_all_actions_from_target(Node* target)
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
    void remove_all_actions()
    {
        _data.clear();
    }
    void remove_action(Action* action)
    {
        auto search = _actions.find(action);
        if (search != _actions.end())
        {
            _data.erase(search->second);
        }
    }
    void remove_action_from_target_by_tag(Node* target, int tag)
    {
        auto search_target = _targets.equal_range(target);
        auto search_action = std::find_if(search_target.first, search_target.second, [tag](typename target_t::value_type const& ele) {
            return ele.second.action->getTag() == tag;
        });
        if (search_action != search_target.second)
        {
            _data.erase(search_action->second);
        }
    }
    void remove_all_actions_from_target_by_tag(Node* target, int tag)
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
                _data.erase(ele);
            }
        }
    }
    void remove_all_actions_from_target_by_flag(Node* target, unsigned int flag)
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

    Action* get_action_from_target_by_tag(Node* target, int tag) const noexcept
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
    std::size_t get_number_of_running_action_from_target(Node* target) const noexcept
    {
        auto search_target = _targets.equal_range(target);
        return std::distance(search_target.first, search_target.second);
    }
    std::vector<Action*> get_all_actions_from_target(Node* target) const
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
    std::vector<Node*> get_all_targets() const
    {
        std::unordered_set<Node*> tmp;

        tmp.reserve(_targets.size());
        for (auto const& it : _targets)
        {
            tmp.emplace(it.first);
        }

        return std::vector<Node*>{tmp.begin(), tmp.end()};
    }

    typename data_t::key_type const& get_element_from_action(Action* action) const noexcept
    {
        static typename ActionManagerData::data_t::key_type const empty;
        auto search = _actions.find(action);
        if (search != _actions.end())
        {
            return search->second;
        }
        return empty;
    }
    inline void remove_element(typename data_t::key_type const& key) { _data.erase(key); }

    inline void clear() { _data.clear(); _actions.clear(); _targets.clear(); _index = 0;  }
    inline bool empty() const noexcept { return _data.empty(); }
    inline typename data_t::iterator begin() { return _data.begin(); }
    inline typename data_t::iterator end() { return _data.end(); }
    inline typename data_t::const_iterator begin() const noexcept { return _data.begin(); }
    inline typename data_t::const_iterator end() const noexcept { return _data.end(); }

    inline typename target_t::size_type count(Node* target) const noexcept { return _targets.count(target); }
    inline typename action_t::size_type count(Action* action) const noexcept { return _actions.count(action); }
};

NS_CC_END

#endif // ACTION_CCACTION_MANAGERDATA_H

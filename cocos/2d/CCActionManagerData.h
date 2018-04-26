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
#include "platform/CCPlatformMacros.h"

#include <set>
#include <unordered_map>
#include <vector>

NS_CC_BEGIN

class ActionManagerData
{
    struct Element
    {
        ActionManagerData* manager = nullptr; // weak ref
        Node* target = nullptr;
        Action* action = nullptr;
        bool paused = false;
        std::size_t index = 0;

        Element() = default;
        explicit Element(Node* t, Action* a, bool p, std::size_t i, ActionManagerData* m);
        Element(Element const&) = delete;
        Element& operator=(Element const&) = delete;
        Element(Element&& other) noexcept = delete;
        Element& operator=(Element&& other) noexcept = delete;
        ~Element() = default;

        void destroy() const;
    };

    struct element_less
    {
        constexpr bool operator()(Element const& a, Element const& b) const
        {
            return a.index < b.index;
        }
    };
    
    using data_t = std::set<Element, element_less>;
    using action_t = std::unordered_map<Action*, typename data_t::key_type const&>;
    using target_t = std::unordered_multimap<Node*, typename data_t::key_type const&>;

    data_t _data;
    action_t _actions; // weak ref
    target_t _targets; // weak ref
    std::size_t _index = 0;
public:
    void pause_target(Node* target);
    void resume_target(Node* target);
    void pause_all_targets();

    void add_action(Node* target, Action* action, bool paused);

    void remove_all_actions_from_target(Node* target);
    void remove_all_actions();
    void remove_action(Action* action);
    void remove_action_from_target_by_tag(Node* target, int tag);
    void remove_all_actions_from_target_by_tag(Node* target, int tag);
    void remove_all_actions_from_target_by_flag(Node* target, unsigned int flag);

    Action* get_action_from_target_by_tag(Node* target, int tag) const noexcept;
    std::size_t get_number_of_running_action_from_target(Node* target) const noexcept;
    std::vector<Action*> get_all_actions_from_target(Node* target) const;
    std::vector<Node*> get_all_targets() const;

    typename data_t::key_type const& get_element_from_action(Action* action) const noexcept;
    void remove_element(typename data_t::key_type const& key);

    inline void clear() { remove_all_actions(); }
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

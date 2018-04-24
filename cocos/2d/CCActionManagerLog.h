//
//  CCActionManagerLog.h
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#ifndef ACTION_CCACTION_MANAGERLOG_H
#define ACTION_CCACTION_MANAGERLOG_H

#include "CCActionManagerData.h"
#include "platform/CCPlatformMacros.h"
#include <cstddef>
#include <deque>
#include <vector>

NS_CC_BEGIN

class Node;
class Action;

class ActionManagerLog
{
    std::deque<Action*> _log;
    ActionManagerData _data;

public:
    inline void pause_target(Node* target) { _data.pause_target(target); }
    inline void resume_target(Node* target) { _data.resume_target(target); }
    inline void pause_all_targets() { _data.pause_all_targets(); }

    void add_action(Node* target, Action* action, bool paused);
    
    inline void remove_all_actions_from_target(Node* target) { _data.remove_all_actions_from_target(target); }
    inline void remove_all_actions() { _data.remove_all_actions(); }
    inline void remove_action(Action* action) { _data.remove_action(action); }
    inline void remove_action_from_target_by_tag(Node* target, int tag) { _data.remove_action_from_target_by_tag(target, tag); }
    inline void remove_all_actions_from_target_by_tag(Node* target, int tag) { _data.remove_all_actions_from_target_by_tag(target, tag); }
    inline void remove_all_actions_from_target_by_flag(Node* target, unsigned int flag) { _data.remove_all_actions_from_target_by_flag(target, flag); }

    inline Action* get_action_from_target_by_tag(Node* target, int tag) const noexcept { return _data.get_action_from_target_by_tag(target, tag); }
    inline std::size_t get_number_of_running_action_from_target(Node* target) const noexcept { return _data.get_number_of_running_action_from_target(target); }
    inline std::vector<Action*> get_all_actions_from_target(Node* target) const { return _data.get_all_actions_from_target(target); }
    inline std::vector<Node*> get_all_targets() const { return _data.get_all_targets(); }

    void update(float dt);
};

NS_CC_END

#endif // ACTION_CCACTION_MANAGERLOG_H

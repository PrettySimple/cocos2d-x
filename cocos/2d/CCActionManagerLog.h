//
//  CCActionManagerLog.h
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#ifndef ACTION_CCACTION_MANAGERLOG_H
#define ACTION_CCACTION_MANAGERLOG_H

#include "CCActionManagerData.h"
#include "CCActionManagerRecord.h"
#include "platform/CCPlatformMacros.h"
#include <deque>
#include <cstddef>
#include <vector>

NS_CC_BEGIN

class Node;
class Action;

class ActionManagerLog
{
    enum struct Mode : std::uint8_t
    {
        DEFAULT = 0,
        UPDATE
    };
    using log_t = std::deque<ActionManagerRecord>;
    log_t _log;
    Mode _mode = Mode::DEFAULT;
    log_t::size_type _scan_insert_ind = 0;
    ActionManagerData _data;
    ActionManagerData _dry_run;

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

    void update(float dt);
};

NS_CC_END

#endif // ACTION_CCACTION_MANAGERLOG_H

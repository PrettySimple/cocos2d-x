//
//  CCActionManagerLog.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#include "CCActionManagerLog.h"

#include "CCActionManagerOperation.h"
#include "2d/CCAction.h"
#include "2d/CCNode.h"

NS_CC_BEGIN

void ActionManagerLog::pause_target(Node* target)
{
    switch (_mode)
    {
        case Mode::UPDATE:
            if (_data.count(target) > 0 || _dry_run.count(target) > 0)
            {
                _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::PAUSE_TARGET, target);
                _dry_run.pause_target(target);
            }
            break;
        case Mode::DEFAULT:
            _data.pause_target(target);
            break;
    }
}

void ActionManagerLog::resume_target(Node* target)
{
    switch (_mode)
    {
        case Mode::UPDATE:
            if (_data.count(target) > 0 || _dry_run.count(target) > 0)
            {
                _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::RESUME_TARGET, target);
                _dry_run.resume_target(target);
            }
            break;
        case Mode::DEFAULT:
            _data.resume_target(target);
            break;
    }
}

void ActionManagerLog::pause_all_targets()
{
    switch (_mode)
    {
        case Mode::UPDATE:
            _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::PAUSE_ALL_TARGETS);
            _dry_run.pause_all_targets();
            break;
        case Mode::DEFAULT:
            _data.pause_all_targets();
            break;
    }
}

void ActionManagerLog::add_action(Node* target, Action* action, bool paused)
{
    switch (_mode)
    {
        case Mode::UPDATE:
            _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::ADD_ACTION, target, action, paused);
            _log.emplace(_log.cbegin() + _scan_insert_ind++,ActionManagerOperation::STEP_ACTION, target, action);
            _dry_run.add_action(target, action, paused);
            break;
        case Mode::DEFAULT:
            _data.add_action(target, action, paused);
            break;
    }
}

void ActionManagerLog::remove_all_actions_from_target(Node* target)
{
    switch (_mode)
    {
        case Mode::UPDATE:
            if (_data.count(target) > 0 || _dry_run.count(target) > 0)
            {
                _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::REMOVE_ALL_ACTIONS_FROM_TARGET, target);
                _dry_run.remove_all_actions_from_target(target);
            }
            break;
        case Mode::DEFAULT:
            _data.remove_all_actions_from_target(target);
            break;
    }
}

void ActionManagerLog::remove_all_actions()
{
    switch (_mode)
    {
        case Mode::UPDATE:
            _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::REMOVE_ALL_ACTIONS);
            _dry_run.remove_all_actions();
            break;
        case Mode::DEFAULT:
            _data.remove_all_actions();
            break;
    }
}

void ActionManagerLog::remove_action(Action* action)
{
    switch (_mode)
    {
        case Mode::UPDATE:
            if (_data.count(action) > 0 || _dry_run.count(action) > 0)
            {
                _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::REMOVE_ACTION, nullptr, action);
                _dry_run.remove_action(action);
            }
            break;
        case Mode::DEFAULT:
            _data.remove_action(action);
            break;
    }
}

void ActionManagerLog::remove_action_from_target_by_tag(Node* target, int tag)
{
    switch (_mode)
    {
        case Mode::UPDATE:
            if (_data.count(target) > 0 || _dry_run.count(target) > 0)
            {
                _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::REMOVE_ACTION_BY_TAG, target, nullptr, false, tag);
                _dry_run.remove_action_from_target_by_tag(target, tag);
            }
            break;
        case Mode::DEFAULT:
            _data.remove_action_from_target_by_tag(target, tag);
            break;
    }
}

void ActionManagerLog::remove_all_actions_from_target_by_tag(Node* target, int tag)
{
    switch (_mode)
    {
        case Mode::UPDATE:
            if (_data.count(target) > 0 || _dry_run.count(target) > 0)
            {
                _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::REMOVE_ALL_ACTIONS_BY_TAG, target, nullptr, false, tag);
                _dry_run.remove_all_actions_from_target_by_tag(target, tag);
            }
            break;
        case Mode::DEFAULT:
            _data.remove_all_actions_from_target_by_tag(target, tag);
            break;
    }
}

void ActionManagerLog::remove_all_actions_from_target_by_flag(Node* target, unsigned int flag)
{
    switch (_mode)
    {
        case Mode::UPDATE:
            if (_data.count(target) > 0 || _dry_run.count(target) > 0)
            {
                _log.emplace(_log.cbegin() + _scan_insert_ind++, ActionManagerOperation::REMOVE_ALL_ACTIONS_BY_FLAG, target, nullptr, false, -1, flag);
                _dry_run.remove_all_actions_from_target_by_flag(target, flag);
            }
            break;
        case Mode::DEFAULT:
            _data.remove_all_actions_from_target_by_flag(target, flag);
            break;
    }
}

Action* ActionManagerLog::get_action_from_target_by_tag(Node* target, int tag) const noexcept
{
    switch (_mode)
    {
        case Mode::UPDATE:
        {
            Action* ret = _data.get_action_from_target_by_tag(target, tag);
            if (ret == nullptr)
            {
                return _dry_run.get_action_from_target_by_tag(target, tag);
            }
            else
            {
                return ret;
            }
        }
        case Mode::DEFAULT:
            return _data.get_action_from_target_by_tag(target, tag);
    }
}

std::size_t ActionManagerLog::get_number_of_running_action_from_target(Node* target) const noexcept
{
    switch (_mode)
    {
        case Mode::UPDATE:
            return _data.get_number_of_running_action_from_target(target) + _dry_run.get_number_of_running_action_from_target(target);
        case Mode::DEFAULT:
            return _data.get_number_of_running_action_from_target(target);
    }
}

std::vector<Action*> ActionManagerLog::get_all_actions_from_target(Node* target) const
{
    switch (_mode)
    {
        case Mode::UPDATE:
        {
            auto dest { _data.get_all_actions_from_target(target) };
            auto const src { _dry_run.get_all_actions_from_target(target) };
            dest.insert(dest.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
            return dest;
        }
        case Mode::DEFAULT:
            return _data.get_all_actions_from_target(target);
    }
}


std::vector<Node*> ActionManagerLog::get_all_targets() const
{
    switch (_mode)
    {
        case Mode::UPDATE:
        {
            auto dest { _data.get_all_targets() };
            auto const src { _dry_run.get_all_targets() };
            dest.insert(dest.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
            return dest;
        }
        case Mode::DEFAULT:
            return _data.get_all_targets();
    }
}

void ActionManagerLog::update(float dt)
{
    if (!_data.empty())
    {
        _mode = Mode::UPDATE;
        
        for (auto const& ele : _data)
        {
            if (!ele.paused)
            {
                _log.emplace_back(ActionManagerOperation::STEP_ACTION, nullptr, ele.action);
            }
        }
        
        while (!_log.empty())
        {
            auto record = std::move(_log.front());
            if (_scan_insert_ind > 0)
            {
                _scan_insert_ind--;
            }
            _log.pop_front();
            switch (record.operation)
            {
                case ActionManagerOperation::UNKNOWN:
                    break;
                case ActionManagerOperation::PAUSE_TARGET:
                    _data.pause_target(record.target);
                    break;
                case ActionManagerOperation::RESUME_TARGET:
                    _data.resume_target(record.target);
                    break;
                case ActionManagerOperation::PAUSE_ALL_TARGETS:
                    _data.pause_all_targets();
                    break;
                case ActionManagerOperation::ADD_ACTION:
                    _data.add_action(record.target, record.action, record.paused);
                    break;
                case ActionManagerOperation::REMOVE_ALL_ACTIONS:
                    _data.remove_all_actions();
                    break;
                case ActionManagerOperation::REMOVE_ALL_ACTIONS_FROM_TARGET:
                    _data.remove_all_actions_from_target(record.target);
                    break;
                case ActionManagerOperation::REMOVE_ACTION:
                    _data.remove_action(record.action);
                    break;
                case ActionManagerOperation::REMOVE_ACTION_BY_TAG:
                    _data.remove_action_from_target_by_tag(record.target, record.tag);
                    break;
                case ActionManagerOperation::REMOVE_ALL_ACTIONS_BY_TAG:
                    _data.remove_all_actions_from_target_by_tag(record.target, record.tag);
                    break;
                case ActionManagerOperation::REMOVE_ALL_ACTIONS_BY_FLAG:
                    _data.remove_all_actions_from_target_by_flag(record.target, record.flags);
                    break;
                case ActionManagerOperation::STEP_ACTION:
                {
                    auto const& ele = _data.get_element_from_action(record.action);
                    if (ele.target != nullptr)
                    {
                        if (ele.target->getReferenceCount() > 1)
                        {
                            if (ele.action != nullptr && !ele.paused)
                            {
                                ele.action->step(dt);
                                
                                if (ele.action->isDone())
                                {
                                    _data.remove_element(ele);
                                }
                            }
                        }
                        else
                        {
                            _data.remove_element(ele);
                        }
                    }
                }
                    break;
            }
        }
        
        _dry_run.clear();
        _scan_insert_ind = 0;
        
        _mode = Mode::DEFAULT;
    }
}

NS_CC_END

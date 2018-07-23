/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2009      Valentin Milea
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 CopyRight (c) 2013-2016 Chukong Technologies Inc.

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

#include <cocos/2d/CCActionManager.h>

#include <cocos/2d/CCAction.h>
#include "2d/CCActionManagerData.h"
#include <cocos/2d/CCNode.h>
#include <cocos/base/ccMacros.h>

NS_CC_BEGIN

ActionManager::~ActionManager()
{
    CCLOGINFO("deallocing ActionManager: %p", this);
    removeAllActions();
}

// pause / resume

void ActionManager::pauseTarget(Node* target)
{
    _actions.pause_target(target);
}

void ActionManager::resumeTarget(Node* target)
{
    _actions.resume_target(target);
}

Vector<Node*> ActionManager::pauseAllRunningActions()
{
    Vector<Node*> ret;

    auto const tmp{_actions.get_all_targets()};
    ret.reserve(tmp.size());
    for (auto const action : tmp)
    {
        ret.pushBack(action);
    }

    _actions.pause_all_targets();

    return ret;
}

void ActionManager::resumeTargets(Vector<Node*> const& targetsToResume)
{
    for (auto node : targetsToResume)
    {
        resumeTarget(node);
    }
}

// run

void ActionManager::addAction(Action* action, Node* target, bool paused)
{
    CCASSERT(action != nullptr, "action can't be nullptr!");
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if (action == nullptr || target == nullptr)
        return;

    _actions_to_process.emplace_back(action);
    _actions.add_action(target, action, paused);
}

// remove

void ActionManager::removeAllActions()
{
    _actions.remove_all_actions();
}

void ActionManager::removeAllActionsFromTarget(Node* target)
{
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if (target == nullptr)
        return;

    _actions.remove_all_actions_from_target(target);
}

void ActionManager::removeAction(Action* action)
{
    //    CCASSERT(action != nullptr && action->getOriginalTarget() != nullptr, "target can't be nullptr!");
    if (action == nullptr)
        return;

    _actions.remove_action(action);
}

void ActionManager::removeAllActionsByTag(int tag, Node* target)
{
    CCASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if (target == nullptr || tag == Action::INVALID_TAG)
        return;

    _actions.remove_all_actions_from_target_by_tag(target, tag);
}

void ActionManager::removeActionsByFlags(unsigned int flags, Node* target)
{
    CCASSERT(target != nullptr, "target can't be nullptr!");
    CCASSERT(flags != 0, "flags can't be 0!");
    if (target == nullptr || flags == 0)
        return;

    _actions.remove_all_actions_from_target_by_flag(target, flags);
}

// get

Action* ActionManager::getActionByTag(int tag, Node* target) const
{
    CCASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if (tag == Action::INVALID_TAG || target == nullptr)
        return nullptr;

    return _actions.get_action_from_target_by_tag(target, tag);
}

std::size_t ActionManager::getNumberOfRunningActionsInTarget(Node* target) const
{
    return _actions.get_number_of_running_action_from_target(target);
}

std::vector<Action*> ActionManager::getRunningActionsInTarget(Node* target) const
{
    return _actions.get_all_actions_from_target(target);
}

// main loop
void ActionManager::update(float dt)
{
    if (!_actions.empty())
    {
        _actions_to_process.clear();
        for (auto const& ele : _actions)
        {
            if (!ele.paused)
            {
                _actions_to_process.emplace_back(ele.action);
            }
        }

        while (!_actions_to_process.empty())
        {
            auto action = _actions_to_process.front();
            _actions_to_process.pop_front();

            auto const& ele = _actions.get_element_from_action(action);
            if (ele.target != nullptr)
            {
                if (ele.target->getReferenceCount() > 1)
                {
                    if (!ele.paused)
                    {
                        action->retain(); // Need to make sure that the action won't be deleted during the execution of step()
                        action->step(dt);

                        if (_actions.count(action) > 0 && action->isDone()) // Need to be sure that the action hasn't been removed during the execution of step()
                        {
                            _actions.remove_element(ele);
                        }
                        action->release();
                    }
                }
                else
                {
                    _actions.remove_element(ele);
                }
            }
        }
    }
}

NS_CC_END

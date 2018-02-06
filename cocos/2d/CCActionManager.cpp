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

#include "2d/CCActionManager.h"

#include "2d/CCAction.h"
#include "2d/CCNode.h"
#include "base/ccMacros.h"
#include "platform/CCPlatformDefine.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <utility>

NS_CC_BEGIN

ActionManager::~ActionManager()
{
    CCLOGINFO("deallocing ActionManager: %p", this);
    removeAllActions();
}

// pause / resume

void ActionManager::pauseTarget(Node *target)
{
    _journal.pause_target(target);
}

void ActionManager::resumeTarget(Node *target)
{
    _journal.resume_target(target);
}

Vector<Node*> ActionManager::pauseAllRunningActions()
{
    Vector<Node*> ret;

    auto const tmp { _journal.get_all_targets() };
    ret.reserve(tmp.size());
    for (auto const action : tmp)
    {
        ret.pushBack(action);
    }

    _journal.pause_all_targets();

    return ret;
}

void ActionManager::resumeTargets(Vector<Node*> const& targetsToResume)
{
    for(auto node : targetsToResume)
    {
        resumeTarget(node);
    }
}

// run

void ActionManager::addAction(Action *action, Node *target, bool paused)
{
    CCASSERT(action != nullptr, "action can't be nullptr!");
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if(action == nullptr || target == nullptr)
        return;

    _journal.add_action(target, action, paused);
}

// remove

void ActionManager::removeAllActions()
{
    _journal.remove_all_actions();
}

void ActionManager::removeAllActionsFromTarget(Node * target)
{
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if (target == nullptr)
        return;

    _journal.remove_all_actions_from_target(target);
}

void ActionManager::removeAction(Action *action)
{
    //    CCASSERT(action != nullptr && action->getOriginalTarget() != nullptr, "target can't be nullptr!");
    if (action == nullptr)
        return;

    _journal.remove_action(action);
}

void ActionManager::removeActionByTag(int tag, Node *target)
{
    CCASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if (target == nullptr || tag == Action::INVALID_TAG)
        return;

    _journal.remove_action_from_target_by_tag(target, tag);
}

void ActionManager::removeAllActionsByTag(int tag, Node *target)
{
    CCASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if (target == nullptr || tag == Action::INVALID_TAG)
        return;

    _journal.remove_all_actions_from_target_by_tag(target, tag);
}

void ActionManager::removeActionsByFlags(unsigned int flags, Node *target)
{
    CCASSERT(target != nullptr, "target can't be nullptr!");
    CCASSERT(flags != 0, "flags can't be 0!");
    if (target == nullptr || flags == 0)
        return;

    _journal.remove_all_actions_from_target_by_flag(target, flags);
}

// get

Action* ActionManager::getActionByTag(int tag, Node *target) const
{
    CCASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");
    CCASSERT(target != nullptr, "target can't be nullptr!");
    if (tag == Action::INVALID_TAG || target == nullptr)
        return nullptr;

    return _journal.get_action_from_target_by_tag(target, tag);
}

ssize_t ActionManager::getNumberOfRunningActionsInTarget(Node *target) const
{
    return _journal.get_number_of_running_action_from_target(target);
}

std::vector<Action*> ActionManager::getRunningActionsInTarget(Node *target) const
{
    return _journal.get_all_actions_from_target(target);
}

// main loop
void ActionManager::update(float dt)
{
    _journal.update(dt);
}

NS_CC_END

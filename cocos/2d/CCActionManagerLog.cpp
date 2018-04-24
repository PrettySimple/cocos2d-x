//
//  CCActionManagerLog.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#include "CCActionManagerLog.h"

#include "2d/CCAction.h"
#include "2d/CCNode.h"

NS_CC_BEGIN

void ActionManagerLog::add_action(Node* target, Action* action, bool paused)
{
    _log.emplace_back(action);
    _data.add_action(target, action, paused);
}

void ActionManagerLog::update(float dt)
{
    if (!_data.empty())
    {
        _log.clear();
        for (auto const& ele : _data)
        {
            if (!ele.paused)
            {
                _log.emplace_back(ele.action);
            }
        }
        
        while (!_log.empty())
        {
            auto action = _log.front();
            _log.pop_front();

            auto const& ele = _data.get_element_from_action(action);
            if (ele.target != nullptr)
            {
                if (ele.target->getReferenceCount() > 1)
                {
                    if (!ele.paused)
                    {
                        action->retain();
                        action->step(dt);

                        if (_data.count(action) > 0 && action->isDone()) // Need to be sure that the action hasn't been removed by some code executed during step previous line
                        {
                            _data.remove_element(ele);
                        }
                        action->release();
                    }
                }
                else
                {
                    _data.remove_element(ele);
                }
            }
        }
    }
}

NS_CC_END

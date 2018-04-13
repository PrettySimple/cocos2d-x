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
    _log.emplace_back(target, action);
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
                _log.emplace_back(nullptr, ele.action);
            }
        }
        
        while (!_log.empty())
        {
            auto record = std::move(_log.front());
            _log.pop_front();

            auto const& ele = _data.get_element_from_action(record.action);
            if (ele.target != nullptr)
            {
                if (ele.target->getReferenceCount() > 1)
                {
                    if (ele.action != nullptr && !ele.paused)
                    {
                        ele.action->step(dt);

                        if (_data.count(record.action) > 0) // Need to be sure that the action hasn't been removed by some code executed during step previous line
                        {
                            if (ele.action->isDone())
                            {
                                _data.remove_element(ele);
                            }
                        }
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

//
//  CCActionManagerData.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 06/02/2018.
//

#include "CCActionManagerData.h"

NS_CC_BEGIN

template <>
ActionManagerData<false>::Element::~Element()
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

        action->onRemove();

        if (manager->_targets.count(target) == 0)
        {
            CC_SAFE_RELEASE(target);
        }
        CC_SAFE_RELEASE(action);
    }
}

template <>
ActionManagerData<true>::Element::~Element()
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

        if (manager->_targets.count(target) == 0)
        {
            CC_SAFE_RELEASE(target);
        }
        CC_SAFE_RELEASE(action);
    }
}

NS_CC_END

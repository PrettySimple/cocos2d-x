//
//  CCActionManagerRecord.h
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#ifndef ACTION_CCACTION_MANAGERRECORD_H
#define ACTION_CCACTION_MANAGERRECORD_H

#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

class Node;
class Action;

struct ActionManagerRecord
{
    Node* target = nullptr;
    Action* action = nullptr;

    ActionManagerRecord() = delete;
    explicit ActionManagerRecord(Node* t = nullptr, Action* a = nullptr);
    ActionManagerRecord(ActionManagerRecord const&) = delete;
    ActionManagerRecord& operator=(ActionManagerRecord const&) = delete;
    ActionManagerRecord(ActionManagerRecord&& other) noexcept;
    ActionManagerRecord& operator=(ActionManagerRecord&& other) noexcept;
    ~ActionManagerRecord();
};

NS_CC_END

#endif // ACTION_CCACTION_MANAGERRECORD_H

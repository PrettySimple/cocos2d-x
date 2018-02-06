//
//  CCActionManagerRecord.h
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#ifndef ACTION_CCACTION_MANAGERRECORD_H
#define ACTION_CCACTION_MANAGERRECORD_H

#include "CCActionManagerOperation.h"
#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

class Node;
class Action;

struct ActionManagerRecord
{
    ActionManagerOperation operation = ActionManagerOperation::UNKNOWN;
    Node* target = nullptr; // weak ref
    Action* action = nullptr; // weak ref
    bool paused = false;
    int tag = -1;
    unsigned int flags = 0;

    ActionManagerRecord() = delete;
    explicit ActionManagerRecord(ActionManagerOperation op, Node* t = nullptr, Action* a = nullptr, bool p = false, int tag = -1, unsigned int flags = 0);
    ActionManagerRecord(ActionManagerRecord const&) = delete;
    ActionManagerRecord& operator=(ActionManagerRecord const&) = delete;
    ActionManagerRecord(ActionManagerRecord&& other) noexcept;
    ActionManagerRecord& operator=(ActionManagerRecord&& other) noexcept;
    ~ActionManagerRecord();
};

NS_CC_END

#endif // ACTION_CCACTION_MANAGERRECORD_H

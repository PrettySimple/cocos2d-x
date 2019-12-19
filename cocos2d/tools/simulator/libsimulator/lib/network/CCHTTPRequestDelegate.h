
#pragma once

#include "cocos2dx_extra.h"

NS_CC_EXTRA_BEGIN

class HTTPRequest;

class HTTPRequestDelegate
{
public:
    virtual void requestFinished(HTTPRequest* request) {}
    virtual void requestFailed(HTTPRequest* request) {}
};

NS_CC_EXTRA_END

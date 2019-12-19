
#pragma once

#include <cocos/math/CCAffineTransform.h>

#include <iosfwd>

NS_CC_BEGIN

struct CC_DLL ResourceData
{
    int type;
    std::string file;
    std::string plist;

    ResourceData()
    {
        type = 0;
        file = "";
        plist = "";
    }

    ResourceData(int iType, std::string sFile, std::string sPlist)
    {
        type = iType;
        file = sFile;
        plist = sPlist;
    }
};

class CC_DLL NodeExtension
{
public:
    NodeExtension();
    ~NodeExtension();

private:
};

NS_CC_END

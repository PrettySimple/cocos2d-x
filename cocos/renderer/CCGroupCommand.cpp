/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.

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

#include "renderer/CCGroupCommand.h"

#include "base/CCDirector.h"
#include "renderer/CCRenderer.h"

#include <unordered_set>

NS_CC_BEGIN

std::unordered_set<std::size_t>& get_unused_ids() noexcept
{
    static std::unordered_set<std::size_t> unusedIDs;
    return unusedIDs;
}

std::size_t get_group_id()
{
    auto& unused_ids = get_unused_ids();

    // Reuse old id
    if (!unused_ids.empty())
    {
        auto it = unused_ids.begin();
        std::size_t groupID = *it;
        unused_ids.erase(it);
        return groupID;
    }

    // Create new ID
    return Director::getInstance()->getRenderer()->createRenderQueue();
}

void recycle_group_id(std::size_t group_id)
{
    if (group_id != std::numeric_limits<std::size_t>::max())
    {
        auto& unused_ids = get_unused_ids();
        unused_ids.emplace(group_id);
    }
}

GroupCommand::GroupCommand()
: RenderCommand(RenderCommand::Type::GROUP_COMMAND)
{
}

GroupCommand::~GroupCommand()
{
    recycle_group_id(_renderQueueID);
}

void GroupCommand::init(float globalOrder)
{
    _globalOrder = globalOrder;
    recycle_group_id(_renderQueueID);
    _renderQueueID = get_group_id();
}

NS_CC_END

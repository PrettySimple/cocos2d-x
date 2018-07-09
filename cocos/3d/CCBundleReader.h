/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.

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

#ifndef CC_3D_BUNDLEREADER_H
#define CC_3D_BUNDLEREADER_H

#include <string>
#include <vector>

#include "base/CCConsole.h"
#include "base/CCRef.h"
#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

/**
 * @addtogroup _3d
 * @{
 */

/**
 * @brief BundleReader is an interface for reading sequence of bytes.
 * @js NA
 * @lua NA
 */
class BundleReader : public cocos2d::Ref
{
public:
    /**
     * Constructor
     */
    BundleReader();

    /**
     * Destructor
     */
    ~BundleReader();

    /**
     * initialise
     * @param buffer The data buffer pointer
     * @param length The data buffer size
     */
    void init(char* buffer, std::size_t length);

    /**
     * Reads an array of elements.
     *
     * @param ptr   The pointer to the memory to copy into.
     *              The available size should be at least bytes.
     * @param size  The size of each element to be read, in bytes.
     * @param count The number of elements to read.
     *
     * @return The number of elements read.
     */
    std::size_t read(void* ptr, std::size_t size, std::size_t count);

    /**
     * Reads a line from the buffer.
     */
    char* readLine(int num, char* line);

    /**
     * Returns true if the end of the buffer has been reached.
     */
    bool eof();

    /**
     * Returns the length of the buffer in bytes.
     */
    std::size_t length();

    /**
     * Returns the position of the file pointer.
     */
    std::size_t tell();

    /**
     * Sets the position of the file pointer.
     */
    bool seek(long int offset, int origin);

    /**
     * Sets the file pointer at the start of the file.
     */
    bool rewind();

    /**
     * read binary typed value.
     */
    template <typename T>
    bool read(T* ptr);
    template <typename T>
    bool readArray(unsigned int* length, std::vector<T>* values);

    /**
     * first read length, then read string text
     */
    std::string readString();

    /**
     * Read the matrix.
     * @note the matrix type must be the 4*4 float matrix
     */
    bool readMatrix(float* m);

private:
    std::size_t _position;
    std::size_t _length;
    char* _buffer;
};

/// @cond

/**
 * template read routines
 */
template <typename T>
inline bool BundleReader::read(T* ptr)
{
    return (read(ptr, sizeof(T), 1) == 1);
}

/**
 * template function to read array of value.
 */
template <typename T>
inline bool BundleReader::readArray(unsigned int* length, std::vector<T>* values)
{
    if (!read(length))
    {
        return false;
    }

    if (*length > 0 && values)
    {
        values->resize(*length);
        if (read(&(*values)[0], sizeof(T), *length) != *length)
        {
            return false;
        }
    }
    return true;
}

/**
 * specialization for char
 */
template <>
inline bool BundleReader::read<char>(char* ptr)
{
    if (read(ptr, sizeof(char), 1) == 1)
    {
        return true;
    }
    else
    {
        *ptr = -1;
        return false;
    }
}

/**
 * specialization for std::string
 */
template <>
inline bool BundleReader::read<std::string>(std::string* ptr)
{
    CCLOG("can not read std::string, use readString() instead");
    return false;
}

/**
 * template function to read array of value.
 */
template <>
inline bool BundleReader::readArray<std::string>(unsigned int* length, std::vector<std::string>* values)
{
    if (!read(length))
    {
        return false;
    }
    values->clear();
    if (*length > 0 && values)
    {
        for (int i = 0; i < static_cast<int>(*length); ++i)
        {
            values->push_back(readString());
        }
    }
    return true;
}

/// @endcond

// end of 3d group
/// @}

NS_CC_END

#endif // CC_3D_BUNDLEREADER_H

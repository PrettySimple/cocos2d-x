/*
 * cocos2d for iPhone: http://www.cocos2d-iphone.org
 *
 * Copyright (c) 2011 Brian Chapados
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

const char* ccPositionTextureColorAlphaTest_frag = R"(
#ifdef GL_ES
precision lowp float;

uniform lowp float CC_alpha_value;

varying lowp vec4 v_fragmentColor;
varying lowp vec2 v_texCoord;
varying mediump vec2 v_mipTexCoord; // DEBUG_TEXTURE_SIZE
#else
uniform float CC_alpha_value;

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
varying vec2 v_mipTexCoord; // DEBUG_TEXTURE_SIZE
#endif

void main()
{
#ifdef DEBUG_TEXTURE_SIZE
    if (CC_Debug == 1)
    {
        vec4 col = texture2D(CC_Texture0, v_texCoord);

        // mimic: glAlphaFunc(GL_GREATER)
        // pass if ( incoming_pixel > CC_alpha_value ) => fail if incoming_pixel <= CC_alpha_value

        if (col.a <= CC_alpha_value)
            discard;

        col *= v_fragmentColor;
        vec4 mip = texture2D(CC_Texture3, v_mipTexCoord);
        gl_FragColor = vec4(mix(col.rgb, mip.rgb, mip.a).rgb, col.a);
    }
    else
#endif
    {
        vec4 texColor = texture2D(CC_Texture0, v_texCoord);

        // mimic: glAlphaFunc(GL_GREATER)
        // pass if ( incoming_pixel > CC_alpha_value ) => fail if incoming_pixel <= CC_alpha_value

        if ( texColor.a <= CC_alpha_value )
            discard;

        gl_FragColor = texColor * v_fragmentColor;
    }
}

)";

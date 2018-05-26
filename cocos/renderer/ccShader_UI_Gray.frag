const char* ccPositionTexture_GrayScale_frag = R"(
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
    vec4 c = texture2D(CC_Texture0, v_texCoord);
    c = v_fragmentColor * c;
    gl_FragColor.xyz = vec3(0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b);

    gl_FragColor.w = c.w;
}

)";

#ifdef GL_ES
precision mediump float;
#endif

// No need to redeclare CC_Texture0 here — it's already defined by Cocos2d-x

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform int brightMode;

void main()
{
    vec4 color = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;

    if (brightMode == 1) {
        // Brighten the color to simulate a hit flash
        float intensity = 1.5;
        vec3 brightColor = min(color.rgb * intensity + vec3(0.5), vec3(1.0));
        gl_FragColor = vec4(brightColor, color.a);
    } else {
        gl_FragColor = color;
    }
}

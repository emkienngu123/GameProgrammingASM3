#ifdef GL_ES
precision mediump float;
#endif

// For desktop GL, you can uncomment this if needed:
// #version 100

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform int chargeMode;
// CC_Texture0 is automatically defined by cocos2d
// uniform sampler2D CC_Texture0;

void main() {
    vec4 texColor = texture2D(CC_Texture0, v_texCoord);

    // simple color tint based on charge level
    if (chargeMode == 1) {
        texColor.rgb *= vec3(1.0, 0.8, 0.8); // slight red tint
    } else if (chargeMode == 2) {
        texColor.rgb *= vec3(0.8, 1.0, 0.8); // slight green tint
    } else if (chargeMode == 3) {
        texColor.rgb *= vec3(0.8, 0.8, 1.0); // slight blue tint
    }

    gl_FragColor = texColor * v_fragmentColor;
}

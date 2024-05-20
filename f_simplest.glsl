#version 330

uniform sampler2D textureMap0;

in vec4 v_normal;
in vec2 v_texCoord;

out vec4 pixelColor;

void main() {
    vec4 kd = texture(textureMap0, v_texCoord);
    pixelColor = kd;
}

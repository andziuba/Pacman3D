#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

in vec4 vertex;
in vec4 normal;
in vec2 texCoord0;

out vec4 v_normal;
out vec2 v_texCoord;

void main() {
    gl_Position = P * V * M * vertex;
    v_normal = normal;
    v_texCoord = texCoord0;
}

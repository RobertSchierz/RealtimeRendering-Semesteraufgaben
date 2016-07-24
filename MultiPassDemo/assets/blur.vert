#version 150

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;

out vec2 texCoord;

void main(void) {
    gl_Position = ciModelViewProjection * ciPosition;
    texCoord = ciTexCoord0;
}

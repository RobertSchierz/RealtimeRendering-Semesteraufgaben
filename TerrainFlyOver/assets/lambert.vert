//
//  Copyright 2016 Henrik Tramberend, Hartmut Schirmacher
//

#version 150

uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

out vec2 TexCoord;
out vec3 Normal;
out highp vec3	NormalWorldSpace;

void main(void) {
    NormalWorldSpace = vec3( ciPosition );

    gl_Position = ciModelViewProjection * ciPosition;
    Normal = ciNormalMatrix * ciNormal;
    TexCoord = ciTexCoord0;
}

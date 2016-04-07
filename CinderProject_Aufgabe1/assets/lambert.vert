//
//  Created by Henrik Tramberend on 07.03.16.
//

#version 150

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

out vec2 TexCoord0;
out vec3 Normal;

void main(void) {
    gl_Position = ciModelViewProjection * ciPosition;
    TexCoord0 = ciTexCoord0;
    Normal = ciNormal;
}

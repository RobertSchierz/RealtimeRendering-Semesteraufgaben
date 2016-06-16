/*
 * Simple Phong Shader
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 * This vertex shader expects a position and normal vector per vertex,
 * and model-view, projection and normal matrices.
 *
 * It transforms the position and normal to eye coordinates and
 * passes them to the fragment shader; it also transforms the vertex
 * position to clip coordinates for the needs of the pipeline.
 *
 */

#version 150

// transformation matrices as provided by Cinder
uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

uniform sampler2D heightMap;
//for fake camera movement
uniform float movSpeed;

uniform vec2 speedVec;

// position and normal vectors
in vec4 ciPosition;
in vec3 ciNormal;

in vec2 ciTexCoord0;

// normal vector mapped into eye coordinates (ec)
out vec4 vertexPositionEC;
out vec3 normalDirEC;

out vec2 TexCoord;

out vec4 vertexPos;

void main(void) {

    //float fog; // amount of fog to apply
    //float fog_coord; // distance for fog calculation...
    //float end_fog = 20;
    //vec4 fog_color = vec4(0.5, 0.5, 0.5);

    float xOffset = speedVec.x;
    float yOffset = speedVec.y;



    vec2 tcc = ciTexCoord0 + vec2(xOffset, yOffset);
    float density = texture(heightMap, tcc).r;
    float height = density * 0.3;

    //float height = texture(heightMap, ciTexCoord - vec2(movSpeed, 0)).r * 0.1;
    vec4 pos = ciPosition + vec4(ciNormal, 0) * height;

    // position to clip coordinates
    gl_Position = ciModelViewProjection * pos;

    // position to eye coordinates
    vertexPositionEC = ciModelView * ciPosition;

    // normal to eye coordinates
    normalDirEC = ciNormalMatrix * ciNormal;

    TexCoord = tcc;

    vertexPos = pos;

    //calculate fog
    //fog_coord = abs(gl_Position.z);
    //fog_coord = clamp( fog_coord, 0.0, end_fog);
    //fog = (end_fog - fog_coord)/end_fog;
    //fog = clamp( fog, 0.0, 1.0);
    //gl_FrontColor = mix(fog_color, gl_Color, fog);
}

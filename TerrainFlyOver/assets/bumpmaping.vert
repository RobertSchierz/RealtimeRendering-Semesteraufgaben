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

uniform bool forward;
uniform bool backward;
uniform bool left;
uniform bool right;

// position and normal vectors
in vec4 ciPosition;
in vec3 ciNormal;

in vec2 ciTexCoord0;

// normal vector mapped into eye coordinates (ec)
out vec4 vertexPositionEC;
out vec3 normalDirEC;

out vec2 TexCoord;

void main(void) {

    //float x = ciTexCoord0.x;
    //if(x < 1.0) {
    //    x -= movSpeed;
    //}else{
    //    x = 0.0;
    //}

    //vec2 movingTexture = vec2(ciTexCoord0.x - movSpeed, ciTexCoord0.y);
    //if(movingTexture.x < 0.0)
    //{
    //    movingTexture.x = 1.0;
    //}

    float xOffset = speedVec.x;
    float yOffset = speedVec.y;



    vec2 tcc = ciTexCoord0 + vec2(xOffset, yOffset);
    float density = texture(heightMap, tcc).r;
    float height = density * 0.2;

    //float height = texture(heightMap, ciTexCoord0 - vec2(movSpeed, 0)).r * 0.1;
    vec4 pos = ciPosition + vec4(ciNormal, 0) * height;

    // position to clip coordinates
    gl_Position = ciModelViewProjection * pos;

    // position to eye coordinates
    vertexPositionEC = ciModelView * ciPosition;

    // normal to eye coordinates
    normalDirEC = ciNormalMatrix * ciNormal;

    TexCoord = tcc;

    //if(movSpeed != 1.0)
    //{
    //    TexCoord = ciTexCoord0;
    //    TexCoord.x *= movSpeed;
    //}else{

    //}
}

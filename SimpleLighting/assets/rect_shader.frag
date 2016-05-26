/*
 * Simple Phong Shader
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 * This fragment shader calculates some direction vectors in eye space
 * and then uses a Phong illum model to calculate output color.
 *
 */

#version 150

// Phong coefficients and exponent
uniform vec3  k_ambient;
uniform vec3  k_diffuse;
uniform vec3  k_specular;
uniform float shininess;

uniform float numberOfShades;
uniform float checkerboard;

// ambient light and point light
uniform vec3 ambientLightColor;
uniform vec3 lightColor;
uniform vec4 lightPositionEC; // in eye coordinates!

// Cinder-provided unoformas
uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

// vertex position from vertex shader, in eye coordinates
in vec4 vertexPositionEC;

// normal vector from vertex shader, in eye coordinates
in vec3 normalDirEC;

// output: color
out vec4 outColor;

uniform vec3 color;
uniform vec3 lightIntensity;

uniform vec3 background;
uniform float radius;
uniform float density;
varying vec2 texCoord;

varying vec3 normalW;
varying vec3 lightDirectionW;
varying vec3 eyeDirectionW;

vec3 toon(vec3 normalDir, vec3 viewDir, vec3 lightDir, vec3 color) {
    // ambient part
    vec3 ambient = k_ambient * ambientLightColor;

    // cos of angle between light and surface.
    float ndotl = dot(normalDir,-lightDir);
    if(ndotl<0.0)
        return vec3(0,0,0); // shadow / facing away from the light source

    float shadeFactor = 1.0 / numberOfShades;

    // diffuse contribution
    vec3 diffuse = k_diffuse * floor(ndotl * numberOfShades) * shadeFactor;

    // reflected light direction = perfect reflection direction
    vec3 r = reflect(lightDir,normalDir);

    // angle between reflection dir and viewing dir
    float rdotv = max( dot(r,viewDir), 0.0);

    // specular contribution
    vec3 specular = k_specular * lightColor * pow(rdotv, shininess);

    if(pow(rdotv, shininess) > 0.01){
        specular = vec3(1, 1, 1);
    }

    // return sum of all contributions
    return color + ambient + diffuse + specular;
}

vec3 calcRects() {
    float modulo = mod(floor(checkerboard * texCoord.x) + floor(checkerboard * texCoord.y), 2.0);

    if (modulo < 1.0) {
        return color;
    } else {
        return background;
    }
}

void main() {
    // normalize normal after projection
    vec3 normalEC = normalize(normalDirEC);

    // calculate light direction (for point light)
    vec3 lightDirEC = normalize(vertexPositionEC - lightPositionEC).xyz;

    // do we use a perspective or an orthogonal projection matrix?
    bool usePerspective = ciProjectionMatrix[2][3] != 0.0;

    // for perspective mode, the viewing direction (in eye coords) points
    // from the vertex to the origin (0,0,0) --> use -ecPosition as direction.
    // for orthogonal mode, the viewing direction is simply (0,0,1)
    vec3 viewdirEC = usePerspective? normalize(-vertexPositionEC.xyz) : vec3(0,0,1);

    // calculate color using phong illumination
    vec3 color = toon(normalEC, viewdirEC, lightDirEC, calcRects());

    // out to frame buffer
    outColor = vec4(color, 1);
}

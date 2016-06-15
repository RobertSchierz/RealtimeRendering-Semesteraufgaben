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

uniform sampler2D normalMap;
uniform sampler2D heightMap;

// ambient light and point light
uniform vec3 ambientLightColor;
uniform vec3 lightColor;
uniform vec4 lightPositionEC; // in eye coordinates!

// Cinder-provided unoformas
uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

uniform float movSpeed;

// vertex position from vertex shader, in eye coordinates
in vec4 vertexPositionEC;

// normal vector from vertex shader, in eye coordinates
in vec3 normalDirEC;

in vec2 TexCoord;

// output: color
out vec4 outColor;

// calculate Phong-style local illumination
vec3 phongIllum(vec3 normalDir, vec3 viewDir, vec3 lightDir)
{
    //normalDir = texture(normalMap, TexCoord).rgb * 2.0 - 1.0;

    // ambient part
    vec3 ambient = k_ambient * ambientLightColor;

    // back face towards viewer?
    //float ndotv = dot(normalDir,viewDir);
    //if(ndotv<0.0)
    //    return vec3(1,0,0);

    // cos of angle between light and surface.
    float ndotl = max(0.0, dot(normalDir,-lightDir));
    //if(ndotl<0.0)
    //    return vec3(0,0,0); // shadow / facing away from the light source

    // diffuse contribution
    vec3 diffuse = k_diffuse * lightColor * ndotl;

    // reflected light direction = perfect reflection direction
    vec3 r = reflect(lightDir,normalDir);

    // angle between reflection dir and viewing dir
    float rdotv = max( dot(r,viewDir), 0.0);

    // specular contribution
    vec3 specular = k_specular * lightColor * pow(rdotv, shininess);

    // return sum of all contributions
    return ambient + diffuse + specular;

}

vec3
calculateFog(vec3 viewDir, vec3 phongIllumColor){
    //distance
    float dist = 0;
    vec3 fogColor = vec3(0.5, 0.5, 0.5);
    float fogFactor = 0;
    float fogEnd = 5;
    float fogStart = 3;


    dist = (gl_FragCoord.z / gl_FragCoord.w);
    //dist = abs(viewDir.z);
    //dist = length(viewDir);
    fogFactor = (fogEnd - dist) / (fogEnd - fogStart);

    // 20 - fog starts; 80 - fog ends
    //fogFactor = (99 - dist)/(80 - 20);
    fogFactor = clamp( fogFactor, 0.0, 1.0 );

    //if you inverse color in glsl mix function you have to
    //put 1.0 - fogFactor
    return mix(fogColor, phongIllumColor, fogFactor);
}

void
main(void)
{
    // normalize normal after projection
    //vec3 normalEC = normalize(normalDirEC);

    //https://www.opengl.org/sdk/docs/tutorials/TyphoonLabs/Chapter_4.pdf
    //calculate normals from normal map
    //texturen werden in einen Bereich von [0,1] in den Fragment Shader geladen
    //da Normalen negative Werte haben können müssen sie mit folgender Berechnung
    //in einen Bereich zwischen [-1,+1] umgewandelt werden

    //backup
    //vec3 normalEC = 2.0 * texture2D(normalMap, TexCoord).rgb - 1.0;
    //vec2 tcc= TexCoord - vec2(movSpeed, 0);
    //float density = texture(normalMap, tcc);

    vec3 normalEC = 2.0 * texture2D(normalMap, TexCoord).rgb - 1.0;

    normalEC = normalize(normalEC);

    // calculate light direction (for point light)
    vec3 lightDirEC = normalize(vertexPositionEC - lightPositionEC).xyz;

    // do we use a perspective or an orthogonal projection matrix?
    bool usePerspective = ciProjectionMatrix[2][3] != 0.0;

    // for perspective mode, the viewing direction (in eye coords) points
    // from the vertex to the origin (0,0,0) --> use -ecPosition as direction.
    // for orthogonal mode, the viewing direction is simply (0,0,1)
    vec3 viewdirEC = usePerspective? normalize(-vertexPositionEC.xyz) : vec3(0,0,1);

    // calculate color using phong illumination
    vec3 color = phongIllum(normalEC, viewdirEC, lightDirEC);

    vec3 interpolatedFogColor = calculateFog(viewdirEC, color);

    // out to frame buffer
    outColor = vec4(interpolatedFogColor, 1);
    //outColor = texture(normalMap, TexCoord);

    // outColor = vec4((vertexPositionEC.xyz+vec3(1))/4,1);
    // return;

}

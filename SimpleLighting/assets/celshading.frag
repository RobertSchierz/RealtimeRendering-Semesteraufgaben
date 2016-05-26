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


// calculate Phong-style local illumination
vec3 celShading(vec3 normalDir, vec3 viewDir, vec3 lightDir)
{
    // ambient part
    vec3 ambient = k_ambient * ambientLightColor;

    // back face towards viewer?
    float ndotv = dot(normalDir,viewDir);
    if(ndotv<0.0)
        return vec3(1,0,0);

    // cos of angle between light and surface.
    float ndotl = dot(normalDir,-lightDir);
    if(ndotl<0.0)
        return vec3(0,0,0); // shadow / facing away from the light source

    float shadeFactor = 1.0 / numberOfShades;

    //if(ndotl < 0) ndotl = 0.25;
      //if(ndotl > 0 && ndotl <= 0.25) ndotl = 0.25;
      //if(ndotl > 0.25 && ndotl <= 0.5) ndotl = 0.5;
      //if(ndotl > 0.5 && ndotl <= 0.75) ndotl = 0.75;
      //if(ndotl > 0.75 && ndotl <= 1.0) ndotl = 1;

    // diffuse contribution
    vec3 diffuse = k_diffuse * floor(ndotl * numberOfShades) * shadeFactor;

    //vec3 diffuse = k_diffuse * floor(acos(ndotl) * numberOfShades) * shadeFactor;

    // reflected light direction = perfect reflection direction
    vec3 r = reflect(lightDir,normalDir);

    // angle between reflection dir and viewing dir
    float rdotv = max( dot(r,viewDir), 0.0);

    // specular contribution
    vec3 specular = k_specular * lightColor * pow(rdotv, shininess);

    if(pow(rdotv, shininess) > 0.01){
        specular = vec3(1, 1, 1);
    }

    //float specMask = (pow(rdotv, shininess) > 0.4) ? 1 : 0;

    // return sum of all contributions
    return  ambient + diffuse + specular;
}

//calculate the shades for cel shading
float makeShades(float ndotl){
    float shade = 1.0 / 4.0;

    for(float i = 0.0; i < 4.0; i++){
        if(ndotl<shade*i){
            return i*shade;
        }
    }
    return 1.0;
}

void
main(void)
{
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
    vec3 color = celShading(normalEC, viewdirEC, lightDirEC);

    // out to frame buffer
    outColor = vec4(color, 1);

    // outColor = vec4((vertexPositionEC.xyz+vec3(1))/4,1);
    // return;

}

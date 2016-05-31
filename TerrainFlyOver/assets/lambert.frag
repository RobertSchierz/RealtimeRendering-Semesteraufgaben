//
//  Copyright 2016 Henrik Tramberend, Hartmut Schirmacher
//

#version 150

uniform vec3 ka;
uniform vec3 kd;
uniform sampler2D map_ka;
uniform sampler2D map_kd;

in vec2 TexCoord;
in vec3 Normal;

out vec4 oColor;

vec3 ambient = ka.x < 0.0 ? -ka * texture(map_ka, TexCoord).rgb : ka;
vec3 diffuse = kd.x < 0.0 ? -kd * texture(map_kd, TexCoord).rgb : kd;

void
main(void)
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(vec3(1, 1, 3));

    vec3 normal = 2.0 * texture(map_ka, TexCoord).rgb - 1.0;
    normal = normalize (normal);

    //oColor = vec4((ambient + diffuse) * dot(N, L), 1);

    // funzt und zeigt die textur der normal map :)
    //oColor = texture(map_ka, TexCoord);
    oColor = vec4(normal, 1);

    // Uncomment to use normal directions as color.
    // vec3 nColor = (normalize(Normal) + 1) / 2;
    // oColor = vec4(nColor, 1);
}

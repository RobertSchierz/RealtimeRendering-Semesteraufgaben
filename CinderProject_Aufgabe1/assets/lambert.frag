//
//  Created by Henrik Tramberend on 07.03.16.
//

#version 150

uniform vec3 ka;
uniform vec3 kd;
uniform sampler2D map_ka;
uniform sampler2D map_kd;

in vec2 TexCoord0;
in vec3 Normal;

out vec4 oColor;

vec3 ambient = ka.x < 0.0 ? -ka * texture(map_ka, TexCoord0).rgb : ka;
vec3 diffuse = kd.x < 0.0 ? -kd * texture(map_kd, TexCoord0).rgb : kd;

void
main(void)
{

    oColor = vec4(ambient + diffuse, 1);

    // Uncomment to use normal directions as color
    // vec3 nColor = (normalize(Normal) + 1) / 2;
    // oColor = vec4(nColor, 1);
}

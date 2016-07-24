#version 150

uniform sampler2D tex;
uniform sampler2D depthTex;
uniform int resolution;

in vec2 texCoord;

out vec4 outColor;

const int samples = 10;
const float center = (float(samples) - 1.0) / 2.0;
const float weight = 1.0f/(samples*samples);

void main(void)
{
	vec3 color = vec3(0,0,0);
	float size = float(samples) / resolution;
	vec3 depth = texture(depthTex, texCoord).rgb;
	vec3 screenTex = texture(tex, texCoord).rgb;

	if(depth.x > 0.98){
		for(int x = 0; x < samples; x++){
			for(int y = 0; y < samples; y++){
				vec2 tc = texCoord + (vec2(x, y) - center) * size;
				color += texture(tex, tc).rgb;
			}
		}
		color *= weight;
		outColor = vec4(color, 1);
	} else {
		outColor = vec4(screenTex, 1.0);
	}
}

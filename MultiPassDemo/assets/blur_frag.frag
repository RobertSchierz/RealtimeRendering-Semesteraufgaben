#version 110

uniform sampler2D	tex;
uniform sampler2D	depthTex;
uniform vec2		sampleOffset;


in vec2 texCoord;

out vec4 outColor;

float weights[21];

void main()
{
	weights[0] = 0.009167927656011385;
	weights[1] = 0.014053461291849008;
	weights[2] = 0.020595286319257878;
	weights[3] = 0.028855245532226279;
	weights[4] = 0.038650411513543079;
	weights[5] = 0.049494378859311142;
	weights[6] = 0.060594058578763078;
	weights[7] = 0.070921288047096992;
	weights[8] = 0.079358891804948081;
	weights[9] = 0.084895951965930902;
	weights[10] = 0.086826196862124602;
	weights[11] = 0.084895951965930902;
	weights[12] = 0.079358891804948081;
	weights[13] = 0.070921288047096992;
	weights[14] = 0.060594058578763092;
	weights[15] = 0.049494378859311121;
	weights[16] = 0.038650411513543079;
	weights[17] = 0.028855245532226279;
	weights[18] = 0.020595286319257885;
	weights[19] = 0.014053461291849008;
	weights[20] = 0.009167927656011385;


	vec3 sum = vec3( 0.0, 0.0, 0.0 );
	vec2 offset = vec2( 0.0, 0.0 );
	vec2 baseOffset = -10.0 * sampleOffset;
	vec3 depth = texture2D(depthTex, texCoord).rgb;
	vec3 screenTex = texture2D(tex, texCoord).rgb;

	if(depth.x > 0.995){
		for( int s = 0; s < 21; ++s ) {
			sum += texture2D( tex, texCoord.st + baseOffset + offset ).rgb * weights[s];
			offset += sampleOffset;
		}

		outColor.rgb = sum;
		outColor.a = 1.0;
		//outColor = vec4(depth, 1.0);
	}else{
		outColor = vec4(screenTex, 1.0);
		//outColor = vec4(depth, 1.0);
	}
}

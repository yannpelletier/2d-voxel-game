#version 140

varying vec2 fragmentUV;

uniform sampler2D lightSampler;

vec4 lightSamplerColor;

varying vec2 v_blurTexCoords[28];
vec2 v_modifiedTexCoords[28];

out vec4 color;

void main() {
	lightSamplerColor = vec4(0.0);
	
	for(int i = 0; i < 14; ++i){
		if(v_blurTexCoords[i].x < 0){
			v_modifiedTexCoords[i].x = fragmentUV.x;
			v_modifiedTexCoords[i].y = v_blurTexCoords[i].y;
		}
		else{
			v_modifiedTexCoords[i] = v_blurTexCoords[i];
		}
	}
	
	for(int i = 14; i < 28; ++i){
		if(v_blurTexCoords[i].x > 1){
			v_modifiedTexCoords[i].x = fragmentUV.x;
			v_modifiedTexCoords[i].y = v_blurTexCoords[i].y;
		}
		else{
			v_modifiedTexCoords[i] = v_blurTexCoords[i];
		}
	}



	lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 0]) * 0.001607;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 1]) * 0.002753;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 2]) * 0.004531;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 3]) * 0.007167;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 4]) * 0.010893;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 5]) * 0.015908;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 6]) * 0.022325;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 7]) * 0.030106;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 8]) * 0.039011;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[ 9]) * 0.048575;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[10]) * 0.05812;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[11]) * 0.066823;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[12]) * 0.073827;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[13]) * 0.078376;
	lightSamplerColor += texture2D(lightSampler, fragmentUV         ) * 0.079954;
	lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[14]) * 0.078376;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[15]) * 0.073827;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[16]) * 0.066823;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[17]) * 0.05812;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[18]) * 0.048575;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[19]) * 0.039011;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[20]) * 0.030106;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[21]) * 0.022325;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[22]) * 0.015908;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[23]) * 0.010893;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[24]) * 0.007167;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[25]) * 0.004531;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[26]) * 0.002753;
    lightSamplerColor += texture2D(lightSampler, v_modifiedTexCoords[27]) * 0.001607;
	
	color = lightSamplerColor;

}
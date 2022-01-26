 #version 140

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

uniform sampler2D lightSampler;
uniform sampler2D tileSampler;

uniform vec4 ambientLightColor;

vec4 lightSamplerColor;
vec4 tileSamplerColor;

out vec4 color;

void main() {
	tileSamplerColor = texture2D(tileSampler, fragmentUV);
		
	if(tileSamplerColor == vec4(0,0,0,0)){
		if(texture2D(lightSampler, fragmentUV) != vec4(0,0,0,0)){
			lightSamplerColor.r = max(texture2D(lightSampler, fragmentUV).r, ambientLightColor.r);
			lightSamplerColor.g = max(texture2D(lightSampler, fragmentUV).g, ambientLightColor.g);
			lightSamplerColor.b = max(texture2D(lightSampler, fragmentUV).b, ambientLightColor.b);
			lightSamplerColor.a = max(texture2D(lightSampler, fragmentUV).a, ambientLightColor.a);
		}
		else{
			lightSamplerColor = ambientLightColor;
		}
	}
	else{
		lightSamplerColor = texture2D(lightSampler, fragmentUV);
	}

	color = lightSamplerColor;
}
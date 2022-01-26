 #version 140

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

uniform sampler2D colorSampler;
uniform sampler2D lightSampler;
uniform sampler2D tileSampler;

uniform vec4 ambientLightColor;

vec4 colorSamplerColor;
vec4 lightSamplerColor;

out vec4 color;

void main() {
    colorSamplerColor = texture2D(colorSampler, fragmentUV);
	lightSamplerColor = texture2D(lightSampler, fragmentUV);

	color = colorSamplerColor * lightSamplerColor;
}
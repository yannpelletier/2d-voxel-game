 #version 330 core
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

uniform sampler2D colorSampler;
uniform sampler2D lightSampler;

out vec4 color;

void main() {
    vec4 colorSamplerColor = texture2D(colorSampler, fragmentUV);
    vec4 lightSamplerColor = texture2D(lightSampler, fragmentUV);

    color = colorSamplerColor;
}
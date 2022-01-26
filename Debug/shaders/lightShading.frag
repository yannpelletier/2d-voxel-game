 #version 330 core
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color;

void main() {
    float distance = length(fragmentUV);

    color = vec4(fragmentColor.rgb, fragmentColor.a / distance);
}
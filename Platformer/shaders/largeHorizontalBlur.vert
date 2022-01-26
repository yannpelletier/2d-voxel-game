#version 140
//The vertex shader operates on each vertex

//input data from the VBO. Each vertex is 2 floats
in vec2 vertexUV;

out vec2 fragmentUV;

varying vec2 v_blurTexCoords[28];

void main() {
    //Set the x,y position on the screen
    gl_Position.x = -1.0 + (vertexUV.x * 2);
    gl_Position.y = -1.0 + (vertexUV.y * 2);
    
    //Indicate that the coordinates are normalized
    gl_Position.w = 1.0;

	gl_Position.z =0.0;

    
    fragmentUV = vertexUV;
    v_blurTexCoords[ 0] = fragmentUV + vec2(-0.112, 0.0);
    v_blurTexCoords[ 1] = fragmentUV + vec2(-0.104, 0.0);
    v_blurTexCoords[ 2] = fragmentUV + vec2(-0.096, 0.0);
    v_blurTexCoords[ 3] = fragmentUV + vec2(-0.088, 0.0);
    v_blurTexCoords[ 4] = fragmentUV + vec2(-0.080, 0.0);
    v_blurTexCoords[ 5] = fragmentUV + vec2(-0.072, 0.0);
    v_blurTexCoords[ 6] = fragmentUV + vec2(-0.064, 0.0);
    v_blurTexCoords[ 7] = fragmentUV + vec2(-0.056, 0.0);
    v_blurTexCoords[ 8] = fragmentUV + vec2(-0.048, 0.0);
    v_blurTexCoords[ 9] = fragmentUV + vec2(-0.040, 0.0);
    v_blurTexCoords[10] = fragmentUV + vec2(-0.032, 0.0);
    v_blurTexCoords[11] = fragmentUV + vec2(-0.024, 0.0);
    v_blurTexCoords[12] = fragmentUV + vec2(-0.016, 0.0);
    v_blurTexCoords[13] = fragmentUV + vec2(-0.008, 0.0);
	v_blurTexCoords[14] = fragmentUV + vec2(0.008, 0.0);
    v_blurTexCoords[15] = fragmentUV + vec2(0.016, 0.0);
    v_blurTexCoords[16] = fragmentUV + vec2(0.024, 0.0);
    v_blurTexCoords[17] = fragmentUV + vec2(0.032, 0.0);
    v_blurTexCoords[18] = fragmentUV + vec2(0.040, 0.0);
    v_blurTexCoords[19] = fragmentUV + vec2(0.048, 0.0);
    v_blurTexCoords[20] = fragmentUV + vec2(0.056, 0.0);
    v_blurTexCoords[21] = fragmentUV + vec2(0.064, 0.0);
    v_blurTexCoords[22] = fragmentUV + vec2(0.072, 0.0);
    v_blurTexCoords[23] = fragmentUV + vec2(0.080, 0.0);
    v_blurTexCoords[24] = fragmentUV + vec2(0.088, 0.0);
    v_blurTexCoords[25] = fragmentUV + vec2(0.096, 0.0);
    v_blurTexCoords[26] = fragmentUV + vec2(0.104, 0.0);
    v_blurTexCoords[27] = fragmentUV + vec2(0.112, 0.0);
}
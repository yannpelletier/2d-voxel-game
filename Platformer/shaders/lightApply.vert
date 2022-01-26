 #version 140
//The vertex shader operates on each vertex

//input data from the VBO. Each vertex is 2 floats
in vec2 vertexPosition;
in vec4 vertexColor;
in vec2 vertexUV;

uniform vec4 ambientLightColor;

out vec2 fragmentUV;

void main() {
    //Set the x,y position on the screen
    gl_Position.x = -1.0 + (vertexUV.x * 2);
    gl_Position.y = -1.0 + (vertexUV.y * 2);
    
    //Indicate that the coordinates are normalized
    gl_Position.w = 1.0;
	
	gl_Position.z =0.0;
    
    fragmentUV = vertexUV;
}
 #version 330 core
//The vertex shader operates on each vertex

//input data from the VBO. Each vertex is 2 floats
in vec2 vertexPosition;
in vec4 vertexColor;
in vec2 vertexUV;

out vec2 fragmentPosition;
out vec4 fragmentColor;
out vec2 fragmentUV;

void main() {
    //Set the x,y position on the screen
    gl_Position.x = -1.0 + vertexUV.x;
    gl_Position.y = vertexUV.y;
    //the z position is zero since we are in 2D
    gl_Position.z = 0.0;
    
    //Indicate that the coordinates are normalized
    gl_Position.w = 1.0;
    
    fragmentPosition = vertexPosition;
    
    fragmentColor = vertexColor;
    
    fragmentUV.x = 1.0;
    fragmentUV.y = 1.0;
}
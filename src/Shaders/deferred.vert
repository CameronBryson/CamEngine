#version 460 core
layout(location = 0) in vec3 aPos;    
layout(location = 1) in vec2 aTexCoords; // Next 2 floats are texture coords

// Output to fragment shader
out vec2 TexCoord;

void main()
{
    // Pass texture coordinates to fragment shader
    TexCoord = aTexCoords;
    
    // Output position directly (already in NDC)
    gl_Position = vec4(aPos, 1.0);
}

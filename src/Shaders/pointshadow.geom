#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];
out vec4 FragPos;

void main()
{
    // For each face of the cubemap
    for(int face = 0; face < 6; face++)
    {
        for(int i = 0; i < 3; i++)
        {
            // Tell the pipeline which cubemap face to write to
            gl_Layer = face;

            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

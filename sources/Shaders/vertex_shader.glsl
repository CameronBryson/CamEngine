#version 330 core
in vec3 position;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main() {
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position,1.0f);

}
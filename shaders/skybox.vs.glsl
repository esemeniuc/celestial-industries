#version 330 
uniform mat4 mvp;
in vec3 position;
out vec3 texture_coordinates;

void main()
{
    texture_coordinates = position;
    gl_Position = mvp * vec4(position, 1);
}
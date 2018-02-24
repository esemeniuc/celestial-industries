#version 330 
uniform mat4 viewProjection;
uniform mat4 model;
uniform vec3 cameraPosition;
in vec3 position;
out vec3 texture_coordinates;

void main()
{
    vec3 worldPosition = vec3(model * vec4(position, 1.0)) + cameraPosition;
    texture_coordinates = position;
    gl_Position = viewProjection * vec4(worldPosition, 1.0);
}
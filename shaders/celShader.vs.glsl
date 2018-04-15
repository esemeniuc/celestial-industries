#version 410 
//uniforms
uniform int modelIndex;
uniform mat4 vp;
uniform mat4 viewMatrix;

layout(std140) uniform InstancesData {
        uint stride;
        mat4 modelMatrices[1000];
} instances;

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;
in vec3 in_normal;

// Passed to fragment shader
out vec2 vs_texcoord;
out vec3 vs_normal;
out vec3 viewDirection;

void main()
{
	vs_texcoord = in_texcoord;
    uint idx = gl_InstanceID*instances.stride+modelIndex;
    mat4 model = instances.modelMatrices[idx];
	viewDirection = -1.0 * normalize(vec3(viewMatrix * model * vec4(in_position, 1.0)));
	vs_normal = vec3( viewMatrix * model * vec4(in_normal, 0.0));
	gl_Position = (vp*model) * vec4(in_position, 1.0);
}
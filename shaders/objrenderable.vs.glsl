#version 440 
// Input attributes
in vec3 in_position;
in vec2 in_texcoord;
in vec3 in_normal;

// Passed to fragment shader
out vec2 vs_texcoord;
out vec3 vs_normal;
out vec3 vs_position;
out vec3 vs_lightVector;


// Application data
uniform mat4 vp;

layout(std140, binding = 2) uniform InstancesData {
        uint stride;
        float padding1;
        float padding2;
        float padding3;
        mat4 modelMatrices[65536];
};

uniform int modelIndex;


void main()
{
    uint idx = gl_InstanceID*stride+modelIndex;
    mat4 model = modelMatrices[idx];
	gl_Position = (vp*model) * vec4(in_position, 1);
	vs_texcoord = in_texcoord;
	vs_normal = in_normal;
	vs_position = gl_Position.xyz;
	vs_lightVector = vec3(0.49, 0.79, 0.49);
}
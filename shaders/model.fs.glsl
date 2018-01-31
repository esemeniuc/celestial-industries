#version 330
// From vertex shader
in vec3 vs_color;
in vec3 vs_texcoord;
in float vs_hasDiffuseMap;

uniform sampler2D diffuseMapSampler;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	if(vs_hasDiffuseMap > 0.5){
		color = vec4(texture(diffuseMapSampler, vec2(vs_texcoord.x, vs_texcoord.y)).rgb, 1.0);
	} else {
		color = vec4(vs_color, 1.0);
	}
}

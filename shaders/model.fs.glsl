#version 330
// From vertex shader
in vec3 vs_color;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(vs_color, 1.0);
}

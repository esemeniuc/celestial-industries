#version 410
// uniforms
uniform mat4 viewMatrix;
uniform vec3 directionalLight;
uniform sampler2D diffuseMapSampler;

// From vertex shader
in vec2 vs_texcoord;
in vec3 vs_normal;
in vec3 viewDirection;

layout(std140) uniform MaterialInfo {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    bool hasDiffuseMap;   
}  mat;

// used for light calculations
float diffuseComponent;
float specularComponent;
vec3 normalizedLightDirection;
vec3 normalizedNormal;
vec3 halfwayVector;
float silhouetteLineThickness = 0.18;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	normalizedLightDirection = normalize(vec3(viewMatrix * vec4(directionalLight, 0.0)));
	normalizedNormal = normalize(vs_normal);
	halfwayVector = normalize(normalizedLightDirection + normalize(viewDirection));
	
	//AMBIENT	
	vec3 light_AMB = vec3(mat.ambient);
	
	//DIFFUSE
	vec3 diffuseColor;
	if(mat.hasDiffuseMap){
		diffuseColor = vec3(mat.diffuse*texture(diffuseMapSampler, vs_texcoord));
	} else {
		diffuseColor = vec3(mat.diffuse);
	}
	diffuseComponent = max(0.0, dot(normalizedLightDirection, normalizedNormal));
	// vec3 light_DFF = vec3(0.5, 0.5, 0.5);
	vec3 light_DFF = diffuseComponent * diffuseColor;

	//SPECULAR	
	specularComponent = pow(max(0.0, dot(halfwayVector, normalizedNormal)), 2.0); // shininess = 10
	vec3 light_SPC = vec3(specularComponent * mat.specular);	
	
	vec3 TOTAL = light_AMB + light_DFF + light_SPC;
	vec4 resultingColor = vec4(TOTAL,0.0);
	float lightIntensity = length(TOTAL); 

	// used to create silhouette
	//if (dot (normalize(viewDirection), normalizedNormal) <= silhouetteLineThickness){
	//	resultingColor = vec4(0.0, 0.0, 0.0, 0.0);
	//}

	//color = floor(4.0 * lightIntensity * resultingColor)/4.0;
	color = lightIntensity * resultingColor;  
}
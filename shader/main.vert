#version 430 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vColor;
layout(location=3) in vec2 vTexCoord;

out vec3 Color;
out vec2 TexCoord;
out vec3 Position_worldspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec3 Normal_cameraspace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

void main() {
	vec3 vPosition_modelspace = vec3(vPosition.x,
					 -vPosition.y,
					 vPosition.z);
	Color = vColor;
	TexCoord = vTexCoord;
	Position_worldspace = (M * vec4(vPosition_modelspace, 1.)).xyz;
	gl_Position = MVP * vec4(vPosition_modelspace, 1.);
	vec3 vPosition_cameraspace = (V * M *
				      vec4(vPosition_modelspace, 1.)).xyz;
	EyeDirection_cameraspace = vec3(0., 0., 0.) - vPosition_cameraspace;
	vec3 LightPosition_cameraspace = (V * M * vec4(5., 10., 5., 1.)).xyz;
	LightDirection_cameraspace = (LightPosition_cameraspace +
				      EyeDirection_cameraspace);
	Normal_cameraspace = (V * M *
			      vec4(vNormal.x, -vNormal.y, vNormal.z, 0.)).xyz;
}

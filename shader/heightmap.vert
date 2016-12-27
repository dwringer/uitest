#version 430 core
#define TERRAIN_SCALE 1.

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vColor;
layout(location=3) in vec2 vTexCoord;

out vec3 Color;
out vec2 TexCoord;
//out vec3 Position_worldspace;
//out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec3 Normal_cameraspace;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 tiMV;
uniform vec3 LightPos_cameraspace;

uniform sampler2D tex;
uniform sampler2D hmap;


void main() {
	Color = vColor;
	TexCoord = vTexCoord;
	vec3 vPosition_modelspace =
		vec3(vPosition.x,
		     -vPosition.y,
		     vPosition.z);
	vec4 vpm4h =
		vec4(vPosition_modelspace, 1.) +
		(vec4(vPosition_modelspace, 1.) *
		 texture(hmap, TexCoord) *
		 vec4(TERRAIN_SCALE * .0013916));
	vec3 vPosition_cameraspace =
		(MV * vpm4h).xyz;
	//Position_worldspace = (M * vpm4h).xyz;
	vec3 EyeDirection_cameraspace =
		vec3(0., 0., 0.) -
		vPosition_cameraspace;
	LightDirection_cameraspace =
		(LightPos_cameraspace +
		 EyeDirection_cameraspace);
	Normal_cameraspace =
		(tiMV *
		 vec4(vNormal.x,
		      -vNormal.y,
		      vNormal.z, 0.)).xyz;
	gl_Position = MVP * vpm4h;
}

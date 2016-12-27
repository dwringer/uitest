#version 430 core

in vec3 Color;
in vec2 TexCoord;
//in vec3 Position_worldspace;
//in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Normal_cameraspace;

out vec4 fColor;

uniform sampler2D tex;
uniform sampler2D hmap;

void main() {
	vec3 n = normalize(
                Normal_cameraspace);
	vec3 l = normalize(
		LightDirection_cameraspace);
	float cosTheta = clamp(dot(n, l), 0., 1.);
	fColor = vec4((texture(tex, TexCoord) *
		       vec4(Color, 1.0) *
		       cosTheta).rgb, 1.);
}

#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 fColor;

void main()
{
	vec3 vPosition1 = vPosition / vPosition.z;
	gl_Position = vec4(vPosition1.xy, 0.0, 1.0);
	gl_PointSize = 4;
	fColor = vColor;
}
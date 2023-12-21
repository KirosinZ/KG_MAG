#version 460

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 fColor;

void main()
{
	gl_Position = vPosition;
	gl_PointSize = 4;
	fColor = vColor;
}
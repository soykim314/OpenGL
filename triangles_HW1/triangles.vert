#version 430
in vec4 vPosition;
in vec4 vColor;
out vec4 fColor;
layout(location=1) uniform mat4 T;
layout(location=2) uniform int mode;

void main()
{
	gl_Position = T* vPosition;
	switch(mode)
	{
		case 0:
			fColor = vColor;
			break;
		case 1:
			fColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
	}
}
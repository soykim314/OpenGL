#version 430
in vec4 vPosition;
in vec4 vColor;
out vec4 fColor;
layout(location=1) uniform mat4 T;
layout(location=2) uniform int mode;

//�� ��Ʈ���� ����
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;


//input�� vertices��ġ�� �� ����.
void main()
{
	gl_Position = P*V*M*T*vPosition;
	switch(mode)
	{
		case 0:
			fColor = vColor;
			break;
		case 1:
			fColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 2:
			fColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		case 3:
			fColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 4:
			fColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
			break;
		case 5:
			fColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}
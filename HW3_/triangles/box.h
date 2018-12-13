#pragma once

#include <vector>
#include <GL/glew.h>

void get_box_3d(std::vector<GLfloat>& p, std::vector<GLfloat>* color = NULL)
{
	static const GLfloat box_vertices[] = {
		 0.5f, 0.5f,-0.5f, // triangle 1 : begin
		-0.5f,-0.5f,-0.5f,
		-0.5f, 0.5f,-0.5f, // triangle 2 : end
		 0.5f, 0.5f,-0.5f,
		 0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,

		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f,-0.5f,

		 0.5f,-0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,
		 0.5f,-0.5f,-0.5f,
		 0.5f,-0.5f, 0.5f,
		-0.5f,-0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,

		-0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f, 0.5f,
		 0.5f,-0.5f, 0.5f,
		 0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		 0.5f,-0.5f, 0.5f,

		 0.5f, 0.5f, 0.5f,
		 0.5f,-0.5f,-0.5f,
		 0.5f, 0.5f,-0.5f,
		 0.5f,-0.5f,-0.5f,
		 0.5f, 0.5f, 0.5f,
		 0.5f,-0.5f, 0.5f,

		 0.5f, 0.5f, 0.5f,
		 0.5f, 0.5f,-0.5f,
		-0.5f, 0.5f,-0.5f,
		 0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f,-0.5f,
		-0.5f, 0.5f, 0.5f
	};

	p.resize(sizeof(box_vertices) / sizeof(GLfloat));
	memcpy(p.data(), box_vertices, sizeof(box_vertices));

	if (color)
	{
		std::vector<GLfloat>& c = *color;
		c.resize(p.size());
		for (size_t i = 0; i < c.size() / 18; ++i)
		{
#if 1
			GLfloat s[3] = {
				0.5f * rand() / RAND_MAX + 0.5f,
				0.5f * rand() / RAND_MAX + 0.5f,
				0.5f * rand() / RAND_MAX + 0.5f
			};
			for (size_t j = 0; j < 6; ++j) {
				c[i * 18 + j * 3 + 0] = s[0];
				c[i * 18 + j * 3 + 1] = s[1];
				c[i * 18 + j * 3 + 2] = s[2];
			}
#else
			switch (i) {
			case 0:
			for (size_t j = 0; j < 6; ++j) {
			c[i * 18 + j * 3 + 0] = 1;
			c[i * 18 + j * 3 + 1] = 0;
			c[i * 18 + j * 3 + 2] = 0;
			}
			break;
			case 1:
			for (size_t j = 0; j < 6; ++j) {
			c[i * 18 + j * 3 + 0] = 0;
			c[i * 18 + j * 3 + 1] = 1;
			c[i * 18 + j * 3 + 2] = 0;
			}
			break;
			case 2:
			for (size_t j = 0; j < 6; ++j) {
			c[i * 18 + j * 3 + 0] = 0;
			c[i * 18 + j * 3 + 1] = 0;
			c[i * 18 + j * 3 + 2] = 1;
			}
			break;
			case 3:
			for (size_t j = 0; j < 6; ++j) {
			c[i * 18 + j * 3 + 0] = 1;
			c[i * 18 + j * 3 + 1] = 1;
			c[i * 18 + j * 3 + 2] = 0;
			}
			break;
			case 4:
			for (size_t j = 0; j < 6; ++j) {
			c[i * 18 + j * 3 + 0] = 0;
			c[i * 18 + j * 3 + 1] = 1;
			c[i * 18 + j * 3 + 2] = 1;
			}
			break;
			case 5:
			for (size_t j = 0; j < 6; ++j) {
			c[i * 18 + j * 3 + 0] = 1;
			c[i * 18 + j * 3 + 1] = 0;
			c[i * 18 + j * 3 + 2] = 1;
			}
			break;
			}
#endif
		}
	}
}
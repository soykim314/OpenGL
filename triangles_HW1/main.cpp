#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "LoadShaders.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include "box.h"
#include "sphere.h"

using namespace glm;


typedef std::vector<GLfloat> GLvec;

int drawing_mode = 0;
int show_wireframe = 0;
int show_point = 0;
int program;


GLuint vertexarray[5];
//콘시작
GLuint vao_cone, buffs[2];
GLvec vtx_pos, vtx_clrs;
GLuint active_vao = 0;
std::vector<size_t> idx_side, idx_bottom;
//콘 끝

//큐브시작
GLuint vao;
GLuint vbo[2];
GLvec vertices[5];
GLvec colors[5];
//큐브끝

//실린더시작
//GLuint cy_vtx_pos;
std::vector<size_t> side_idx, top_idx, bottom_idx;
GLuint cyl[2];
//실린더끝


//구시작
GLuint spy[2];
//구끝

//도넛시작
//벡터에 벡터로 받아야 한다. SIDE_IDX를 사용.
GLuint torus[2];
std::vector<std::vector<size_t>> sides_idx;



int build_program()
{
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },{ GL_FRAGMENT_SHADER, "triangles.frag" },{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders(shaders);
	glUseProgram(program);
	return program;
}

void init();
void keyboard(unsigned char key, int x, int y);
void display();


void main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutCreateWindow(argv[0]);
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
}


#define FPUSH_VTX3(p,vx,vy,vz)\
do { \
p.push_back(vx); \
p.push_back(vy); \
p.push_back(vz); \
} while(0)

#define FSET_VTX3(vx,vy,vz, valx,valy,valz)\
do { \
vx=(float)(valx); \
vy=(float)(valy); \
vz=(float)(valz); \
} while(0)


#define FPUSH_VTX3_AT(p,i,vx,vy,vz)\
do { \
size_t i3 = 3*(i); \
p[i3+0] = (float)(vx); \
p[i3+1] = (float)(vy); \
p[i3+2] = (float)(vz); \
} while(0)



void get_cone_3d(std::vector<GLfloat>& p,
	std::vector<size_t>& side_idx, std::vector<size_t>& bottom_idx,
	GLfloat radius, GLfloat height, GLint n)
{
	GLfloat half_height = height / 2;
	GLfloat theta, x, z;
	FPUSH_VTX3(p, 0, half_height, 0); // top vertex
	side_idx.push_back(0);
	for (int i = 0; i <= n; ++i) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3(p, x, -half_height, z);
		side_idx.push_back(i + 1);
		bottom_idx.push_back(n + 2 - i);
	}
	FPUSH_VTX3(p, 0, -half_height, 0); // bottom-center vertex
	bottom_idx.push_back(1);
}

void get_cylinder_3d(
	std::vector<GLfloat>& p,
	std::vector<size_t>& side_idx,
	std::vector<size_t>& top_idx,
	std::vector<size_t>& bottom_idx,
	GLfloat radius,
	GLfloat height,
	GLint n)
{
	GLfloat half_height = height / 2;
	GLfloat theta, x, z;
	p.resize(3 * (2 * n + 4));
	FPUSH_VTX3_AT(p, 0, 0, half_height, 0);
	top_idx.push_back(0);
	bottom_idx.push_back(2 * n + 3);
	for (int i = 0; i <= n; ++i) {
		theta = (GLfloat)(2.0 * M_PI * i / n);
		x = radius * sin(theta);
		z = radius * cos(theta);
		FPUSH_VTX3_AT(p, 2 * i + 1, x, half_height, z);
		FPUSH_VTX3_AT(p, 2 * i + 2, x, -half_height, z);
		side_idx.push_back(2 * i + 1);
		side_idx.push_back(2 * i + 2);
		top_idx.push_back(2 * i + 1);
		bottom_idx.push_back(2 * n + 2 - 2 * i);
	}
	FPUSH_VTX3_AT(p, 2 * n + 3, 0, -half_height, 0);
}



void get_torus_3d(
	std::vector<GLfloat>& p,
	std::vector<std::vector<size_t>>& sides_idx,
	GLfloat r0, GLfloat r1, GLint longs, GLint lats)
{
	GLfloat theta, gamma, x_ij, y_i, z_ij;

	sides_idx.resize(lats);
	for (int i = 0; i < lats; ++i) {
		theta = (GLfloat)(2.0*M_PI*i / lats);
		GLfloat x_i, y_i, z_i;

		x_i = (r0 + r1) * sin(theta);
		z_i = (r0 + r1) * cos(theta);

		for (int j = 0; j <= longs; ++j) {
			gamma = (GLfloat)(2.0*M_PI*j / longs);
			GLfloat l_j, tx_ij, tz_ij;

			l_j = r1 * cos(gamma);
			tx_ij = l_j * sin(theta);
			tz_ij = l_j * cos(theta);
			
			x_ij = x_i + tx_ij;
			y_i = r1 * sin(gamma);
			z_ij = z_i + tz_ij;

			FPUSH_VTX3(p, x_ij, y_i, z_ij);

			sides_idx[i].push_back((longs + 1)*i + j);
			sides_idx[i].push_back((longs + 1)*(i + 1) + j);

		}

	}
	for (int j = 0; j <= longs; ++j) {
		FPUSH_VTX3(p, p[j * 3 + 0], p[j * 3 + 1], p[j * 3 + 2]);
	}
}



void get_color_3d_by_pos(GLvec& c, GLvec& p, GLfloat offset = 0)
{
	GLfloat max_val[3] = { -INFINITY,-INFINITY,-INFINITY };
	GLfloat min_val[3] = { INFINITY, INFINITY, INFINITY };
	int n = (int)(p.size() / 3);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < 3; ++j) {
			GLfloat val = p[i * 3 + j];
			if (max_val[j] < val) max_val[j] = val;
			else if (min_val[j] > val) min_val[j] = val;
		}
	}
	GLfloat width[3] = {
		max_val[0] - min_val[0],
		max_val[1] - min_val[1],
		max_val[2] - min_val[2]
	};
	c.resize(p.size());
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < 3; ++j) {
			int k = i * 3 + j;
			c[k] = std::fminf((p[k] - min_val[j]) / width[j] + offset, 1.0f);
		}
	}
}


void bind_buffer(GLint buffer, GLvec&vec, int program, const GLchar* attri_name, GLint attri_size)
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vec.size(), vec.data(), GL_STATIC_DRAW);
	GLuint location = glGetAttribLocation(program, attri_name);
	glVertexAttribPointer(location, attri_size, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(location);
}


void drawing_primitive(GLuint active_vao, mat4 matrix)
{
	glUniformMatrix4fv(1, 1, GL_FALSE, value_ptr(matrix));

	switch (active_vao) {
		//
		//		//glBindVertexArray(vertexarray[active_vao]);

	case 0:
		glBindVertexArray(vertexarray[0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		break;
	case 1:
		glBindVertexArray(vertexarray[1]);
		glDrawArrays(GL_TRIANGLES, 0, vertices[1].size());
		break;
	case 2:
		glBindVertexArray(vertexarray[2]);
		glDrawElements(GL_TRIANGLE_FAN, idx_side.size(), GL_UNSIGNED_INT, idx_side.data());
		glDrawElements(GL_TRIANGLE_FAN, idx_bottom.size(), GL_UNSIGNED_INT, idx_bottom.data());
		break;
	case 3:
		glBindVertexArray(vertexarray[3]);
		glDrawElements(GL_TRIANGLE_STRIP, side_idx.size(), GL_UNSIGNED_INT, side_idx.data());
		glDrawElements(GL_TRIANGLE_FAN, top_idx.size(), GL_UNSIGNED_INT, top_idx.data());
		glDrawElements(GL_TRIANGLE_FAN, bottom_idx.size(), GL_UNSIGNED_INT, bottom_idx.data());
		break;
	case 4:
		glBindVertexArray(vertexarray[4]);
		for (auto iter = sides_idx.begin(); iter != sides_idx.end(); ++iter)
		{
			glDrawElements(GL_TRIANGLE_STRIP, (*iter).size(), GL_UNSIGNED_INT, (*iter).data());
		}
		break;
	}
}


void init()
{
	program = build_program();

	//큐브시작
	get_box_3d(vertices[0]);
	get_color_3d_by_pos(colors[0], vertices[0]);

	glGenVertexArrays(1, vertexarray+0);
	glBindVertexArray(vertexarray[0]);
	glGenBuffers(2, vbo);
	bind_buffer(vbo[0], vertices[0], program, "vPosition", 3);
	bind_buffer(vbo[1], colors[0], program, "vColor", 3);
	//큐브 끝	
	
	
	//콘 시작
	get_cone_3d(vertices[2], idx_side, idx_bottom, 0.5, 1.0, 6);
	get_color_3d_by_pos(colors[2], vertices[2]);

	glGenVertexArrays(1, vertexarray+2);
	glBindVertexArray(vertexarray[2]);

	//const GLchar* attri_name[2] = { "vPosition", "vColor" };
	//GLvec* vtx_list[2] = { &vtx_pos, &vtx_clrs };

	glGenBuffers(2, buffs);
	bind_buffer(buffs[0], vertices[2], program, "vPosition", 3);
	bind_buffer(buffs[1], colors[2], program, "vColor", 3);

	/*
	for (int i = 0; i < 2; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, buffs[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vtx_list[i]->size(), vtx_list[i]->data(),
			GL_STATIC_DRAW);
		GLint location = glGetAttribLocation(program, attri_name[i]);
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(location);
	}*/
	//콘 끝


	//실린더시작

	get_cylinder_3d(vertices[3], side_idx, top_idx, bottom_idx, 0.5, 1.0, 10);
	get_color_3d_by_pos(colors[3], vertices[3]);
	glGenVertexArrays(1, vertexarray+3);
	glBindVertexArray(vertexarray[3]);

	/*const GLchar* cy_attri_name[2] = { "vPosition", "vColor" };
	GLvec* cy_vtx_list[2] = { &vtx_pos, &vtx_clrs };*/

	glGenBuffers(2, cyl);
	bind_buffer(cyl[0], vertices[3], program, "vPosition", 3);
	bind_buffer(cyl[1], colors[3], program, "vColor", 3);

	//for (int i = 0; i < 2; ++i) {
	//	glBindBuffer(GL_ARRAY_BUFFER, buffs[i]);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vtx_list[i]->size(), vtx_list[i]->data(),
	//		GL_STATIC_DRAW);
	//	GLint location = glGetAttribLocation(program, attri_name[i]);
	//	glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//	glEnableVertexAttribArray(location);
	//}
	//실린더끝

	//구시작
	get_sphere_3d(vertices[1], 0.5, 30, 30);
	get_color_3d_by_pos(colors[1], vertices[1]);
	glGenVertexArrays(1, vertexarray+1);
	glBindVertexArray(vertexarray[1]);

	glGenBuffers(2, spy);
	bind_buffer(spy[0], vertices[1], program, "vPosition", 3);
	bind_buffer(spy[1], colors[1], program, "vColor", 3);
	//구끝


	//도넛시작. 
	get_torus_3d(vertices[4], sides_idx, 0.2, 0.2, 10, 10);
	get_color_3d_by_pos(colors[4], vertices[4]);
	glGenVertexArrays(1, vertexarray + 4);
	glBindVertexArray(vertexarray[4]);

	glGenBuffers(2, torus);

	bind_buffer(torus[0], vertices[4], program, "vPosition", 3);
	bind_buffer(torus[1], colors[4], program, "vColor", 3);
	//도넛끝

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}


void train()
{
		

	GLfloat theta = 0.001f* clock();


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glPolygonOffset(1, 1);

	GLuint wire_location = glGetUniformLocation(program, "mode");
	glUniform1i(wire_location, 0);
	mat4 T(1.0f);
	T = rotate(T, theta, vec3(-1.0f, 1.0f, 0.0f));
	T = scale(T, vec3(0.5f));
	GLuint location = glGetUniformLocation(program, "T");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(T));
	glEnableVertexAttribArray(location);
	glDisable(GL_POLYGON_OFFSET_FILL);


	
	mat4 Group(2.0f);
	Group = rotate(Group, theta, vec3(0.0f, 1.0f, 0.0f));
	Group = translate(Group, vec3(0.0f, -0.5f, 0.0f));
		
	//바퀴1
	mat4 frontwheel1(1.0f);
	frontwheel1 = translate(frontwheel1, vec3(-0.3f, 0.05f, -0.3f));
	frontwheel1 = rotate(frontwheel1, theta, vec3(0.0f, 0.0f, 1.0f)); //바퀴돌도록
	frontwheel1 = rotate(frontwheel1, radians(90.0f), vec3(1.0f, 0.0f, 0.0f)); //바퀴뒤집기
	frontwheel1 = scale(frontwheel1, vec3(0.3f));
	glUniform1i(wire_location, 0);
	drawing_primitive(4, Group*frontwheel1);



	//바퀴2
	mat4 frontwheel2(1.0f);
	frontwheel2 = translate(frontwheel2, vec3(0.3f, 0.05f, -0.3f));
	frontwheel2 = rotate(frontwheel2, theta, vec3(0.0f, 0.0f, 1.0f)); //바퀴돌도록
	frontwheel2 = rotate(frontwheel2, radians(90.0f), vec3(1.0f, 0.0f, 0.0f)); //바퀴뒤집기
	frontwheel2 = scale(frontwheel2, vec3(0.3f));

	glUniform1i(wire_location, 0);
	drawing_primitive(4, Group*frontwheel2);

	//바퀴3
	mat4 frontwheel3(1.0f);
	frontwheel3 = translate(frontwheel3, vec3(0.3f, 0.05f, 0.3f));
	frontwheel3 = rotate(frontwheel3, theta, vec3(0.0f, 0.0f, 1.0f)); //바퀴돌도록
	frontwheel3 = rotate(frontwheel3, radians(90.0f), vec3(1.0f, 0.0f, 0.0f)); //바퀴뒤집기
	frontwheel3 = scale(frontwheel3, vec3(0.3f));

	glUniform1i(wire_location, 0);
	drawing_primitive(4, Group*frontwheel3);

	////바퀴4
	mat4 frontwheel4(1.0f);
	frontwheel4 = translate(frontwheel4, vec3(-0.3f, 0.05f, 0.3f));
	frontwheel4 = rotate(frontwheel4, theta, vec3(0.0f, 0.0f, 1.0f)); //바퀴돌도록
	frontwheel4 = rotate(frontwheel4, radians(90.0f), vec3(1.0f, 0.0f, 0.0f)); //바퀴뒤집기
	frontwheel4 = scale(frontwheel4, vec3(0.3f));

	glUniform1i(wire_location, 0);
	drawing_primitive(4, Group*frontwheel4);

	//몸1
	mat4 body1(1.0f);
	body1 = translate(body1, vec3(0.0f, 0.2f, 0.0f)); 
	body1 = rotate(body1, radians(90.0f), vec3(1.0f, 0.0f, 0.0f)); 
	body1 = scale(body1, vec3(1.0f,0.48f,0.3f));

	glUniform1i(wire_location, 0);
	drawing_primitive(0, Group*body1);
	
	//몸2
	mat4 body2(1.0f);
	body2 = translate(body2, vec3(-0.25f, 0.55f, 0.0f));
	body2 = rotate(body2, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	body2 = scale(body2, vec3(0.5f, 0.48f, 0.4f));

	glUniform1i(wire_location, 0);
	drawing_primitive(0, Group*body2);

	//콘
	mat4 cone(1.0f);
	cone = translate(cone, vec3(-0.25f, 0.9f, 0.0f));
	cone = rotate(cone, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	cone = scale(cone, vec3(0.8f, 0.3f,0.8f));

	glUniform1i(wire_location, 0);
	drawing_primitive(2, Group*cone);

	//실린더
	mat4 cylin(1.0f);
	cylin = translate(cylin, vec3(0.0f, 0.35f, 0.0f));
	cylin = rotate(cylin, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	cylin = scale(cylin, vec3(0.4f, 0.4f, 0.4f));

	glUniform1i(wire_location, 0);
	drawing_primitive(3, Group*cylin);

	//코1
	mat4 cir1(1.0f);
	cir1 = translate(cir1, vec3(0.5f, 0.2f, 0.1f));
	cir1 = rotate(cir1, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	cir1 = scale(cir1, vec3(0.05f, 0.05f, 0.05f));

	glUniform1i(wire_location, 0);
	drawing_primitive(1, Group*cir1);

	//코2
	mat4 cir2(1.0f);
	cir2 = translate(cir2, vec3(0.5f, 0.2f, -0.1f));
	cir2 = rotate(cir2, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	cir2 = scale(cir2, vec3(0.05f, 0.05f, 0.05f));

	glUniform1i(wire_location, 0);
	drawing_primitive(1, Group*cir2);

	//바퀴실1
	mat4 wc1(1.0f);
	wc1 = translate(wc1, vec3(0.3f, 0.05f, 0.0f));
	wc1 = rotate(wc1, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	wc1 = scale(wc1, vec3(0.1f, 0.6f, 0.1f));

	glUniform1i(wire_location, 0);
	drawing_primitive(3, Group*wc1);


	//바퀴실2
	mat4 wc2(1.0f);
	wc2 = translate(wc2, vec3(-0.3f, 0.05f, 0.0f));
	wc2 = rotate(wc2, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	wc2 = scale(wc2, vec3(0.1f, 0.6f, 0.1f));

	glUniform1i(wire_location, 0);
	drawing_primitive(3, Group*wc2);

	if (show_wireframe)
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform1i(wire_location, 1);
		drawing_primitive(4, Group*frontwheel1);
		drawing_primitive(4, Group*frontwheel2);
		drawing_primitive(4, Group*frontwheel3);
		drawing_primitive(4, Group*frontwheel4);
		drawing_primitive(0, Group*body1);
		drawing_primitive(0, Group*body2);
		drawing_primitive(2, Group*cone);
		drawing_primitive(3, Group*cylin);
		drawing_primitive(1, Group*cir1);
		drawing_primitive(1, Group*cir2);
		drawing_primitive(3, Group*wc1);
		drawing_primitive(3, Group*wc2);
		

	}	
		
	if (show_point)
	{
		glPointSize(3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glUniform1i(wire_location, 1);
		drawing_primitive(4, Group*frontwheel1);
		drawing_primitive(4, Group*frontwheel2);
		drawing_primitive(4, Group*frontwheel3);
		drawing_primitive(4, Group*frontwheel4);
		drawing_primitive(0, Group*body1);
		drawing_primitive(0, Group*body2);
		drawing_primitive(2, Group*cone);
		drawing_primitive(3, Group*cylin);
		drawing_primitive(1, Group*cir1);
		drawing_primitive(1, Group*cir2);
		drawing_primitive(3, Group*wc1);
		drawing_primitive(3, Group*wc2);
	}

}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1': active_vao = 0; glutPostRedisplay(); break;
	case '2': active_vao = 1; glutPostRedisplay(); break;
	case '3': active_vao = 2; glutPostRedisplay(); break;
	case '4': active_vao = 3; glutPostRedisplay(); break;
	case '5': active_vao = 4; glutPostRedisplay(); break;
	case '6': active_vao = 5; glutPostRedisplay(); break;
	case 'w': show_wireframe = !show_wireframe;
		 glutPostRedisplay(); break;
	case 'v': show_point = !show_point;
		 glutPostRedisplay(); break;
	}
}



void display()
{
	using namespace glm;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	//int location = glGetUniformLocation(program, "show_wireframe");
	//glUniform1i(location, drawing_mode);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glPolygonOffset(1, 1);
	

	using namespace glm;
	GLfloat theta = 0.001f * clock();
	mat4 T(1.0f);
	T = rotate(T, theta, vec3(-1.0f, 1.0f, 0.0f));
	T = scale(T, vec3(0.5f));
	GLuint location = glGetUniformLocation(program, "T");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(T));
	glEnableVertexAttribArray(location);


	GLuint wire_location = glGetUniformLocation(program, "mode");
	glUniform1i(wire_location, 0);

	if (active_vao < 5) {
		drawing_primitive(active_vao, T);
	}
	else{
		train();
	}


	glBindVertexArray(vertexarray[active_vao]);

	//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	
	drawing_primitive(active_vao, T);

	
	glDisable(GL_POLYGON_OFFSET_FILL);

	if (show_wireframe)
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform1i(wire_location, 1);
		drawing_primitive(active_vao, T);

	}

	if (show_point)
	{
		glPointSize(3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glUniform1i(wire_location, 1);
		drawing_primitive(active_vao, T);
	}
	

	glFlush();
	glutPostRedisplay();
}



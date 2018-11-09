#include "pch.h"
#include "TriangleRenderer.h"
#include <string>


TriangleRenderer::TriangleRenderer()
{
	const std::string vs = R"(
		attribute vec4 vPosition;
		void main()
		{
			gl_Position = vPosition;
		})";
	const std::string fs = R"(
		precision mediump float;
		void main()
		{
			gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		}
	)";

	program = CompileProgram(vs, fs);

	if (!program)
		throw Exception::CreateException(E_FAIL, "Failed to compile shader language units");

	glClearColor(0.0f, 0.0f, 0.0f, 0.f);

}


TriangleRenderer::~TriangleRenderer()
{
	if (program != 0)
	{
		glDeleteProgram(program);
		program = 0;
	}

}

void sggraphs::TriangleRenderer::Draw()
{
	GLfloat vertices[] =
	{
		0.f, .5f, 0.f,
		-.5f, -.5f, .0f,
		.5f, -.5f, .0f
	};

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Use the program object
	glUseProgram(program);

	// Load the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 3);



}

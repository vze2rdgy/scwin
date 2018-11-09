#include "pch.h"
#include "SimpleRenderer.h"
#include "MathHelper.h"
#include <vector>
#include <iostream>
#include <fstream>

using namespace Platform;
using namespace sggraphs;

#define STRING(s) #s


SimpleRenderer::SimpleRenderer() : drawCount(0)
{
	std::string vs =
		STRING
		(
			uniform mat4 uModelMatrix;
			uniform mat4 uViewMatrix;
			uniform mat4 uProjMatrix;
			attribute vec4 aPosition;
			attribute vec4 aColor;
			varying vec4 vColor;
			void main()
			{
				gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
				vColor = aColor;
			}
		);
	//vs.insert(0, "#version 300 es ");

	std::string fs =
		STRING
		(
			precision mediump float;
			varying vec4 vColor;
			void main()
			{
				gl_FragColor = vColor;
			}
		);
	//fs.insert(0, "#version 300 es ");

	program = CompileProgram(vs, fs);
	positionAttribLocation = glGetAttribLocation(program, "aPosition");
	colorAttribLocation = glGetAttribLocation(program, "aColor");
	modelUniformLocation = glGetUniformLocation(program, "uModelMatrix");
	viewUniformLocation = glGetUniformLocation(program, "uViewMatrix");
	projUniformLocation = glGetUniformLocation(program, "uProjMatrix");

	// Then set up the cube geometry.

	GLfloat vertexPositions[] =
	{
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
	};

	glGenBuffers(1, &vertexPositionBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	GLfloat vertexColors[] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
	};

	glGenBuffers(1, &vertexColorBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_STATIC_DRAW);

	short indices[] =
	{
		0, 1, 2, // -x
		1, 3, 2,

		4, 6, 5, // +x
		5, 6, 7,

		0, 5, 1, // -y
		0, 4, 5,

		2, 7, 6, // +y
		2, 3, 7,

		0, 6, 4, // -z
		0, 2, 6,

		1, 7, 3, // +z
		1, 5, 7,
	};



	glGenBuffers(1, &indexBuffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}



SimpleRenderer::~SimpleRenderer()
{
	if (program != 0)
	{
		glDeleteProgram(program);
		program = 0;
	}

	if (vertexPositionBuffer != 0)
	{
		glDeleteBuffers(1, &vertexPositionBuffer);
		vertexPositionBuffer = 0;
	}

	if (vertexColorBuffer != 0)
	{
		glDeleteBuffers(1, &vertexColorBuffer);
		vertexColorBuffer = 0;
	}

	if (indexBuffer != 0)
	{
		glDeleteBuffers(1, &indexBuffer);
		indexBuffer = 0;
	}
}

void sggraphs::SimpleRenderer::Draw()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (program == 0)
		return;
	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
	glEnableVertexAttribArray(positionAttribLocation);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);
	glEnableVertexAttribArray(colorAttribLocation);
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	Matrix4 modelMatrix = SimpleModelMatrix((float)drawCount / 50.0f);
	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &(modelMatrix.m[0][0]));

	Matrix4 viewMatrix = SimpleViewMatrix();
	glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, &(viewMatrix.m[0][0]));
	Matrix4 projectionMatrix = SimpleProjectionMatrix(float(winWidth) / float(winHeight));

	glUniformMatrix4fv(projUniformLocation, 1, GL_FALSE, &(projectionMatrix.m[0][0]));

	// Draw 36 indices: six faces, two triangles per face, 3 indices per triangle
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, (6 * 2) * 3, GL_UNSIGNED_SHORT, 0);

	drawCount += 1;
}


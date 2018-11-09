#pragma once

#include "RendererBase.h"

namespace sggraphs
{

	class SimpleRenderer : public RendererBase
	{
	private:
		GLint positionAttribLocation;
		GLint colorAttribLocation;
		GLint modelUniformLocation;
		GLint viewUniformLocation;
		GLint projUniformLocation;
		GLuint vertexPositionBuffer;
		GLuint vertexColorBuffer;
		GLuint indexBuffer;

		int drawCount;

	public:
		SimpleRenderer();
		~SimpleRenderer();

		void Draw() override;

	private:


	};

}
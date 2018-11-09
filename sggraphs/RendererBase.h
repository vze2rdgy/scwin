#pragma once

using namespace Platform;

namespace sggraphs
{

	class RendererBase
	{
	protected:
		GLuint program;
		GLsizei winWidth;
		GLsizei winHeight;

	public:
		RendererBase();
		~RendererBase();

		virtual void UpdateWindowSize(GLsizei width, GLsizei height);
		virtual void Draw() = 0;

	};


	GLuint CompileShader(GLenum type, const std::string& source);
	GLuint CompileProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

}
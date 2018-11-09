#include "pch.h"
#include "RendererBase.h"

#include <vector>
#include <iostream>
#include <fstream>

using namespace Platform;
using namespace sggraphs;

#define STRING(s) #s


namespace sggraphs
{

	GLuint CompileShader(GLenum type, const std::string& source)
	{
		GLuint shader = glCreateShader(type);
		const char* sourceArray[1] = { source.c_str() };
		glShaderSource(shader, 1, sourceArray, NULL);
		glCompileShader(shader);

		GLint compileResult;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == 0)
		{
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::vector<GLchar> infoLog(infoLogLength);
			glGetShaderInfoLog(shader, (GLsizei)infoLog.size(), NULL, infoLog.data());
			std::wstring errorMessage = std::wstring(L"Shader compilation failed.");
			errorMessage += std::wstring(infoLog.begin(), infoLog.end());

			throw Exception::CreateException(E_FAIL, ref new String(errorMessage.c_str()));
		}

		return shader;
	}

	GLuint CompileProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
	{
		GLuint program = glCreateProgram();
		if (program == 0)
			throw Exception::CreateException(E_FAIL, "Program creation failed.");

		GLuint vs = sggraphs::CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
		GLuint fs = sggraphs::CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

		if (vs == 0 || fs == 0)
		{
			glDeleteShader(fs);
			glDeleteShader(vs);
			glDeleteProgram(program);

			return 0;
		}

		glAttachShader(program, vs);
		glDeleteShader(vs);
		glAttachShader(program, fs);
		glDeleteShader(fs);

		glLinkProgram(program);

		GLint linkStatus;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == 0)
		{
			GLint infoLogLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::vector<GLchar> infoLog(infoLogLength);
			glGetProgramInfoLog(program, (GLsizei)infoLog.size(), NULL, infoLog.data());

			std::wstring errorMessage = std::wstring(L"Program link failed.");
			errorMessage += std::wstring(infoLog.begin(), infoLog.end());

			throw Exception::CreateException(E_FAIL, ref new String(errorMessage.c_str()));

		}

		return program;
	}

}

RendererBase::RendererBase() : winWidth(0), winHeight(0)
{

}


RendererBase::~RendererBase()
{
}



void sggraphs::RendererBase::UpdateWindowSize(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
	winWidth = width;
	winHeight = height;
}

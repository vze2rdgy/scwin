#pragma once
#include "RendererBase.h"

namespace sggraphs
{
	class TriangleRenderer : public RendererBase
	{
	private:
		GLuint program;

	public:
		TriangleRenderer();
		~TriangleRenderer();

		void Draw() override;

	};

}
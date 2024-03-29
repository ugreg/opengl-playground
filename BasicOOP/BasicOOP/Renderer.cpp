#include "Renderer.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCALL(x) MyGLClearError();\
	x;\
	ASSERT(MyGLLogCall(#x, __FILE__, __LINE__))

void MyGLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool MyGLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "\n[!!! OpenGL ERROR!!!] (0x0" << std::hex << error << ")\n"
			<< function << "\n" << file << ":" << line << "\n";
		return false;
	}
	return true;
}

Renderer::Renderer()
{
	m_increment_color = 0.05f;
}

void Renderer::clear() const
{
	MyGLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::draw(Shader& myshader, VertexArray& vertex_array, IndexBuffer& index_buffer) const
{
	// Before this we UNBIND
	// Generally don't need to call unbind since going to bind something new anyway
	// * * * * * * * * * * * * *
	// bind everything back
	// * * * * * * * * * * * * *

	myshader.bind();
	vertex_array.bind();
	index_buffer.bind();

	// DRAW what's bound
	// use below for index buffer
	// triple AAA games use me :) the right way to draw triple AAA mario is below
	// want to draw mario? one vertex buffer for all his vertex data, then many index buffers to draw each pieces of the model
	// marios boots will be a different material than his hat
	// the shader program is bound to the current opengl context and used in this draw elements calls
	unsigned int index_buffer_object_size = index_buffer.get_num_of_indicies();
	glDrawElements(GL_TRIANGLES, index_buffer_object_size, GL_UNSIGNED_INT, nullptr); // nullptr since we bind buffers using glGenBuffers
}

void Renderer::draw_glow_quad(Shader& myshader, VertexArray& vertex_array, IndexBuffer& index_buffer, float& color, std::string uniform, std::string mvp_uniform)
{
	myshader.bind();
	vertex_array.bind();
	index_buffer.bind();		

	if (color > 1.0f) {
		m_increment_color = -0.05f;
	}
	else if (color < 0.0f) {
		m_increment_color = 0.05f;
	}
	color += m_increment_color;

	myshader.set_uniform4f(uniform, 0.5f, 0.2f, color, 1.0f);	

	unsigned int index_buffer_object_size = index_buffer.get_num_of_indicies();
	glDrawElements(GL_TRIANGLES, index_buffer_object_size, GL_UNSIGNED_INT, nullptr);
}
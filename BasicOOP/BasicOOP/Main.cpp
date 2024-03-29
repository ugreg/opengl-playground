#include <iostream>
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "imgui_v_1_60\imgui.h"
#include "imgui_v_1_60\opengl3_example\imgui_impl_glfw_gl3.h"

#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Test.h"
#include "TestClearColor.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"


static void vert_and_proj_setup(float* tri_vert_buffer, glm::mat4 &proj_matrix, bool one_to_one_pixel_mapping)
{
	if (one_to_one_pixel_mapping) {
		
		tri_vert_buffer[0] = 100.0f;
		tri_vert_buffer[1] = 100.0f;
		tri_vert_buffer[2] = 0.0f;
		tri_vert_buffer[3] = 0.0f;
		
		tri_vert_buffer[4] = 200.0f;
		tri_vert_buffer[5] = 100.0f;
		tri_vert_buffer[6] = 1.0f;
		tri_vert_buffer[7] = 0.0f;

		tri_vert_buffer[8] = 200.0f;
		tri_vert_buffer[9] = 200.0f;
		tri_vert_buffer[10] = 1.0f;
		tri_vert_buffer[11] = 1.0f;

		tri_vert_buffer[12] = 100.0f;
		tri_vert_buffer[13] = 200.0f;
		tri_vert_buffer[14] = 0.0f;
		tri_vert_buffer[15] = 1.0f;

		// center object around (0,0)
		tri_vert_buffer[0] = -30.0f;
		tri_vert_buffer[1] = -30.0f;
		tri_vert_buffer[2] = 0.0f;
		tri_vert_buffer[3] = 0.0f;

		tri_vert_buffer[4] = 30.0f;
		tri_vert_buffer[5] = -30.0f;
		tri_vert_buffer[6] = 1.0f;
		tri_vert_buffer[7] = 0.0f;

		tri_vert_buffer[8] = 30.0f;
		tri_vert_buffer[9] = 30.0f;
		tri_vert_buffer[10] = 1.0f;
		tri_vert_buffer[11] = 1.0f;

		tri_vert_buffer[12] = -30.0f;
		tri_vert_buffer[13] = 30.0f;
		tri_vert_buffer[14] = 0.0f;
		tri_vert_buffer[15] = 1.0f;
		
		float left_right = 960.0f;
		float bottom_top = 540.0f;
		proj_matrix
			= glm::ortho(0.0f, left_right, 0.0f, bottom_top, -1.0f, 1.0f);
		
	} 
	else {
		tri_vert_buffer[0] = -0.5f;
		tri_vert_buffer[1] = -0.5f;
		tri_vert_buffer[2] = 0.0f;
		tri_vert_buffer[3] = 0.0f;

		tri_vert_buffer[4] = 0.5f;
		tri_vert_buffer[5] = -0.5f;
		tri_vert_buffer[6] = 1.0f;
		tri_vert_buffer[7] = 0.0f;

		tri_vert_buffer[8] = 0.5f;
		tri_vert_buffer[9] = 0.5f;
		tri_vert_buffer[10] = 1.0f;
		tri_vert_buffer[11] = 1.0f;

		tri_vert_buffer[12] = -0.5f;
		tri_vert_buffer[13] = 0.5f;
		tri_vert_buffer[14] = 0.0f;
		tri_vert_buffer[15] = 1.0f;

		// 4:3 ratio works well for a 640 by 480 window
		float left_right = 4.0f;
		float bottom_top = 3.0f;
		proj_matrix
			= glm::ortho(-1 * left_right, left_right, -1 * bottom_top, bottom_top, -1.0f, 1.0f);
	}

	// simulate projections on the cpu
	// for breakpoint use ONLY no effect to code
	// great test to show how the orthographic_projection_matrix is used to convert a vertex to the device space we define
	// in this case we define it as between -1.0 and 1.0
	/*
	glm::mat4 orthographic_projection_matrix
		= glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

	glm::vec4 vertex_position(100.0f, 100.0f, 0.0f, 1.0f);
	glm::vec4 result = orthographic_projection_matrix * vertex_position;
	*/
}

static void animate(Shader shader, std::string mvp_uniform, float left_right, float bottom_top, float increment_bottom_top, float increment_left_right, bool one_to_one_pixel_mapping, glm::mat4 orthographic_projection_matrix, float blue, float increment_color)
{
	if (blue > 1.0f) {
		increment_color = -0.05f;
	}
	else if (blue < 0.0f) {
		increment_color = 0.05f;
	}
	blue += increment_color;

	// ANIMATION
	// grow and shrink badge on 640 by 480 window
	// should be handled in renderer so should blue coloring
	if (!one_to_one_pixel_mapping) {
		orthographic_projection_matrix = glm::ortho(-1 * left_right, left_right, -1 * bottom_top, bottom_top, -1.0f, 1.0f);
		shader.set_uniform_mat4f(mvp_uniform, orthographic_projection_matrix);
		if (left_right > 8.0f && bottom_top > 6.0f) {
			increment_left_right = -1.0f;
			increment_bottom_top = -0.75f;
		}
		else if (left_right == 4.0f && bottom_top == 3.0f) {
			increment_left_right = 1.0f;
			increment_bottom_top = 0.75f;
		}

		left_right = left_right + increment_left_right;
		bottom_top = bottom_top + increment_bottom_top;	
	}
}

int main(void)
{
	std::cout << "Object Oriented OpenGL\n";

	GLFWwindow* window;
	if (!glfwInit()) { return -1; }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int window_width = 960;
	int window_height = 540;
	bool one_to_one_pixel_mapping = true;

	/*int window_width = 640;
	int window_height = 480;
	bool one_to_one_pixel_mapping = false;	*/

	window = glfwCreateWindow(window_width, window_height, "Object Oriented OpenGL", NULL, NULL);
	// glfwSetWindowSize(window, 640, 480); appears to bug out AVOID for now
	// if window is already 960 x 540
	if (!window) {
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	glfwSwapInterval(16);

	if (GLEW_OK != glewInit()) { std::cout << "ERROR initializing OpenGL GLEW\n"; }

	std::cout << "Rendering...\nOpenGL version " << glGetString(GL_VERSION)
		<< "\nGL Extension Wrangler version " << glewGetString(GLEW_VERSION)
		<< '\n';

	// scope prevents glfwTerminate() infinite loop
	// must call glfwTerminate() BEFORE exiting main scope while buffers still in stack mem
	{
		const int num_verticies = 4;
		const int elements_per_vert = 4;
		const int buffer_object_size = num_verticies * elements_per_vert;
		const int index_buffer_object_size = 6;
		
		float tri_vert_buffer[buffer_object_size] = {
			0.0f, 0.0f, 0.0f, 0.0f, // 0
			0.0f, 0.0f, 0.0f, 0.0f, // 1
			0.0f, 0.0f, 0.0f, 0.0f, // 2
			0.0f, 0.0f, 0.0f, 0.0f  // 3
		};

		float left_right = 0.0f;
		float bottom_top = 0.0f;
		glm::mat4 orthographic_projection_matrix
			= glm::ortho(0.0f, left_right, 0.0f, bottom_top, -1.0f, 1.0f);
		// camera position
		// tested only for
		/*int window_width = 960;
		int window_height = 540;*/
		// simulate moving camera to the right, result is moves all scene objects left
		// glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
		// to make object centered around (0,0)
		glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		// simulate moving on screen object
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
		
		vert_and_proj_setup(tri_vert_buffer, orthographic_projection_matrix, one_to_one_pixel_mapping);		
		
		// highly depdendent on memory layout
		glm::mat4 model_view_projection_matrix 
			= orthographic_projection_matrix * view_matrix * model_matrix; // KEEP THIS ORDER

		unsigned int tri_indicies[index_buffer_object_size] = {
			0, 1, 2,
			2, 3, 0
		};

		MyGLCall(glEnable(GL_BLEND));
		MyGLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray vertexArray;
		VertexBuffer vertexBuffer(tri_vert_buffer, buffer_object_size * sizeof(float));

		int num_vertex_coordindates = 2;
		int num_texture_sample_coordinates = 2;
		VertexBufferLayout vertexBufferLayout;
		vertexBufferLayout.push<float>(num_vertex_coordindates);
		vertexBufferLayout.push<float>(num_texture_sample_coordinates);
		vertexArray.add_buffer(vertexBuffer, vertexBufferLayout);
		
		IndexBuffer indexBuffer(tri_indicies, index_buffer_object_size);				

		Shader shader("UniformTexture.shader");
		shader.bind();
		std::string color_uniform = "u_color";		
		shader.set_uniform4f(color_uniform, 0.5f, 0.2f, 0.7f, 1.0f);
		std::string mvp_uniform = "u_modelview_projection_matrix";
		// used prior to using model_view_projection_matrix
		// shader.set_uniform_mat4f(mvp_uniform, orthographic_projection_matrix);
		// shader.set_uniform_mat4f(mvp_uniform, model_view_projection_matrix);
		
		Texture texture("Group_Focus_Badge.png");
		int texture_slot = 0;
		texture.bind(texture_slot);
		std::string texture_uniform = "u_texture";
		shader.set_uniform1i(texture_uniform, texture_slot);
			
		vertexArray.unbind();		
		vertexBuffer.unbind();
		indexBuffer.unbind();
		shader.unbind();

		Renderer renderer;

		float blue = 0.7f;

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		// TESTING
		/*
		test::TestClearColor test;*/

		glm::vec3 quad_matrix_translation(700, 400, 0);
		glm::vec3 model_a_matrix_translation(200, 200, 0);
		glm::vec3 model_b_matrix_translation(0, 0, 0);
		
		float increment_left_right = 1.0f;
		float increment_bottom_top = 0.75f;		

		while (!glfwWindowShouldClose(window))
		{
			renderer.clear();			

			// Testing
			// test.on_update(0.0f);
			// test.on_render();

			ImGui_ImplGlfwGL3_NewFrame();

			// set uniform every frame... maybe not do this???
			// pixel shader fills in the object
			// (rasterizes) fragment shader sets color for each pixel			
			{
				model_matrix = glm::translate(glm::mat4(1.0f), quad_matrix_translation);
				model_view_projection_matrix
					= orthographic_projection_matrix * view_matrix * model_matrix;
				shader.bind();
				shader.set_uniform_mat4f(mvp_uniform, model_view_projection_matrix);
				renderer.draw_glow_quad(shader, vertexArray, indexBuffer, blue, color_uniform, mvp_uniform);
			}

			{
				model_matrix = glm::translate(glm::mat4(1.0f), model_a_matrix_translation);
				// recalculate projection every frame
				model_view_projection_matrix
					= orthographic_projection_matrix * view_matrix * model_matrix;	
				shader.bind(); // at scale need a cahce that let's us use an already bound shader
				shader.set_uniform_mat4f(mvp_uniform, model_view_projection_matrix);
				// at scale use batching (bundle all vertex buffers into a single buffer)
				// avoid using a million draw calls
				renderer.draw(shader, vertexArray, indexBuffer);
			}						

			{
				model_matrix = glm::translate(glm::mat4(1.0f), model_b_matrix_translation);
				// recalculate projection every frame
				model_view_projection_matrix
					= orthographic_projection_matrix * view_matrix * model_matrix;
				shader.bind(); 
				shader.set_uniform_mat4f(mvp_uniform, model_view_projection_matrix);
				renderer.draw(shader, vertexArray, indexBuffer);
			}				

			{				   
				// SliderFloat second param exepcts a float array 
				// a vec3 is a struct of { T x, y, z; }, so passing
				// in the of memory address of x using &model_matrix_translation.x
				// gives us y and z as well because of this memory layout
				ImGui::SliderFloat3("Model A Matrix Translation", &model_a_matrix_translation.x, 0.0f, 960.0f);
				ImGui::SliderFloat3("Model B Matrix Translation", &model_b_matrix_translation.x, 0.0f, 960.0f);
				ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 
					1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			// testing
			// test.on_imgui_render();
			ImGui::Render();					
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());			

			glfwSwapBuffers(window); 
			glfwPollEvents();
		}
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}
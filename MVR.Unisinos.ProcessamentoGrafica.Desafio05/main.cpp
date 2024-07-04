#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Sprite.h"

const GLint WIDTH = 384, HEIGHT = 224;


static bool loadTexture(const char* fileName, GLuint* texture, int* width, int* height)
{
	int components;
	int force_channels = 4;
	glEnable(GL_TEXTURE_2D);

	unsigned char* image_data = stbi_load(fileName, width, height, &components, force_channels);

	if (!image_data)
	{
		std::cerr << "ERROR: could not load " << fileName << std::endl;
		return false;
	}

	//power of 2 size check
	if ((*width & (*width - 1)) != 0 || (*height & (*height - 1)) != 0)
	{
		std::cerr << "WARNING: texture " << fileName << "is not power-of-2 dimensions\n";
	}

	glGenTextures(1, texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);

	//set to the maximum degree of anisotropy
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

	return true;
}

Sprite* CreateBackgroundLayer(const char* textureFileName, const int shaderProgram, const float zDepth)
{
	GLuint modelTexture;
	int width, height;

	loadTexture(textureFileName, &modelTexture, &width, &height);

	glm::mat4 modelProjection = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
	glm::mat4 modelTransform = glm::mat4(1);

	return new Sprite(modelTexture, width, height, zDepth, shaderProgram, modelProjection, modelTransform);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);//2
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#pragma region Basic Setup

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "MVR - Proc. Grafico - Desafio 05", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed no init GLEW." << std::endl;
		return EXIT_FAILURE;
	}

#pragma endregion

	const char* vertexShaderSourceCode =
		"#version 410\n"
		"layout ( location = 0 ) in vec3 vPosition;"
		"layout ( location = 1 ) in vec3 vColor;"
		"layout ( location = 2 ) in vec2 vTexture;"
		"uniform mat4 proj;"
		"uniform mat4 matrix;"
		"out vec2 text_map;"
		"out vec3 color;"
		"void main() {"
		"    color = vColor;"
		"    text_map = vTexture;"
		"    gl_Position = proj * matrix * vec4 ( vPosition, 1.0);"
		"}";

	const char* fragmentShaderSourceCode =
		"#version 410\n"
		"in vec2 text_map;"
		"in vec3 color;"
		"uniform sampler2D basic_texture;"
		"out vec4 frag_color;"
		"void main(){"
		"   vec4 textel = texture(basic_texture, text_map);"
		"   if(textel.a < 0.5){ discard;}"
		"   frag_color = textel;"
		"}";
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderId);

	int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderId);

	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fragmentShaderId);
	glAttachShader(shaderProgram, vertexShaderId);
	glLinkProgram(shaderProgram);

	/*
	*
	  1,0 __________ 1,1
		 |          |
		 |          |
		 |    uv    |
		 |          |
	  0,0 __________ 1,0

	*/


	Sprite* background_01 = CreateBackgroundLayer("ken_stage_01.png", shaderProgram, 0.50f);
	background_01->Translate(-20, HEIGHT - 176 - 72);

	Sprite* background_02 = CreateBackgroundLayer("ken_stage_02.png", shaderProgram, 0.51f);
	background_02->Translate(0, HEIGHT - 72);

	Sprite* background_03 = CreateBackgroundLayer("ken_stage_03_2.png", shaderProgram, 0.52f);
	background_03->Translate(0, HEIGHT - 176 - 77);

	Sprite* background_04 = CreateBackgroundLayer("ken_stage_04_2.png", shaderProgram, 0.53f);
	background_04->Translate(550, HEIGHT - 130);

	background_01->SideScroll(-200);
	background_02->SideScroll(-200);
	background_03->SideScroll(-200);
	background_04->SideScroll(-200);

	int oldScreenWidth = WIDTH;
	int oldScrenHeight = HEIGHT;
	double scrollSpeed = 50;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int screenWidth, screenHeight;

		glfwGetWindowSize(window, &screenWidth, &screenHeight);

		glViewport(0, 0, screenWidth, screenHeight);

		if (screenWidth != oldScreenWidth || screenHeight != oldScrenHeight)
		{
			glm::mat4 proj = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);

			background_01->SetProjection(proj);
			background_02->SetProjection(proj);
			background_03->SetProjection(proj);
			background_04->SetProjection(proj);

			oldScreenWidth = screenWidth;
			oldScrenHeight = screenHeight;
		}

		const int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		bool leftPressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
		bool rightPressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

		if (state == GLFW_PRESS)
		{
			double mouseX, mouseY;

			glfwGetCursorPos(window, &mouseX, &mouseY);

			double dx = mouseX - screenWidth / 2.0;
			double dy = mouseY - screenHeight / 2.0;

			float bg_01X = background_01->GetX();

			if ((bg_01X > -343 && dx < 0) || (bg_01X < -87 && dx > 0))
			{
				background_01->SideScroll(dx * 0.001);
				background_02->SideScroll(dx * 0.0015);
				background_03->SideScroll(dx * 0.0014);
				background_04->SideScroll(dx * 0.0015);
			}
		}
		else if (leftPressed)
		{
			float bg_01X = background_01->GetX();

			if (bg_01X > -343)
			{
				background_01->SideScroll(scrollSpeed * 0.001 * -1.0f);
				background_02->SideScroll(scrollSpeed * 0.0015 * -1.0f);
				background_03->SideScroll(scrollSpeed * 0.0014 * -1.0f);
				background_04->SideScroll(scrollSpeed * 0.0015 * -1.0f);
			}
		}
		else if (rightPressed)
		{
			float bg_01X = background_01->GetX();

			if (bg_01X < -87)
			{
				background_01->SideScroll(scrollSpeed * 0.001);
				background_02->SideScroll(scrollSpeed * 0.0015);
				background_03->SideScroll(scrollSpeed * 0.0014);
				background_04->SideScroll(scrollSpeed * 0.0015);
			}
		}

		background_01->Draw();
		background_02->Draw();
		background_03->Draw();
		background_04->Draw();

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return EXIT_SUCCESS;
}

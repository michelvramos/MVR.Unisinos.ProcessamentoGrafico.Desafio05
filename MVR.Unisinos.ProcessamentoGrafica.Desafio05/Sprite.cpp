#include "Sprite.h"
#include <iostream>


Sprite::Sprite( GLuint texture, unsigned int tWidth, unsigned int tHeight, float zDepth, int shaderProgram, glm::mat4 projection, glm::mat4 transform)
{
	modelProjection = projection;
	modelTransform = transform;
	Texture = texture;
	Shader = shaderProgram;

	const int verticesSize = 48;
	GLfloat modelVertices[verticesSize] = {
		// Positions          // Colors           // texture map
		   0.0f,  tHeight, zDepth,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,//bottom left
		 tWidth,  tHeight, zDepth,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,//bottom right
		 tWidth,     0.0f, zDepth,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f,//top right
		 tWidth,     0.0f, zDepth,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f,//top right
		   0.0f,     0.0f, zDepth,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,//top left
		   0.0f,  tHeight, zDepth,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f //bottom left
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(GLfloat), modelVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	
}

void Sprite::Draw()
{
	glUseProgram(Shader);

	glUniformMatrix4fv(glGetUniformLocation(Shader, "proj"), 1, GL_FALSE, glm::value_ptr(modelProjection));
	glUniformMatrix4fv(glGetUniformLocation(Shader, "matrix"), 1, GL_FALSE, glm::value_ptr(modelTransform));

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(glGetUniformLocation(Shader, "basic_texture"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}

void Sprite::Translate(double x, double y)
{
	modelTransform = glm::translate(glm::mat4(1), glm::vec3(x, y, 0));
}

void Sprite::SideScroll(double x)
{
	modelTransform = glm::translate(modelTransform, glm::vec3(x, 0, 0));
	                         //[col][row]
	//std::cout << modelTransform[3][0] << std::endl;
	//std::cout << modelTransform[3][1] << std::endl;
	//std::cout << modelTransform[3][2] << std::endl;
	//std::cout << modelTransform[3][3] << std::endl;
}

void Sprite::SetProjection(glm::mat4 projection)
{
	this->modelProjection = projection;
}

float Sprite::GetX()
{
	return modelTransform[3][0];
}


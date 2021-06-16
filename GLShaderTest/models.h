#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"Shader.h"
#include"Model.h"


void loadTexture(char const* path, unsigned int* textureID);
GLuint loadCubeMapTexture(std::vector<const char*> picFilePathVec,
	GLint internalFormat = GL_RGB,
	GLenum picFormat = GL_RGB,
	GLenum picDataType = GL_UNSIGNED_BYTE,
	int loadChannels = SOIL_LOAD_RGB);

enum MOUSE_EVENT {
	LCLICK,
	RCLICK,
	MCLICK,
	MOVE
};

class BaseModel {

protected:
	glm::vec3 Position;
	glm::vec3 scale_value;
	Shader* shader;
	Model* model;

public:


	BaseModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader) {
		this->Position = pos; this->scale_value = scale;  this->shader = shader;  if (modelPath.compare("") != 0) this->model = new Model(modelPath);
	}

	virtual ~BaseModel() {
		delete this->model;
	}

	virtual void draw() = 0;

	virtual glm::mat4 getModel() = 0;

	virtual void ProcessKeyboard(Movement direction, float deltaTime) = 0;

	virtual void ProcessMouse(MOUSE_EVENT event) = 0;

	void setPosition(glm::vec3 Position) {
		this->Position = Position;
	}

	void setScaleValue(glm::vec3 scale_value) {
		this->scale_value = scale_value;
	}


	void setShader(Shader* shader) {
		this->shader = shader;
	}

	void setModel(Model* model) {
		this->model = model;
	}



};

/** 普通物体 */
class PlainModel : public BaseModel
{
public:


	PlainModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader) :BaseModel(pos, scale, modelPath, shader) {

	}

	~PlainModel() {}

	void draw() {
		model->Draw(*shader);
	}

	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position); // 平移到pos
		model = glm::scale(model, scale_value);	//  缩放标准：1
		return model;
	}

	void ProcessKeyboard(Movement direction, float deltaTime) {

	}

	void ProcessMouse(MOUSE_EVENT event) {

	}




};

class Ball : public BaseModel
{
public:


	Ball(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader) :BaseModel(pos, scale, modelPath, shader) {
		rotateAngle = 0.0f; MovementSpeed = 2.5f;
		ifKey[0] = ifKey[1] = ifKey[2] = ifKey[3] = false;
		rotateRate = 120; axis = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	~Ball() {}

	void draw() {
		model->Draw(*shader);

	}

	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);
		// render the loaded model
		model = glm::translate(model, this->Position); // 平移到场景中央，y为10的位置
		model = glm::scale(model, this->scale_value);	// 
		if (!ifKey[0] && !ifKey[1] && !ifKey[2] && !ifKey[3]) return model = glm::rotate(model, glm::radians(rotateAngle), axis);;
		if (ifKey[0] && ifKey[2])
			axis = this->Right + this->Front;
		else if (ifKey[1] && ifKey[3])
			axis = this->Right + this->Front;
		else if (ifKey[0] && ifKey[3])
			axis = this->Right - this->Front;
		else if (ifKey[1] && ifKey[2])
			axis = this->Right - this->Front;
		else if (ifKey[0])
			axis = this->Right;
		else if (ifKey[1])
			axis = this->Right;
		else if (ifKey[2])
			axis = this->Front;
		else if (ifKey[3])
			axis = this->Front;
		model = glm::rotate(model, glm::radians(rotateAngle), axis);
		ifKey[0] = ifKey[1] = ifKey[2] = ifKey[3] = false;
		return model;
	}

	//在街道平面上移动。
	void setOrientation(glm::vec3 front, glm::vec3 right) {
		Front = glm::normalize(front);
		Front.y = 0;
		Right = glm::normalize(right);
		Right.y = 0;
	}

	void ProcessKeyboard(Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD) {
			Position += Front * velocity;
			ifKey[0] = true;
		}
		if (direction == BACKWARD) {
			Position -= Front * velocity;
			ifKey[1] = true;
		}
		if (direction == LEFT) {
			Position -= Right * velocity;
			ifKey[2] = true;
		}
		if (direction == RIGHT) {
			Position += Right * velocity;
			ifKey[3] = true;
		}


		if (ifKey[0] && ifKey[2])
			rotateAngle = (rotateAngle - velocity * rotateRate);
		else if (ifKey[1] && ifKey[3])
			rotateAngle = (rotateAngle + velocity * rotateRate);
		else if (ifKey[0] && ifKey[3])
			rotateAngle = (rotateAngle - velocity * rotateRate);
		else if (ifKey[1] && ifKey[2])
			rotateAngle = (rotateAngle + velocity * rotateRate);
		else if (ifKey[0])
			rotateAngle = (rotateAngle - velocity * rotateRate);
		else if (ifKey[1])
			rotateAngle = (rotateAngle + velocity * rotateRate);
		else if (ifKey[2])
			rotateAngle = (rotateAngle - velocity * rotateRate);
		else if (ifKey[3])
			rotateAngle = (rotateAngle + velocity * rotateRate);

		if (fabs(rotateAngle) > 360) rotateAngle = 0.0f;

		if (direction == UP)
			Position += glm::vec3(0, 1, 0) * velocity;
		if (direction == DOWN)
			Position += glm::vec3(0, -1, 0) * velocity;
		if (direction == SHIFT_PRESS)
			MovementSpeed = SPEED * 2;
		if (direction == SHIFT_RELEASE)
			MovementSpeed = SPEED;
	}

	void ProcessMouse(MOUSE_EVENT event) {

	}


	glm::vec3 Front; //此物体的前方
	glm::vec3 Right;//此物体的右方
	std::string path; //obj模型文件路径


	float MovementSpeed; //此物体移动速度

	int rotateRate;
	float rotateAngle;
	glm::vec3 axis;

	bool ifKey[4];


};


class Flowerpot : public BaseModel
{
public:


	Flowerpot(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader) :BaseModel(pos, scale, modelPath, shader) {
		this->Position = pos;
		this->scale_value = scale;

		float vertices[] = {
			// positions          // normals           // texture coords
			-0.25f, -0.5f, -0.25f,  0.0f,  -0.25f, -1.0f, 0.0f,  0.0f,
			 0.25f, -0.5f, -0.25f,  0.0f,  -0.25f, -1.0f,1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  -0.25f, -1.0f,1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  -0.25f, -1.0f, 1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  -0.25f, -1.0f, 0.0f,  1.0f,
			-0.25f, -0.5f, -0.25f,  0.0f,  -0.25f, -1.0f, 0.0f,  0.0f,

			-0.25f, -0.5f,  0.25f,  0.0f,   -0.25f,  1.0f, 0.0f,  0.0f,
			 0.25f, -0.5f,  0.25f,  0.0f,   -0.25f,  1.0f,1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,   -0.25f,  1.0f, 1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,   -0.25f,  1.0f, 1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,   -0.25f,  1.0f, 0.0f,  1.0f,
			-0.25f, -0.5f,  0.25f,  0.0f,   -0.25f,  1.0f,0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,   -0.25f,  0.0f,1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,   -0.25f,  0.0f,1.0f,  1.0f,
			-0.25f, -0.5f, -0.25f, -1.0f,   -0.25f,  0.0f,0.0f,  1.0f,
			-0.25f, -0.5f, -0.25f, -1.0f,   -0.25f,  0.0f,0.0f,  1.0f,
			-0.25f, -0.5f,  0.25f, -1.0f,   -0.25f,  0.0f, 0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,   -0.25f,  0.0f, 1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,   -0.25f,  0.0f, 1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  -0.25f,  0.0f,1.0f,  1.0f,
			 0.25f, -0.5f, -0.25f,  1.0f,   -0.25f,  0.0f, 0.0f,  1.0f,
			 0.25f, -0.5f, -0.25f,  1.0f,   -0.25f,  0.0f, 0.0f,  1.0f,
			 0.25f, -0.5f,  0.25f,  1.0f,   -0.25f,  0.0f, 0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,   -0.25f,  0.0f,1.0f,  0.0f,

			-0.25f, -0.5f, -0.25f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.25f, -0.5f, -0.25f,  0.0f, -1.0f,  0.0f, 1.0f,  1.0f,
			 0.25f, -0.5f,  0.25f,  0.0f, -1.0f,  0.0f,1.0f,  0.0f,
			 0.25f, -0.5f,  0.25f,  0.0f, -1.0f,  0.0f,1.0f,  0.0f,
			-0.25f, -0.5f,  0.25f,  0.0f, -1.0f,  0.0f, 0.0f,  0.0f,
			-0.25f, -0.5f, -0.25f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.4375f,  0.25f, -0.4375f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.4375f,  0.25f, -0.4375f,  0.0f,  1.0f,  0.0f, 1.0f,  1.0f,
			 0.4375f,  0.25f,  0.4375f,  0.0f,  1.0f,  0.0f,1.0f,  0.0f,
			 0.4375f,  0.25f,  0.4375f,  0.0f,  1.0f,  0.0f,1.0f,  0.0f,
			-0.4375f,  0.25f,  0.4375f,  0.0f,  1.0f,  0.0f, 0.0f,  0.0f,
			-0.4375f,  0.25f, -0.4375f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);


		glEnableVertexAttribArray(0);

		glGenTextures(1, &diffuseMap);

		loadTexture("pic/container2.jpg", &diffuseMap);


	}

	~Flowerpot() {}

	//绘制模型
	void draw() {
		//将实现绘制的代码写在这
			   // bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

	}

	//得到模型坐标系，以下为默认实现
	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, this->Position); // 平移
		model = glm::scale(model, this->scale_value);	//  缩放标准：1
		return model;
	}

	void ProcessKeyboard(Movement direction, float deltaTime) {

	}

	void ProcessMouse(MOUSE_EVENT event) {

	}

	unsigned int VBO, cubeVAO;
	unsigned int diffuseMap;



};


// utility function for loading a 2D texture from file
// ---------------------------------------------------



class WoodenCase : public BaseModel
{

public:


	WoodenCase(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader) :BaseModel(pos, scale, modelPath, shader) {
		this->Position = pos;
		this->scale_value = scale;
		//顶点属性
		float vertices[] = {
			//--------点坐标--------   -------法向量-------   ---纹理坐标---
		   -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f,  1.0f,
			0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f,  1.0f,
		   -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f,  1.0f,
		   -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    1.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    1.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f,  1.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   0.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f,  0.0f,
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);


		glEnableVertexAttribArray(0);

		glGenTextures(1, &diffuseMap);

		loadTexture("pic/container.jpg", &diffuseMap);


	}

	~WoodenCase() {}

	//绘制模型
	void draw() {
		//将实现绘制的代码写在这
			   // bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

	}

	//得到模型坐标系，以下为默认实现
	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position); // 平移
		model = glm::scale(model, scale_value);	//  缩放标准：1
		return model;
	}

	void ProcessKeyboard(Movement direction, float deltaTime) {

	}

	void ProcessMouse(MOUSE_EVENT event) {

	}


	unsigned int VBO, cubeVAO;
	unsigned int diffuseMap;




};



class SkyBox : public BaseModel
{
public:


	SkyBox(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader) :BaseModel(pos, scale, modelPath, shader) {
		this->shader = shader;
		this->Position = pos;
		this->scale_value = scale;

		GLfloat cubeVertices[] = {
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// A
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,	// B
		0.5f, 0.5f, 0.5f,1.0f, 1.0f,	// C
		0.5f, 0.5f, 0.5f,1.0f, 1.0f,	// C
		-0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// D
		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A


		-0.5f, -0.5f, -0.5f,0.0f, 0.0f,	// E
		-0.5f, 0.5f, -0.5f,0.0, 1.0f,   // H
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// F
		-0.5f, -0.5f, -0.5f,0.0f, 0.0f,	// E

		-0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// D
		-0.5f, 0.5f, -0.5f,1.0, 1.0f,   // H
		-0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// E
		-0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// E
		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A
		-0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// D

		0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// F
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// C
		0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// C
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// B
		0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// F

		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		-0.5f, 0.5f, -0.5f,0.0, 1.0f,   // H
		-0.5f, 0.5f, 0.5f,0.0f, 0.0f,	// D
		-0.5f, 0.5f, 0.5f,0.0f, 0.0f,	// D
		0.5f, 0.5f, 0.5f,1.0f, 0.0f,	// C
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G

		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,// E
		0.5f, -0.5f, -0.5f,1.0f, 1.0f,	// F
		0.5f, -0.5f, -0.5f,1.0f, 1.0f,	// F
		0.5f, -0.5f, 0.5f,1.0f, 0.0f,	// B
		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A
		};
		// 指定包围盒的顶点属性 位置
		GLfloat skyboxVertices[] = {
			// 背面
			-1.0f, 1.0f, -1.0f,		// A
			-1.0f, -1.0f, -1.0f,	// B
			1.0f, -1.0f, -1.0f,		// C
			1.0f, -1.0f, -1.0f,		// C
			1.0f, 1.0f, -1.0f,		// D
			-1.0f, 1.0f, -1.0f,		// A

			// 左侧面
			-1.0f, -1.0f, 1.0f,		// E
			-1.0f, -1.0f, -1.0f,	// B
			-1.0f, 1.0f, -1.0f,		// A
			-1.0f, 1.0f, -1.0f,		// A
			-1.0f, 1.0f, 1.0f,		// F
			-1.0f, -1.0f, 1.0f,		// E

			// 右侧面
			1.0f, -1.0f, -1.0f,		// C
			1.0f, -1.0f, 1.0f,		// G
			1.0f, 1.0f, 1.0f,		// H
			1.0f, 1.0f, 1.0f,		// H
			1.0f, 1.0f, -1.0f,		// D
			1.0f, -1.0f, -1.0f,		// C

			// 正面
			-1.0f, -1.0f, 1.0f,  // E
			-1.0f, 1.0f, 1.0f,  // F
			1.0f, 1.0f, 1.0f,  // H
			1.0f, 1.0f, 1.0f,  // H
			1.0f, -1.0f, 1.0f,  // G
			-1.0f, -1.0f, 1.0f,  // E

			// 顶面
			-1.0f, 1.0f, -1.0f,  // A
			1.0f, 1.0f, -1.0f,  // D
			1.0f, 1.0f, 1.0f,  // H
			1.0f, 1.0f, 1.0f,  // H
			-1.0f, 1.0f, 1.0f,  // F
			-1.0f, 1.0f, -1.0f,  // A

			// 底面
			-1.0f, -1.0f, -1.0f,  // B
			-1.0f, -1.0f, 1.0f,   // E
			1.0f, -1.0f, 1.0f,    // G
			1.0f, -1.0f, 1.0f,    // G
			1.0f, -1.0f, -1.0f,   // C
			-1.0f, -1.0f, -1.0f,  // B
		};


		// Section2 准备缓存对象
		glGenVertexArrays(1, &cubeVAOId);
		glGenBuffers(1, &cubeVBOId);
		glBindVertexArray(cubeVAOId);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
		// 顶点位置数据
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			5 * sizeof(GL_FLOAT), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// 顶点纹理数据
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);


		glGenVertexArrays(1, &skyBoxVAOId);
		glGenBuffers(1, &skyBoxVBOId);
		glBindVertexArray(skyBoxVAOId);
		glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		// 顶点位置数据
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GL_FLOAT), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		// Section3 加载纹理
		loadTexture("pic/container.jpg", &cubeTextId);
		std::vector<const char*> faces;
		faces.push_back("pic/skyboxes/sky/right.jpg");
		faces.push_back("pic/skyboxes/sky/left.jpg");
		faces.push_back("pic/skyboxes/sky/bottom.jpg");
		faces.push_back("pic/skyboxes/sky/top.jpg");
		faces.push_back("pic/skyboxes/sky/front.jpg");
		faces.push_back("pic/skyboxes/sky/back.jpg");


		skyBoxTextId = loadCubeMapTexture(faces);
		// Section4 准备着色器程序

		 /*glEnable(GL_DEPTH_TEST);
		 glEnable(GL_CULL_FACE);*/



	}

	~SkyBox() {}

	void draw() {
		/*glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);*/
		glDepthFunc(GL_LESS);
		glBindVertexArray(skyBoxVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextId); // 注意绑定到CUBE_MAP
		glUniform1i(glGetUniformLocation(shader->ID, "skybox"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glUseProgram(0);
		glDepthFunc(GL_LESS);

	}

	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, scale_value);//
		return model;
	}

	void ProcessKeyboard(Movement direction, float deltaTime) {

	}

	void ProcessMouse(MOUSE_EVENT event) {

	}


	GLuint cubeVAOId, cubeVBOId;
	GLuint skyBoxVAOId, skyBoxVBOId;
	GLuint cubeTextId;
	GLuint skyBoxTextId;

};



















void loadTexture(char const* path, unsigned int* textureID)
{

	glGenTextures(1, textureID);

	int width, height, nrComponents;
	unsigned char* data = SOIL_load_image(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, *textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		SOIL_free_image_data(data);
	}
}


/*
 * 加载一个cubeMap
 */
GLuint loadCubeMapTexture(std::vector<const char*> picFilePathVec,
	GLint internalFormat,
	GLenum picFormat,
	GLenum picDataType,
	int loadChannels)
{
	GLuint textId;
	glGenTextures(1, &textId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textId);
	GLubyte *imageData = NULL;
	int picWidth, picHeight;
	for (std::vector<const char*>::size_type i = 0; i < picFilePathVec.size(); ++i)
	{
		int channels = 0;
		imageData = SOIL_load_image(picFilePathVec[i], &picWidth,
			&picHeight, &channels, loadChannels);
		if (imageData == NULL)
		{
			std::cerr << "Error::loadCubeMapTexture could not load texture file:"
				<< picFilePathVec[i] << std::endl;
			return 0;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, internalFormat, picWidth, picHeight, 0, picFormat, picDataType, imageData);
		SOIL_free_image_data(imageData);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textId;
}
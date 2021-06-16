#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "SOIL2/SOIL2.h"


#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include"models.h"

#include <iostream>
#include<string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void deConstructModels();

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 5.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//light
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

//model position
glm::vec3 lightPosition = glm::vec3(0.0f, 32.0f, 0.0f);

//models
SkyBox* skybox;
PlainModel *street;
PlainModel *plant;
//PlainModel *city;
Ball *ball;
Flowerpot *pot;
WoodenCase *max_s_o;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	//stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader skyBoxShader("shaders/skyboxShader/skyboxVertexShader.vs", "shaders/skyboxShader/skyboxFragmentShader.fs");
	Shader envShader("selfDefinedVertexShader.vs", "selfDefinedFragmentShader.fs");

	// load models

	skybox = new SkyBox(camera.Position, glm::vec3(70.0f, 70.0f, 70.0f), "", &skyBoxShader);
	street = new PlainModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", &envShader);
	//city = new PlainModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/City Islands/City Islands.obj", &envShader);
	ball = new Ball(glm::vec3(0.0f, 0.20f, 0.0f), glm::vec3(0.0025f, 0.0025f, 0.0025f), "model/football/soccer ball.obj", &envShader);
	pot = new Flowerpot(glm::vec3(5.0f, 0.6f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "", NULL);
	max_s_o = new WoodenCase(glm::vec3(8.0f, 0.6f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), "", NULL);
	plant = new PlainModel(glm::vec3(10.0f, 0.1f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), "model/plant/indoor plant_02.obj", &envShader);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		//=========================envShader====================================
		envShader.use();
		envShader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		envShader.setMat4("projection", projection);
		envShader.setMat4("view", view);
		envShader.setVec3("viewPos", camera.Position);
		envShader.setInt("material.diffuse", 0);
		envShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		envShader.setFloat("material.shininess", 32.0f);
		envShader.setVec3("light.position", lightPosition);
		envShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
		envShader.setVec3("light.diffuse", 0.6f, 0.6f, 0.6f); // 将光照调暗了一些以搭配场景
		envShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);



		envShader.setMat4("model", street->getModel());
		street->draw();


		envShader.setMat4("model", ball->getModel());
		ball->setOrientation(camera.Front, camera.Right);
		ball->draw();

		envShader.setMat4("model", pot->getModel());
		pot->draw();

		envShader.setMat4("model", pot->getModel());
		max_s_o->draw();

		envShader.setMat4("model", max_s_o->getModel());
		max_s_o->draw();

		envShader.setMat4("model", plant->getModel());
		plant->draw();


		//=====================================skyBoxShader=================================
		// 绘制包围盒
		//glDepthFunc(GL_LEQUAL); // 深度测试条件 小于等于
		skyBoxShader.use();
		skyBoxShader.setMat4("projection", projection);
		skyBoxShader.setMat4("view", view);


		skybox->setPosition(camera.Position);
		skyBoxShader.setMat4("model", skybox->getModel());
		skybox->draw();


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	deConstructModels();

	return 0;
}

void deConstructModels() {
	delete skybox;
	delete street;
	delete ball;
	delete pot;
	delete max_s_o;
	delete plant;
}


//按键时，窗口的处理逻辑
void windowProcessInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//按键时，相机的处理逻辑
void cameraProcessInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(SHIFT_PRESS, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.ProcessKeyboard(SHIFT_RELEASE, deltaTime);
}

//按键时，模型[football]的处理逻辑
void modelFootballProcessInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		ball->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		ball->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		ball->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		ball->ProcessKeyboard(RIGHT, deltaTime);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	windowProcessInput(window);
	cameraProcessInput(window);
	modelFootballProcessInput(window);



}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
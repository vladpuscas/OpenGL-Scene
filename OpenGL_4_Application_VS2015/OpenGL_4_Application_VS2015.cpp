//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include <fstream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;


bool pressedKeys[1024];
float angle = 0.0f;

gps::Camera myCamera(glm::vec3(0.0f, 1.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f)); 
float cameraSpeed = 0.5f;
float mouseSpeed = 0.001f;


gps::Model3D myModel;
gps::Shader myCustomShader;
gps::Model3D myModel2;
gps::Model3D myModel3;
gps::Model3D myModel4;
gps::Model3D saloon;
gps::Model3D house;
gps::Model3D church;
gps::Model3D well;
gps::Model3D hotel;
gps::Model3D store;
gps::Model3D undertakers;
gps::Model3D rock;
gps::Model3D windmillBase;
gps::Model3D windmillSpin;
gps::Model3D backWheel1;
gps::Model3D backWheel2;
gps::Model3D frontWheel1;
gps::Model3D frontWheel2;
gps::Model3D frontWheel3;
gps::Model3D frontWheel4;
gps::Model3D frontWheel5;
gps::Model3D frontWheel6;
gps::Model3D dome;
gps::Model3D man1;
gps::Model3D man2;
gps::Model3D gun1;
gps::Model3D gun2;
gps::Model3D torch1;
gps::Model3D torch2;


gps::Shader lightShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;
GLfloat lightAngle;

gps::SkyBox mySkyBox;
gps::Shader skyShader;

int xpos, ypos;
double centerx = 1908 / 2;
double centery = 1080 / 2;
double horizontalAngle = 3.14f, verticalAngle;

int onoff, fog;
bool shouldRead = false;
bool animate = false;
const GLuint SHADOW_WIDTH = 16384, SHADOW_HEIGHT = 16384;
std::ofstream fin;
std::ifstream read;
glm::vec3 pointLightColors[] = {
	glm::vec3(2.0f, 2.0f, 2.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
};
float delta = 0;
float movementSpeed = 0.3f; // units per second
int mode = 1;
void updateDelta(double elapsedSeconds) {
	delta = delta + movementSpeed * elapsedSeconds;
}
double lastTimeStamp = glfwGetTime();

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	horizontalAngle += mouseSpeed  * float(centerx - xpos);
	verticalAngle += mouseSpeed * float(centery - ypos);
	myCamera.rotate(horizontalAngle, verticalAngle);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetCursorPos(glWindow, centerx, centery);
}
void recordMovement() {
	
	
	

}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_Q]) {
		onoff = 0;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		onoff = 1;
	}

	if (pressedKeys[GLFW_KEY_K]) {
		fog = 0;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		fog = 1;
	}
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//if (shouldRead)
		//	fin << myCamera.getPosition().x << ' ' << myCamera.getPosition().y << ' ' << myCamera.getPosition().z <<' '<< myCamera.getCameraTarget().x<<' '<<myCamera.getCameraTarget().y<<' '<<myCamera.getCameraTarget().z<<'\n';
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		//if (shouldRead)
		//	fin << myCamera.getPosition().x << ' ' << myCamera.getPosition().y << ' ' << myCamera.getPosition().z <<' '<< myCamera.getCameraTarget().x << ' ' << myCamera.getCameraTarget().y << ' ' << myCamera.getCameraTarget().z << '\n';
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		//if (shouldRead)
		//	fin << myCamera.getPosition().x << ' ' << myCamera.getPosition().y << ' ' << myCamera.getPosition().z <<' '<< myCamera.getCameraTarget().x << ' ' << myCamera.getCameraTarget().y << ' ' << myCamera.getCameraTarget().z << '\n';
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		//if (shouldRead)
		//	fin << myCamera.getPosition().x << ' ' << myCamera.getPosition().y << ' ' << myCamera.getPosition().z <<' '<< myCamera.getCameraTarget().x << ' ' << myCamera.getCameraTarget().y << ' ' << myCamera.getCameraTarget().z << '\n';
	}
	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		depthMapShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_H]) {
		lightAngle -= 0.3f;
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		depthMapShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}
	if (pressedKeys[GLFW_KEY_V]) {
		mode = 1;
	}
	if (pressedKeys[GLFW_KEY_B]) {
		mode = 2;
	}
	if (pressedKeys[GLFW_KEY_N]) {
		mode = 3;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
						  //glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	GLfloat near_plane = 0.0f, far_plane = 70.0f;
	glm::mat4 lightProjection = glm::ortho(-300.0f,300.0f, -20.0f, 40.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr,
		myCamera.getCameraTarget(),
		glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	myModel = gps::Model3D("objects/loco_final.obj","objects/");
	myModel2 = gps::Model3D("objects/railtrack.obj", "objects/");
	myModel3 = gps::Model3D("objects/sand.obj", "objects/");
	myModel4 = gps::Model3D("objects/blacksmith.obj", "objects/");
	saloon = gps::Model3D("objects/saloon.obj", "objects/");
	house = gps::Model3D("objects/house.obj", "objects/");
	church = gps::Model3D("objects/church.obj", "objects/");
	well = gps::Model3D("objects/well.obj", "objects/");
	hotel = gps::Model3D("objects/hotel.obj", "objects/");
	store = gps::Model3D("objects/store.obj", "objects/");
	undertakers = gps::Model3D("objects/undertakers.obj", "objects/");
	rock = gps::Model3D("objects/rock.obj", "objects/");
	windmillBase = gps::Model3D("objects/windmill_base.obj", "objects/");
	windmillSpin = gps::Model3D("objects/windmill_spin.obj", "objects/");
	backWheel1 = gps::Model3D("objects/BackWheels1.obj", "objects/");
	backWheel2 = gps::Model3D("objects/BackWheels2.obj", "objects/");
	frontWheel1 = gps::Model3D("objects/FrontWheel1.obj", "objects/");
	frontWheel2 = gps::Model3D("objects/FrontWheel2.obj", "objects/");
	frontWheel3 = gps::Model3D("objects/FrontWheel3.obj", "objects/");
	frontWheel4 = gps::Model3D("objects/FrontWheel4.obj", "objects/");
	frontWheel5 = gps::Model3D("objects/FrontWheel5.obj", "objects/");
	frontWheel6 = gps::Model3D("objects/FrontWheel6.obj", "objects/");
	dome = gps::Model3D("objects/Dome.obj", "objects/");
	//man1 = gps::Model3D("objects/Man1.obj", "objects/");
	//man2 = gps::Model3D("objects/man3.obj", "objects/");
	//gun1 = gps::Model3D("objects/Gun1.obj","objects/");
	//gun2 = gps::Model3D("objects/Gun2.obj","objects/");
	//torch1 = gps::Model3D("objects/Torch1.obj","objects/");
	//torch2 = gps::Model3D("objects/Torch2.obj","objects/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
	skyShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");

	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 2.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}
void animation()
{
	if (!read.eof()) {
		float x, y, z, xx, yy, zz;
		read >> x >> y >> z >> xx >> yy >> zz;
		myCamera.setPosition(glm::vec3(x, y, z), glm::vec3(xx, yy, zz));
	}
	else
		animate = false;
	
}
void displayMode()
{
	switch (mode)
	{
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 3:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	default:
		break;
	}
}
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (pressedKeys[GLFW_KEY_P])
		animate = true;
	if (pressedKeys[GLFW_KEY_O])
		animate = false;
	if (animate)
		animation();
	else
		processMovement();
	displayMode();
		
	double currentTimeStamp = glfwGetTime();
	updateDelta(currentTimeStamp - lastTimeStamp);
	lastTimeStamp = currentTimeStamp;
	delta += 0.001;

	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//send model matrix to shader
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	myModel.Draw(depthMapShader);
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	myModel2.Draw(depthMapShader);
	myModel3.Draw(depthMapShader);
	myModel4.Draw(depthMapShader);
	saloon.Draw(depthMapShader);
	house.Draw(depthMapShader);
	church.Draw(depthMapShader);
	well.Draw(depthMapShader);
	hotel.Draw(depthMapShader);
	store.Draw(depthMapShader);
	undertakers.Draw(depthMapShader);
	rock.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-56.693f, 0.888f, -0.302f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(56.693f, -0.888f, 0.302f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	frontWheel1.Draw(depthMapShader);

	/*model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-61.294f, 0.207f, 0.86f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(61.294f, -0.207f, -0.86f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	backWheel1.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-59.422f, 0.207f, 0.849f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(59.422f, -0.207f, -0.849f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	backWheel2.Draw(depthMapShader);*/
	

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-53.958f, 0.888f, -0.319f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(53.958f, -0.888f, 0.319f));

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));	frontWheel2.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-51.221f, 0.888f, -0.335f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(51.221f, -0.888f, 0.335f));

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));	frontWheel3.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-51.206f, 0.89f, 1.937f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(51.206f, -0.89f, -1.937f));

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));	frontWheel4.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-53.941f, 0.892f, 1.954f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(53.941f, -0.892f, -1.954f));

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));	frontWheel5.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-56.667f, 0.891f, 1.97f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(56.667f, -0.891f, -1.97f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));	frontWheel6.Draw(depthMapShader);
	//create model matrix for ground
	//model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	//send model matrix to shader

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render the scene (second pass)

	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	//create model matrix for nanosuit
	//model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	//send model matrix data to shader

	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights.position"), myCamera.getPosition().x, myCamera.getPosition().y, myCamera.getPosition().z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights.ambient"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights.diffuse"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights.specular"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights.linear"), 0.09);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights.quadratic"), 0.032);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "onOff"), onoff);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogSwitch"), fog);
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	myModel2.Draw(myCustomShader);
	myModel3.Draw(myCustomShader);
	myModel4.Draw(myCustomShader);
	saloon.Draw(myCustomShader);
	house.Draw(myCustomShader);
	church.Draw(myCustomShader);
	well.Draw(myCustomShader);
	hotel.Draw(myCustomShader);
	store.Draw(myCustomShader);
	undertakers.Draw(myCustomShader);
	rock.Draw(myCustomShader);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	myModel.Draw(myCustomShader);

	//create model matrix for ground
	//model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	//send model matrix data to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-56.693f, 0.888f, -0.302f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(56.693f, -0.888f, 0.302f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//backWheel1.Draw(myCustomShader);
	//backWheel2.Draw(myCustomShader);
	frontWheel1.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-53.958f, 0.888f, -0.319f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(53.958f, -0.888f, 0.319f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	frontWheel2.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-51.221f, 0.888f, -0.335f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(51.221f, -0.888f, 0.335f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	frontWheel3.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-51.206f, 0.89f, 1.937f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(51.206f, -0.89f, -1.937f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	frontWheel4.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-53.941f, 0.892f, 1.954f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(53.941f, -0.892f, -1.954f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	frontWheel5.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-56.667f, 0.891f, 1.97f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(56.667f, -0.891f, -1.97f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	frontWheel6.Draw(myCustomShader);

	/*model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-61.294f, 0.207f, 0.86f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(61.294f, -0.207f, -0.86f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	backWheel1.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(delta, 0, 0));
	model = glm::translate(model, glm::vec3(-59.422f, 0.207f, 0.849f));
	model = glm::rotate(model, -delta, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(59.422f, -0.207f, -0.849f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	backWheel2.Draw(myCustomShader);*/
	
	skyShader.useShaderProgram();
	glDepthFunc(GL_LEQUAL);
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));

	mySkyBox.Draw(skyShader, view, projection);

	glDepthFunc(GL_LESS);

	
}
void initSkyBox()
{
	std::vector<const GLchar*> faces;
	faces.push_back("textures/skybox/right.tga");
	faces.push_back("textures/skybox/left.tga");
	faces.push_back("textures/skybox/top.tga");
	faces.push_back("textures/skybox/bottom.tga");
	faces.push_back("textures/skybox/back.tga");
	faces.push_back("textures/skybox/front.tga");

	mySkyBox.Load(faces);
}

int main(int argc, const char * argv[]) {

	read.open("Movement.txt");
	//fin.open("Movement.txt");
	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();
	initSkyBox();
	
	while (!glfwWindowShouldClose(glWindow)) {

		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}

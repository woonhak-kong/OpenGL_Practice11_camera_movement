#include <iostream>
#include <fstream>

// glew.h�� gl.h�� �����ϱ� ���� �����ؾ��Ѵ�.
#include "GL/glew.h"
#include "GL/freeglut.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// ���ϴ� ������ ��
#define FPS 30

#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,0.9,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)

GLuint vaoHandle;
GLuint programHandle;
GLuint mvpId;
glm::mat4 MVP, View, Projection;
GLuint setShader(const char* shaderType, const char* shaderName);
char* loadShaderAsString(std::string fileName);

float angle = 0.0f, xVal = 0.0f, yVal = 0.0f, zVal = 5.0f;;

void timer(int); // Prototype.


GLshort cube_indices[] = {
	// Front.
	3, 2, 1, 0,
	// Left.
	0, 3, 7, 4,
	// Bottom.
	4, 0, 1, 5,
	// Right.
	5, 1, 2, 6,
	// Back.
	6, 5, 4, 7,
	// Top.
	7, 6, 2, 3
};

GLfloat cube_vertices[] = {
	-0.5f, -0.5f, 0.5f,		// 0.
	0.5f, -0.5f, 0.5f,		// 1.
	0.5f, 0.5f, 0.5f,		// 2.
	-0.5f, 0.5f, 0.5f,		// 3.
	-0.5f, -0.5f, -0.5f,	// 4.
	0.5f, -0.5f, -0.5f,		// 5.
	0.5f, 0.5f, -0.5f,		// 6.
	-0.5f, 0.5f, -0.5f,		// 7.
};

GLfloat colors[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.5f, 0.0f, 0.0f,
	0.0f, 0.5f, 0.0f
};


void init()
{

	// 1. ���̴� ������Ʈ�� �����Ѵ�.
	GLuint vertShader = setShader("vertex", "basic.vert");
	GLuint fragShader = setShader("fragment", "basic.frag");

	// ������Ʈ �������� ������ OpenGL pipeline�� �������� ���� ���̴��� ��� Ȥ�� ��ġ�ؾ��Ѵ�.

	// 1. ���� Program object�� �����Ѵ�.
	// �� ���α׷� ����
	programHandle = glCreateProgram();
	if (0 == programHandle)
	{
		fprintf(stderr, "Error creating program object.\n");
		exit(1);
	}

	// 2. ���̴����� ���α׷��� ���δ�.
	glAttachShader(programHandle, vertShader);
	glAttachShader(programHandle, fragShader);

	// 3. ���α׷��� ��ũ�Ѵ�. ����
	glLinkProgram(programHandle);

	// 4. ��ũ ���� Ȯ��
	GLint status;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		fprintf(stderr, "Failed to link shader program!\n");
		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH,
			&logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen,
				&written, log);
			fprintf(stderr, "Program log: \n%s", log);
			free(log);
		}
	}
	// 5. ���� ��ũ�� �����ߴٸ� ���α׷��� OpenGL pipeline�� ��ġ �Ѵ�.
	else
	{
		glUseProgram(programHandle);
	}

	// uniform ���
	mvpId = glGetUniformLocation(programHandle, "MVP");

	// Projection matrix : 45�� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	View = glm::lookAt(
		glm::vec3(0, 0, zVal), // Origin. Camera is at (0,0,3), in World Space
		glm::vec3(xVal, yVal, 0),	  // Look target. Looks at the origin
		glm::vec3(0, 1, 0)   // Up vector. Head is up (set to 0,-1,0 to look upside-down)
	);

	// ���� ������Ʈ�� �����Ͽ��� ������, �̰͵��� vertex array obejct(VAO) �� ���´�.
	// VAO �����Ѵ�. (���������� vaoHandle �ʿ�)
	// Create and set-up the vertex array object
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);



	// �����ǰ� ������ �����ϱ� ���� ���۸� �����Ѵ�.
	// Create the buffer objects
	GLuint vboHandles[2];
	// ���� �ΰ�����.
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];





	// �� ������ ����Ÿ�� ���, �ű��.
	// Populate the position buffer
	// ������ ���۸� ���ε� �Ѵ�.
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	// ������ ���� ����Ѵ�.
	// �ι�° ���ڴ� �迭�� ������ �̴�. ����° ���ڴ� �ش� �迭
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), cube_vertices, GL_STATIC_DRAW);
	// ù��°���� - generic attribute index, �ι�°���� - �� vertex�� ��Ҽ�(���⼭�� 3)(1,2,3 or 4)
	// ����°���� - �� ����� ����Ÿ Ÿ��, �׹�°���� - normalize ����, �ټ���° ���� - byte offset
	// ������°���� - ������ ���� offset(���⼭�� �����ǰ��ۿ� ���⶧���� 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	// �� �Ӽ� �ε����� generic vertex attribute �� Ȱ��ȭ ��Ų��.
	// Enable the vertex attribute arrays
	glEnableVertexAttribArray(0); // for Vertex position
	//
	//
	// Populate the color buffer
	// ���� ���۸� ���ε��Ѵ�.
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // for Vertex color



	// �ε����� ���� ������Ʈ
	GLuint indexBufferObjec;
	glGenBuffers(1, &indexBufferObjec);
	// GL_ELEMENT_ARRAY_BUFFER�� ����Ѵ�.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjec);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


	//���� ���
	glEnable(GL_DEPTH_TEST);
	// Ÿ�̸� ��ŸƮ
	timer(0);
}
//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(float scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, glm::vec3(scale));
	// position ���� �����Ѵ�.
	View = glm::lookAt(
		glm::vec3(0, 0, zVal), // Origin. Camera is at (0,0,3), in World Space
		glm::vec3(xVal, yVal, 0),	  // Look target. Looks at the origin
		glm::vec3(0, 1, 0)   // Up vector. Head is up (set to 0,-1,0 to look upside-down)
	);
	MVP = Projection * View * Model;
	glUniformMatrix4fv(mvpId, 1, GL_FALSE, &MVP[0][0]);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoHandle);

	transformObject(1.0f, XY_AXIS, angle +=  0.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	// 3��° ���� - ���ý��� ��
	//glDrawArrays(GL_QUADS, 0, 4);

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);

	glutSwapBuffers(); // Now for a potentially smoother render.
}

void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timer, 0);
}

// Keyboard input processing routine.
void keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:
			exit(0);
			break;
		case 'q':
			zVal -= 0.1f;
			break;
		case 'e':
			zVal += 0.1f;
			break;
		case 'w':
			yVal -= 0.1f;
			break;
		case 's':
			yVal += 0.1f;
			break;
		case 'a':
			xVal += 0.1f;
			break;
		case 'd':
			xVal -= 0.1f;
			break;
		default:
			break;
	}
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	//������ ������ ����
	glutInitWindowSize(512, 512);

	// top-left corner �ʱ� ���������� �ʱ�ȭ
	glutInitWindowPosition(0, 0);

	// ������â ����
	glutCreateWindow("Hello World");


	// glew�� �ʱ�ȭ ���ش�. opengl�� ����ϱ����ؼ�
	GLenum err = glewInit();
	// glewInit()�� �����ν� ��� OpenGL���̺귯���� ã�� ��� ��밡���� �Լ������͸� �ʱ�ȭ�Ѵ�.

	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error initializing GLEW: %s\n",
			glewGetErrorString(err));
	}

	// to compile shader
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown); // Input.
	// glut�� �̺�Ʈ ���μ��� loop�� ����.
	glutMainLoop();

	return 0;
}



GLuint setShader(const char* shaderType, const char* shaderName)
{
	GLuint shaderObj;
	if (strcmp(shaderType, "vertex") == 0)
	{
		shaderObj = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (strcmp(shaderType, "fragment") == 0)
	{
		shaderObj = glCreateShader(GL_FRAGMENT_SHADER);
	}


	if (0 == shaderObj)
	{
		fprintf(stderr, "Error creating shader obj.\n");
		exit(1);
	}


	// 2. ���̴� �ҽ��ڵ带 ���̴� ������Ʈ�� �����Ѵ�.
	const GLchar* shaderCode = loadShaderAsString(shaderName);
	// �ҽ� �迭�� ���� �ҽ��ڵ带 ���� �� �ִ�.
	// �迭�� �ҽ��ڵ带 ������.
	const GLchar* codeArray[] = { shaderCode };
	// vertShader object�� codeArray�� �����Ѵ�.
	// ù��° ���ڴ� ���̴� ������Ʈ, �ι�° ���ڴ� �ҽ��ڵ��� �� ���� ���⼭�� shaderCode �Ѱ��� ���� 1
	// ����° ���ڴ� �ڵ带 ���� �迭, �׹�° ���ڴ� �� �ҽ��ڵ��� ���̸� ���� int�迭�̴� ���⼭�� null character�� �־ �ڵ����� Ȯ�εǱ⶧���� NULL�� �־���.
	glShaderSource(shaderObj, 1, codeArray, NULL);

	// 3. ���̴��� ������ �Ѵ�.
	glCompileShader(shaderObj);


	// 4. ������ �Ϸ� Ȯ��.
	GLint result;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result)
	{
		fprintf(stderr, "shader compilation failed!\n");
		GLint logLen;
		glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			glGetShaderInfoLog(shaderObj, logLen, &written, log);
			fprintf(stderr, "Shader log:\n%s", log);
			free(log);
		}
	}

	return shaderObj;
}

char* loadShaderAsString(std::string fileName)
{
	// Initialize input stream.
	std::ifstream inFile(fileName.c_str(), std::ios::binary);

	// Determine shader file length and reserve space to read it in.
	inFile.seekg(0, std::ios::end);
	int fileLength = inFile.tellg();
	char* fileContent = (char*)malloc((fileLength + 1) * sizeof(char));

	// Read in shader file, set last character to NUL, close input stream.
	inFile.seekg(0, std::ios::beg);
	inFile.read(fileContent, fileLength);
	fileContent[fileLength] = '\0';
	inFile.close();

	return fileContent;
}
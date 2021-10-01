#include <iostream>
#include <fstream>

// glew.h는 gl.h를 포함하기 전에 포함해야한다.
#include "GL/glew.h"
#include "GL/freeglut.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// 원하는 프레임 값
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

	// 1. 쉐이더 오브젝트를 생성한다.
	GLuint vertShader = setShader("vertex", "basic.vert");
	GLuint fragShader = setShader("fragment", "basic.frag");

	// 오브젝트 컴파일이 끝나면 OpenGL pipeline에 컴파일이 끝난 쉐이더를 등록 혹은 설치해야한다.

	// 1. 먼저 Program object를 생성한다.
	// 빈 프로그램 생성
	programHandle = glCreateProgram();
	if (0 == programHandle)
	{
		fprintf(stderr, "Error creating program object.\n");
		exit(1);
	}

	// 2. 쉐이더들을 프로그램에 붙인다.
	glAttachShader(programHandle, vertShader);
	glAttachShader(programHandle, fragShader);

	// 3. 프로그램을 링크한다. 연결
	glLinkProgram(programHandle);

	// 4. 링크 상태 확인
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
	// 5. 만약 링크가 성공했다면 프로그램을 OpenGL pipeline에 설치 한다.
	else
	{
		glUseProgram(programHandle);
	}

	// uniform 사용
	mvpId = glGetUniformLocation(programHandle, "MVP");

	// Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	View = glm::lookAt(
		glm::vec3(0, 0, zVal), // Origin. Camera is at (0,0,3), in World Space
		glm::vec3(xVal, yVal, 0),	  // Look target. Looks at the origin
		glm::vec3(0, 1, 0)   // Up vector. Head is up (set to 0,-1,0 to look upside-down)
	);

	// 버퍼 오브젝트롤 설정하였기 때문에, 이것들을 vertex array obejct(VAO) 에 묶는다.
	// VAO 생성한다. (전역변수로 vaoHandle 필요)
	// Create and set-up the vertex array object
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);



	// 포지션과 색상을 저장하기 위한 버퍼를 생성한다.
	// Create the buffer objects
	GLuint vboHandles[2];
	// 버퍼 두개생성.
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];





	// 각 버퍼의 데이타를 등록, 옮긴다.
	// Populate the position buffer
	// 포지션 버퍼를 바인드 한다.
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	// 포지션 값을 등록한다.
	// 두번째 인자는 배열의 사이즈 이다. 세번째 인자는 해당 배열
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), cube_vertices, GL_STATIC_DRAW);
	// 첫번째인자 - generic attribute index, 두번째인자 - 각 vertex의 요소수(여기서는 3)(1,2,3 or 4)
	// 세번째인자 - 각 요소의 데이타 타입, 네번째인자 - normalize 여부, 다섯번째 인자 - byte offset
	// 여섯번째인자 - 버퍼의 시작 offset(여기서는 포지션값밖에 없기때문에 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	// 각 속성 인덱스의 generic vertex attribute 를 활성화 시킨다.
	// Enable the vertex attribute arrays
	glEnableVertexAttribArray(0); // for Vertex position
	//
	//
	// Populate the color buffer
	// 색상 버퍼를 바인드한다.
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // for Vertex color



	// 인덱스용 버퍼 오브젝트
	GLuint indexBufferObjec;
	glGenBuffers(1, &indexBufferObjec);
	// GL_ELEMENT_ARRAY_BUFFER를 사용한다.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjec);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


	//깊이 사용
	glEnable(GL_DEPTH_TEST);
	// 타이머 스타트
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
	// position 값을 갱신한다.
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

	// 3번째 인자 - 버택스의 수
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

	//윈도우 사이즈 변경
	glutInitWindowSize(512, 512);

	// top-left corner 초기 포지션으로 초기화
	glutInitWindowPosition(0, 0);

	// 윈도우창 생성
	glutCreateWindow("Hello World");


	// glew를 초기화 해준다. opengl을 사용하기위해서
	GLenum err = glewInit();
	// glewInit()을 함으로써 모든 OpenGL라이브러리를 찾고 모든 사용가능한 함수포인터를 초기화한다.

	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error initializing GLEW: %s\n",
			glewGetErrorString(err));
	}

	// to compile shader
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown); // Input.
	// glut의 이벤트 프로세싱 loop을 시작.
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


	// 2. 쉐이더 소스코드를 쉐이더 오브젝트로 복사한다.
	const GLchar* shaderCode = loadShaderAsString(shaderName);
	// 소스 배열에 여러 소스코드를 담을 수 있다.
	// 배열에 소스코드를 담은후.
	const GLchar* codeArray[] = { shaderCode };
	// vertShader object로 codeArray를 복사한다.
	// 첫번째 인자는 쉐이더 오브젝트, 두번째 인자는 소스코드의 총 개수 여기서는 shaderCode 한개만 들어가서 1
	// 세번째 인자는 코드를 넣은 배열, 네번째 인자는 각 소스코드의 길이를 넣은 int배열이다 여기서는 null character를 넣어서 자동으로 확인되기때무에 NULL을 넣었다.
	glShaderSource(shaderObj, 1, codeArray, NULL);

	// 3. 쉐이더를 컴파일 한다.
	glCompileShader(shaderObj);


	// 4. 컴파일 완료 확인.
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
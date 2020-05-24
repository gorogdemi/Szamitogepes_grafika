#pragma warning (disable: 26451)
#pragma warning (disable: 26495)

#include "Shader.hpp"
#include "Subdiv.hpp"

#include <algorithm>
#include <memory>
#include <ctime>
#include <cmath>

#define _USE_MATH_DEFINES

using namespace std;

bool isKeyPressed = false;

struct UniformDataModel
{
	glm::mat4 m_modelView;
	glm::mat4 m_view;
	glm::mat4 m_normal;
	glm::mat4 m_mvp;
};

array<glm::mat4, 1> s_models =
{
	glm::translate(glm::vec3(0.0f, -1.0f, 0.0f)) * glm::scale(glm::vec3(1.0f)),
	//=================================================================================
};

static GLuint s_uboModel = 0, s_uboMaterial = 0, s_uboLight = 0;
/** Ablakunk mérete. */
static int s_width = 1366, s_height = 768;
static GLuint s_vboCube = 0, s_iboCube = 0, s_vaoCube = 0;
/** Az OpenGL kontextussal rendelkező GLFW ablakunk. */
static GLFWwindow* s_window = NULL;

/** Frissítések közötti minimális idő. */
static double s_updateDeltaTime = 1.0 / 60.0;

extern Mesh s_mesh;

static GLuint s_program = 0;
static GLuint boundingbox = 2;

static glm::mat4 s_view, s_projection;

static glm::vec3 s_eye = glm::vec3(0.0f, 0.0f, 3.0f), s_forward = glm::vec3(0.0f), s_right = glm::vec3(0.0f), s_up = glm::vec3(0.0f);

static GLfloat s_phi = glm::radians(90.0f), s_theta = glm::radians(270.0f);

static GLfloat s_moveSpeed = 2.0f, s_turnSpeed = glm::radians(3.0f);

static double s_mouseX = -1.0, s_mouseY = -1.0;

//=====================================================================================================================

void computeCameraMatrices()
{
	s_forward = glm::vec3(
		glm::cos(s_theta) * glm::sin(s_phi),
		glm::cos(s_phi),
		glm::sin(s_theta) * glm::sin(s_phi)
	);
	s_right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), s_forward));
	s_up = glm::normalize(glm::cross(s_forward, s_right));

	s_view = glm::lookAt(s_eye, s_eye + s_forward, glm::vec3(0.0f, 1.0f, 0.0f));
	s_projection = glm::perspective(glm::radians(55.0f), (float)s_width / (float)s_height, 0.01f, 20.0f);
}

//=====================================================================================================================

void initScene()
{
	computeCameraMatrices();

	/** Betöltjük a rajzoláshoz használandó shader programot. */
	s_program = loadProgram("shader");
	boundingbox = loadProgram("bounds");

	/** Betöltjük a mesht. */
	s_mesh = loadMesh("piramis.obj");
	cout << "Loading done." << endl;

	for (int i = 0; i < s_mesh.vertices.size(); i++)
	{
		//cout << s_mesh.vertices[i].position.x << " " << s_mesh.vertices[i].position.y << " " << s_mesh.vertices[i].position.z << endl;
		//cout << s_mesh.vertices[i].normal.x << " " << s_mesh.vertices[i].normal.y << " " << s_mesh.vertices[i].normal.z << endl;
		//cout << "=============================================" << endl;
	}

	glGenBuffers(1, &s_uboModel);

	for (int h = 0; h < s_mesh.halfEdge.size(); h++)
	{
		cout << s_mesh.halfEdge[h].v1 << " " << s_mesh.halfEdge[h].v2 << " " << &s_mesh.halfEdge[h] << " " << s_mesh.halfEdge[h].pair << " ";
		if (s_mesh.halfEdge[h].pair != NULL)
		{
			cout << s_mesh.halfEdge[h].pair->v1 << " " << s_mesh.halfEdge[h].pair->v2;
		}
		cout << endl;
	}

	//for (int i = 0; i < s_mesh.indices.size(); i++)
	//{
	//	cout << s_mesh.indices[i] << " " ;
	//	if ((i + 1) % 3 == 0)cout << endl;
	//}
	//cout <<"======="<< endl;
	//for (int i = 0; i < s_mesh.faces.size(); i++)
	//{
	//	cout << s_mesh.faces[i].vertices[0] << " " << s_mesh.faces[i].vertices[1] << " " << s_mesh.faces[i].vertices[2] << endl;
	//	//if ((i+1) % 3 == 0)cout << endl;
	//}
	//cout << "=======" << endl;
	//for (int i = 0; i < s_mesh.halfEdge.size(); i++)
	//{
	//	cout << s_mesh.halfEdge[i].v1 << " " << s_mesh.halfEdge[i].v2 << " " << s_mesh.halfEdge[i].pair << endl;

	//}

	//cout << "=======" << endl;
	//cout << s_mesh.verticesSubdiv.size() << endl;
}

void cleanUpScene()
{
	/** Töröljük a submesheket. */

	if (s_mesh.vao != 0)
	{
		glDeleteVertexArrays(1, &s_mesh.vao);
	}

	if (s_mesh.vbo != 0)
	{
		glDeleteBuffers(1, &s_mesh.vbo);
	}

	if (s_mesh.ibo != 0)
	{
		glDeleteBuffers(1, &s_mesh.ibo);
	}

	/** Töröljük a shader programot. */
	glDeleteProgram(s_program);
}

void renderScene()
{
	glClearColor(0.8f, 0.8f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(s_program);

	for (size_t i = 0; i < s_models.size(); ++i)
	{
		UniformDataModel modelData;
		modelData.m_modelView = s_view * s_models[i];
		modelData.m_view = s_view;
		modelData.m_normal = glm::inverseTranspose(modelData.m_modelView);
		modelData.m_mvp = s_projection * modelData.m_modelView;
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, s_uboModel);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformDataModel), &modelData, GL_STREAM_DRAW);

		glBindVertexArray(s_mesh.vao);
		glPointSize(10);
		glDrawElements(GL_TRIANGLES, s_mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	glBindVertexArray(0);
	glUseProgram(boundingbox);

	glBindVertexArray(s_mesh.vao);

	/** Kirajzoljuk a határolókat. */
	for (size_t i = 0; i < s_models.size(); ++i)
	{
		/** Feltöltjük a uniformokat mátrixot. */
		glm::mat4 mvp = s_projection * s_view * s_models[i];
		glm::vec3 boundsColor = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform3fv(1, 1, glm::value_ptr(boundsColor));

		/** Kirajzoljuk indexelve a kockát. */
		glLineWidth(4);
		glDrawElements(GL_POINTS, s_mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
		//glDrawArrays(GL_POINTS,0, s_mesh.indices.size()-1);
	}

	glBindVertexArray(0);
}

void updateScene(double delta)
{
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	s_models[0] = s_models[0] * trans;

	/** Escape billentyű. */
	if (glfwGetKey(s_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(s_window, true);
	}

	/** Mozgás kezelése. */
	if (glfwGetKey(s_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		s_eye += s_forward * s_moveSpeed * (float)delta;
	}

	if (glfwGetKey(s_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		s_eye -= s_forward * s_moveSpeed * (float)delta;
	}

	if (glfwGetKey(s_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		s_eye += s_right * s_moveSpeed * (float)delta;
	}

	if (glfwGetKey(s_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		s_eye -= s_right * s_moveSpeed * (float)delta;
	}

	if (glfwGetKey(s_window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		if (!isKeyPressed) {
			subdivide();
			
			s_mesh.loadSubdivData();
			for (int i = 0; i < s_mesh.indices.size(); i++)
			{
				//cout << s_mesh.vertices[i].position.x << " " << s_mesh.vertices[i].position.y << " " << s_mesh.vertices[i].position.z << endl;
				//cout << s_mesh.vertices[i].normal.x << " " << s_mesh.vertices[i].normal.y << " " << s_mesh.vertices[i].normal.z << endl;
				//cout << "=============================================" << endl;
				cout << s_mesh.indices[i] << endl;
			}
			for (int i = 0; i < s_mesh.vertices.size(); i++)
			{
				cout << s_mesh.vertices[i].position.x << " " << s_mesh.vertices[i].position.y << " " << s_mesh.vertices[i].position.z << endl;
				//cout << s_mesh.vertices[i].normal.x << " " << s_mesh.vertices[i].normal.y << " " << s_mesh.vertices[i].normal.z << endl;
				//cout << "=============================================" << endl;
				//cout << s_mesh.indices[i] << endl;
			}
			cout << s_mesh.indices.size() << endl;

			glBindBuffer(GL_ARRAY_BUFFER, s_mesh.vbo);
			glBufferData(GL_ARRAY_BUFFER, s_mesh.vertices.size() * sizeof(Mesh::Vertex), s_mesh.vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_mesh.ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_mesh.indices.size() * sizeof(GLuint), s_mesh.indices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			/* Konfiguráljuk a VAO-t. */

			/*glBindVertexArray(s_mesh.vao);
			glBindBuffer(GL_ARRAY_BUFFER, s_mesh.vbo);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (const void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (const void*)(sizeof(glm::vec3)));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_mesh.ibo);
			glBindVertexArray(0);*/

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			cout << s_mesh.vertices.size() << endl;
		}

		isKeyPressed = true;
	}

	if (glfwGetKey(s_window, GLFW_KEY_ENTER) == GLFW_RELEASE)
	{
		isKeyPressed = false;
	}

	/** Forgás kezelése. */
	double mouseX, mouseY;
	glfwGetCursorPos(s_window, &mouseX, &mouseY);

	if (s_mouseX != -1.0)
	{
		float deltaX = (float)(mouseX - s_mouseX);
		float deltaY = (float)(mouseY - s_mouseY);

		s_phi += s_turnSpeed * delta * deltaY;
		s_theta += s_turnSpeed * delta * deltaX;
	}

	s_mouseX = mouseX;
	s_mouseY = mouseY;

	/** Újraszámítjuk a kamera mátrixokat. */
	computeCameraMatrices();
}

//=====================================================================================================================

void glfwErrorFunc(int errorCode, const char* errorStr)
{
	cerr << "GLFW error: " << errorStr << endl;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	/** Eltároljuk az ablakunk méreteit. */
	s_width = width;
	s_height = height;

	/** Frissítjük a rendereléshez használt területet. */
	glViewport(0, 0, width, height);

	/** Frissítjük a kamera mátrixot, mivel megváltozott az ablak méretaránya. */
	computeCameraMatrices();
}

//=====================================================================================================================

int main(int argc, char** argv)
{
	//vector <int> valami = { 5,6,7,8 };
	//cout << abs(valami.begin()-find(valami.begin(), valami.end(), 6));

	/** Próbáljuk meg felállítani a GLFW-t. */
	if (!glfwInit())
	{
		cerr << "Failed to initialize GLFW!" << endl;
		return -1;
	}

	/** A kívánt OpenGL profil és verzió (4.3 Core profil) */
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);

	/** Hiba callback függvény. */
	glfwSetErrorCallback(glfwErrorFunc);

	/** Próbáljuk meg létrehozni az ablakunkat. */
	s_window = glfwCreateWindow(s_width, s_height, "Számítógépes grafika beadandó", NULL, NULL);

	if (!s_window)
	{
		cerr << "Failed to create a GLFW window!" << endl;
		glfwTerminate();
		return -2;
	}

	/** Kapcsoljuk ki az egérkurzort. */
	glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/** Callback függvények az egyéb eseményekhez. */
	glfwSetFramebufferSizeCallback(s_window, resizeCallback);

	/** Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	glfwMakeContextCurrent(s_window);

	/** Inicializáljuk a GLEW-ot, hogy elérhetővé váljanak az OpenGL függvények. */
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();

	if (glewError != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW, reason: " << glewGetErrorString(glewError) << endl;
		glfwTerminate();
		return -3;
	}

	/** Inicializáljuk az stb_image-et. */
	stbi_set_flip_vertically_on_load(1);

	/** Inicializáljuk a jelenetünket. */
	initScene();

	/** Előző frissítés időpontja. */
	double lastTime = glfwGetTime();

	/** Fő ciklus. */
	while (!glfwWindowShouldClose(s_window))
	{
		/** Jelenlegi idő. */
		double currentTime = glfwGetTime();

		/** Eltelt idő. */
		double deltaTime = currentTime - lastTime;

		/** Először az eseményeket kezeljük. */
		glfwPollEvents();

		/** Ha elég idő eltelt, frissítünk és rajzolunk. */
		if (deltaTime > s_updateDeltaTime)
		{
			/** Jelenet frissítése (input kezelés, alkalmazáslogika). */
			updateScene(deltaTime);

			/** Jelenet kirajzolása. */
			renderScene();
			glfwSwapBuffers(s_window);

			/** Eltároljuk a mostani időpontot, mint a legutolsó frissítés idejét. */
			lastTime = currentTime;
		}
	}

	/** Szabaduljunk meg a már feleslegessé vált objektumoktól. */
	cleanUpScene();

	/** Állítsuk le a GLFW-t. */
	glfwTerminate();

	return 0;
}
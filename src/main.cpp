#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <demoShaderLoader.h> // Assure-toi que ce fichier est bien configuré
#include <imgui.h>

// Dimensions de la fenêtre
const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

// Prototypes
void processInput(GLFWwindow* window);

struct Camera
{
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.005f;
	float sensitivity = 20.0f;

	Camera() {}

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position)
	{
		Camera::width = width;
		Camera::height = height;
		Position = position;
	}

	void InitCamera(int width, int height, glm::vec3 position)
	{
		Camera::width = width;
		Camera::height = height;
		Position = position;
	}

	// Updates and exports the camera matrix to the Vertex Shader
	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
	{
		// Initializes matrices since otherwise they will be the null matrix
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// Makes camera look in the right direction from the right position
		view = glm::lookAt(Position, Position + Orientation, Up);
		// Adds perspective to the scene
		projection = glm::perspective(glm::radians(FOVdeg), (float)(width / height), nearPlane, farPlane);

		// Exports the camera matrix to the Vertex Shader
		glUniformMatrix4fv(glGetUniformLocation(shader.id, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
	}

	// Handles camera inputs
	void Inputs(GLFWwindow* window)
	{
		// Handles key inputs
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			Position += speed * Orientation;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Position += speed / 2 * -glm::normalize(glm::cross(Orientation, Up));
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Position += speed * -Orientation;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Position += speed / 2 * glm::normalize(glm::cross(Orientation, Up));
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			Position += speed / 2 * Up;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			Position += speed / 2 * -Up;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			speed = 0.01f;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			speed = 0.005f;
		}


		// Handles mouse inputs
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			// Hides mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			// Prevents camera from jumping on the first click
			if (firstClick)
			{
				glfwSetCursorPos(window, (width / 2), (height / 2));
				firstClick = false;
			}

			// Stores the coordinates of the cursor
			double mouseX;
			double mouseY;
			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window, &mouseX, &mouseY);

			// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
			// and then "transforms" them into degrees 
			float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
			float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

			// Calculates upcoming vertical change in the Orientation
			glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

			// Decides whether or not the next vertical Orientation is legal or not
			if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				Orientation = newOrientation;
			}

			// Rotates the Orientation left and right
			Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

			// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
			glfwSetCursorPos(window, (width / 2), (height / 2));
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			// Unhides cursor since camera is not looking around anymore
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Makes sure the next time the camera looks around it doesn't jump
			firstClick = true;
		}
	}
};

// Sommets et indices pour un cube
std::vector<GLfloat> vertices = {
    // Position des sommets du cube
    -0.5f, -0.5f,  0.5f,   // Avant-bas-gauche
     0.5f, -0.5f,  0.5f,   // Avant-bas-droite
     0.5f,  0.5f,  0.5f,   // Avant-haut-droite
    -0.5f,  0.5f,  0.5f,   // Avant-haut-gauche

    -0.5f, -0.5f, -0.5f,   // Arrière-bas-gauche
     0.5f, -0.5f, -0.5f,   // Arrière-bas-droite
     0.5f,  0.5f, -0.5f,   // Arrière-haut-droite
    -0.5f,  0.5f, -0.5f    // Arrière-haut-gauche
};

std::vector<GLuint> indices = {
    0, 1, 2, 2, 3, 0, // Face avant
    4, 5, 6, 6, 7, 4, // Face arrière
    4, 0, 3, 3, 7, 4, // Face gauche
    1, 5, 6, 6, 2, 1, // Face droite
    3, 2, 6, 6, 7, 3, // Face supérieure
    4, 5, 1, 1, 0, 4  // Face inférieure
};

Camera camera;

int main() {
    // Initialisation de GLFW
    if (!glfwInit()) {
        std::cerr << "Erreur : Impossible d'initialiser GLFW." << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Chute libre en 3D - Modern OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Erreur : Impossible de créer une fenêtre GLFW." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur : Impossible de charger les fonctions OpenGL avec GLAD." << std::endl;
        return -1;
    }

	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
    
	glEnable(GL_DEPTH_TEST);

	camera.InitCamera(600, 600, glm::vec3(0.0f, 1.0f, 17.0f));

    Shader s;
    s.loadShaderProgramFromFile(RESOURCES_PATH "vertex.vert", RESOURCES_PATH "fragment.frag");
    s.bind();

    // Configurer VAO, VBO et EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

	float deltaTime = 0.0f; // Temps écoulé entre deux frames
	float lastFrame = 0.0f; // Temps de la dernière frame

    // Boucle principale
	while (!glfwWindowShouldClose(window)) {
		// Calculer Delta Time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		// Effacer l'écran
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 cubePos = glm::vec3(0.0f);

		// Configurer les matrices
		glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -10.0f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 50.0f);

		// Activer les shaders
		s.bind();
		s.setMat4("model", model);
		s.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));

		camera.Inputs(window);
		camera.Matrix(45.0f, 0.1f, 100.0f, s, "camMatrix");

		std::cout << "Cam X : " << camera.Position.x << " ; Cam Y : " << camera.Position.y << " ; Cam Z : " << camera.Position.z << std::endl;

		//ImGui::NewFrame();
		//
		//// Ajoute ici ton contenu ImGui
		//ImGui::ShowDemoWindow();
		//
		//// Fin de la frame ImGui
		//ImGui::Render();

		// Dessiner le cube
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    // Nettoyage
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
	//ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
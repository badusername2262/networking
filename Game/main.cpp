#include <Extern/glad/gl.h>
#include <Extern/glfw/include/GLFW/glfw3.h>

#include <Utils/Logging.h>
#include <iostream>

GLFWwindow* window = NULL;
double p_xPos = 0.0, p_yPos = 0.0;

void Render();
void Events();
void Update();

static void CursorCallback(GLFWwindow* window, double xPos, double yPos);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

bool Init() {

    if (!glfwInit()) {
        LOG_ERROR("GLFW Failed to Initialise");
        glfwTerminate();
        return false;
    }

    window = glfwCreateWindow(1280, 720, "test", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, CursorCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
        LOG_ERROR("OpenGL Failed to Initialise");
        glfwTerminate();
        return false;
    }
    return true;
}

static void CursorCallback(GLFWwindow* window, double xPos, double yPos) {
    p_xPos = xPos;
    p_yPos = yPos;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    
}

int main() {
    if (!Init()) {
        LOG_ERROR("Failed to Initialise, Shutting Down...");
        return -1;
    }
    LOG_INFO("Successfuly Initialised OpenGL");

    while (!glfwWindowShouldClose(window)) {
        Render();
        Events();
        Update();
    }

}

void Render() {
    glClearColor(.7, .7, .7, 1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
}

void Events() {
    glfwPollEvents();
}

void Update() {

}
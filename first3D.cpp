#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float lastX = 400, lastY = 300, yaw = -90.0f, pitch = 0.0f;
float fov = 45.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow *, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;
  float sensitivity = 0.1f;
  yaw += xoffset * sensitivity;
  pitch += yoffset * sensitivity;
  pitch = glm::clamp(pitch, -89.0f, 89.0f);
  glm::vec3 dir;
  dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  dir.y = sin(glm::radians(pitch));
  dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(dir);
}
void scroll_callback(GLFWwindow *, double, double yoffset) {
  fov -= yoffset;
  fov = glm::clamp(fov, 1.0f, 45.0f);
}
void processInput(GLFWwindow *window) {
  float speed = 2.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += speed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= speed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
}

const char *vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vertexColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
})";

const char *fragmentShaderSrc = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vertexColor, 1.0);
})";

unsigned int compileShader(unsigned int type, const char *src) {
  unsigned int id = glCreateShader(type);
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);
  return id;
}
unsigned int createShaderProgram() {
  unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
  unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
  unsigned int program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glDeleteShader(vs);
  glDeleteShader(fs);
  return program;
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "GL 3D Cube & Prism", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glEnable(GL_DEPTH_TEST);

  float cubeVertices[] = {-0.5, -0.5, -0.5, 1, 0, 0, 0.5,  -0.5, -0.5, 0, 1, 0,
                          0.5,  0.5,  -0.5, 0, 0, 1, -0.5, 0.5,  -0.5, 1, 1, 0,
                          -0.5, -0.5, 0.5,  1, 0, 1, 0.5,  -0.5, 0.5,  0, 1, 1,
                          0.5,  0.5,  0.5,  1, 1, 1, -0.5, 0.5,  0.5,  0, 0, 0};
  unsigned int cubeIndices[] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4,
                                0, 1, 5, 5, 4, 0, 2, 3, 7, 7, 6, 2,
                                0, 3, 7, 7, 4, 0, 1, 2, 6, 6, 5, 1};

  float prismVertices[] = {
      0.0f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // A (front top)
      -0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, // B (front left)
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // C (front right)
      0.0f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.0f, // A'
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // B'
      0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 1.0f  // C'
  };
  unsigned int prismIndices[] = {0, 1, 2, 3, 5, 4, 0, 3, 1, 1, 3, 4,
                                 0, 2, 3, 2, 5, 3, 1, 4, 2, 2, 4, 5};

  unsigned int cubeVAO, cubeVBO, cubeEBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glGenBuffers(1, &cubeEBO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  unsigned int prismVAO, prismVBO, prismEBO;
  glGenVertexArrays(1, &prismVAO);
  glGenBuffers(1, &prismVBO);
  glGenBuffers(1, &prismEBO);
  glBindVertexArray(prismVAO);
  glBindBuffer(GL_ARRAY_BUFFER, prismVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(prismVertices), prismVertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prismEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(prismIndices), prismIndices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  unsigned int shader = createShaderProgram();
  glUseProgram(shader);

  while (!glfwWindowShouldClose(window)) {
    float time = glfwGetTime();
    deltaTime = time - lastFrame;
    lastFrame = time;

    processInput(window);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection =
        glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));

    // Prism (right side)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, time, glm::vec3(0.2f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));
    glBindVertexArray(prismVAO);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

    // Cube (left side)
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, time, glm::vec3(0.5f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

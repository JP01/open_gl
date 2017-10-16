#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Controls
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();


Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));
GLfloat lastX = WIDTH/2.0f;
GLfloat lastY = HEIGHT/2.0f;
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Lights
//glm::vec3 lightPos(0.0f, 1.5f, 20.0f);
void ApplyPointLighting(Shader &shader, std::vector<glm::vec3> pointLightPositions);
void ApplySpotLighting(Shader &shader);
void ApplyDirectionalLighting(Shader &shader);

int main()
{
    // Init GLFW
    glfwInit();
    
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // Create a GLFW window object that we can use for GLFW functions
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Using ASSIMP", nullptr, nullptr);
    
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);  // Adjust width/height for high res screens eg 4k/retina
    
    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent(window);
    
    // Handle input
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glewExperimental = GL_TRUE; // glew experimental so we can use newer OpenGL features
    
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define viewport dimensions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Depth perception
    glEnable(GL_DEPTH_TEST);
    
    // Enable alpha for texturing
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup and compile shaders
    Shader modelShader("resources/shaders/modelLoading.vs", "resources/shaders/modelLoading.frag");
    Shader lampShader("resources/shaders/lamp.vs", "resources/shaders/lamp.frag");
    
    // Load models
    Model nanosuit("resources/models/nanosuit/nanosuit.obj");
    
    // Setup vertex data, buffers and attribute pointers
    float vertices[] = {
        // positions           // normals             // texture coords
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,    0.0f,  0.0f,
        
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,    0.0f,  0.0f,
        
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   1.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,    0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,    0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,    0.0f,  1.0f,
        
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,    0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,    0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,    0.0f,  1.0f
    };
    
    std::vector<glm::vec3> pointLightPositions = {
        glm::vec3(1.0f, 1.5f, -0.5f),
        glm::vec3(0.0f, 1.0f, -1.5f),
        glm::vec3(-1.0f, 0.0f, -0.5f),
        glm::vec3(-2.0f, -1.0f, 0.5f)
    };
    
    // Draw wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Lamp
    GLuint VBO, lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    
    // Bind Vertex Array Object first, then bind and set vertex buffers and attr pointers
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 *sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0); // Unbind VAO
    
    // Setup the view
    glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (GLfloat)(SCREEN_WIDTH/ SCREEN_HEIGHT), 0.1f, 1000.0f);
    
    ////////////////////////////////////////////////////////////////////////////
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check any window events
        glfwPollEvents();
        DoMovement();
        
        // Render
        // Clear the colorbuffer
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // intialise some matrices for calculations
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model;
        
        // MODEL //////////////////////////////////////////////////////
        modelShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        
        GLint viewPosLoc = glGetUniformLocation(modelShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        modelShader.setFloat("material.shininess", 32.0f);
        // Draw the model
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        nanosuit.Draw(modelShader);
        
        ////////////////////////////////////////////////////////////////
        // Apply Lighting
        ApplyDirectionalLighting(modelShader);
        ApplyPointLighting(modelShader, pointLightPositions);
        //ApplySpotLighting(modelShader);

        glBindVertexArray(0);
        lampShader.Use();
        GLuint modelLoc = glGetUniformLocation(lampShader.Program, "model");
        GLuint viewLoc = glGetUniformLocation(lampShader.Program, "view");
        GLuint projLoc = glGetUniformLocation(lampShader.Program, "projection");
        // set matricies
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        // Point Lights
        glBindVertexArray(lightVAO);
        
        for(GLuint i = 0; i < pointLightPositions.size(); i++)
        {
            pointLightPositions[i].x += 0.05f * glm::sin(currentFrame + i);
            pointLightPositions[i].y += 0.05f * glm::sin(currentFrame + 2.0f * i);
            pointLightPositions[i].z += 0.05f * glm::cos(currentFrame + i);
            model = glm::mat4();
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.1f)); // shrink lamp to 20% in all directions
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        //////////////////////////////////////////////////////////////
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
    // Deallocate resources
    // here
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    
    return EXIT_SUCCESS;
}

void ApplyDirectionalLighting(Shader &shader)
{
    shader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader.setVec3("dirLight.ambient", glm::vec3(0.05f));
    shader.setVec3("dirLight.diffuse", glm::vec3(0.4f));
    shader.setVec3("dirLight.specular", glm::vec3(0.5f));
}

// Apply lighting calculations to the given model
void ApplyPointLighting(Shader &shader, std::vector<glm::vec3> pointLightPositions)
{
    // Point lights
    for(GLint i = 0; i < pointLightPositions.size(); i++)
    {
        std::string index, pos, amb, dif, spec, con, lin, quad;
        index = std::to_string(i);
        pos = "pointLights[" + index + "].position";
        amb = "pointLights[" + index + "].ambient";
        dif = "pointLights[" + index + "].diffuse";
        spec = "pointLights[" + index + "].specular";
        con = "pointLights[" + index+ "].constant";
        lin = "pointLights[" + index + "].linear";
        quad = "pointLights[" + index + "].quadratic";
        
        shader.setVec3(pos, pointLightPositions[i]);
        shader.setVec3(amb, glm::vec3(0.05f));
        shader.setVec3(dif, glm::vec3(0.8f));
        shader.setVec3(spec, glm::vec3(1.0f));
        shader.setFloat(con, 1.0f);
        shader.setFloat(lin, 0.09f);
        shader.setFloat(quad, 0.0032f);
    }
}

void ApplySpotLighting(Shader &shader)
{
    // Spot Light / flashlight
    shader.setVec3("spotLight.position", camera.GetPosition());
    shader.setVec3("spotLight.direction", camera.GetFront());
    shader.setVec3("spotLight.ambient", glm::vec3(0.0f));
    shader.setVec3("spotLight.diffuse", glm::vec3(1.0f));
    shader.setVec3("spotLight.specular", glm::vec3(1.0f));
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.09f);
    shader.setFloat("spotLight.quadratic", 0.0032f);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}


void DoMovement()
{
    if(keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    
    if(keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    
    if(keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    
    if(keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (key >= 0 && key < 1024) {
        if (GLFW_PRESS == action) {
            keys[key] = true;
        }
        else if (GLFW_RELEASE == action) {
            keys[key] = false;
        }
    }
}


void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
    // use the coordinates of mouse as it enters the screen
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    // Update mouse
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement(xOffset, yOffset);
}


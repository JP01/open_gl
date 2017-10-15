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

// Window dimensions
const GLint WIDTH = 400, HEIGHT = 300;
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
glm::vec3 lightPos(0.0f, 1.5f, 20.0f);

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
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Multiple Lights", nullptr, nullptr);
    
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
    
    // Shaders
    Shader lightingShader("resources/shaders/lighting.vs", "resources/shaders/lighting.frag");
    Shader lampShader("resources/shaders/lamp.vs", "resources/shaders/lamp.frag");
    
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
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };
    
    // Create vertex buffer, array objects, element buffer
    // Box
    GLuint VBO, boxVAO;
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &VBO);
    
    // Bind Vertex Array Object first, then bind and set vertex buffers and attr pointers
    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 *sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 *sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 *sizeof(GLfloat), (GLvoid *) (6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO
    
    // Lamp
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    // Bind Vertex Array Object first, then bind and set vertex buffers and attr pointers
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 *sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0); // Unbind VAO
    
    // Texture ///////////////////////////////
    GLuint diffuseMap, specularMap, emissionMap;
    glGenTextures(1, &diffuseMap);
    glGenTextures(1, &specularMap);
    glGenTextures(1, &emissionMap);
    
    int textureWidth, textureHeight;
    unsigned char *image;
    
    // diffuse map
    image = SOIL_load_image("resources/images/container2.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
    // specular map
    image = SOIL_load_image("resources/images/container2_specular.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    
    lightingShader.Use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    
    ////////////////////////////////////////////////////////////////////////////
    glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (GLfloat)(SCREEN_WIDTH/ SCREEN_HEIGHT), 0.1f, 1000.0f);
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
        
        lightingShader.Use();
        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        lightingShader.setFloat("material.shininess", 32.0f);
        
        // Directional Light
        lightingShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightingShader.setVec3("dirLight.ambient", glm::vec3(0.05f));
        lightingShader.setVec3("dirLight.diffuse", glm::vec3(0.2f));
        lightingShader.setVec3("dirLight.specular", glm::vec3(0.3f));
        
        // Point lights
        for(GLint i = 0; i < sizeof(pointLightPositions)/sizeof(pointLightPositions[0]); i++)
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
            
            lightingShader.setVec3(pos, pointLightPositions[i]);
            lightingShader.setVec3(amb, glm::vec3(0.05f));
            lightingShader.setVec3(dif, glm::vec3(0.4f));
            lightingShader.setVec3(spec, glm::vec3(0.8f));
            lightingShader.setFloat(con, 1.0f);
            lightingShader.setFloat(lin, 0.09f);
            lightingShader.setFloat(quad, 0.0032f);
        }
        
        // Spot Light
        lightingShader.setVec3("spotLight.position", camera.GetPosition());
        lightingShader.setVec3("spotLight.direction", camera.GetFront());
        lightingShader.setVec3("spotLight.ambient", glm::vec3(0.0f));
        lightingShader.setVec3("spotLight.diffuse", glm::vec3(0.5f));
        lightingShader.setVec3("spotLight.specular", glm::vec3(1.0f));
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.0032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(6.0f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(6.5f)));
        
        // Create camera transformations
        glm::mat4 view = camera.GetViewMatrix();
        
        // get uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
        // Pass the matrices to shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Bind diffuse and specular maps
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        
        // Draw the 10 containers
        glm::mat4 model;
        glBindVertexArray(boxVAO);
        for(GLuint i = 0; i < 10; i++)
        {
            model = glm::mat4();
            model = glm::translate(model, cubePositions[i]);
            //glm::vec3 boxPos = glm::vec3(3.0f * sin(currentFrame/2.0f), 0.0f, 3.0f * cos(currentFrame/2.0f));
            //model = glm::translate(model, boxPos);
            model = glm::rotate(model, currentFrame * 2.0f/i, glm::vec3(0.7f, 0.5f, 0.3f));
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        
        // Draw lamp, binding shader
        lampShader.Use();
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc = glGetUniformLocation(lampShader.Program, "view");
        projLoc = glGetUniformLocation(lampShader.Program, "projection");
        // set matricies
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // shrink lamp to 20% in all directions
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        // Point Lights
        glBindVertexArray(lightVAO);
        for(GLuint i = 0; i < sizeof(pointLightPositions)/sizeof(pointLightPositions[0]); i++)
        {
            pointLightPositions[i].x += 0.05f * glm::sin(currentFrame + 2*i);
            pointLightPositions[i].y += 0.05f * glm::sin(currentFrame + i);
            pointLightPositions[i].z += 0.15f * glm::cos(currentFrame - 2*i);
            model = glm::mat4();
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // shrink lamp to 20% in all directions
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
    // Deallocate resources
    glDeleteVertexArrays(1, &boxVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    
    return EXIT_SUCCESS;
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


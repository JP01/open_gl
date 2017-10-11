#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// SOIL
#include "SOIL2/SOIL2.h"

// Shaders
#include "Shader.h"

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

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
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", nullptr, nullptr);
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);  // Adjust width/height for high res screens eg 4k/retina
    
    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE; // glew experimental so we can use newer OpenGL features
    
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        
        return EXIT_FAILURE;
    }
    
    // Define viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Shaders
    Shader ourShader("resources/shaders/core.vs", "resources/shaders/core.frag");
    
    // Create vertex data
    GLfloat vertices[] =
    {
        // position             //color
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, // bottom left, color(r,g,b)
        0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f, // bottom right, color(r,g,b)
        0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f  // top middle, color(r,g,b)
    };
    
    // Create vertex buffer and array objects
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Create vertex pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 *sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 *sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0); // Unbind the vertex array object
    
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Check any window events
        glfwPollEvents();
        
        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ourShader.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0); // unbind vertex array
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
    // Deallocate resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    
    return EXIT_SUCCESS;
}



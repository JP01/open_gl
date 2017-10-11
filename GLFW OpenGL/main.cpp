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
    
    // Depth perception
    glEnable(GL_DEPTH_TEST);
    
    // Enable alpha for texturing
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Shaders
    Shader ourShader("resources/shaders/core.vs", "resources/shaders/core.frag");
    
    // Setup vertex data, buffers and attribute pointers
    // use with Orthographic Projection
    /*
     GLfloat vertices[] = {
     -0.5f * 500, -0.5f * 500, -0.5f * 500,  0.0f, 0.0f,
     0.5f * 500, -0.5f * 500, -0.5f * 500,  1.0f, 0.0f,
     0.5f * 500,  0.5f * 500, -0.5f * 500,  1.0f, 1.0f,
     0.5f * 500,  0.5f * 500, -0.5f * 500,  1.0f, 1.0f,
     -0.5f * 500,  0.5f * 500, -0.5f * 500,  0.0f, 1.0f,
     -0.5f * 500, -0.5f * 500, -0.5f * 500,  0.0f, 0.0f,
     
     -0.5f * 500, -0.5f * 500,  0.5f * 500,  0.0f, 0.0f,
     0.5f * 500, -0.5f * 500,  0.5f * 500,  1.0f, 0.0f,
     0.5f * 500,  0.5f * 500,  0.5f * 500,  1.0f, 1.0f,
     0.5f * 500,  0.5f * 500,  0.5f * 500,  1.0f, 1.0f,
     -0.5f * 500,  0.5f * 500,  0.5f * 500,  0.0f, 1.0f,
     -0.5f * 500, -0.5f * 500,  0.5f * 500,  0.0f, 0.0f,
     
     -0.5f * 500,  0.5f * 500,  0.5f * 500,  1.0f, 0.0f,
     -0.5f * 500,  0.5f * 500, -0.5f * 500,  1.0f, 1.0f,
     -0.5f * 500, -0.5f * 500, -0.5f * 500,  0.0f, 1.0f,
     -0.5f * 500, -0.5f * 500, -0.5f * 500,  0.0f, 1.0f,
     -0.5f * 500, -0.5f * 500,  0.5f * 500,  0.0f, 0.0f,
     -0.5f * 500,  0.5f * 500,  0.5f * 500,  1.0f, 0.0f,
     
     0.5f * 500,  0.5f * 500,  0.5f * 500,  1.0f, 0.0f,
     0.5f * 500,  0.5f * 500, -0.5f * 500,  1.0f, 1.0f,
     0.5f * 500, -0.5f * 500, -0.5f * 500,  0.0f, 1.0f,
     0.5f * 500, -0.5f * 500, -0.5f * 500,  0.0f, 1.0f,
     0.5f * 500, -0.5f * 500,  0.5f * 500,  0.0f, 0.0f,
     0.5f * 500,  0.5f * 500,  0.5f * 500,  1.0f, 0.0f,
     
     -0.5f * 500, -0.5f * 500, -0.5f * 500,  0.0f, 1.0f,
     0.5f * 500, -0.5f * 500, -0.5f * 500,  1.0f, 1.0f,
     0.5f * 500, -0.5f * 500,  0.5f * 500,  1.0f, 0.0f,
     0.5f * 500, -0.5f * 500,  0.5f * 500,  1.0f, 0.0f,
     -0.5f * 500, -0.5f * 500,  0.5f * 500,  0.0f, 0.0f,
     -0.5f * 500, -0.5f * 500, -0.5f * 500,  0.0f, 1.0f,
     
     -0.5f * 500,  0.5f * 500, -0.5f * 500,  0.0f, 1.0f,
     0.5f * 500,  0.5f * 500, -0.5f * 500,  1.0f, 1.0f,
     0.5f * 500,  0.5f * 500,  0.5f * 500,  1.0f, 0.0f,
     0.5f * 500,  0.5f * 500,  0.5f * 500,  1.0f, 0.0f,
     -0.5f * 500,  0.5f * 500,  0.5f * 500,  0.0f, 0.0f,
     -0.5f * 500,  0.5f * 500, -0.5f * 500,  0.0f, 1.0f
     };
    */
    
    // use with Perspective Projection
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    

    // Create vertex buffer, array objects, element buffer
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind Vertex Array Object first, then bind and set vertex buffers and attr pointers
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 *sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    // Texture Coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 *sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0); // Unbind VAO
    
    GLuint texture1;
    
    int width, height;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Wrapping and filtering of currently bound texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load and generate texture
    unsigned char *image = SOIL_load_image("resources/images/wall.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
    if(image){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    // Cleanup
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glm::mat4 projection;
    projection = glm::perspective(45.0f, (GLfloat)(screenWidth/ screenHeight), 0.1f, 1000.0f);
    //projection = glm::ortho(0.0f, (GLfloat)screenWidth, 0.0f, (GLfloat)screenHeight, 0.1f, 1000.0f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Check any window events
        glfwPollEvents();
        
        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
        
        ourShader.Use();
        
        glm::mat4 model;
        glm::mat4 view;
        model = glm::rotate(model, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::rotate(model, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::translate(view,  glm::vec3(0.0f, 0.0f, -3.0f));
        //view = glm::translate(view, glm::vec3(screenWidth/2, screenHeight/2, -700.0f));
        
        
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
    // Deallocate resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    
    return EXIT_SUCCESS;
}

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "shader_s.h"

enum state
{
    leftPaddleCollision, rightPaddleCollision, noCollision, leftGoal, rightGoal
};
typedef enum state State;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void debugPrint(float* xPositionRight, float* yPositionRight, float* xPositionLeft, float* yPositionLeft, float* xPositionBall, float* yPositionBall);

State checkBallCollision(float* xPositionPaddle, float* yPositionPaddle, float* xPositionBall, float*yPositionBall);
void checkBoundsCollision(float* xPositionRight, float* yPositionRight, float* xPositionLeft, float* yPositionLeft, float* xPositionBall, float* yPositionBall);

void ballMovement(float* xPositionBall, float* yPositionBall);

float xPositionRight = 0.95f;
float yPositionRight = 0.0f;

float xPositionLeft = -0.95f;
float yPositionLeft = 0.0f;

float xPositionBall = 0.0f;
float yPositionBall = 0.0f;

int shapeState;
int width = 800;
int height = 600;

float screenRatio = (float)width / (float)height;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800,600, "Pong", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initalize GLAD" << std::endl;
        return -1;
    }

    //include shader files
    Shader ourShader ("shader.vs", "shader.fs");
    //Get location of uniform float/setup position float for CPU
    int position = glGetUniformLocation(ourShader.ID, "position");
    int shapeWho = glGetUniformLocation(ourShader.ID, "shapeWho");
    float aspectScreen = glGetUniformLocation(ourShader.ID, "aspectRatio");

    float vertices[] = 
    {
        0.10f,  0.25f, 0.0f,  // top right
        0.10f, -0.25f, 0.0f,  // bottom right
        -0.10f, -0.25f, 0.0f,  // bottom left
        -0.10f,  0.25f, 0.0f,   // top left 
    
        0.10f, 0.10f, 0.0f,
        0.10f, -0.10f, 0.0f,
        -0.10f, -0.10f, 0.0f,
        -0.10f, 0.10f, 0.0f
    };

    
    unsigned int indices[]
    {
        0, 1, 3, 1, 2, 3, //paddle
        4, 5, 7, 5, 6, 7
    };

    // initialize element buffer object
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //initialize vertex array object and bind it for later use in render loop
    unsigned int VAO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);

    unsigned int VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // unsigned int VAO2;
    // glGenVertexArrays(1, &VAO2);
    // glBindVertexArray(VAO2);

    // //initialize vertex buffer object
    // unsigned int VBO2;
    // glGenBuffers(1, &VBO2);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSquare), verticesSquare, GL_STATIC_DRAW);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);  
    
    debugPrint(&xPositionRight, &yPositionRight, &xPositionLeft, &yPositionLeft, &xPositionBall, &yPositionBall);

    //rendering loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        checkBoundsCollision(&xPositionRight, &yPositionRight, &xPositionLeft, &yPositionLeft, &xPositionBall, &yPositionBall);
        ballMovement(&xPositionBall, &yPositionBall);

        glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();
        glBindVertexArray(VAO);
        glUniform1f(aspectScreen, screenRatio);
        //Paddles
        shapeState = 1;
        glUniform1f(shapeWho, shapeState);
        glUniform2f(position, xPositionRight, yPositionRight);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniform1f(shapeWho, shapeState);
        glUniform2f(position, xPositionLeft, yPositionLeft);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        shapeState = 0;
        glUniform1f(shapeWho, shapeState);
        glUniform2f(position, xPositionBall, yPositionBall);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO1);
    //glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO1);
    //glDeleteBuffers(1, &VBO2);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    screenRatio = (float)width / (float)height;
    glViewport(0,0,width,height);
}

void checkBoundsCollision(float* xPositionRight, float* yPositionRight, float* xPositionLeft, float* yPositionLeft, float* xPositionBall, float* yPositionBall)
{
    if (*yPositionRight >= 0.76f)
    {
        std::cout << "yPositionRight at upper bounds." << std::endl;
        *yPositionRight = 0.75f;
    }
    if (*yPositionRight <= -0.76f)
    {
        std::cout << "yPositionRight at lower bounds." << std::endl;
        *yPositionRight = -0.75f;
    }
    if (*yPositionLeft >= 0.76f)
    {
        std::cout << "yPositionLeft at upper bounds." << std::endl;
        *yPositionLeft = 0.75f;
    }
    if (*yPositionLeft <= -0.76f)
    {
        std::cout << "\nyPositionLeft at lower bounds." << std::endl;
        *yPositionLeft = -0.75f;
    }
    if (*yPositionBall >= 0.76f)
    {
        std::cout << "\nyPositionBall at upper bounds." << std::endl;
        *yPositionBall = 0.75f;
    }
    if (*yPositionBall <= -0.76f)
    {
        std::cout << "\nyPositionBall at lower bounds." << std::endl;
        *yPositionBall = -0.75f;
    }
    if (*xPositionBall >= 0.95f)
    {
        std::cout << "\nGOAL FOR LEFT PADDLE" << std::endl;
        *xPositionBall = 0.00f;
    }
    if (*xPositionBall <= -0.95f)
    {
        std::cout << "\nGOAL FOR RIGHT PADDLE" << std::endl;
        *xPositionBall = 0.00f;
    }
}

State checkBallCollision(float* xPositionPaddle, float* yPositionPaddle, float* xPositionBall, float*yPositionBall)
{
    float result;
    //distance formula
    result = sqrt(pow((*xPositionPaddle - *xPositionBall), 2) - 
             pow((*yPositionPaddle - *yPositionBall), 2));
    /*
    The idea with this collision system is to find the distance between the bvall and a paddle, then if the result is 0, that indicates a collision
    This system works in states, like an elevator.
    */
    if (result <= 0.00f)
    {
        if (*xPositionBall > 0.00f) //right collision
        {
            return rightPaddleCollision;
        }
        return leftPaddleCollision; //assume left collision
    }
    return noCollision;
}

void ballMovement(float* xPositionBall, float* yPositionBall)
{
    float velocity = 0.0f;
    State ballState;

    ballState = checkBallCollision(&xPositionRight, &yPositionRight, xPositionBall , yPositionBall);
    if (ballState == rightPaddleCollision || ballState == leftPaddleCollision)
    {
        velocity *= -1.00f;
    }
    //*xPositionBall += -0.01f;
    //*xPositionBall += velocity;
    //Figure out y-axis collision later
    // if (*yPositionBall > 0.00f) {*yPositionBall += -0.00f;}
    // else {*yPositionBall += 0.01f;}
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        std::cout << "\nClosing Window...\n";
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {yPositionRight += 0.01f;}
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {yPositionRight -= 0.01f;}

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {yPositionLeft -= 0.01f;}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {yPositionLeft += 0.01f;}
}

void debugPrint(float* xPositionRight, float* yPositionRight, float* xPositionLeft, float* yPositionLeft, float* xPositionBall, float* yPositionBall)
{
    std::cout << 
    "\r\033[KxPositionRight: " << *xPositionRight << " | yPositionRight: " << *yPositionRight <<
    " | xPositionLeft: " << *xPositionLeft << "  | yPositionLeft: " << *yPositionLeft <<
    " | xPositionBall: " << *xPositionBall << "  | yPositionBall: " << *yPositionBall << std::flush;
}
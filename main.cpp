#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "shader_s.h"

struct AABB
{
    float minX;
    float maxX;
    float minY;
    float maxY;
};
typedef struct AABB aabb;

struct Paddle
{
    float x;
    float y;
    float minX;
    float maxX;
    float minY;
    float maxY;
    float moveSpeed;
};
typedef struct Paddle paddle;

struct Ball
{
    float x;
    float y;
    float minX;
    float maxX;
    float minY;
    float maxY;
    float moveX;
    float moveY;  
};
typedef struct Ball balls;

int shapeState;
int width = 800;
int height = 600;
float screenRatio = (float)width / (float)height;

// //position offset
// float xPositionRight = 0.95f;
// float yPositionRight = 0.0f;   

// float xPositionLeft = -0.95f;
// float yPositionLeft = 0.0f;

// float xPositionBall = 0.0f;
// float yPositionBall = 0.0f;

float paddles[]
{
    //Paddles
    0.05f,  0.25f, 0.0f,    // top right
    0.05f, -0.25f, 0.0f,    // bottom right
    -0.05f, -0.25f, 0.0f,   // bottom left
    -0.05f,  0.25f, 0.0f,   // top left 
};

float ball[]
{
    //Ball
    0.05f, 0.05f, 0.0f,
    0.05f, -0.05f, 0.0f,
    -0.05f, -0.05f, 0.0f,
    -0.05f, 0.05f, 0.0f
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Paddle& rightPaddle, Paddle& leftPaddle);
void debugPrint(Paddle& rightPaddle, Paddle& leftPaddle, Ball& ball);

void checkBallPaddleCollision(Paddle& rightPaddle, Paddle& leftPaddle,
                              Ball& ball, float previousX);
void checkBallBoundsCollision(Ball& ball);
void updateHitBox(Paddle& rightPaddle, Paddle& leftPaddle, Ball& ball);
bool handleGoal(Ball& ball, int& leftScore, int& rightScore);

void ballMovement(Ball& ball);

int main()
{
    Paddle leftPaddle = 
    {
        -0.95f, //x
        0.0f,   //y
        -1.00f, //minX
        -0.90f, //maxX
        -0.25f, //minY
        0.25f,  //maxY
        0.015f, //movespeed
    };
    Paddle rightPaddle = 
    {
        0.95f,  //x
        0.0f,   //y
        0.90f,  //minX
        1.00f,  //maxX
        -0.25f, //minY
        0.25f,  //maxY
        0.015f, //movespeed
    };
    Ball ball =
    {
        0.0f,   //x
        0.0f,   //y
        -0.05f, //minX
        0.05f,  //maxX
        -0.05f, //minY
        0.05f,  //maxY
        0.01f,  //moveX
        0.01f   //moveY
    };
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
        //Paddles
        0.05f,  0.25f, 0.0f,    // top right
        0.05f, -0.25f, 0.0f,    // bottom right
        -0.05f, -0.25f, 0.0f,   // bottom left
        -0.05f,  0.25f, 0.0f,   // top left 
        //Ball
        0.05f, 0.05f, 0.0f,
        0.05f, -0.05f, 0.0f,
        -0.05f, -0.05f, 0.0f,
        -0.05f, 0.05f, 0.0f
    };

    unsigned int indices[]
    {
        0, 1, 3, 1, 2, 3, //paddle
        4, 5, 7, 5, 6, 7  //ball
    };

    // initialize element buffer object
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //initialize vertex array object and bind it for later use in render loop
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

    int leftScore = 0;
    int rightScore = 0;
    bool debugEnabled = false;
    bool dWasPressed = false;
    double lastDebugPrint = -0.25;

    //rendering loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window, rightPaddle, leftPaddle);

        const bool dPressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        if (dPressed && !dWasPressed)
            debugEnabled = !debugEnabled;
        dWasPressed = dPressed;

        const float previousX = ball.x;
        ballMovement(ball);
        updateHitBox(rightPaddle, leftPaddle, ball);

        checkBallBoundsCollision(ball);
        updateHitBox(rightPaddle, leftPaddle, ball);

        checkBallPaddleCollision(rightPaddle, leftPaddle, ball, previousX);
        updateHitBox(rightPaddle, leftPaddle, ball);

        if (handleGoal(ball, leftScore, rightScore))
            updateHitBox(rightPaddle, leftPaddle, ball);

        const double now = glfwGetTime();
        if (debugEnabled && now - lastDebugPrint >= 0.25)
        {
            debugPrint(rightPaddle, leftPaddle, ball);
            lastDebugPrint = now;
        }

        glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();
        glBindVertexArray(VAO);
        glUniform1f(aspectScreen, screenRatio);
        //Paddles
        shapeState = 1;
        glUniform1f(shapeWho, shapeState);
        glUniform2f(position, rightPaddle.x, rightPaddle.y);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniform1f(shapeWho, shapeState);
        glUniform2f(position, leftPaddle.x, leftPaddle.y);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //Ball
        shapeState = 0;
        glUniform1f(shapeWho, shapeState);
        glUniform2f(position, ball.x, ball.y);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    //glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &VBO2);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    if (width > 0 && height > 0)
        screenRatio = static_cast<float>(width) / static_cast<float>(height);
    glViewport(0,0,width,height);
}

void checkBallBoundsCollision(Ball& ball)
{
    const float halfHeight = 0.05f / screenRatio;

    if (ball.maxY >= 1.0f)
    {
        ball.y = 1.0f - halfHeight;
        ball.moveY = -std::abs(ball.moveY);
    }
    else if (ball.minY <= -1.0f)
    {
        ball.y = -1.0f + halfHeight;
        ball.moveY = std::abs(ball.moveY);
    }
}

void checkBallPaddleCollision(Paddle& rightPaddle, Paddle& leftPaddle,
                              Ball& ball, float previousX)
{
    const float halfWidth = 0.05f / screenRatio;

    // Only a crossing of the paddle's front face can save the ball.
    if (ball.moveX > 0.0f &&
        previousX + halfWidth <= rightPaddle.minX &&
        ball.maxX >= rightPaddle.minX &&
        ball.maxY >= rightPaddle.minY &&
        ball.minY <= rightPaddle.maxY)
    {
        ball.x = rightPaddle.minX - halfWidth;
        ball.moveX = -ball.moveX;
    }
    else if (ball.moveX < 0.0f &&
             previousX - halfWidth >= leftPaddle.maxX &&
             ball.minX <= leftPaddle.maxX &&
             ball.maxY >= leftPaddle.minY &&
             ball.minY <= leftPaddle.maxY)
    {
        ball.x = leftPaddle.maxX + halfWidth;
        ball.moveX = -ball.moveX;
    }
}

bool handleGoal(Ball& ball, int& leftScore, int& rightScore)
{
    const bool rightGoal = ball.minX > 1.0f;
    const bool leftGoal = ball.maxX < -1.0f;

    if (!rightGoal && !leftGoal)
        return false;

    if (rightGoal)
        ++leftScore;
    else
        ++rightScore;

    std::cout << (rightGoal ? "Left" : "Right")
              << " scored | Left: " << leftScore
              << " | Right: " << rightScore << std::endl;

    ball.x = 0.0f;
    ball.y = 0.0f;
    ball.moveX = rightGoal ? 0.01f : -0.01f;
    ball.moveY = 0.01f;

    return true;
}

void ballMovement(Ball& ball)
{
    ball.x += ball.moveX;
    ball.y += ball.moveY;
}

void updateHitBox(Paddle& rightPaddle, Paddle& leftPaddle, Ball& ball)
{
    const float halfWidth = 0.05f / screenRatio;
    const float paddleHalfHeight = 0.25f / screenRatio;

    rightPaddle.minX = rightPaddle.x - halfWidth;
    rightPaddle.maxX = rightPaddle.x + halfWidth;
    rightPaddle.minY = rightPaddle.y - paddleHalfHeight;
    rightPaddle.maxY = rightPaddle.y + paddleHalfHeight;

    leftPaddle.minX = leftPaddle.x - halfWidth;
    leftPaddle.maxX = leftPaddle.x + halfWidth;
    leftPaddle.minY = leftPaddle.y - paddleHalfHeight;
    leftPaddle.maxY = leftPaddle.y + paddleHalfHeight;

    ball.minX = ball.x - halfWidth;
    ball.maxX = ball.x + halfWidth;
    ball.minY = ball.y - halfWidth;
    ball.maxY = ball.y + halfWidth;
}

void processInput(GLFWwindow *window, Paddle& rightPaddle, Paddle& leftPaddle)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        std::cout << "\nClosing Window...\n";
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rightPaddle.y += rightPaddle.moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rightPaddle.y -= rightPaddle.moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        leftPaddle.y += leftPaddle.moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        leftPaddle.y -= leftPaddle.moveSpeed;

    const float limit = std::max(0.0f, 1.0f - 0.25f / screenRatio);
    rightPaddle.y = std::clamp(rightPaddle.y, -limit, limit);
    leftPaddle.y = std::clamp(leftPaddle.y, -limit, limit);
}

void debugPrint(Paddle& rightPaddle, Paddle& leftPaddle, Ball& ball)
{
    std::cout << "DEBUG | Left Y: " << leftPaddle.y
              << " | Right Y: " << rightPaddle.y
              << " | Ball: (" << ball.x << ", " << ball.y << ')'
              << " | Movement: (" << ball.moveX << ", " << ball.moveY << ')'
              << std::endl;
}

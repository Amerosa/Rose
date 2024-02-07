#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <vector>
#include <numbers>
#include <iostream>

#include "shader.h"

using Eigen::Matrix3d;
using Eigen::Vector2d;
using Eigen::RowVector3f;

inline float deg2rad(float degree) {
    return degree * (std::numbers::pi / 180);
}

Eigen::Matrix4f getPerspectiveTransform(float fov, float aspect, float near, float far) {
    Eigen::Matrix4f projectionMat = Eigen::Matrix4f::Identity();
    float theta = fov * 0.5;
    float range = far - near;
    float invtan = 1. / std::tan(theta);

    projectionMat(0, 0) = invtan / aspect;
    projectionMat(1, 1) = invtan;
    projectionMat(2, 2) = -(near + far) / range;
    projectionMat(3, 2) = -1;
    projectionMat(2, 3) = (-2 * near * far) / range;
    projectionMat(3, 3) = 0;
    return projectionMat;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    std::vector<RowVector3f> points = {
        RowVector3f(0.5, 0.5, 0.0),  RowVector3f(1., 1., 1.),
        RowVector3f(0.5, -0.5, 0.),  RowVector3f(1., 0, 0),
        RowVector3f(-0.5, -0.5, 0.), RowVector3f(0, 1., 0),
        RowVector3f(-0.5, 0.5, 0.),  RowVector3f(0, 0, 1.) };

    std::vector<RowVector3f> simpleTriangle = {
        RowVector3f(0.5, 0.5, 0.0),  RowVector3f(1., 0, 0),
        RowVector3f(0.5, -0.5, 0.),  RowVector3f(0., 1, 0),
        RowVector3f(-0.5, -0.5, 0.), RowVector3f(0, 0., 1 )};

    std::vector<RowVector3f> tripleTriangle = {
        RowVector3f(-0.5, 0., 0.),
        RowVector3f(0., 1.0, 0.),
        RowVector3f(0.5, 0., 0.),
        RowVector3f(1., -1.0, 0.),
        RowVector3f(0., -1.0, 0.),
        RowVector3f(-1.0, -1.0, 0.)};

    std::vector<unsigned int> indices = { 0, 1, 2,
                                          2, 3, 0 };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(RowVector3f), points.data(), GL_STATIC_DRAW);

    GLuint vao; 
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    int success;
    char infoLog[512];

    Shader vert("./src/vertex.glsl", GL_VERTEX_SHADER);
    Shader frag("./src/fragment.glsl", GL_FRAGMENT_SHADER);


    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert.ID);
    glAttachShader(shaderProgram, frag.ID);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) std::cout << infoLog << "\n";

    glDeleteShader(vert.ID);
    glDeleteShader(frag.ID);

    glBindVertexArray(0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        using namespace Eigen;
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        //glBindBuffer(GL_ARRAY_BUFFER, vbo); //second call is just to update in larger programs
        glUseProgram(shaderProgram);
        
        auto axis = Eigen::Vector3f(0.5, 1.0, 0.0).normalized();
        auto model = Eigen::Affine3f(Eigen::AngleAxisf( glfwGetTime() * deg2rad(50), axis)).matrix();
        auto view =  Eigen::Affine3f(Eigen::Translation3f(Eigen::Vector3f(0, 0, -3))).matrix();

        Matrix4f proj = getPerspectiveTransform(deg2rad(45.0f), 800. / 600., 0.1, 100.);
        Matrix4f transform = proj * view * model;

        int tloc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(tloc, 1, GL_FALSE, transform.data());

        glBindVertexArray(vao);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
    }

    glfwTerminate();
    return 0;
}




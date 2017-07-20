#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

#define GLFW_INCLUDE_GLU
#include <glfw3.h>
#include <glut.h>
#include <string.h>
#include <SOIL.h>

using namespace std;

#define WIDTH 512
#define HEIGHT 400

#define BLOCK_SIZE 8

void update(float delta);
void draw();
void input(GLFWwindow* window, int key, int scancode, int action, int mods);

int init(const char *title);
void render();
void destroy();
void drawString(string s, int x, int y);
void drawImage(GLuint img, int x, int y, int w, int h);

GLuint loadImage(const char *path);
void unloadImage(GLuint tex);

#endif // WINDOW_H

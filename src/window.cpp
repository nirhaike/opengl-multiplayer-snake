#include <window.h>

static GLFWwindow *window;

static double lastTime = 0.0;
static double currentTime = 0.0;
static float delta = 0.1f;

static bool running = true;

int init(const char *title) {
    if (!glfwInit()) {
        return 0;
    }
    int argc = 1;
    char *argv[1] = {(char*)"Something"};
    glutInit(&argc, argv);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // create the window
    window = glfwCreateWindow(WIDTH, HEIGHT, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    glViewport(0, 0, WIDTH, HEIGHT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, 0, 1);

    return 1;
}

void render() {
    while (!glfwWindowShouldClose(window) && running) {
        lastTime = glfwGetTime();
        // swap front & back buffers
        glfwSwapBuffers(window);
        // process events
        glfwPollEvents();
        // set keyboard callback
        glfwSetKeyCallback(window, input);
        // update the game
        update(delta);
        // draw the screen
        draw();
        currentTime = glfwGetTime();
        delta = (float)(currentTime - lastTime);
    }
}

void quit() {
    running = false;
}

void destroy() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void drawString(string s, int x, int y) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, WIDTH, HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x, y);
    for (string::iterator i = s.begin(); i != s.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void drawImage(GLuint img, int x, int y, int w, int h) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, img);
    glBegin (GL_QUADS);
    glTexCoord2d(0.0,0.0); glVertex2d(x, y);
    glTexCoord2d(1.0,0.0); glVertex2d(x+w, y);
    glTexCoord2d(1.0,1.0); glVertex2d(x+w, y+h);
    glTexCoord2d(0.0,1.0); glVertex2d(x, y+h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

GLuint loadImage(const char *path) {
    GLuint tex = SOIL_load_OGL_texture(
		path,
		SOIL_LOAD_RGBA,
		SOIL_CREATE_NEW_ID,
		0
	);
	if (tex == 0) {
	    string msg = "Can't load image ";
        throw msg + path;
	}
    return tex;
}

void unloadImage(GLuint img) {
    glDeleteTextures(1, &img);
}

#include <window.h>
#include <net.h>
#include <snake.h>

using namespace std;

//GLuint tex;
Snake *snake;
float time = 0;
bool enlarge = false;

bool gameOver = false;

void drawGrid();

int main() {
    /*
    char *buff = new char[256];
    Net *net = new Net("127.0.0.1", 1234);
    if (net->connect() != 0) {
        cout << "Can't connect!" << endl;
        delete net;
        return 0;
    }
    net->setNonBlocking();
    if (net->send("Hello World\n", 13) != 0) {
        cout << "Can't send!" << endl;
        delete net;
        return 0;
    }
    int len = net->recv(buff, 256);
    while (len <= 0) {
        cout << len << endl;
        len = net->recv(buff, 256);
    }
    // we received the socket
    cout << buff << endl;

    net->close();
    delete net;
    */

    if (!init("Hello World")) {
        return -1;
    }
    //tex = loadImage("img.png");
    snake = new Snake(100, 10, 10);
    render();
    destroy();
    delete snake;
    //unloadImage(tex);

    return 0;
}

void update(float delta) {
    time += delta;
    // update snake
    if (time >= 0.3f) {
        if (enlarge) {
            snake->enlarge();
            enlarge = false;
        } else {
            snake->update();
        }
        time = 0;
    }
    // collision check
    if (snake->collides(1, snake->getLength(), snake->getX(), snake->getY())) {
        gameOver = true;
    }
}

void input(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS)
        return;
    switch (key) {
    case GLFW_KEY_UP:
        snake->setDirection(UP);
        break;
    case GLFW_KEY_DOWN:
        snake->setDirection(DOWN);
        break;
    case GLFW_KEY_LEFT:
        snake->setDirection(LEFT);
        break;
    case GLFW_KEY_RIGHT:
        snake->setDirection(RIGHT);
        break;
    case GLFW_KEY_A:
        enlarge = true;
        break;
    }
}

void draw() {
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawGrid();
    if (!gameOver) {
        snake->draw(0, 0);
    } else {
        glColor3f(0.0f, 0.0f, 0.0f);
        drawString("Game Over!", 0, 30);
    }
    //glColor3f(1.0, 1.0, 1.0);
    //drawImage(tex, 100, 70, 96, 96);
}

void drawGrid() {
    glLineWidth(1);
    // draw the grid
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    int x, y;
    for (y = 0; y < HEIGHT; y += BLOCK_SIZE) {
        glVertex2f(0, y);
        glVertex2f(WIDTH, y);
    }
    for (x = 0; x < WIDTH; x += BLOCK_SIZE) {
        glVertex2f(x, 0);
        glVertex2f(x, HEIGHT);
    }
    glEnd();
}

#include <window.h>
#include <game.h>
#include <net.h>
#include <snake.h>

using namespace std;

Net *net;

game_info_t *game_info;
Snake *snake;

float time = 0;
bool enlarge = false;

bool gameStarted = false;
bool gameOver = false;

void drawGrid();
void drawGame();
void updateGame(float delta);

int main() {
    // get the player's name
    char *name = new char[50];
    cin >> name;
    cout << strlen(name) << endl;
    // connect to the server
    char *buff = new char[256];
    net = new Net("127.0.0.1", 1234);
    if (net->connect() != 0) {
        cout << "Can't connect!" << endl;
        delete net;
        return 0;
    }
    cout << "Connected!" << endl;
    // send the player's name
    if (net->send(name, strlen(name)+1) != 0) {
        cout << "Can't send!" << endl;
        delete net;
        return 0;
    }
    // receive game info
    int len = net->recv(buff, 256);
    if (len < 3) {
        cout << "Invalid handshake message: " << len << endl;
        delete net;
        return 0;
    }
    game_info = init_game_info(buff[0], buff[1], buff[2]);
    // set the socket connection to be non-blocking
    net->setNonBlocking();
    // open the game's screen
    if (!init("Hello World")) {
        return -1;
    }
    //tex = loadImage("img.png");
    snake = new Snake(name, 100, 10, 10);
    render();
    // close
    destroy();
    net->close();
    delete net;
    delete buff;
    delete snake;
    delete game_info;
    //unloadImage(tex);

    return 0;
}

void update(float delta) {
    if (gameStarted) {
        updateGame(delta);
    } else {

    }
}

void updateGame(float delta) {
    if (!gameOver) {
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
}

void draw() {
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (gameStarted) {
        drawGame();
    }
    //drawImage(tex, 100, 70, 96, 96);
}

void drawGame() {
    if (!gameOver) {
        snake->draw(0, 0);
        drawGrid();
    } else {
        drawGrid();
        glColor3f(1.0, 1.0, 1.0);
        drawString("Game Over!", 0, 30);
    }
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

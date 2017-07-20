#include <window.h>
#include <net.h>
#include <snake.h>
#include <apple.h>
#include <game.h>

#define MAX_ENEMIES 5
#define SNAKE_MAX_LEN 512
#define NUM_OF_PLAYERS_INDEX 21

using namespace std;
using namespace game;

Net *net;

game_info_t *game_info;
Snake *snake;

Snake **enemies;
int numEnemies;

Apple **apples;
int numApples;

color_t COLORS_ENEMY[] = {{0.1f, 0.1f, 0.1f}};

char numPlayersString[] = "- Number of players: 1";

float time = 0;
bool enlarge = false;

bool gameStarted = false;
bool gameOver = false;

void drawGrid();
void drawGame();
void updateGame(float delta);
void drawApples();
void addEnemy(char *name, int nameSize, int x, int y);

int main() {
    /*
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
    */
    // open the game's screen
    if (!init("Hello World")) {
        return -1;
    }
    //tex = loadImage("img.png");
    snake = new Snake("Nir", SNAKE_MAX_LEN, 10, 10, 3); //name
    // create instances for the apples
    numApples = 5;
    apples = new Apple*[numApples];
    for (int i = 0; i < numApples; i++) {
        apples[i] = new Apple((i * 5 + i/2) % WIDTH, (i * 7 - i/2) % HEIGHT);
    }
    // create the enemies list
    enemies = new Snake*[MAX_ENEMIES];
    numEnemies = 0;
    addEnemy("Thomas", 6, 51, 23);
    // start the waiting room
    gameStarted = false;
    render();
    // close
    destroy();
    /*
    net->close();
    delete net;
    delete buff;
    */
    delete snake;
    delete apples;
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
        if (time >= 0.1f) {
            // apple eat check
            for (int i = 0; i < numApples; i++) {
                if (apples[i]->canEat(snake->getX()[0], snake->getY()[0])) {
                    apples[i]->movePosition(-1, -1);
                    enlarge = true;
                }
            }
            if (enlarge) {
                    cout << "enlarge" << endl;
                snake->enlarge();
                enlarge = false;
            } else {
                cout << "update" << endl;
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
    } else {
        glColor3f(1.0, 1.0, 1.0);
        drawString("WAITING ROOM", 0, 25);
        glColor3f(0.7, 0.7, 0.7);
        drawString(numPlayersString, 0, 50);
        color_t *col = snake->getColor();
        glColor3f(col->r, col->g, col->b);
        glRectd(5, 58, 10, 63);
        drawString(snake->getName(), 15, 65);
        for (int i = 0; i < numEnemies; i++) {
            int y = 80 + 13*i;
            col = enemies[i]->getColor();
            glColor3f(col->r, col->g, col->b);
            glRectd(5, y-7, 10, y-2);
            drawString(enemies[i]->getName(), 15, y);
        }
    }
    //drawImage(tex, 100, 70, 96, 96);
}

void drawGame() {
    if (!gameOver) {
        drawApples();
        snake->draw(0, 0);
        drawGrid();
    } else {
        drawApples();
        drawGrid();
        glColor3f(1.0, 1.0, 1.0);
        drawString("Game Over!", 0, 30);
    }
}

void drawApples() {
    for (int i = 0; i < numApples; i++) {
        apples[i]->draw(0, 0);
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

void addEnemy(char *name, int nameSize, int x, int y) {
    if (numEnemies >= MAX_ENEMIES)
        return;
    char *c = new char[nameSize+1];
    for (int i = 0; i < nameSize; i++) {
        c[i] = name[i];
    }
    enemies[numEnemies] = new Snake(c, SNAKE_MAX_LEN, x, y, 13);
    enemies[numEnemies]->setColor(1, 0, 0);
    numEnemies++;
    numPlayersString[NUM_OF_PLAYERS_INDEX]++;
}

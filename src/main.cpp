#include <window.h>
#include <net.h>
#include <snake.h>
#include <apple.h>
#include <game.h>

#define MAX_ENEMIES 4
#define SNAKE_MAX_LEN 255
#define NUM_OF_PLAYERS_INDEX 21
#define BUFF_SIZE 256
#define MAX_SPARKLES ((MAX_ENEMIES+1)*SNAKE_MAX_LEN)

#define OPCODE_QUIT_GAME   0x00
#define OPCODE_MOVE        0x01
#define OPCODE_EAT_APPLE   0x02
#define OPCODE_DEFEATED    0x03
#define OPCODE_EAT_SPARKLE 0x04

using namespace std;
using namespace game;

Net *net;
char *buff = new char[BUFF_SIZE];

game_info_t *game_info;
Snake *snake;

Snake **enemies;
int numEnemies;

Apple **apples;
int numApples;

Apple **sparkles;
int currSparkles = 0;

color_t COLORS_ENEMY[] = {{0.95f, 0.45f, 0.95f}, {0.45f, 0.95f, 0.95f}, {0.95f, 0.45f, 0.45f}, {0.45f, 0.45f, 0.95f}};

char numPlayersString[] = "- Number of players: 1";

float time = 0;
bool enlarge = false;

bool gameStarted = false;
bool gameOver = false;

void drawGrid();
void drawGame();
void updateWaitRoom(float delta);
void updateGame(float delta);
void drawApples();
void addEnemy(char *name, int nameSize, int x, int y);
int addPlayer(char *data, bool isEnemy);
Snake *getSnakeByIndex(char index);
Apple *getSparkleByInfo(char snake, unsigned char ind);
bool removeSnakeByIndex(char index);
void removeSparkleByInfo(char snake, unsigned char ind);

int main() {
    // get the player's name (TODO make the size between 3-9)
    char *name = new char[50];
    cin >> name;
    cout << strlen(name) << endl;
    // connect to the server
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
    int len = net->recv(buff, BUFF_SIZE);
    if (len < 3) {
        cout << "Invalid handshake message: " << len << endl;
        delete net;
        return 0;
    }
    game_info = init_game_info(buff[0], buff[1], buff[2], buff[3]);
    // open the game's screen
    if (!init("Hello World")) {
        return -1;
    }
    //tex = loadImage("img.png");
    //snake = new Snake("Nir", SNAKE_MAX_LEN, 10, 10, 3); //name
    // create instances for the apples
    numApples = game_info->amount_apples;
    apples = new Apple*[numApples];
    sparkles = new Apple*[MAX_SPARKLES];
    for (int i = 0; i < numApples; i++) {
        apples[i] = new Apple((i * 5 + i/2) % WIDTH, (i * 7 - i/2) % HEIGHT);
    }
    // create the enemies list
    enemies = new Snake*[MAX_ENEMIES];
    numEnemies = 0;
    // start the waiting room
    gameStarted = false;
    // add players
    len = net->recv(buff, BUFF_SIZE);
    if (len < 2) {
        cout << "Invalid message size: " << len << endl;
        quit();
    } else {
        int index = 1;
        // add enemies
        for (int i = 0; i < buff[0]-1; i++) {
            addPlayer(buff+index, true);
            index += 4 + buff[index+1];
        }
        // add our player
        addPlayer(buff+index, false);
    }
    // add timeout for the socket receiving function
    //net->setTimeout(1);
    // set the receive call to be non-blocking
    net->setNonBlocking();
    // start rendering the game
    render();
    // close
    destroy();
    net->close();
    delete net;
    delete buff;
    delete snake;
    delete apples;
    delete game_info;
    //unloadImage(tex);
    return 0;
}

int handleByOpcode(char opcode, char *data) {
    char x;
    char y;
    char dx;
    char dy;
    int appleId;
    char snakeId;
    unsigned char sparkleIndex;
    int used;
    int side;
    Snake *s;
    Apple *sparkle;

    switch (opcode) {
    // player disconnection
    case 0:
        // try to remove an enemy or quit if the index is ours
        if (!removeSnakeByIndex(data[0]) && snake->getIndex()==data[0]) {
            cout << "QUIT1" << endl;
            quit();
        }
        return 2;
    // player connection
    case 0x01:
        used = addPlayer(data, true);
        return used+1;
    // game server shutdown
    case 0x02:
        cout << "QUIT2" << endl;
        quit();
        return 1;
    // start game
    case 0x03:
        gameStarted = true;
        cout << "Game started!" << endl;
        time = 0;
        return 1;
    // movement
    case 0x10:
        s = getSnakeByIndex(data[0]);
        dx = data[1];
        dy = data[2];
        side = data[3];
        s->setDirection((direction)side);
        s->moveAll(dx, dy);
        return 5;
    // apple was eaten by an enemy
    case 0x11:
        s = getSnakeByIndex(data[0]);
        appleId = data[1];
        dx = (apples[(int)appleId]->getX() - s->getX()[0]) % (WIDTH/BLOCK_SIZE);
        dy = (apples[(int)appleId]->getY() - s->getY()[0]) % (HEIGHT/BLOCK_SIZE);
        apples[(int)appleId]->movePosition(-1, -1);
        s->enlarge();
        s->moveAll(dx, dy);
        return 3;
    // apple was moved
    case 0x12:
        cout << "A";
        appleId = data[0];
        x = data[1];
        y = data[2];
        cout << "B";
        apples[appleId]->movePosition(x, y);
        cout << "C";
        return 4;
    // sparkle was eaten
    case 0x13:
        s = getSnakeByIndex(data[0]);
        snakeId = data[1];
        sparkleIndex = data[2];
        sparkle = getSparkleByInfo(snakeId, sparkleIndex);
        dx = (sparkle->getX() - s->getX()[0]) % (WIDTH/BLOCK_SIZE);
        dy = (sparkle->getY() - s->getY()[0]) % (HEIGHT/BLOCK_SIZE);
        removeSparkleByInfo(snakeId, sparkleIndex);
        s->enlarge();
        s->moveAll(dx, dy);
        return 4;
    }
    return -1;
}

void update(float delta) {
    if (gameStarted) {
        cout << "Updating game!" << endl;
        updateGame(delta);
    } else {
        updateWaitRoom(delta);
    }
}

void recv() {
    int len = net->recv(buff, BUFF_SIZE);
    int used;
    char *buffPtr = buff;
    while (len > 0 && used != -1) {
        cout << "Got opcode " << (int)buffPtr[0] << "  " << len << endl;
        used = handleByOpcode(buffPtr[0], buffPtr+1);
        buffPtr += used;
        len -= used;
    }
}

void updateWaitRoom(float delta) {
    time += delta;
    if (time > game_info->wait_time && numEnemies == 0) {
        cout << "QUIT3" << endl;
        quit();
    } else {
        recv();
        cout << "Done!" << endl;
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
                    // update the server
                    char msg[] = {OPCODE_EAT_APPLE, (char)i, 0};
                    net->send(msg, 3);
                }
            }
            for (int i = 0; i < currSparkles; i++) {
                if (sparkles[i]->canEat(snake->getX()[0], snake->getY()[0])) {
                    char sparkleSnake = (char) sparkles[i]->getSparkleSnake();
                    unsigned char sparkleIndex = (unsigned char)sparkles[i]->getSparkleIndex();
                    removeSparkleByInfo(sparkleSnake, sparkleIndex);
                    enlarge = true;
                    // update the server
                    char msg[] = {OPCODE_EAT_SPARKLE, sparkleSnake, (char)sparkleIndex, 0};
                    net->send(msg, 4);
                }
            }
            if (enlarge) {
                snake->enlarge();
                enlarge = false;
            } else {
                // update the server
                int dx = 0;
                int dy = 0;
                if (snake->getDirection() == UP) {dy = -1;}
                if (snake->getDirection() == DOWN) {dy = 1;}
                if (snake->getDirection() == LEFT) {dx = -1;}
                if (snake->getDirection() == RIGHT) {dx = 1;}
                char msg[] = {OPCODE_MOVE, (char)dx, (char)dy, (char)snake->getDirection(), 0};
                net->send(msg, 5);
                // update the user interface
                snake->update();
            }
            time = 0;
        }
        // collision check
        bool collision = snake->collides(1, snake->getLength(), snake->getX(), snake->getY());
        for (int i = 0; i < numEnemies; i++) {
            collision = collision || snake->collides(0, enemies[i]->getLength(), enemies[i]->getX(), enemies[i]->getY());
        }
        if (collision) {
            // update the server
            char msg[] = {OPCODE_DEFEATED, 0};
            net->send(msg, 2);
            // update the user interface
            gameOver = true;
        }
        // receive data from the server
        recv();
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
        for (int i = 0; i < numEnemies; i++) {
            enemies[i]->draw(0, 0);
        }
        drawGrid();
    } else {
        drawApples();
        for (int i = 0; i < numEnemies; i++) {
            enemies[i]->draw(0, 0);
        }
        drawGrid();
        glColor3f(1.0, 1.0, 1.0);
        drawString("Game Over!", 0, 30);
    }
}

void drawApples() {
    for (int i = 0; i < numApples; i++) {
        apples[i]->draw(0, 0);
    }
    for (int i = 0; i < currSparkles; i++) {
        sparkles[i]->draw(0, 0);
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
    //case GLFW_KEY_A:
    //    enlarge = true;
    //    break;
    }
}

void addEnemy(char *name, int nameSize, int x, int y, char index) {
    if (numEnemies >= MAX_ENEMIES)
        return;
    char *c = copyPointer(name, nameSize);
    enemies[numEnemies] = new Snake(c, SNAKE_MAX_LEN, x, y, game_info->snake_initial_size);
    color_t col = COLORS_ENEMY[numEnemies];
    enemies[numEnemies]->setColor(col.r, col.g, col.b);
    enemies[numEnemies]->setIndex(index);
    numEnemies++;
    numPlayersString[NUM_OF_PLAYERS_INDEX]++;
}

int addPlayer(char *data, bool isEnemy) {
    char index = data[0];
    int nameLen = data[1];
    char *name = copyPointer(&data[2], nameLen);
    int x = data[2+nameLen];
    int y = data[3+nameLen];
    if (isEnemy) {
        addEnemy(name, nameLen, x, y, index);
    } else {
        snake = new Snake(name, SNAKE_MAX_LEN, x, y, game_info->snake_initial_size);
        snake->setIndex(index);
    }
    return 4+nameLen;
}

Snake *getSnakeByIndex(char index) {
    if (snake->getIndex() == index) {
        return snake;
    }
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i]->getIndex() == index) {
            return enemies[i];
        }
    }
    return NULL;
}

void makeSparkles(Snake *s) {
    for (int i = 0; i < s->getLength(); i++) {
        int x = s->getX()[i];
        int y = s->getY()[i];
        Apple *a = new Apple(x, y);
        a->toSparkle(s->getIndex(), i);
        sparkles[currSparkles++] = a;
    }
}

Apple *getSparkleByInfo(char snake, unsigned char ind) {
    int i;
    for (i = 0; i < currSparkles; i++) {
        if (sparkles[i]->getSparkleSnake() == snake && sparkles[i]->getSparkleIndex() == ind) {
            return sparkles[i];
        }
    }
    return NULL;
}

void removeSparkleByInfo(char snake, unsigned char ind) {
    int i;
    for (i = 0; i < currSparkles; i++) {
        if (sparkles[i]->getSparkleSnake() == snake && sparkles[i]->getSparkleIndex() == ind) {
            break;
        }
    }
    for (; i < currSparkles-1; i++) {
        sparkles[i] = sparkles[i+1];
    }
    sparkles[i] = NULL;
    currSparkles--;
}

/** returns whether the operation has been completed */
bool removeSnakeByIndex(char index) {
    if (snake->getIndex() == index) {
        return false;
    }
    int i;
    for (i = 0; i < numEnemies; i++) {
        if (enemies[i]->getIndex() == index) {
            if (gameStarted && !gameOver) {
                // turn the snake into sparkles
                makeSparkles(enemies[i]);
            }
            break;
        }
    }
    for (; i < numEnemies-1; i++) {
        enemies[i] = enemies[i+1];
    }
    enemies[numEnemies-1] = NULL;
    numEnemies--;
    return true;
}

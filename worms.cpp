#include "Game.h"

int main(int, char **) {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    const int CHARACTER_COUNT = 2;

    Game game(SCREEN_WIDTH, SCREEN_HEIGHT, CHARACTER_COUNT);
    game.run();
    return 0;
}

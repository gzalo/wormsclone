#include "Game.h"

int main(int, char **) {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    const int CHARACTER_COUNT = 2;

    Game game(SCREEN_WIDTH, SCREEN_HEIGHT, CHARACTER_COUNT);
    game.run();
    return 0;
}

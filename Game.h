#ifndef GAME_H
#define GAME_H

#include "graphics.h"
#include "Character.h"
#include "Bullet.h"
#include "CollisionManager.h"
#include <list>
#include <vector>
#include <map>

using namespace std;

class Game {

public:
    Game(int width, int height, int charCount);

    void run();

private:
    bool frontImgNeedsUpdate = true;
    list <Bullet> bullets;
    vector<Character> characters;
    CollisionManager collisionManager = CollisionManager(characters);
    const SDL_Surface *front;
    const int width;
    SDL_Window *window;
    GLuint frontImgId;
    GLuint wormImgId;
    GLuint bulletImgId;
    GLuint backgroundImgId;
    bool quit = false;
    int mcam = 0;
    int xoff = 0;

    bool handleKeyDown(const SDL_Event &event);

    void handleKeyUp(const SDL_Event &event);

    [[nodiscard]] int handleMouseMotion(const SDL_Event &event) const;

    void updateTitle();

    void drawCharacter(const Character &character) const;

    void updateBullets();

    void fireBullets(Character &character);
};

#endif

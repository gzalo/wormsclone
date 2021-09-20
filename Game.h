#ifndef GAME_H
#define GAME_H

#include "graphics.h"
#include "Character.h"
#include "Bullet.h"
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
    map <int, SDL_Surface*> surfaces;
    list <Bullet> balas;
    vector <Character> personajes;
    bool mov = false;
    const SDL_Surface *front;
    const int width;
    const int height;
    SDL_Window *window;
    GLuint frontImgId;
    GLuint gusanoImgId;
    GLuint balaImgId;
    GLuint fondoImgId;

    bool handleKeyDown(const SDL_Event &event);

    void handleKeyUp(const SDL_Event &event);

    int handleMouseMotion(const SDL_Event &event);

    void updateTitle();

    void drawCharacter(const Character &personaje) const;
};

#endif

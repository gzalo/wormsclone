#ifndef BULLET_H
#define BULLET_H

#include "graphics.h"
#include "Character.h"
#include <vector>
class Character;
using namespace std;

class Bullet {
public:
    Bullet(float x, float y, float vx, float vy, int ow) : x(x), y(y), vx(vx), vy(vy), owner(ow) { }
    bool update(const SDL_Surface *front, bool &updated, vector<Character> &characters);
    [[nodiscard]] float getX() const;
    [[nodiscard]] float getY() const;

private:
    float x;
    float y;
    float vx;
    float vy;
    int owner;
    bool collidesWith(Character &character) const;
};

#endif

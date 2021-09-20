#ifndef BULLET_H
#define BULLET_H

#include "graphics.h"
#include "Character.h"
#include <vector>

class Character;

using namespace std;

class Bullet {
public:
    Bullet(double x, double y, double vx, double vy, int ow) : x(x), y(y), vx(vx), vy(vy), owner(ow) {}

    bool update(const SDL_Surface *front, bool &updated, vector<Character> &characters);

    [[nodiscard]] double getX() const;

    [[nodiscard]] double getY() const;

private:
    double x;
    double y;
    double vx;
    double vy;
    int owner;

    bool collidesWith(Character &character) const;
};

#endif

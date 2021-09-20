#ifndef BULLET_H
#define BULLET_H

#include "graphics.h"
#include "CollisionManager.h"
#include <vector>

using namespace std;

class Bullet {
public:
    Bullet(double x, double y, double vx, double vy, int ow, CollisionManager &collisionManager) : x(x), y(y), vx(vx),
                                                                                                   vy(vy), owner(ow),
                                                                                                   collisionManager(
                                                                                                           collisionManager) {}

    bool update(const SDL_Surface *front, bool &updated);

    [[nodiscard]] double getX() const;

    [[nodiscard]] double getY() const;

private:
    double x;
    double y;
    double vx;
    double vy;
    int owner;
    CollisionManager &collisionManager;
};

#endif

#include "Bullet.h"

bool Bullet::update(const SDL_Surface *front, bool &updated) {
    auto steps = (int) max(abs(vx), abs(vy));

    for (int i = 0; i < steps; i++) {

        x += vx / (double) steps;
        y += vy / (double) steps;

        if (getPixel(front, (int) (x + width / 2), (int) (y + height / 2)) & 0xFF000000) {
            filledCircleColor(front, (Sint16) (x + width / 2), (Sint16) (y + height / 2), (Sint16) EXPLOSION_RADIUS,
                              0xFFFFFF00);
            updated = true;
            return true;
        }

        if (collisionManager.collidesWithAnyCharacter(x, y, max(width, height) / 2, owner)) {
            return true;
        }
    }

    if (y > front->h) return true;
    return false;
}


double Bullet::getX() const {
    return x;
}

double Bullet::getY() const {
    return y;
}

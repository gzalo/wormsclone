#include "Bullet.h"

bool Bullet::update(const SDL_Surface *front, bool &updated) {
    auto steps = (int) max(abs(vx), abs(vy));

    for (int i = 0; i < steps; i++) {

        x += vx / (float) steps;
        y += vy / (float) steps;

        if (getPixel(front, (int) (x + 4.0), (int) (y + 4.0)) & 0xFF000000) {
            filledCircleColor(front, (Sint16) (x + 4.0), (Sint16) (y + 4.0), 16, 0xFFFFFF00);
            updated = true;
            return true;
        }

        if (collisionManager.collidesWithAnyCharacter((int) x, (int) y, owner)) {
            return true;
        }
    }

    if (y > 600) return true;
    return false;
}


double Bullet::getX() const {
    return x;
}

double Bullet::getY() const {
    return y;
}

#include "Bullet.h"

bool Bullet::update(const SDL_Surface *front, bool &updated, vector<Character> &characters) {
    auto steps = (int) max(abs(vx), abs(vy));

    for (int i = 0; i < steps; i++) {

        x += vx / (float) steps;
        y += vy / (float) steps;

        if (getPixel(front, (int) (x + 4.0), (int) (y + 4.0)) & 0xFF000000) {
            filledCircleColor(front, (Sint16) (x + 4.0), (Sint16) (y + 4.0), 16, 0xFFFFFF00);
            updated = true;
            return true;
        }

        for (int char_idx = 0; char_idx < characters.size(); char_idx++) {
            if (char_idx != owner && collidesWith(characters[char_idx])) {
                return true;
            }
        }
    }

    if (y > 600) return true;
    return false;
}

bool Bullet::collidesWith(Character &character) const {
    auto dx = (int) (character.getX() + 8 - x);
    auto dy = (int) (character.getY() + 8 - y);
    if (dx * dx + dy * dy <= 12 * 12) {
        character.hurt(1);
        return true;
    } else {
        return false;
    }
}

double Bullet::getX() const {
    return x;
}

double Bullet::getY() const {
    return y;
}

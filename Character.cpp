#include <cmath>
#include "Character.h"

void Character::update(const SDL_Surface *front) {
    ay = 5;
    ax = 0;

    if (!usingRope) {
        movementLogic(front, keyStatus.getMovementType());
    } else {
        ropeLogic(front);
    }

    double tx = x;
    double ty = y;
    x = x + (x - ox) + ax * 0.01666;
    y = y + (y - oy) + ay * 0.01666;

    ox = tx;
    oy = ty;

    int press = 0;

    while (getPixel(front, (int) x - 1, (int) y + 8) & 0xFF000000) {
        x++;
        ox = x;
        press++;
    }
    while (getPixel(front, (int) x + 16, (int) y + 8) & 0xFF000000) {
        x--;
        ox = x;
        press++;
    }

    while (getPixel(front, (int) x + 8, (int) y + 16) & 0xFF000000) {
        y--;
        oy = y;
        jumping = false;
        press++;
    }
    while (getPixel(front, (int) x + 8, (int) y - 1) & 0xFF000000) {
        y++;
        oy = y;
        jumping = true;
        press++;
    }
    if (usingRope) {
        if (press > 8 && changedRope != 0)
            ropeLen -= changedRope;

        double dx = x + 8 - ropeX;
        double dy = y + 8 - ropeY;
        double a = sqrt(dx * dx + dy * dy);
        double diff = (a - ropeLen) / a;
        x -= dx * diff;
        y -= dy * diff;
    }

    if (y > 600 && !usingRope) hp = 0;
}

void Character::movementLogic(const SDL_Surface *front, int movementType) {
    if (keyStatus.isLeft()) {
        ax -= 10;
    } else if (keyStatus.isRight()) {
        ax += 10;
    }

    if (getPixel(front, (int) x + 8, (int) y + 17) & 0xFF000000) {
        if (x - ox > 0) {
            ax -= 8;
        } else if (x - ox < 0) {
            ax += 8;
        }
    }

    if (keyStatus.isJump() && !jumping) {
        ay -= 200;
        jumping = true;
    }
    ropeX = 0;
    if (keyStatus.isRope() && movementType != 5) {
        throwRope(front, movementType);
    }
}

void Character::ropeLogic(const SDL_Surface *front) {
    if (keyStatus.isDown()) {
        ropeLen += 2.0;
        changedRope = +2;
    }
    if (keyStatus.isUp()) {
        ropeLen -= 2.0;
        changedRope = -2;
    }

    if (ropeLen < 10.0) ropeLen = 10.0;
    if (ropeLen > 500.0) ropeLen = 500.0;

    if (keyStatus.isLeft()) {
        ax -= 10;
    } else if (keyStatus.isRight()) {
        ax += 10;
    }

    if (!keyStatus.isRope() || !(getPixel(front, (int) ropeX, (int) ropeY) & 0xFF000000))
        usingRope = false;
}

void Character::throwRope(const SDL_Surface *front, int movementType) {
    double rx = x + 8;
    double ry = y;
    double angle = getAngle(movementType) * M_PI / 180.0;
    double stepx = cos(angle);
    double stepy = sin(angle);
    int i;

    for (i = 0; i < 500; i++) {
        if (getPixel(front, (int) (rx), (int) (ry)) & 0xFF000000)
            break;

        rx += stepx;
        ry += stepy;
    }
    if (i != 500) {
        usingRope = true;
        ropeX = rx;
        ropeY = ry;
        double dx = rx - (x + 8);
        double dy = ry - y;
        ropeLen = sqrt(dx * dx + dy * dy);
    }
}

void Character::hurt(int damage) {
    hp -= damage;
}

double Character::getX() const {
    return x;
}

double Character::getY() const {
    return y;
}

int Character::getHp() const {
    return hp;
}

int Character::getId() const {
    return id;
}

double Character::getRopeX() const {
    return ropeX;
}

double Character::getRopeY() const {
    return ropeY;
}

KeyStatus &Character::getKeyStatus() {
    return keyStatus;
}

double Character::getAngle(int idx) const {
    if (idx == 1) {
        return -135;
    }
    if (idx == 2) {
        return -90;
    }
    if (idx == 3) {
        return -45;
    }
    if (idx == 4) {
        return -180;
    }
    if (idx == 5) {
        return -135;
    }
    if (idx == 6) {
        return 0;
    }
    if (idx == 7) {
        return 135;
    }
    if (idx == 8) {
        return 90;
    }
    if (idx == 9) {
        return 45;
    }
    return 0;
}

#ifndef CHARACTER_H
#define CHARACTER_H

#include "graphics.h"
#include "KeyStatus.h"
#include <list>

using namespace std;

class Character {

public:
    explicit Character(int id, double width, double height) : id(id), width(width), height(height) {}

    void update(const SDL_Surface *front);

    void hurt(int damage);

    [[nodiscard]] double getX() const;

    [[nodiscard]] double getY() const;

    [[nodiscard]] int getHp() const;

    [[nodiscard]] int getId() const;

    [[nodiscard]] double getRopeX() const;

    [[nodiscard]] double getRopeY() const;

    [[nodiscard]] double getWidth() const;

    [[nodiscard]] double getHeight() const;

    KeyStatus &getKeyStatus();

private:
    int id;
    bool jumping = false;
    bool usingRope = false;
    KeyStatus keyStatus;
    double x = 400;
    double y = 100;
    double ox = 400;
    double oy = 100;
    double ax = 0;
    double ay = 5;
    double ropeX = 0;
    double ropeY = 0;
    double ropeLen = 0;
    int hp = 100;
    int changedRope = 0;
    const double width;
    const double height;

    const double MIN_ROPE_LENGTH = 10.0;
    const double MAX_ROPE_LENGTH = 500.0;

    [[nodiscard]] double getAngle(int idx) const;

    void throwRope(const SDL_Surface *front, int movementType);

    void ropeLogic(const SDL_Surface *front);

    void movementLogic(const SDL_Surface *front, int movementType);
};

#endif

#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include <vector>
#include "Character.h"

using namespace std;

class CollisionManager {
public:
    [[nodiscard]] bool collidesWith(double x, double y, double rad, const Character &character) const;

    [[nodiscard]] bool collidesWithAnyCharacter(double x, double y, double rad, int owner) const;

    explicit CollisionManager(vector<Character> &characters) : characters(characters) {}

private:
    vector<Character> &characters;
};

#endif

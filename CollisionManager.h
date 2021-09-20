#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include <vector>
#include "Character.h"

using namespace std;

class CollisionManager {
public:
    [[nodiscard]] bool collidesWith(int x, int y, const Character &character) const;

    [[nodiscard]] bool collidesWithAnyCharacter(int x, int y, int owner) const;

    explicit CollisionManager(vector<Character> &characters) : characters(characters) {}

private:
    vector<Character> &characters;
};

#endif

#include "CollisionManager.h"

bool CollisionManager::collidesWith(double x, double y, double rad, const Character &character) const {
    auto dx = character.getX() + character.getWidth() / 2 - x;
    auto dy = character.getY() + character.getHeight() / 2 - y;
    double maxRad = max(character.getWidth() / 2, character.getHeight() / 2) + rad;
    return dx * dx + dy * dy <= maxRad * maxRad;
}

bool CollisionManager::collidesWithAnyCharacter(double x, double y, double rad, int owner) const {
    for (Character &character: characters) {
        if (character.getId() != owner && collidesWith(x, y, rad, character)) {
            character.hurt(1);
            return true;
        }
    }
    return false;
}
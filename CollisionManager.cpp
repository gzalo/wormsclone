#include "CollisionManager.h"

bool CollisionManager::collidesWith(int x, int y, const Character &character) const {
    auto dx = (int) (character.getX() + 8 - x);
    auto dy = (int) (character.getY() + 8 - y);
    return dx * dx + dy * dy <= 12 * 12;
}

bool CollisionManager::collidesWithAnyCharacter(int x, int y, int owner) const {
    for (Character &character: characters) {
        if (character.getId() != owner && collidesWith(x, y, character)) {
            character.hurt(1);
            return true;
        }
    }
    return false;
}
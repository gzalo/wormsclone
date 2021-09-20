#include "KeyStatus.h"

bool KeyStatus::isUp() const {
    return up;
}

void KeyStatus::setUp(bool _up) {
    up = _up;
}

bool KeyStatus::isDown() const {
    return down;
}

void KeyStatus::setDown(bool _down) {
    down = _down;
}

bool KeyStatus::isLeft() const {
    return left;
}

void KeyStatus::setLeft(bool _left) {
    left = _left;
}

bool KeyStatus::isRight() const {
    return right;
}

void KeyStatus::setRight(bool _right) {
    right = _right;
}

bool KeyStatus::isJump() const {
    return jump;
}

void KeyStatus::setJump(bool _jump) {
    jump = _jump;
}

bool KeyStatus::isFire() const {
    return fire;
}

void KeyStatus::setFire(bool _fire) {
    fire = _fire;
}

bool KeyStatus::isRope() const {
    return rope;
}

void KeyStatus::setRope(bool _rope) {
    rope = _rope;
}

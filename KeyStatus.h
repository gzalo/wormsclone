#ifndef KEYSTATUS_H
#define KEYSTATUS_H

class KeyStatus {
public:
    [[nodiscard]] bool isUp() const;

    void setUp(bool up);

    [[nodiscard]] bool isDown() const;

    void setDown(bool down);

    [[nodiscard]] bool isLeft() const;

    void setLeft(bool left);

    [[nodiscard]] bool isRight() const;

    void setRight(bool right);

    [[nodiscard]] bool isJump() const;

    void setJump(bool jump);

    [[nodiscard]] bool isFire() const;

    void setFire(bool fire);

    [[nodiscard]] bool isRope() const;

    void setRope(bool rope);

    [[nodiscard]] int getMovementType() const;

private:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool jump = false;
    bool fire = false;
    bool rope = false;
};

#endif
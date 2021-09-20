#ifndef CHARACTER_H
#define CHARACTER_H

#include "graphics.h"
#include "Bullet.h"
#include <list>
using namespace std;
class Bullet;

class Character {

public:
    Character(int id) : id(id),
                        jumping (false), k_up (false), k_down (false), k_left (false), k_right(false), k_space(false), k_fire(false), k_rope(false), usingRope(false),
                        ox (400), gx (400), oy (100), gy (100), ax (0), ay (5),
                        hp (100),
                        changedRope (0)
    { }

    void update(const SDL_Surface *front, list<Bullet> &balas);

    bool jumping, k_up,k_down,k_left,k_right,k_space,k_fire,k_rope, usingRope;
    float gx, gy, ox, oy, ax, ay, ropeX, ropeY, ropeLen;
    int hp, id, changedRope;
};

#endif

#include <cmath>
#include "Character.h"

void Character::update(const SDL_Surface *front, list<Bullet> &balas){
    int m = 5;

    if(k_up && k_left) m = 1;
    if(k_up && !k_left && !k_right) m = 2;
    if(k_up && k_right) m = 3;

    if(!k_up && !k_down && k_left) m = 4;
    if(!k_up && !k_down && k_right) m = 6;

    if(k_down && k_left) m = 7;
    if(k_down && !k_left && !k_right) m = 8;
    if(k_down && k_right) m = 9;

    if(k_fire && m!= 5){
        float vy;
        if(m == 1 || m == 2 || m == 3) vy = -10;
        if(m == 4 || m == 6) vy = 0;
        if(m == 7 || m == 8 || m == 9) vy = +10;

        float vx;
        if(m == 1 || m == 4 || m == 7) vx = -10;
        if(m == 2 || m == 8) vx = 0;
        if(m == 3 || m == 6 || m == 9) vx = +10;

        float gx_new = gx+8 + vx*0.6;
        float gy_new = gy+8 + vy*0.6;

        Bullet b(gx_new, gy_new, vx,vy, id);
        balas.push_back(b);
    }

    ay = 5;
    ax = 0;

    int angles[10] = {0,
                      -135,-90,-45,
                      180,0,0
            ,135,90,45};

    if(!usingRope){

        if(k_left) {
            ax-=10;
        }else if(k_right){
            ax+=10;
        }

        if(getPixel(front, (int)gx+8, (int)gy+17) & 0xFF000000){
            if(gx-ox > 0){
                ax -= 8;
            }else if(gx-ox < 0){
                ax += 8;
            }
        }

        if(k_space && !jumping){
            ay-=200;
            jumping=true;
        }
        ropeX = 0;
        if(k_rope && m!=5){
            int x = (int)gx+8, y = (int)gy;
            float rx=x, ry=y;
            float angle = angles[m]*M_PI/180.0;
            float stepx = cos(angle), stepy = sin(angle);
            int i;

            for(i=0;i<500;i++){
                if(getPixel(front, (int)(rx), (int)(ry)) & 0xFF000000)
                    break;

                rx+=stepx;
                ry+=stepy;
            }
            if(i!=500){
                usingRope = true;
                ropeX = rx, ropeY = ry;
                float dx = rx-x, dy = ry-y;
                ropeLen = sqrt(dx*dx+dy*dy);
            }
        }

    }else{
        if(k_down) ropeLen+=2, changedRope = +2;
        if(k_up) ropeLen-=2, changedRope = -2;

        if(ropeLen<10) ropeLen = 10;
        if(ropeLen>500) ropeLen = 500;

        if(k_left){
            ax-=10;
        }else if(k_right){
            ax+=10;
        }

        if(!k_rope || !(getPixel(front, (int)ropeX, (int)ropeY) & 0xFF000000))
            usingRope = false;
    }

    for(int i=0;i<1;i++){

        float tx = gx;
        float ty = gy;
        gx = gx + (gx-ox) + ax * 0.01666;
        gy = gy + (gy-oy) + ay * 0.01666;

        ox = tx;
        oy = ty;

        int press = 0;

        while(getPixel(front, (int)gx-1, (int)gy+8) & 0xFF000000) {
            gx++;
            ox = gx;
            press++;
        }
        while(getPixel(front, (int)gx+16, (int)gy+8) & 0xFF000000) {
            gx--;
            ox = gx;
            press++;
        }

        while(getPixel(front, (int)gx+8, (int)gy+16) & 0xFF000000) {
            gy--;
            oy = gy;
            jumping = false;
            press++;
        }
        while(getPixel(front, (int)gx+8, (int)gy-1) & 0xFF000000) {
            gy++;
            oy = gy;
            jumping = true;
            press++;
        }
        if(usingRope){
            if(press>8 && changedRope != 0)
                ropeLen -= changedRope;

            float dx = gx+8-ropeX;
            float dy = gy+8-ropeY;
            float a = sqrt(dx*dx+dy*dy);
            float diff = (a - ropeLen)/(a);
            gx -= dx * diff;
            gy -= dy * diff;
        }
    }

    if(gy>600 && !usingRope) hp = 0;
}
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "Game.h"

void Game::run() {
    while (!quit) {
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
            if (event.type == SDL_KEYDOWN) {
                quit = handleKeyDown(event);
            }
            if (event.type == SDL_KEYUP) {
                handleKeyUp(event);
            }

            if (event.type == SDL_MOUSEMOTION) {
                mcam = handleMouseMotion(event);
            }
        }

        xoff -= mcam;
        if (xoff > 0) xoff = 0;
        if (xoff < -front->w + width) xoff = -front->w + width;

        for (auto &character : characters) {
            character.update(front);
            fireBullets(character);
        }

        drawRect(backgroundImgId, (int) (xoff / 2.0), 0, 2048, 1024);

        glTranslated(xoff, 0, 0);

        if (frontImgNeedsUpdate) {
            updateImage(frontImgId, front);
            frontImgNeedsUpdate = false;
        }
        drawRect(frontImgId, 0, 0, 1200, 600);

        for (auto const &character : characters) {
            drawCharacter(character);
        }

        updateBullets();

        updateTitle();

        SDL_GL_SwapWindow(window);
        SDL_Delay(1000 / 60);
    }

    SDL_Quit();

}

void Game::fireBullets(Character &character) {
    int movementType = character.getKeyStatus().getMovementType();
    if (!character.getKeyStatus().isFire() || movementType == 5) {
        return;
    }

    double vx = 0.0;
    double vy = 0.0;

    if (movementType == 1 || movementType == 4 || movementType == 7) vx = -10.0;
    if (movementType == 3 || movementType == 6 || movementType == 9) vx = +10.0;
    if (movementType == 1 || movementType == 2 || movementType == 3) vy = -10.0;
    if (movementType == 7 || movementType == 8 || movementType == 9) vy = +10.0;

    double gx_new = character.getX() + 8.0 + vx * 0.6;
    double gy_new = character.getY() + 8.0 + vy * 0.6;

    Bullet b(gx_new, gy_new, vx, vy, character.getId(), collisionManager);
    bullets.push_back(b);
}


void Game::updateBullets() {
    for (auto it = bullets.begin(); it != bullets.end(); it++) {
        bool remove = (*it).update(front, frontImgNeedsUpdate);
        if (remove) {
            it = bullets.erase(it);
        } else {
            drawRect(bulletImgId, (int) (*it).getX() - 4, (int) (*it).getY() - 4, 8, 8);
        }
    }
}

void Game::drawCharacter(const Character &character) const {
    if (character.getHp() > 0) {
        if (character.getRopeX() != 0) {
            glDisable(GL_TEXTURE_2D);
            glColor4d(0.0, 0.0, 0.0, 1.0);

            glBegin(GL_LINES);
            glVertex3d(character.getX() + 8, character.getY() + 8, 0);
            glVertex3d(character.getRopeX(), character.getRopeY(), 0);
            glEnd();

            glColor4d(1.0, 1.0, 1.0, 1.0);
            glEnable(GL_TEXTURE_2D);
        }
        drawRect(wormImgId, (int) character.getX(), (int) character.getY(), 16, 16);
    }
}

void Game::updateTitle() {
    ostringstream buff;
    buff << setfill('0') << setw(2) << characters[0].getHp() << " ";
    buff << setfill('0') << setw(2) << characters[1].getHp();
    SDL_SetWindowTitle(window, buff.str().c_str());
}

int Game::handleMouseMotion(const SDL_Event &event) const {
    if (event.motion.x > 700) {
        return (event.motion.x - 700) / 10;
    } else if (event.motion.x < 100) {
        return -(100 - event.motion.x) / 10;
    } else {
        return 0;
    }
}

void Game::handleKeyUp(const SDL_Event &event) {
    KeyStatus &keyStatus = characters[0].getKeyStatus();
    switch (event.key.keysym.sym) {
        case SDLK_UP:
            keyStatus.setUp(false);
            break;
        case SDLK_DOWN:
            keyStatus.setDown(false);
            break;
        case SDLK_LEFT:
            keyStatus.setLeft(false);
            break;
        case SDLK_RIGHT:
            keyStatus.setRight(false);
            break;
        case SDLK_x:
            keyStatus.setJump(false);
            break;
        case SDLK_z:
            keyStatus.setFire(false);
            break;
        case SDLK_c:
            keyStatus.setRope(false);
            break;
        default:
            break;
    }
}

bool Game::handleKeyDown(const SDL_Event &event) {
    KeyStatus &keyStatus = characters[0].getKeyStatus();
    switch (event.key.keysym.sym) {
        case SDLK_UP:
            keyStatus.setUp(true);
            break;
        case SDLK_DOWN:
            keyStatus.setDown(true);
            break;
        case SDLK_LEFT:
            keyStatus.setLeft(true);
            break;
        case SDLK_RIGHT:
            keyStatus.setRight(true);
            break;
        case SDLK_x:
            keyStatus.setJump(true);
            break;
        case SDLK_z:
            keyStatus.setFire(true);
            break;
        case SDLK_c:
            keyStatus.setRope(true);
            break;
        case SDLK_ESCAPE:
            return true;
        default:
            break;
    }
    return false;
}

Game::Game(const int width, const int height, const int charCount) : width(width) {
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("WormsClone",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_OPENGL);

    SDL_GL_CreateContext(window);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4d(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.9f, 0.9f, 0.9f, 0.1f);
    glEnable(GL_TEXTURE_2D);
    glLineWidth(2);

    auto[front, frontImgId] = loadImage("res/front.png");
    Game::front = front;
    Game::frontImgId = frontImgId;
    wormImgId = get<1>(loadImage("res/worm.png"));
    bulletImgId = get<1>(loadImage("res/bullet.png"));
    backgroundImgId = get<1>(loadImage("res/background.png"));

    for (int i = 0; i < charCount; i++) {
        characters.emplace_back(i);
    }
}

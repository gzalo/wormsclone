#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "Game.h"

void Game::run() {
    SDL_Event event;

    bool quit=false;
    int mcam = 0;
    int xoff = 0;

    while(!quit){
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT) quit = true;
            if(event.type == SDL_KEYDOWN) {
                quit = handleKeyDown(event);
            }
            if(event.type == SDL_KEYUP) {
                handleKeyUp(event);
            }

            if(event.type == SDL_MOUSEMOTION){
                mcam = handleMouseMotion(event);
            }
        }

        if(mov)
            xoff-=mcam;

        if(xoff>0) xoff = 0;
        if(xoff<-front->w+width) xoff = -front->w+width;

        for(auto &personaje : personajes){
            personaje.update(front, balas);
        }

        drawRect(fondoImgId, (int)(xoff / 2.0), 0, 2048, 1024);

        glTranslated(xoff,0,0);

        if(frontImgNeedsUpdate){
            updateImage(frontImgId, front);
            frontImgNeedsUpdate = false;
        }
        drawRect(frontImgId, 0,0, 1200, 600);

        for(auto const &personaje : personajes){
            drawCharacter(personaje);
        }

        for(auto it = balas.begin(); it!= balas.end(); it++)
        {
            bool remove = (*it).update(front, frontImgNeedsUpdate, personajes);
            if(remove){
                it = balas.erase(it);
            }else{
                drawRect(balaImgId, (int)(*it).getX() - 4, (int)(*it).getY() - 4, 8, 8);
            }
        }

        updateTitle();

        SDL_GL_SwapWindow(window);
        SDL_Delay(1000/60);
    }

    SDL_Quit();

}

void Game::drawCharacter(const Character &personaje) const {
    if(personaje.hp > 0){
        if(personaje.ropeX != 0){
            glDisable(GL_TEXTURE_2D);
            glColor4f(0.0, 0.0, 0.0, 1.0);

            glBegin(GL_LINES);
            glVertex3f(personaje.gx+8,personaje.gy+8,0);
            glVertex3f(personaje.ropeX,personaje.ropeY,0);
            glEnd();

            glColor4f(1.0, 1.0, 1.0, 1.0);
            glEnable(GL_TEXTURE_2D);
        }
        drawRect(gusanoImgId, (int)personaje.gx, (int)personaje.gy, 16, 16);
    }
}

void Game::updateTitle() {
    ostringstream buff;
    buff << setfill('0') << setw(2) << personajes[0].hp << " ";
    buff << setfill('0') << setw(2) << personajes[1].hp;
    SDL_SetWindowTitle(window, buff.str().c_str());
}

int Game::handleMouseMotion(const SDL_Event &event) {
    int mcam;
    if(event.motion.x > 700){
        mcam=(event.motion.x-700)/10;
    }else if(event.motion.x < 100){
        mcam=-(100-event.motion.x)/10;
    }else{
        mcam=0;
    }
    return mcam;
}

void Game::handleKeyUp(const SDL_Event &event) {
    switch( event.key.keysym.sym ) {
        case SDLK_UP: personajes[0].k_up=false; break;
        case SDLK_DOWN: personajes[0].k_down=false; break;
        case SDLK_LEFT: personajes[0].k_left=false; break;
        case SDLK_RIGHT: personajes[0].k_right=false; break;
        case SDLK_x: personajes[0].k_space=false; break;
        case SDLK_z: personajes[0].k_fire=false; break;
        case SDLK_c: personajes[0].k_rope=false; break;
        default: break;
    }
}

bool Game::handleKeyDown(const SDL_Event &event) {
    switch( event.key.keysym.sym ) {
        case SDLK_UP: personajes[0].k_up=true; break;
        case SDLK_DOWN: personajes[0].k_down=true; break;
        case SDLK_LEFT: personajes[0].k_left=true; break;
        case SDLK_RIGHT: personajes[0].k_right=true; break;
        case SDLK_x: personajes[0].k_space=true; break;
        case SDLK_z: personajes[0].k_fire=true; break;
        case SDLK_c: personajes[0].k_rope=true; break;
        case SDLK_ESCAPE: return true;
        default: break;
    }
    return false;
}

Game::Game(const int width, const int height, const int charCount) : width(width), height(height){
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("WormsClone",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          width, height,
                                          SDL_WINDOW_OPENGL);

    SDL_GL_CreateContext(window);

    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4d( 1.0,1.0,1.0,1.0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.9f, 0.9f, 0.9f, 0.1f);
    glEnable(GL_TEXTURE_2D);
    glLineWidth(2);

    auto escenario = loadImage("res/escenario.png");
    front = get<0>(escenario);
    frontImgId = get<1>(escenario);
    gusanoImgId = get<1>(loadImage("res/gusano.png"));
    balaImgId = get<1>(loadImage("res/bala.png"));
    fondoImgId = get<1>(loadImage("res/fondo.png"));

    for(int i=0;i<charCount;i++){
        Character temp(i);
        personajes.push_back(temp);
    }
}

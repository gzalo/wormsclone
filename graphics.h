#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <string>
#include <tuple>
#include <GL/gl.h>

using namespace std;

tuple<const SDL_Surface *, GLuint> loadImage(const string &filename);

void updateImage(GLuint imgId, const SDL_Surface *img);

unsigned int getPixel(const SDL_Surface *surface, int x, int y);

void setPixel(const SDL_Surface *surface, int x, int y, unsigned int color);

int drawRect(GLuint img, int x, int y, int w, int h);

int hlineColor(const SDL_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color);

int filledCircleColor(const SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color);

#endif

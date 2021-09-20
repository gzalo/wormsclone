#include <cstring>
#include "graphics.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

tuple<const SDL_Surface *, GLuint> loadImage(const string &filename) {
    int req_format = STBI_rgb_alpha;
    int width = 0;
    int height = 0;
    int orig_format;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &orig_format, req_format);

    const SDL_Surface *loadedImage = SDL_CreateRGBSurfaceWithFormatFrom((void *) data, width, height, 32,
                                                                        4 * width, SDL_PIXELFORMAT_RGBA32);

    GLuint imgId;
    glGenTextures(1, &imgId);
    glBindTexture(GL_TEXTURE_2D, imgId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loadedImage->w, loadedImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 loadedImage->pixels);
    return {loadedImage, imgId};
}

void updateImage(GLuint imgId, const SDL_Surface *img) {
    glBindTexture(GL_TEXTURE_2D, imgId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
}

unsigned int getPixel(const SDL_Surface *surface, int x, int y) {
    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) return 0;
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;
    return *(Uint32 *) p;
}

void setPixel(const SDL_Surface *surface, int x, int y, unsigned int color) {
    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) return;
    Uint32 *p = (Uint32 *) surface->pixels + y * surface->w + x;
    *p = color;
}

int drawRect(GLuint img, int x, int y, int w, int h) {
    auto x0 = (GLdouble) x;
    auto y0 = (GLdouble) y;
    auto x1 = (GLdouble) (x + w);
    auto y1 = (GLdouble) (y + h);

    glBindTexture(GL_TEXTURE_2D, img);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex3d(x0, y0, 0.0);
    glTexCoord2d(1.0, 0.0);
    glVertex3d(x1, y0, 0.0);
    glTexCoord2d(1.0, 1.0);
    glVertex3d(x1, y1, 0.0);
    glTexCoord2d(0.0, 1.0);
    glVertex3d(x0, y1, 0.0);
    glEnd();

    return 0;
}

int hlineColor(const SDL_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color) {
    Sint16 left, right, top, bottom;
    Uint8 *pixel, *pixellast;
    int dx;
    int pixx, pixy;
    Sint16 w;
    Sint16 xtmp;
    Uint8 *colorptr;

    /*
     * Check visibility of clipping rectangle
     */
    if ((dst->clip_rect.w == 0) || (dst->clip_rect.h == 0)) {
        return 0;
    }

    /*
     * Swap x1, x2 if required to ensure x1<=x2
     */
    if (x1 > x2) {
        xtmp = x1;
        x1 = x2;
        x2 = xtmp;
    }

    /*
     * Get clipping boundary and
     * check visibility of hline
     */
    left = dst->clip_rect.x;
    if (x2 < left) {
        return 0;
    }
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    if (x1 > right) {
        return 0;
    }
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
    if ((y < top) || (y > bottom)) {
        return 0;
    }

    /*
     * Clip x
     */
    if (x1 < left) {
        x1 = left;
    }
    if (x2 > right) {
        x2 = right;
    }

    /*
     * Calculate width
     */
    w = x2 - x1;

    /*
     * No alpha-blending required
     */

    /*
     * Setup color
     */
    colorptr = (Uint8 *) &color;
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
    } else {
        color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
    }

    /*
     * More variable setup
     */
    dx = w;
    pixx = dst->format->BytesPerPixel;
    pixy = dst->pitch;
    pixel = ((Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y;

    /*
     * Draw
     */
    switch (dst->format->BytesPerPixel) {
        case 1:
            memset(pixel, color, dx + 1);
            break;
        case 2:
            pixellast = pixel + dx + dx;
            for (; pixel <= pixellast; pixel += pixx) {
                *(Uint16 *) pixel = color;
            }
            break;
        case 3:
            pixellast = pixel + dx + dx + dx;
            for (; pixel <= pixellast; pixel += pixx) {
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                    pixel[0] = (color >> 16) & 0xff;
                    pixel[1] = (color >> 8) & 0xff;
                    pixel[2] = color & 0xff;
                } else {
                    pixel[0] = color & 0xff;
                    pixel[1] = (color >> 8) & 0xff;
                    pixel[2] = (color >> 16) & 0xff;
                }
            }
            break;
        default:        /* case 4 */
            dx = dx + dx;
            pixellast = pixel + dx + dx;
            for (; pixel <= pixellast; pixel += pixx) {
                *(Uint32 *) pixel = color;
            }
            break;
    }

    return 0;
}

int filledCircleColor(const SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color) {
    Sint16 left, right, top, bottom;
    int result;
    Sint16 x1, y1, x2, y2;
    Sint16 cx = 0;
    Sint16 cy = r;
    Sint16 ocx = (Sint16) 0xffff;
    Sint16 ocy = (Sint16) 0xffff;
    Sint16 df = 1 - r;
    Sint16 d_e = 3;
    Sint16 d_se = -2 * r + 5;
    Sint16 xpcx, xmcx, xpcy, xmcy;
    Sint16 ypcy, ymcy, ypcx, ymcx;

    /*
     * Check visibility of clipping rectangle
     */
    if ((dst->clip_rect.w == 0) || (dst->clip_rect.h == 0)) {
        return 0;
    }

    /*
     * Sanity check radius
     */
    if (r < 0) {
        return (-1);
    }

    /*
     * Special case for r=0 - draw a point
     */
    if (r == 0) {
        setPixel(dst, x, y, color);
        return 0;
    }

    /*
     * Get circle and clipping boundary and
     * test if bounding box of circle is visible
     */
    x2 = x + r;
    left = dst->clip_rect.x;
    if (x2 < left) {
        return 0;
    }
    x1 = x - r;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    if (x1 > right) {
        return 0;
    }
    y2 = y + r;
    top = dst->clip_rect.y;
    if (y2 < top) {
        return 0;
    }
    y1 = y - r;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
    if (y1 > bottom) {
        return 0;
    }

    /*
     * Draw
     */
    result = 0;
    do {
        xpcx = x + cx;
        xmcx = x - cx;
        xpcy = x + cy;
        xmcy = x - cy;
        if (ocy != cy) {
            if (cy > 0) {
                ypcy = y + cy;
                ymcy = y - cy;
                result |= hlineColor(dst, xmcx, xpcx, ypcy, color);
                result |= hlineColor(dst, xmcx, xpcx, ymcy, color);
            } else {
                result |= hlineColor(dst, xmcx, xpcx, y, color);
            }
            ocy = cy;
        }
        if (ocx != cx) {
            if (cx != cy) {
                if (cx > 0) {
                    ypcx = y + cx;
                    ymcx = y - cx;
                    result |= hlineColor(dst, xmcy, xpcy, ymcx, color);
                    result |= hlineColor(dst, xmcy, xpcy, ypcx, color);
                } else {
                    result |= hlineColor(dst, xmcy, xpcy, y, color);
                }
            }
            ocx = cx;
        }
        /*
         * Update
         */
        if (df < 0) {
            df += d_e;
            d_e += 2;
            d_se += 2;
        } else {
            df += d_se;
            d_e += 2;
            d_se += 4;
            cy--;
        }
        cx++;
    } while (cx <= cy);

    return (result);
}
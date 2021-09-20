#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cmath>
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

const int SCREEN_WIDTH=800;
const int SCREEN_HEIGHT=600;
const int SCREEN_BPP=32;

int fondoImg, frontImg, gusanoImg, balaImg;
bool frontImgNeedsUpdate = true;

map <int, SDL_Surface*> surfaces;
SDL_Surface *front;

int getNextPowerOfTwo(int val){
	val--;
	val = (val >> 1) | val;
	val = (val >> 2) | val;
	val = (val >> 4) | val;
	val = (val >> 8) | val;
	val = (val >> 16) | val;
	val++;
	return val;
}

int loadImage(const string &filename){
	GLuint imgId;

    int req_format = STBI_rgb_alpha;
    int width,height,orig_format;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &orig_format, req_format);

    int depth, pitch;
    Uint32 pixel_format;
    if (req_format == STBI_rgb) {
        depth = 24;
        pitch = 3*width; // 3 bytes per pixel * pixels per row
        pixel_format = SDL_PIXELFORMAT_RGB24;
    } else { // STBI_rgb_alpha (RGBA)
        depth = 32;
        pitch = 4*width;
        pixel_format = SDL_PIXELFORMAT_RGBA32;
    }

    SDL_Surface* loadedImage = SDL_CreateRGBSurfaceWithFormatFrom((void*)data, width, height,
                                                           depth, pitch, pixel_format);
	glGenTextures(1, &imgId);
	glBindTexture(GL_TEXTURE_2D, imgId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,loadedImage->w,loadedImage->h,0,GL_RGBA,GL_UNSIGNED_BYTE,loadedImage->pixels);
	surfaces[imgId] = loadedImage;
	return imgId;
}
void updateImage(int imgId){
	SDL_Surface *img = surfaces[imgId];
	glBindTexture(GL_TEXTURE_2D, imgId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img->w,img->h,0,GL_RGBA,GL_UNSIGNED_BYTE,img->pixels);
}
void applySurface(int x,int y,SDL_Surface *source,SDL_Surface *destination){
	SDL_Rect offset; 
	offset.x=x;offset.y=y;
	SDL_BlitSurface(source,NULL,destination,&offset); 
} 
	
unsigned int getPixel(SDL_Surface *surface, int x, int y){
	if(x<0 || y<0 || x>=surface->w || y>=surface->h ) return 0;
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    return *(Uint32 *)p;
} 
void setPixel(SDL_Surface *surface, int x, int y, unsigned int color){
	if(x<0 || y<0 || x>=surface->w || y>=surface->h ) return;
    Uint32 *p = (Uint32 *)surface->pixels + y * surface->w + x;
	*p = color;
} 
int drawRect(int img, int x, int y, int w, int h){
	glBindTexture(GL_TEXTURE_2D, img);
	
	glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f( x  ,   y, 0);
		glTexCoord2d(1, 0); glVertex3f( x+w,   y, 0);
		glTexCoord2d(1, 1); glVertex3f( x+w, y+h, 0);
		glTexCoord2d(0, 1); glVertex3f( x  , y+h, 0);
	glEnd();
	
	return 0;
}
int hlineColor(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color){
    Sint16 left, right, top, bottom;
    Uint8 *pixel, *pixellast;
    int dx;
    int pixx, pixy;
    Sint16 w;
    Sint16 xtmp;
    int result = -1;
    Uint8 *colorptr;

    /*
     * Check visibility of clipping rectangle
     */
    if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
     return(0);
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
    if (x2<left) {
     return(0);
    }
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    if (x1>right) {
     return(0);
    }
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
    if ((y<top) || (y>bottom)) {
     return (0);
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
     * Alpha check 
     */
   // if ((color & 255) == 255) {

	/*
	 * No alpha-blending required 
	 */

	/*
	 * Setup color 
	 */
	colorptr = (Uint8 *) & color;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	    color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
	} else {
	    color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
	}

	/*
	 * Lock surface 
	 */
	SDL_LockSurface(dst);

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
	    memset(pixel, color, dx+1);
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
	default:		/* case 4 */
	    dx = dx + dx;
	    pixellast = pixel + dx + dx;
	    for (; pixel <= pixellast; pixel += pixx) {
		*(Uint32 *) pixel = color;
	    }
	    break;
	}

	/*
	 * Unlock surface 
	 */
	SDL_UnlockSurface(dst);

	/*
	 * Set result code 
	 */
	result = 0;

   /* } else {

	

	result = HLineAlpha(dst, x1, x1 + w, y, color);

    }*/

    return (result);
}
int filledCircleColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color){
	frontImgNeedsUpdate = true;
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
    if ((dst->clip_rect.w==0) || (dst->clip_rect.h==0)) {
     return(0);
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
    if (x2<left) {
     return(0);
    } 
    x1 = x - r;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    if (x1>right) {
     return(0);
    } 
    y2 = y + r;
    top = dst->clip_rect.y;
    if (y2<top) {
     return(0);
    } 
    y1 = y - r;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
    if (y1>bottom) {
     return(0);
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
inline float _gabs( float x){
	if(x<0) return -x;
	return x;
}

bool mov = false;

int angles[10] = {0, 
-135,-90,-45,
180,0,0 
,135,90,45};

struct bala{
	float gx, gy, vx, vy;
	int owner;	
	
	bala(float x, float y, float sx, float sy, int ow){
		gx = x, gy = y;
		vx = sx, vy = sy;
		owner = ow;
	}
	bool update();
};

#define GRAV 5
#define BGRAV 0.2

struct personaje{
	bool jumping, k_up,k_down,k_left,k_right,k_space,k_fire,k_rope, usingRope;
	float gx, gy, ox, oy, ax, ay, ropeX, ropeY, ropeLen;
	int hp, id, changedRope;

	personaje(int _id){
		jumping = k_up = k_down = k_left = k_right = k_space = k_fire = k_rope = usingRope = false;
		ox = gx = 400, oy = gy = 100, ax = 0, ay = GRAV;
		hp = 100;
		id = _id;
		changedRope = 0;
	}
	
	void update();
};

list <bala> balas;
vector <personaje> personajes;

bool bala::update(){			
	int steps = (int)max(_gabs(vx),_gabs(vy));
	
	for(int i=0;i<steps;i++){
	
		gx += vx/(float)steps;
		gy += vy/(float)steps;
	
		if(getPixel(front, (int)(gx+4.0), (int)(gy+4.0)) & 0xFF000000){
			filledCircleColor(front, (int)(gx+4.0), (int)(gy+4.0),16,0xFFFFFF00);
			return true;
		}
		
		for(int i=0;i<personajes.size();i++){
			if(i != owner){
				int dx = (int)(personajes[i].gx+8 - gx);
				int dy = (int)(personajes[i].gy+8 - gy);
				if(dx*dx+dy*dy <= 12*12){
					personajes[i].hp--;
					return true;
				}
			}
		}
	}
	
	if(gy>600) return true;
	return false;
}
void personaje::update(){
	mov = false;
	
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
		bala b(gx+8, gy+8, 0,0, id);
		
		if(m == 1 || m == 2 || m == 3) b.vy = -10;
		if(m == 4 || m == 5 || m == 6) b.vy = 0;
		if(m == 7 || m == 8 || m == 9) b.vy = +10;
		
		if(m == 1 || m == 4 || m == 7) b.vx = -10;
		if(m == 2 || m == 5 || m == 8) b.vx = 0;
		if(m == 3 || m == 6 || m == 9) b.vx = +10;
		
		b.gx += b.vx*0.6;
		b.gy += b.vy*0.6;
		
		balas.push_back(b);
	}
	
	ay = GRAV;
	ax = 0;
	
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
		
		while((getPixel(front, (int)gx-1, (int)gy+8) & 0xFF000000)) {
			gx++;
			ox = gx;
			press++;
		}
		while((getPixel(front, (int)gx+16, (int)gy+8) & 0xFF000000)) {
			gx--;
			ox = gx;
			press++;
		}		
		
		while((getPixel(front, (int)gx+8, (int)gy+16) & 0xFF000000)) {
			gy--;
			oy = gy;
			jumping = false;
			press++;
		}
		while((getPixel(front, (int)gx+8, (int)gy-1) & 0xFF000000)) {
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

int main(int argc, char** args){
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Joystick *joy0 = SDL_JoystickOpen(0);
	SDL_Joystick *joy1 = SDL_JoystickOpen(1);
	SDL_Joystick *joy2 = SDL_JoystickOpen(2);
	
    SDL_Window *window = SDL_CreateWindow("WormsClone",
                     SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                     SCREEN_WIDTH, SCREEN_HEIGHT,
                     SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(window);

	glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,SCREEN_WIDTH,SCREEN_HEIGHT,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4d( 1.0,1.0,1.0,1.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.9f, 0.9f, 0.9f, 0.1f);
	glEnable(GL_TEXTURE_2D);
	glLineWidth(2);
	
	frontImg = loadImage("res/escenario.png");
	front =	surfaces[frontImg];
	
	gusanoImg = loadImage("res/gusano.png");
	balaImg = loadImage("res/bala.png");	
	fondoImg = loadImage("res/fondo.png");
	
	SDL_Event event;

	bool quit=false;
	int mouseDown = 0;
	int mcam = 0, xoff = 0;
	bool mov = false;
		
	for(int i=0;i<4;i++){
		personaje temp(i);
		personajes.push_back(temp);
	}
	
	while(!quit){	
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		while(SDL_PollEvent(&event)){ 
			if(event.type==SDL_QUIT) quit = true; 
			if(event.type == SDL_KEYDOWN) { 
				switch( event.key.keysym.sym ) {
					case SDLK_UP: 		personajes[0].k_up=true; break;
					case SDLK_DOWN: 	personajes[0].k_down=true; break;
					case SDLK_LEFT: 	personajes[0].k_left=true; break;
					case SDLK_RIGHT: 	personajes[0].k_right=true; break; 
					case SDLK_x: 		personajes[0].k_space=true; break; 
					case SDLK_z: 		personajes[0].k_fire=true; break; 
					case SDLK_c: 		personajes[0].k_rope=true; break; 
					
					case SDLK_ESCAPE: quit=true; break; 
				} 
			}
			if(event.type == SDL_KEYUP) { 
				switch( event.key.keysym.sym ) {
					case SDLK_UP: 		personajes[0].k_up=false; break;
					case SDLK_DOWN: 	personajes[0].k_down=false; break;
					case SDLK_LEFT: 	personajes[0].k_left=false; break;
					case SDLK_RIGHT: 	personajes[0].k_right=false; break; 
					case SDLK_x: 		personajes[0].k_space=false; break; 
					case SDLK_z: 		personajes[0].k_fire=false; break; 
					case SDLK_c: 		personajes[0].k_rope=false; break; 
					
				} 
			}			
			
			if( event.type == SDL_JOYAXISMOTION ) { 
				if(event.jaxis.axis == 0) {	
					personajes[event.jaxis.which+1].k_left = personajes[event.jaxis.which+1].k_right = false;
					if(event.jaxis.value < -8000) personajes[event.jaxis.which+1].k_left = true;
					if(event.jaxis.value > 8000) personajes[event.jaxis.which+1].k_right = true;
				}
				if(event.jaxis.axis == 1) {
					personajes[event.jaxis.which+1].k_up = personajes[event.jaxis.which+1].k_down = false;
					if(event.jaxis.value < -8000) personajes[event.jaxis.which+1].k_up = true;
					if(event.jaxis.value > 8000) personajes[event.jaxis.which+1].k_down = true;
				}
			}
			if(event.type == SDL_JOYBUTTONDOWN){
				switch(event.jbutton.button){
					case 0: personajes[event.jaxis.which+1].k_space = true; break;
					case 1:	personajes[event.jaxis.which+1].k_fire = true; break;
					case 2:	personajes[event.jaxis.which+1].k_rope = true; break;						
					default: break;
				}
			}
			if(event.type == SDL_JOYBUTTONUP){
				switch(event.jbutton.button){
					case 0: personajes[event.jaxis.which+1].k_space = false; break;
					case 1:	personajes[event.jaxis.which+1].k_fire = false; break;
					case 2:	personajes[event.jaxis.which+1].k_rope = false; break;						
					default: break;
				}
			}
			if(event.type == SDL_MOUSEMOTION){
				if(event.motion.x > 700){
					mcam=(event.motion.x-700)/10;
					mov=true;
				}else if(event.motion.x < 100){
					mcam=-(100-event.motion.x)/10;
					mov=true;
				}else{
					mcam=0;
				}
			}
			if(event.type == SDL_MOUSEBUTTONDOWN){
				if(event.button.button == SDL_BUTTON_RIGHT){
					mouseDown = 3;
				}else if(event.button.button == SDL_BUTTON_LEFT){
					mouseDown = 1;
				}
			}
			if(event.type == SDL_MOUSEBUTTONUP){
				mouseDown = 0;
			}

		}
		
		{ // Camera movement
		if(mov)
			xoff-=mcam;
		
		if(xoff>0) xoff = 0;
		if(xoff<-front->w+SCREEN_WIDTH) xoff = -front->w+SCREEN_WIDTH;
		}
		
		for(vector<personaje>::iterator it = personajes.begin(); it!= personajes.end();it++){	
			(*it).update();
		}
		
		drawRect(fondoImg, (int)(xoff/2.0),0, 2048, 1024);
		
		glTranslated(xoff,0,0);
		
		if(frontImgNeedsUpdate){
			updateImage(frontImg);			
			frontImgNeedsUpdate = false;
		}
		drawRect(frontImg, 0,0, 1200, 600);
		
		for(vector<personaje>::iterator it = personajes.begin(); it!= personajes.end();it++){
			if((*it).hp > 0){
				if((*it).ropeX != 0){
				
					glDisable(GL_TEXTURE_2D);
					glColor4f(0.0, 0.0, 0.0, 1.0);
				
					glBegin(GL_LINES);
						glVertex3f((*it).gx+8,(*it).gy+8,0);
						glVertex3f((*it).ropeX,(*it).ropeY,0);
					glEnd();
				
					glColor4f(1.0, 1.0, 1.0, 1.0);
					glEnable(GL_TEXTURE_2D);			
				}
				drawRect(gusanoImg, (int)(*it).gx, (int)(*it).gy, 16, 16);			
			}
		}
		
		for(list<bala>::iterator it = balas.begin(); it!= balas.end(); it++)
		{
			bool remove = (*it).update();
			if(remove){
				it = balas.erase(it);
			}else{
				drawRect(balaImg, (int)(*it).gx-4,(int)(*it).gy-4, 8, 8);
			}
		}
		
		char buff[128];
		sprintf(buff, "%.3d %.3d %.3d %.3d", personajes[0].hp, personajes[1].hp, personajes[2].hp, personajes[3].hp);
		SDL_SetWindowTitle(window, buff);
		
		SDL_GL_SwapWindow(window);
		SDL_Delay(1000/60);		
	}
	
	SDL_Quit();
	return 0;
}



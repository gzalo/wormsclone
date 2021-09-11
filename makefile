worms.exe: worms.cpp
	g++ worms.cpp -o worms.exe -lmingw32 -lSDLmain -lSDL -lSDL_image -mwindows -lopengl32 -O3

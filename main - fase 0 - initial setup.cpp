// 3DSage - Let's program Doom - part 1
//
// Youtube: https://youtu.be/huMO4VQEwPc
// got upto: 5m50s
//
// Implemented by Joseph21 on olc::PixelGameEngine
// February 24, 2024

#include <iostream>
#include <cmath>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


#define res         1                 // 1 = 160x120, 2 = 320x240, 4 = 640x480
#define SW          160*res           // screen width
#define SH          120*res           // screen height
#define SW2         (SW/2)            // half of screen width
#define SH2         (SH/2)            // half of screen height
#define pixelScale  4/res             // pixel scale

//-----------------------------------------------------------------------------

// aux. struct for timing
typedef struct {
    float fr1, fr2;      // frame 1, frame 2, to create constant frame rate (in seconds)
} myTime;
myTime T;

//-----------------------------------------------------------------------------

class DoomGame : public olc::PixelGameEngine {

public:
    DoomGame() {
        sAppName = "DoomGame";
    }

private:

    // draw a pixel at (x, y) with rgb
    void myPixel( int x, int y, int c ) {
        olc::Pixel rgbPixel;
        switch (c) {
            case 0:  rgbPixel = olc::Pixel( 255, 255,   0 ); break;  // Bright yellow
            case 1:  rgbPixel = olc::Pixel( 160, 160,   0 ); break;  // Darker yellow
            case 2:  rgbPixel = olc::Pixel(   0, 255,   0 ); break;  // Bright green
            case 3:  rgbPixel = olc::Pixel(   0, 160,   0 ); break;  // Darker green
            case 4:  rgbPixel = olc::Pixel(   0, 255, 255 ); break;  // Bright cyan
            case 5:  rgbPixel = olc::Pixel(   0, 160, 160 ); break;  // Darker cyan
            case 6:  rgbPixel = olc::Pixel( 160, 100,   0 ); break;  // Bright brown
            case 7:  rgbPixel = olc::Pixel( 110,  50,   0 ); break;  // Darker brown
            case 8:  rgbPixel = olc::Pixel(   0,  60, 130 ); break;  // background colour
            default: rgbPixel = olc::Pixel( 255,   0, 255 ); break;  // error = magenta
        }
        Draw( x, SH - 1 - y, rgbPixel );   // draw pixel having screen origin left *down* (and y going up)
    }

    // adapt player variables (lookup value l, position in the world x, y or z value or orientation a)
    void movePlayer() {
        if (GetKey( olc::Key::M ).bHeld) {
        // move up, down, look up, down
            if (GetKey( olc::Key::A ).bHeld) { std::cout << "loop up"    << std::endl; }
            if (GetKey( olc::Key::D ).bHeld) { std::cout << "look down"  << std::endl; }
            if (GetKey( olc::Key::W ).bHeld) { std::cout << "move up"    << std::endl; }
            if (GetKey( olc::Key::S ).bHeld) { std::cout << "move down"  << std::endl; }
        } else {
            // move up, down, left, right
            if (GetKey( olc::Key::A ).bHeld) { std::cout << "left"  << std::endl; }
            if (GetKey( olc::Key::D ).bHeld) { std::cout << "right" << std::endl; }
            if (GetKey( olc::Key::W ).bHeld) { std::cout << "up"    << std::endl; }
            if (GetKey( olc::Key::S ).bHeld) { std::cout << "down"  << std::endl; }
        }
        // strafe left, right
        if (GetKey( olc::Key::LEFT  ).bHeld) { std::cout << "strafe left"   << std::endl; }
        if (GetKey( olc::Key::RIGHT ).bHeld) { std::cout << "strafe right"  << std::endl; }
    }

    void clearBackground() {
        for (int y = 0; y < SH; y++) {
            for (int x = 0; x < SW; x++) {
                myPixel( x, y, 8 );  // clear background colour
            }
        }
    }

    int tick = 0;

    // For now this is just a dummy function performing some illustrative graphics stuff
    void draw3D() {
        int c = 0;
        for (int y = 0; y < SH2; y++) {
            for (int x = 0; x < SW2; x++) {
                myPixel( x, y, c );
                c += 1;
                if (c > 8) { c = 0; }
            }
        }
        // frame rate
        tick += 1; if (tick > 20) { tick = 0; }
        myPixel( SW2, SH2 + tick, 0 );
    }

    // this function is OpenGL's analogon of OnUserUpdate() ig
    void display( float fElapsedTime ) {
        if (T.fr1 - T.fr2 >= 0.05f) {  // only draw 20 frames / second
            clearBackground();
            movePlayer();
            draw3D();
            T.fr2 = T.fr1;
        }
        T.fr1 += fElapsedTime;
    }

    // this function is OpenGL's analogon of OnUserCreate() ig
    void init() {
        // nothing yet
    }

public:
    bool OnUserCreate() override {

        // initialize your assets here
        init();
        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here
        display( fElapsedTime );
        return true;
    }

    bool OnUserDestroy() override {

        // your clean up code here
        return true;
    }
};

int main()
{
	DoomGame demo;
	if (demo.Construct( SW, SH, pixelScale, pixelScale ))
		demo.Start();

	return 0;
}


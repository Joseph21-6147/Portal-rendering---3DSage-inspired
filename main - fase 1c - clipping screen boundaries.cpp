// 3DSage - Let's program Doom - part 1
//
// Youtube: https://youtu.be/huMO4VQEwPc
// got upto: 13m45s
//
// Implemented by Joseph21 on olc::PixelGameEngine
// February 24, 2024

#include <iostream>
#include <cmath>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


#define res         2                 // 1 = 160x120, 2 = 320x240, 4 = 640x480
#define SW          160*res           // screen width
#define SH          120*res           // screen height
#define SW2         (SW/2)            // half of screen width
#define SH2         (SH/2)            // half of screen height
#define pixelScale  4/res             // pixel scale

#define PI          3.1415926535f

//-----------------------------------------------------------------------------

// aux. struct for timing
typedef struct {
    float fr1, fr2;      // frame 1, frame 2, to create constant frame rate (in seconds)
} myTime;
myTime T;

// lookup table for conversion of degrees to sine/cosine values
typedef struct {
    float cos[360];      // save sin and cos values for [0, 359] degrees
    float sin[360];
} myMath;
myMath M;

// player info
typedef struct {
    int x, y, z;         // player position, z is up
    int a;               // player angle of rotation left right
    int l;               // variable to look up and down
} Player;
Player P;

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
        int dx = M.sin[P.a] * 10.0f;
        int dy = M.cos[P.a] * 10.0f;
        if (GetKey( olc::Key::M ).bHeld) {
            // move up, down, look up, down
            if (GetKey( olc::Key::A ).bHeld) { P.l -= 1; }
            if (GetKey( olc::Key::D ).bHeld) { P.l += 1; }
            if (GetKey( olc::Key::W ).bHeld) { P.z -= 4; }
            if (GetKey( olc::Key::S ).bHeld) { P.z += 4; }
        } else {
            // move forward, back, rotate left, right
            if (GetKey( olc::Key::A ).bHeld) { P.a -= 4; if (P.a <   0) { P.a += 360; } }
            if (GetKey( olc::Key::D ).bHeld) { P.a += 4; if (P.a > 359) { P.a -= 360; } }
            if (GetKey( olc::Key::W ).bHeld) { P.x += dx; P.y += dy; }
            if (GetKey( olc::Key::S ).bHeld) { P.x -= dx; P.y -= dy; }
        }
        // strafe left, right
        if (GetKey( olc::Key::LEFT  ).bHeld) { P.x += dy; P.y += dx; }
        if (GetKey( olc::Key::RIGHT ).bHeld) { P.x -= dy; P.y -= dx; }
    }

    void clearBackground() {
        for (int y = 0; y < SH; y++) {
            for (int x = 0; x < SW; x++) {
                myPixel( x, y, 8 );  // clear background colour
            }
        }
    }

    // the top and bottom lines share their x values (vertical lines)
    // b1 and b2 are the bottom two y value points
    // t1 and t2 are the top    two y value points
    void drawWall( int x1, int x2, int b1, int b2, int t1, int t2 ) {
        // hold difference in distance
        int dyb = b2 - b1;                              // y distance of bottom line
        int dyt = t2 - t1;                              // y distance of top    line
        // we will eventually divide by the x distance, so let's prevent division by zero
        int dx  = x2 - x1; if (dx == 0) { dx = 1; }     // x distance
        int xs = x1;                                    // cache initial x1 starting position

        // clip x - clip at 1 so we can visually see that clipping is happening
        if (x1 <       1) { x1 =      1; }   // clip left
        if (x2 <       1) { x2 =      1; }   // clip left
        if (x1 >= SW - 1) { x1 = SW - 2; }   // clip right
        if (x2 >= SW - 1) { x2 = SW - 2; }   // clip right
        // draw x verticle lines
        for (int x = x1; x < x2; x++) {
            // the y start and end point
            // the + 0.5f is to prevent rounding issues
            int y1 = dyb * (x - xs + 0.5f) / dx + b1;   // y bottom point
            int y2 = dyt * (x - xs + 0.5f) / dx + t1;   // y top point

            // clip y - similar to clip x
            if (y1 <       1) { y1 =      1; }    // clip bottom
            if (y2 <       1) { y2 =      1; }    // clip bottom
            if (y1 >= SH - 1) { y1 = SH - 2; }    // clip top
            if (y2 >= SH - 1) { y2 = SH - 2; }    // clip top
            // draw the wall by drawing vertical lines between the bottom and top points
            for (int y = y1; y < y2; y++) {
                myPixel( x, y, 0 );
            }
        }
    }

    // performs projection to draw the scene in 3D:
    //  1. world is translated so that world origin coincides with player position
    //  2. world is rotated around player with player angle
    //  3. looking up/down is implemented using the player's z coordinate
    //  4. projection from world to screen space is done
    //  5. drawing pixels only when they are really on screen
    void draw3D() {
        int wx[4], wy[4], wz[4];                   // local x, y, z values for the wall
        float CS = M.cos[P.a], SN = M.sin[P.a];    // local copies of sin and cos of player angle
        // offset bottom 2 points by player
        int x1 = 40 - P.x, y1 =  10 - P.y;
        int x2 = 40 - P.x, y2 = 290 - P.y;
        // world X position
        wx[0] = x1 * CS - y1 * SN;
        wx[1] = x2 * CS - y2 * SN;
        wx[2] = wx[0];                             // top line has same x value as bottom line
        wx[3] = wx[1];
        // world Y position (depth)
        wy[0] = y1 * CS + x1 * SN;
        wy[1] = y2 * CS + x2 * SN;
        wy[2] = wy[0];                             // top line has same y value as bottom line
        wy[3] = wy[1];
        // world Z position (height)
        wz[0] = 0 - P.z + ((P.l * wy[0]) / 32.0f);
        wz[1] = 0 - P.z + ((P.l * wy[1]) / 32.0f);
        wz[2] = wz[0] + 40;                        // top line has elevated z w.r.t. bottom line
        wz[3] = wz[1] + 40;
        // screen x, screen y position
        wx[0] = wx[0] * 200 / wy[0] + SW2; wy[0] = wz[0] * 200 / wy[0] + SH2;
        wx[1] = wx[1] * 200 / wy[1] + SW2; wy[1] = wz[1] * 200 / wy[1] + SH2;
        wx[2] = wx[2] * 200 / wy[2] + SW2; wy[2] = wz[2] * 200 / wy[2] + SH2;
        wx[3] = wx[3] * 200 / wy[3] + SW2; wy[3] = wz[3] * 200 / wy[3] + SH2;
        // draw lines
        drawWall( wx[0], wx[1], wy[0], wy[1], wy[2], wy[3] );
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
        // store sin & cos in degrees
        for (int v = 0; v < 360; v++) {
            M.cos[v] = cos( v / 180.0f * PI );
            M.sin[v] = sin( v / 180.0f * PI );
        }
        // init player
        P.x =   70;
        P.y = -110;
        P.z =   20;
        P.a =    0;
        P.l =    0;
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

        // display test info on the player
        DrawString( 2, SH - 18, "Pos: ("  + std::to_string( P.x ) + ", "         + std::to_string( P.y ) + ", " + std::to_string( P.z ) + ")" );
        DrawString( 2, SH - 10, "Angle: " + std::to_string( P.a ) + ", lookup: " + std::to_string( P.l ) );

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


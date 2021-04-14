#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

#define FLYROW 100
#define FLYCOL 80
#define FLYSIZE 5
#define MAX_FLIES 2

#define FROGGYROW 120
#define FROGGYCOL 100
#define FROGGYWIDTH 40
#define FROGGYHEIGHT 32

// TODO: Create any necessary structs

/*
* For example, for a Snake game, one could be:
*
* struct snake {
*   int heading;
*   int length;
*   int row;
*   int col;
* };
*
* Example of a struct to hold state machine data:
*
* struct state {
*   int currentState;
*   int nextState;
* };
*
*/

struct fly {
    int row;
    int col;
    int rd;
    int cd;
    int size;
    u16 color;
} currFly1, oldFly1, currFly2, oldFly2;

struct froggo {
    int row;
    int col;
    int rd;
    int cd;
    int width;
    int height;
} currFroggy, oldFroggy;

struct state {
    int nflies;
    struct fly flies[MAX_FLIES];
} cs, ps;

// Prototypes
void prepareBouncing(void);
void updateLocations(void);
int updateTimer(void);
void drawGameText(int score, int timer);
void bounceCheck(void);
void collisionCheck(void);
void frogBorderCheck(void);
void reset(void);
void drawIntro(void);

#endif
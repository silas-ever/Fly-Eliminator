/* TODO: */
// Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
// #include "images/garbage.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "gba.h"

#include "images/welcome.h"
#include "images/end.h"
#include "images/fly.h"
#include "images/swatter.h" // 40 x 32

/* TODO: */
// Add any additional states you need for your app. You are not requried to use
// these specific provided states.
enum gba_state {
  START,
  INTRO,
  PLAY,
  END
};

int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE; // Manipulate REG_DISPCNT here to set Mode 3.
  u32 previousButtons = BUTTONS; // Save current and previous state of button input
  u32 currentButtons = BUTTONS;
  enum gba_state state = START; // Load initial application state

  int timer = 0;
  int score = 0;
  int victory = 0;
  int fly1Caught = 0;
  int fly2Caught = 0;
  char bufferEnd[51];

  int deltas[] = {-2, -1, 1, 2};
  int ndeltas = sizeof(deltas) / sizeof(deltas[0]);

  currFroggy.row = FROGGYROW;
  currFroggy.col = FROGGYCOL;
  currFroggy.width = FROGGYWIDTH;
  currFroggy.height = FROGGYHEIGHT;
  currFly1.size = FLYSIZE;
  currFly2.size = FLYSIZE;


  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons
    /* TODO:Manipulate the state machine below as needed
    NOTE: Call waitForVBlank() before you draw */

    // CALCULATIONS STATE
    switch (state) {
      case START:
        score = 5;
        fly1Caught = 0;
        fly2Caught = 0;
        reset();
        prepareBouncing(); // Generating random locations for fly to start
        break;

      case INTRO:
        break;
      
      case PLAY:
        updateLocations();
        frogBorderCheck();
        bounceCheck();

        // Collision check
        if ((currFly1.row < currFroggy.row + currFroggy.width &&
         currFly1.row + currFly1.size > currFroggy.row &&
         currFly1.col < currFroggy.col + currFroggy.height &&
         currFly1.col + currFly1.size > currFroggy.col) && (fly1Caught == 0)) {
          fly1Caught = 1;
          score -= 3;
        }
        if ((currFly2.row < currFroggy.row + currFroggy.width &&
         currFly2.row + currFly2.size > currFroggy.row &&
         currFly2.col < currFroggy.col + currFroggy.height &&
         currFly2.col + currFly2.size > currFroggy.col) && (fly2Caught == 0)) {
          fly2Caught = 1;
          score -= 2;
        }

        // Update timer
        timer = updateTimer();

        // Swatter movement
        if (KEY_JUST_PRESSED(BUTTON_UP, currentButtons, previousButtons)) {
          currFroggy.rd = -1;
          currFroggy.cd = 0;
        }
        if (KEY_JUST_PRESSED(BUTTON_DOWN, currentButtons, previousButtons)) {
          currFroggy.rd = 1;
          currFroggy.cd = 0;
        }
        if (KEY_JUST_PRESSED(BUTTON_LEFT, currentButtons, previousButtons)) {
          currFroggy.rd = 0;
          currFroggy.cd = -1;
        }
        if (KEY_JUST_PRESSED(BUTTON_RIGHT, currentButtons, previousButtons)) {
          currFroggy.rd = 0;
          currFroggy.cd = 1;
        }

        // Fly movement
        currFly1.rd = deltas[rand() % ndeltas];
        currFly1.cd = deltas[rand() % ndeltas];
        currFly2.rd = deltas[rand() % ndeltas];
        currFly2.cd = deltas[rand() % ndeltas];

        break;

      case END:
        break;
    }

    waitForVBlank();

    // DRAWING STATE
    switch(state) {
      case START:
        drawFullScreenImageDMA(welcome);
        drawString(150, 60, "Press start to play.", BLUE);
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          state = INTRO;
        }
        break;
      case INTRO:
        drawIntro();

        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
          state = START;
        }
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          state = PLAY;
          fillScreenDMA(WHITE);
          vBlankCounter = 0;
        }
        break;

      case PLAY:
        // Erase old flies, frog, timer, and score
        drawRectDMA(oldFroggy.row, oldFroggy.col, FROGGYWIDTH, FROGGYHEIGHT, WHITE);
        drawRectDMA(oldFly1.row, oldFly1.col, 5, 5, WHITE);
        drawRectDMA(oldFly2.row, oldFly2.col, 5, 5, WHITE);
        drawRectDMA(130, 5, 80, 30, WHITE);

        // Draw new flies, frog, timer, and score
        drawGameText(score, timer);
        if (fly1Caught == 0) {
          drawRectDMA(currFly1.row, currFly1.col, FLYSIZE, FLYSIZE, RED);
        }
        if (fly2Caught == 0) {
          drawRectDMA(currFly2.row, currFly2.col, FLYSIZE, FLYSIZE, BLACK);
        }
        drawImageDMA(currFroggy.row, currFroggy.col, FROGGYWIDTH, FROGGYHEIGHT, swatter); // Draw frog
        
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
          state = START;
        }
        if ((fly1Caught) && (fly2Caught) && (timer > 0)) {
          victory = 1;
          state = END;
        }
        if (timer == 0) {
          victory = 0;
          state = END;
        }

        break;
      
      case END:
        drawFullScreenImageDMA(end);
        if (victory) {
          sprintf(bufferEnd, "You Won!");
          drawCenteredString(0, 0, 240, 160, bufferEnd, WHITE);
        } else {
          sprintf(bufferEnd, "You Lost!");
          drawCenteredString(0, 0, 240, 160, bufferEnd, WHITE);
        }

        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
          state = START;
        }
        break;
    }

    previousButtons = currentButtons; // Store the current state of the buttons
    ps = cs;
    oldFroggy = currFroggy;
    oldFly1 = currFly1;
    oldFly2 = currFly2;
  } // Game loop
  return 0;
}

void prepareBouncing(void) {
  int deltas[] = {-1, 1};
  int ndeltas = sizeof(deltas) / sizeof(deltas[0]);
  
  currFly1.row = rand() % (HEIGHT - 70);
  currFly1.col = rand() % (WIDTH);
  currFly1.rd = deltas[rand() % ndeltas];
  currFly1.cd = deltas[rand() % ndeltas];

  currFly2.row = rand() % (HEIGHT - 70);
  currFly2.col = rand() % (WIDTH);
  currFly2.rd = deltas[rand() % ndeltas];
  currFly2.cd = deltas[rand() % ndeltas];
}

void updateLocations(void) {
  // Update frog location
  currFroggy.row = currFroggy.row + currFroggy.rd;
  currFroggy.col = currFroggy.col + currFroggy.cd;

  // Update fly locations
  currFly1.row = currFly1.row + currFly1.rd;
  currFly1.col = currFly1.col + currFly1.cd;
  currFly2.row = currFly2.row + currFly2.rd;
  currFly2.col = currFly2.col + currFly2.cd;
}

int updateTimer(void) {
  return 30 - (vBlankCounter / 60);
}

void drawGameText(int score, int timer) {
  char buffer[51];
  char buffer2[51];
  sprintf(buffer, "Hunger: %d", score);
  drawString(145, 5, buffer, MAGENTA);
  sprintf(buffer2, "Time Left: %d", timer);
  drawString(135, 5, buffer2, BLUE);
}

void bounceCheck(void) {
  // Fly 1
  if (currFly1.row < 0) {
    currFly1.row = 0; 
    currFly1.rd = -currFly1.rd;
  }
  if (currFly1.row > 155) {
    currFly1.row = 155;
    currFly1.rd = -currFly1.rd;
  }
  if (currFly1.col < 0) {
    currFly1.col = 0;
    currFly1.cd = -currFly1.cd;
  }
  if (currFly1.col > 235) {
    currFly1.col = 235;
    currFly1.cd = -currFly1.cd;
  }

  // Fly 2
  if (currFly2.row < 0) {
    currFly2.row = 0; 
    currFly2.rd = -currFly1.rd;
  }
  if (currFly2.row > 155) {
    currFly2.row = 155;
    currFly2.rd = -currFly1.rd;
  }
  if (currFly2.col < 0) {
    currFly2.col = 0;
    currFly2.cd = -currFly1.cd;
  }
  if (currFly2.col > 235) {
    currFly2.col = 235;
    currFly2.cd = -currFly1.cd;
  }
}

void frogBorderCheck(void) {
  if (currFroggy.row > (HEIGHT - FROGGYHEIGHT)) {
    currFroggy.row = (HEIGHT - FROGGYHEIGHT);
  }
  if (currFroggy.row < 0) {
    currFroggy.row = 0;
  }
  if (currFroggy.col > (WIDTH - FROGGYWIDTH)) {
    currFroggy.col = (WIDTH - FROGGYWIDTH);
  }
  if (currFroggy.col < 0) {
    currFroggy.col = 0;
  }
}

void reset(void) {
  vBlankCounter = 0;
  currFroggy.row = FROGGYROW;
  currFroggy.col = FROGGYCOL;
  currFroggy.cd = 0;
  currFroggy.rd = 0;
}

void drawIntro(void) {
  char introString[51];
  char introString2[51];
  char introString3[51];

  fillScreenDMA(BLACK);
  sprintf(introString, "You are a hungry frog.");
  drawCenteredString(-30, 0, 240, 160, introString, GREEN);
  sprintf(introString2, "Eat the flies");
  drawCenteredString(30, 0, 240, 160, introString2, WHITE);
  sprintf(introString3, "before time runs out!");
  drawCenteredString(40, 0, 240, 160, introString3, WHITE);
}
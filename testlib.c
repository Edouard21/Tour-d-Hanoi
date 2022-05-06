#include "primlib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NB_DISCS 20 // 2..20
#define NB_PEGS 10  // 3..10
#define PEGS_HEIGHT 400
#define PEGS_WIDTH 5
#define MARGIN 50
#define DISCS_MARGE 30
#define DISCS_HEIGHT_MAX (PEGS_HEIGHT - DISCS_MARGE) / NB_DISCS
#define MAX_SIZE_DISCS 75
#define DELAY 2

typedef struct {
	int x;
	int y;
	int place;
	int peg;
} discs;

typedef struct {
	int x;
	int y;
} pegs;

void moveDiscs(int start, int arrival, int indexDiscs, int heightDiscs);

discs positions[NB_DISCS];

pegs positionsPegs[NB_PEGS];

void drawPegs();

void drawDiscs();

void discsPoint();

int checkHigh(int pegStart);

bool checkWin();

int discsCount();

int checkWidth(int indexDiscs, int arrival);

void drawGame();

void takeUserCommand(int* value1, int* value2);

void endGame();

void initGame();

void animateDiscs(int value1, int value2);

void pressSpaceToLeave(int value);

int main(int argc, char* argv[]) {

	if (gfx_init() || NB_PEGS < 2 || NB_PEGS > 10 || NB_DISCS < 2 ||
		NB_PEGS > 20) {
		exit(3);
	}

	initGame();

	int value1;
	int value2;

	do {

		takeUserCommand(&value1, &value2);

		animateDiscs(value1, value2);

		endGame();

	} while (!checkWin());

	gfx_getkey();
}

int checkWidth(int indexDiscs, int arrival) {

	for (int i = 0; i < NB_DISCS; i++) {
		if (positions[i].peg == arrival) {
			if (i >= indexDiscs) {
				return 0;
			}
		}
	}

	return 1;
}

int discsCount(int arrival) {

	int count = 0;
	for (int j = 0; j < NB_DISCS; j++) {
		if (positions[j].peg == arrival) {
			count = count + 1;
		} else {
			continue;
		}
	}

	return count;
}

bool checkWin() {

	int i = 0;
	int pegIndex = positions[i].peg;

	while (i < NB_DISCS && positions[i].peg != 0 &&
		   positions[i].peg == pegIndex) {

		i++;
	}

	if (i >= NB_DISCS) {

		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);

		gfx_textout(500, 250, "well played ! YOU WON !", RED);

		gfx_updateScreen();

		return true;

	} else {
		return false;
	}
}

int checkHigh(int pegStart) {

	int indexHigh = -1;

	for (int i = 0; i < NB_DISCS; i++) {

		if (positions[i].peg == pegStart) {

			if (indexHigh == -1) {

				indexHigh = i;

			} else if (positions[indexHigh].y >= positions[i].y) {

				indexHigh = i;
			}
		}
	}
	return indexHigh;
}

void moveDiscs(int start, int arrival, int indexDiscs, int heightDiscs) {

	if (positions[indexDiscs].x == positionsPegs[arrival].x &&
		positions[indexDiscs].y >= positionsPegs[arrival].y -
									   DISCS_HEIGHT_MAX / 2 -
									   DISCS_HEIGHT_MAX * heightDiscs) {

		positions[indexDiscs].peg = arrival;

	} else if (positions[indexDiscs].x == positionsPegs[start].x &&
			   positions[indexDiscs].y >
				   positionsPegs[arrival].y - PEGS_HEIGHT - DISCS_HEIGHT_MAX) {

		positions[indexDiscs].y = positions[indexDiscs].y - 1;

	} else if (positions[indexDiscs].x == positionsPegs[arrival].x &&
			   positions[indexDiscs].y < positionsPegs[arrival].y -
											 DISCS_HEIGHT_MAX / 2 -
											 DISCS_HEIGHT_MAX * heightDiscs) {

		positions[indexDiscs].y = positions[indexDiscs].y + 1;

	} else if (positions[indexDiscs].x < positionsPegs[arrival].x) {

		positions[indexDiscs].x = positions[indexDiscs].x + 1;

	} else if (positions[indexDiscs].x > positionsPegs[arrival].x) {
		positions[indexDiscs].x = positions[indexDiscs].x - 1;
	}
}

void drawPegs() {

	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
	gfx_filledRect(0, 601, gfx_screenWidth(), gfx_screenHeight(), YELLOW);

	int screenSize = gfx_screenWidth() - 2 * MARGIN;

	for (int i = 0; i < NB_PEGS; i++) {
		positionsPegs[i].x = ((screenSize * (i + 1)) / (NB_PEGS + 1)) + MARGIN;
		positionsPegs[i].y = 600;
		gfx_filledRect(positionsPegs[i].x - PEGS_WIDTH / 2, positionsPegs[i].y,
					   positionsPegs[i].x + PEGS_WIDTH / 2,
					   positionsPegs[i].y - PEGS_HEIGHT, RED);
	}
}

void discsPoint() {

	for (int i = 0; i < NB_DISCS; i++) {
		positions[i].x = positionsPegs[0].x;
		positions[i].y =
			positionsPegs[0].y - (i * DISCS_HEIGHT_MAX + DISCS_HEIGHT_MAX / 2);
	}
}

void drawDiscs() {

	for (int k = 0; k < NB_DISCS; k++) {

		int halfwidth = (((100.0 / NB_DISCS) * (NB_DISCS - k + 1.0)) / 100.0) *
						(MAX_SIZE_DISCS / 2);

		gfx_filledRect(positions[k].x - halfwidth,
					   positions[k].y + DISCS_HEIGHT_MAX / 2,
					   positions[k].x + halfwidth,
					   positions[k].y - DISCS_HEIGHT_MAX / 2, MAGENTA);
		gfx_rect(positions[k].x - halfwidth,
				 1 + positions[k].y + DISCS_HEIGHT_MAX / 2,
				 1 + positions[k].x + halfwidth,
				 positions[k].y - DISCS_HEIGHT_MAX / 2, BLUE);

		positions[k].place = k;
	}
}

void drawGame() {

	drawPegs();
	drawDiscs();
	gfx_updateScreen();
}

void takeUserCommand(int* value1, int* value2) {

	do {
		*value1 = gfx_getkey();
		pressSpaceToLeave(*value1);
		*value2 = gfx_getkey();
		pressSpaceToLeave(*value2);
	} while ((*value1 < '0') || (*value1 > '0' + NB_PEGS) || (*value2 < '0') ||
			 (*value2 > '0' + NB_PEGS) || (*value1 == *value2));

	*value1 = *value1 - '0';
	*value2 = *value2 - '0';
}

void endGame() {
	pressSpaceToLeave(gfx_pollkey());
}

void initGame() {

	drawPegs();
	discsPoint();

	for (int k = 0; k < NB_DISCS; k++) {
		positions[k].peg = 0;
	}

	drawDiscs();
	gfx_updateScreen();
}

void animateDiscs(int value1, int value2) {

	int high = checkHigh(value1);
	int count = discsCount(value2);

	while (high != -1 && checkWidth(high, value2) != 0 &&
		   positions[high].peg == value1) {

		moveDiscs(value1, value2, high, count);
		drawGame();
		SDL_Delay(DELAY);

		endGame();
	}
}

void pressSpaceToLeave(int value) {
	if (value == ' ') {
		exit(0);
	}
}

#include <nds.h>
#include <stdio.h>
#include "console.h"

void waitPressA() {
	iprintf("press <A>\n\n");
	while(1) {
		scanKeys();
		if(keysDown() & KEY_A)
			break;
		swiWaitForVBlank();
	}
}

int percent(int c, int t) {
    return c * 100 / t;
}

void printProgress(const char *string, uint32_t curr, uint32_t total) {
    static int old = 100;
    int pct = percent(curr, total);
    if (pct % 5 == 0 && old != pct);
    int space = 27 - strlen(string);
    iprintf("\r");
    iprintf(string);
    while (space--) {
        iprintf(" ");
    }
    iprintf("%3d%%", pct);
    old = pct;
}



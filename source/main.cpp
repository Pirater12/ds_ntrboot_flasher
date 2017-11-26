#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "device.h"
#include "console.h"
#include "binaries.h"
#include "nds_platform.h"

using namespace flashcart_core;

// FIXME: not fully overwrite
u8 orig_flashrom[0xA0000] = {0};
u8 curr_flashrom[0xA0000] = {0};

PrintConsole topScreen;
PrintConsole bottomScreen;

Flashcart* selectCart() {
    uint32_t idx = 0;

    consoleSelect(&bottomScreen);
    consoleClear();

    iprintf("<UP/DOWN> Select flashcart\n<A> Confirm\n<B> Exit");

    while (true) {
        consoleSelect(&topScreen);
        consoleClear();
        Flashcart *cart = flashcart_list->at(idx);
        iprintf("Flashcart: %s\n", cart->getName());
        //iprintf("    - %s\n", cart->getAuthor());
        iprintf("\n%s", cart->getDescription());

        while (true) {
            scanKeys();
            if (keysDown(); & KEY_UP) {
                if (idx > 0) {
                    idx--;
                }
                break;
            }
            if (keysDown(); & KEY_DOWN) {
                idx++;
                if (idx >= flashcart_list->size()) {
                    idx = flashcart_list->size() - 1;
                }
                break;
            }
            if (keysDown(); & KEY_A) {
                return cart;
            }
            if (keysDown(); & KEY_B) {
                return NULL;
            }
            swiWaitForVBlank();
        }
    }
}

int inject(Flashcart *cart) {
    consoleSelect(&bottomScreen);
    consoleClear();
    iprintf("Inject ntrboothax\n\nDO NOT EJECT FLASHCART\nUNTIL GET SPECIAL ORDER\n\n");
    cart->injectNtrBoot(blowfish_retail_bin, boot9strap_ntr_firm, boot9strap_ntr_firm_size);
    iprintf("\nDone\n\n");
    waitPressA();
    return 0;
}

int compareBuf(u8 *buf1, u8 *buf2, u32 len) {
    for (uint32_t i = 0; i < len; i++) {
        if (buf1[i] != buf2[i]) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
	consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true); //Necessary stuff to print to console
    sysSetBusOwners(true, true); // give ARM9 access to the cart

    consoleSelect(&topScreen);
    iprintf("=    NDS NTRBOOT FLASHER    =\n\n");
    consoleSelect(&bottomScreen);
    iprintf("* Your cart cannot be used  *\n* as a flashcart after it   *\n* is flashed (except AK2i)  *\n* WARNING: ONLY TESTED ON   *\n* 3DS TWL MODE              *\n* AT YOUR OWN RISK          *\n");
    waitPressA();
    consoleClear();

    Flashcart *cart; cart = NULL;

    while (true) {
        cart = selectCart();
        if (cart == NULL) {
            return 0;
        }

        consoleSelect(&bottomScreen);
        consoleClear();
        reset();
        if (cart->initialize()) {
            break;
        }
        iprintf("Flashcart setup failed\n");
        waitPressA();
    }

    while (true) {
        consoleSelect(&bottomScreen);
        consoleClear();
        iprintf("NOT SUPPORT RESTORE\nFlashcart functionality will\nbe lost.\n\n<A> Inject ntrboothax\n<B> Exit\n");

        while (true) {
            scanKeys();

            if (keysDown() & KEY_A) {
                inject(cart);
                break;
            }

            if (keysDown() & KEY_B) {
                cart->shutdown();
                return 0;
            }
            swiWaitForVBlank();
        }
    }
    return 0;
}

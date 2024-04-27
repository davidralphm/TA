#include "button.h"
#include "font.h"
#include "game.h"
#include "sounds.h"
#include "ui.h"

#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>

void Button_Init(Button *button, int x, int y, int w, int h, const char *buttonText, char centered, void (*callback)(Game *game)) {
    if (button == NULL) return;

    button->buttonRect.x = x;
    button->buttonRect.y = y;
    button->buttonRect.w = SDL_max((strlen(buttonText) + 4) * FONT_CHAR_W, w);
    button->buttonRect.h = h;

    if (centered) {
        button->buttonRect.x -= button->buttonRect.w / 2;
        button->buttonRect.y -= button->buttonRect.h / 2;
    }

    snprintf(button->buttonText, BUTTON_TEXT_LEN_MAX, buttonText);

    button->callback = callback;
}

void Button_Update(Game *game, Button *button, int mouseX, int mouseY, int mouseClick) {
    if (button->callback == NULL)
        return;

    int x1 = button->buttonRect.x;
    int y1 = button->buttonRect.y;
    int x2 = button->buttonRect.x + button->buttonRect.w;
    int y2 = button->buttonRect.y + button->buttonRect.h;

    if (mouseX >= x1 && mouseX <= x2 && mouseY >= y1 && mouseY <= y2 && mouseClick == 1) {
        button->callback(game);

        Sounds_Play(SOUND_EFFECT_BUTTON);

        game->globals.player.mouseClick = 0;
    }
}

void Button_Render(Game *game, Button *button) {
    UI_DrawButton(game, NULL, &button->buttonRect);

    Font_Draw(
        game->renderer,
        button->buttonText,
        button->buttonRect.x + button->buttonRect.w / 2,
        button->buttonRect.y + button->buttonRect.h / 2,
        1
    );
}
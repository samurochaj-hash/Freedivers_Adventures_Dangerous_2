#include <libdragon.h>
#include <stdint.h>

/* Freediver's Adventures Dangerous 2
 * Nintendo 64 starter scene. This first milestone establishes the video,
 * controller, HUD and a simple underwater placeholder scene.
 * 3D meshes/camera are the next milestone; this is not the finished game.
 */

#define SCREEN_W 320
#define SCREEN_H 240

static int diver_x = 150;
static int diver_y = 112;
static int air_level = 100;
static int hp_level = 100;

static void draw_rect(int x1, int y1, int x2, int y2, color_t color) {
    rdpq_set_mode_fill(color);
    rdpq_fill_rectangle(x1, y1, x2, y2);
}

static void draw_scene(void) {
    /* Water background and distant seabed bands. */
    draw_rect(0, 0, SCREEN_W, SCREEN_H, RGBA32(5, 35, 82, 255));
    draw_rect(0, 145, SCREEN_W, 190, RGBA32(8, 65, 105, 255));
    draw_rect(0, 190, SCREEN_W, SCREEN_H, RGBA32(45, 48, 42, 255));

    /* Stylized coral silhouettes. */
    draw_rect(25, 174, 43, 207, RGBA32(210, 80, 65, 255));
    draw_rect(31, 158, 37, 180, RGBA32(230, 120, 70, 255));
    draw_rect(270, 180, 292, 211, RGBA32(210, 90, 110, 255));
    draw_rect(278, 164, 284, 185, RGBA32(235, 140, 90, 255));

    /* Diver placeholder: body, tank, mask and fins. */
    draw_rect(diver_x, diver_y, diver_x + 27, diver_y + 10, RGBA32(235, 185, 45, 255));
    draw_rect(diver_x + 7, diver_y - 7, diver_x + 21, diver_y + 17, RGBA32(30, 95, 125, 255));
    draw_rect(diver_x + 21, diver_y - 4, diver_x + 30, diver_y + 5, RGBA32(100, 210, 230, 255));
    draw_rect(diver_x - 9, diver_y + 4, diver_x + 4, diver_y + 10, RGBA32(235, 185, 45, 255));
    draw_rect(diver_x + 25, diver_y + 8, diver_x + 39, diver_y + 13, RGBA32(235, 185, 45, 255));

    /* Bubbles. */
    draw_rect(diver_x + 27, diver_y - 17, diver_x + 30, diver_y - 14, RGBA32(180, 225, 245, 255));
    draw_rect(diver_x + 34, diver_y - 27, diver_x + 37, diver_y - 24, RGBA32(180, 225, 245, 255));

    /* HUD panels and meters. */
    draw_rect(8, 8, 116, 34, RGBA32(0, 0, 0, 190));
    draw_rect(14, 15, 54, 21, RGBA32(110, 25, 25, 255));
    draw_rect(14, 15, 14 + (40 * hp_level / 100), 21, RGBA32(220, 55, 55, 255));
    draw_rect(14, 25, 54, 31, RGBA32(20, 45, 100, 255));
    draw_rect(14, 25, 14 + (40 * air_level / 100), 31, RGBA32(45, 145, 240, 255));

    /* Simple depth gauge. */
    draw_rect(300, 48, 308, 170, RGBA32(0, 0, 0, 180));
    draw_rect(302, 50, 306, 168, RGBA32(65, 165, 205, 255));
}

int main(void) {
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_DISABLED);
    rdpq_init();
    joypad_init();

    while (1) {
        joypad_poll();
        joypad_buttons_t keys = joypad_get_buttons_held(JOYPAD_PORT_1);

        if (keys.d_left  && diver_x > 12)  diver_x -= 2;
        if (keys.d_right && diver_x < 265) diver_x += 2;
        if (keys.d_up    && diver_y > 48)  diver_y -= 2;
        if (keys.d_down  && diver_y < 175) diver_y += 2;

        surface_t *disp;
        while (!(disp = display_try_get())) { }
        rdpq_attach_clear(disp, NULL);
        draw_scene();
        rdpq_detach_show();
    }
    return 0;
}

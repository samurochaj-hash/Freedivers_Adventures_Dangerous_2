#include <libdragon.h>
#include <math.h>
#include <stdint.h>

/* Freediver's Adventures Dangerous 2 - Milestone 3
 * CPU-side perspective projection + low-poly triangle scene for Nintendo 64.
 * This is an original homebrew prototype, not a commercial-game reverse port.
 */
#define SCREEN_W 320
#define SCREEN_H 240
#define FOCAL 145.0f
#define PI 3.14159265358979323846f

typedef struct { float x, y, z; } Vec3;
typedef struct { Vec3 a, b, c; color_t color; } Face;

static float player_x = 0.0f;
static float player_y = 0.0f;
static float swim_phase = 0.0f;
static int hp_level = 100;
static int air_level = 100;
static int frame_count = 0;

static void rect(int x1, int y1, int x2, int y2, color_t color) {
    rdpq_set_mode_fill(color);
    rdpq_fill_rectangle(x1, y1, x2, y2);
}

/* Camera looks forward along +Z. Player is the camera target. */
static int project(Vec3 p, float *sx, float *sy) {
    float z = p.z + 7.0f;
    if (z < 1.0f) return 0;
    *sx = 160.0f + (p.x * FOCAL / z);
    *sy = 124.0f - (p.y * FOCAL / z);
    return 1;
}

static void triangle_3d(Vec3 a, Vec3 b, Vec3 c, color_t color) {
    float ax, ay, bx, by, cx, cy;
    if (!project(a, &ax, &ay) || !project(b, &bx, &by) || !project(c, &cx, &cy)) return;
    if ((ax < -80 && bx < -80 && cx < -80) || (ax > 400 && bx > 400 && cx > 400) ||
        (ay < -80 && by < -80 && cy < -80) || (ay > 320 && by > 320 && cy > 320)) return;
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_set_prim_color(color);
    float v1[2] = { ax, ay };
    float v2[2] = { bx, by };
    float v3[2] = { cx, cy };
    rdpq_triangle(&TRIFMT_FILL, v1, v2, v3);
}

static void face(Vec3 a, Vec3 b, Vec3 c, color_t col) {
    triangle_3d(a, b, c, col);
}

static void draw_rock(float x, float y, float z, float s) {
    Vec3 p0={x-s,y,z}, p1={x,y+s,z}, p2={x+s,y,z}, p3={x,y-s,z};
    Vec3 top={x,y+s*1.4f,z+0.4f};
    face(p0,p1,top,RGBA32(55,91,104,255));
    face(p1,p2,top,RGBA32(45,78,94,255));
    face(p2,p3,top,RGBA32(35,65,82,255));
    face(p3,p0,top,RGBA32(48,78,91,255));
}

static void draw_coral(float x, float y, float z, float s) {
    color_t coral1=RGBA32(225,92,78,255), coral2=RGBA32(248,142,90,255);
    face((Vec3){x-s,y,z},(Vec3){x+s,y,z},(Vec3){x,y+s*2.2f,z+0.2f},coral1);
    face((Vec3){x-s*0.65f,y+s*0.7f,z+0.1f},(Vec3){x-s*1.3f,y+s*1.5f,z},(Vec3){x-s*0.1f,y+s*1.8f,z+0.1f},coral2);
    face((Vec3){x+s*0.2f,y+s*0.8f,z+0.1f},(Vec3){x+s*1.2f,y+s*1.6f,z},(Vec3){x+s*0.4f,y+s*2.0f,z+0.1f},coral2);
}

static void draw_fish(float x, float y, float z, float s) {
    color_t body=RGBA32(245,183,54,255), shade=RGBA32(195,105,45,255);
    face((Vec3){x-s,y,z},(Vec3){x,y+s*0.55f,z},(Vec3){x+s,y,z},body);
    face((Vec3){x-s,y,z},(Vec3){x+s,y,z},(Vec3){x,y-s*0.55f,z},shade);
    face((Vec3){x+s*0.75f,y,z},(Vec3){x+s*1.45f,y+s*0.5f,z},(Vec3){x+s*1.45f,y-s*0.5f,z},body);
}

static void draw_diver(void) {
    float kick = sinf(swim_phase) * 0.12f;
    color_t suit=RGBA32(28,78,112,255), suit_light=RGBA32(48,125,155,255);
    color_t yellow=RGBA32(238,190,48,255), glass=RGBA32(110,220,238,255);
    /* Diver is a small low-poly mesh centered in front of the camera. */
    Vec3 head={0.0f,0.42f,1.8f}, shoulderL={-0.28f,0.25f,1.9f}, shoulderR={0.28f,0.25f,1.9f};
    Vec3 hipL={-0.20f,-0.12f,2.0f}, hipR={0.20f,-0.12f,2.0f};
    Vec3 footL={-0.48f,-0.43f+kick,2.35f}, footR={0.48f,-0.43f-kick,2.35f};
    face(shoulderL,head,shoulderR,glass);
    face(shoulderL,shoulderR,hipR,suit_light); face(shoulderL,hipR,hipL,suit);
    face(hipL,hipR,footR,yellow); face(hipL,footR,footL,yellow);
    /* Air tank and mask highlight. */
    face((Vec3){-0.36f,0.20f,2.05f},(Vec3){-0.25f,0.22f,2.05f},(Vec3){-0.27f,-0.18f,2.05f},RGBA32(175,185,190,255));
    face((Vec3){0.03f,0.43f,1.76f},(Vec3){0.25f,0.40f,1.76f},(Vec3){0.15f,0.28f,1.74f},glass);
}

static void draw_hud(void) {
    rect(7,7,111,38,RGBA32(0,8,22,210));
    rect(13,14,58,21,RGBA32(85,20,25,255));
    rect(13,14,13+(45*hp_level/100),21,RGBA32(225,55,55,255));
    rect(13,26,58,33,RGBA32(15,38,100,255));
    rect(13,26,13+(45*air_level/100),33,RGBA32(40,145,245,255));
    /* Depth scale on the right. */
    rect(302,54,309,174,RGBA32(0,8,20,180));
    rect(304,56,307,172,RGBA32(25,105,145,255));
    rect(300,120,311,124,RGBA32(235,225,160,255));
    /* Tiny label bars: HP and AIR represented by colored marks for this milestone. */
    rect(13,41,33,43,RGBA32(235,65,65,255));
    rect(38,41,65,43,RGBA32(55,155,245,255));
}

static void draw_world(void) {
    rect(0,0,SCREEN_W,SCREEN_H,RGBA32(4,28,67,255));
    /* Distant light shafts and layered seabed establish depth. */
    face((Vec3){-5.5f,3.8f,14.0f},(Vec3){-2.8f,3.8f,14.0f},(Vec3){-4.0f,-2.0f,14.0f},RGBA32(12,67,105,255));
    face((Vec3){-2.4f,3.8f,14.0f},(Vec3){0.0f,3.8f,14.0f},(Vec3){-1.0f,-2.0f,14.0f},RGBA32(10,58,96,255));
    face((Vec3){-7.0f,-1.5f,10.0f},(Vec3){7.0f,-1.5f,10.0f},(Vec3){7.0f,-3.8f,10.0f},RGBA32(38,55,55,255));
    face((Vec3){-7.0f,-1.5f,10.0f},(Vec3){7.0f,-3.8f,10.0f},(Vec3){-7.0f,-3.8f,10.0f},RGBA32(48,61,55,255));

    /* Scenery is positioned relative to the diver so the camera follows. */
    draw_rock(-3.0f-player_x,-1.3f-player_y,8.0f,1.0f);
    draw_rock(3.2f-player_x,-1.4f-player_y,9.5f,1.2f);
    draw_coral(-2.1f-player_x,-1.0f-player_y,7.0f,0.42f);
    draw_coral(2.4f-player_x,-1.0f-player_y,8.0f,0.5f);
    draw_fish(-1.8f-player_x,1.0f-player_y,5.5f,0.32f);
    draw_fish(2.0f-player_x,1.45f-player_y,7.5f,0.42f);
    draw_rock(0.3f-player_x,-1.8f-player_y,13.0f,1.7f);
    draw_diver();

    /* Suspended particles/bubbles as small screen-space squares. */
    for (int i=0;i<9;i++) {
        int x=(i*43 + frame_count*2)%310 + 4;
        int y=(i*29 + frame_count/3)%180 + 42;
        rect(x,y,x+2,y+2,RGBA32(130,205,225,210));
    }
    draw_hud();
}

int main(void) {
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_DISABLED);
    rdpq_init();
    joypad_init();

    while (1) {
        joypad_poll();
        joypad_buttons_t keys = joypad_get_buttons_held(JOYPAD_PORT_1);
        if (keys.d_left)  player_x -= 0.045f;
        if (keys.d_right) player_x += 0.045f;
        if (keys.d_up)    player_y += 0.035f;
        if (keys.d_down)  player_y -= 0.035f;
        if (player_x < -2.6f) player_x=-2.6f;
        if (player_x >  2.6f) player_x= 2.6f;
        if (player_y < -1.4f) player_y=-1.4f;
        if (player_y >  1.4f) player_y= 1.4f;
        swim_phase += 0.12f;
        if (swim_phase > 2.0f*PI) swim_phase -= 2.0f*PI;
        frame_count++;

        surface_t *disp;
        while (!(disp = display_try_get())) { }
        rdpq_attach_clear(disp, NULL);
        draw_world();
        rdpq_detach_show();
    }
    return 0;
}

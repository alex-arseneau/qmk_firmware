/* Copyright 2022 ziptyze
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include <quantum/deferred_exec.h>
#include <stdint.h>

// VIA's built-in qmk_rgb_matrix menu sends effect numbers by position; keep the enabled effects in config.h aligned with it.
_Static_assert(RGB_MATRIX_PIXEL_FRACTAL == 30 && RGB_MATRIX_DIGITAL_RAIN == 32, "RGB effect numbers no longer match VIA's lighting menu");


enum custom_keycodes {
    MS_JIGL = QK_KB_0, // must stay in step with customKeycodes in via.json
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT_20_ansi(
        MS_JIGL,       KC_MPRV,                KC_MPLY,            KC_MNXT,            KC_MUTE,
        KC_F11,         LSFT(KC_F11),           KC_F10,             LSFT(KC_F9),        KC_F9,
        KC_F12,         LSFT(KC_F12),           S(KC_ENT),          LCTL(KC_D),         KC_F5,
        LCTL(KC_MINS),  LSFT(LCTL(KC_MINS)),    LCTL(LSFT(KC_B)),   LCTL(LSFT(KC_R)),   KC_F1
    )
};

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] =  { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), },
    };

static void render_logo(void) {
    static const char PROGMEM qmk_logo[] = {
    // 'blitzpad', 128x32px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
0x80, 0x00, 0x00, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 0xc0, 0xe0, 0xe0,
0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0,
0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xc0,
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x1f, 0x1c, 0x1c, 0xff, 0xff, 0xf7,
0x01, 0x80, 0xfc, 0xff, 0x7f, 0x03, 0x80, 0xfe, 0xff, 0x7f, 0x03, 0x00, 0xff, 0xff, 0xff, 0x0f,
0x07, 0x01, 0xc2, 0xe7, 0xf7, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x80, 0xfe, 0xff, 0xff, 0x0f, 0x07,
0x07, 0x07, 0x87, 0xff, 0xfe, 0xfc, 0x00, 0xe0, 0xf8, 0xfc, 0x9e, 0x0f, 0x07, 0x07, 0x07, 0x87,
0xfe, 0xff, 0xff, 0x03, 0xe0, 0xf8, 0xfc, 0x9e, 0x0f, 0x07, 0x07, 0x07, 0x87, 0xfe, 0xff, 0xff,
0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x1f, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x1f, 0x01, 0x00, 0x00, 0xf0, 0xff, 0xff,
0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, 0x01,
0x00, 0x07, 0x07, 0x07, 0x00, 0x00, 0x07, 0x07, 0x07, 0x00, 0x00, 0x06, 0x07, 0x07, 0x01, 0x00,
0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x30, 0x7f, 0x7f, 0x1f, 0x07, 0x07, 0x07,
0x07, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x07,
0x07, 0x07, 0x00, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x07, 0x07, 0x07, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00,
0x00, 0x06, 0x07, 0x07, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    oled_write_raw_P(qmk_logo, sizeof(qmk_logo));
}

static char * pDisplayText = NULL;
static volatile bool bDisplayChanged = false;

void display_text(char * pText) {
    bDisplayChanged = true;

    if (pDisplayText == NULL && pText == NULL)
        bDisplayChanged = false;

    pDisplayText = pText;
}

uint32_t display_logo(uint32_t trigger_time, void* args) {
    display_text(NULL);
    return 0;
}

#define JIGGLE_TICK_MS 8    // one mouse report per tick while moving, about the rate of a 125 Hz mouse
#define JIGGLE_RANGE_X 1000 // how far the pointer roams left/right of where it was at switch-on, in mouse counts
#define JIGGLE_RANGE_Y 300  // ...and how far up/down

static deferred_token jiggle_token = INVALID_DEFERRED_TOKEN;

static struct {
    int16_t  x, y;           // pointer position relative to where it was when the jiggler was switched on
    int16_t  from_x, from_y; // start of the current move
    int16_t  to_x, to_y;     // end of the current move
    float    bend;           // how far the path bows sideways, as a fraction of its length
    uint32_t move_started;
    uint16_t duration;       // ms the current move takes; 0 while pausing between moves
    uint32_t started;        // when the jiggler was switched on; times the LED flash
    bool     rgb_was_on;
} jig;

static int clamp(int value, int low, int high) {
    return value < low ? low : value > high ? high : value;
}

static int rand_between(int low, int high) {
    return low + rand() % (high - low + 1);
}

// Moves the pointer the way a hand does: a curved path that starts slow, speeds up and slows down again,
// then a pause, then the next move. Each call sends at most one mouse report and returns the ms until the next call.
uint32_t jiggle(uint32_t trigger_time, void* args) {
    if (jig.duration == 0) {
        // Pause over: pick the next move. One in four is a long sweep anywhere in range, the rest are short hops.
        if (rand() % 4 == 0) {
            jig.to_x = rand_between(-JIGGLE_RANGE_X, JIGGLE_RANGE_X);
            jig.to_y = rand_between(-JIGGLE_RANGE_Y, JIGGLE_RANGE_Y);
        } else {
            jig.to_x = clamp(jig.x + rand_between(-200, 200), -JIGGLE_RANGE_X, JIGGLE_RANGE_X);
            jig.to_y = clamp(jig.y + rand_between(-120, 120), -JIGGLE_RANGE_Y, JIGGLE_RANGE_Y);
        }
        jig.from_x = jig.x;
        jig.from_y = jig.y;
        jig.bend   = rand_between(-25, 25) / 100.0f;
        // Longer moves take longer: about 0.3 s for a hop and over a second for a full sweep, varied by -20%..+30%.
        jig.duration     = (200 + (abs(jig.to_x - jig.from_x) + abs(jig.to_y - jig.from_y)) * 2 / 5) * rand_between(80, 130) / 100;
        jig.move_started = timer_read32();
        // Count as pointer activity, so the RGB idle timeout does not switch off the red flash.
        set_activity_timestamps(last_matrix_activity_time(), last_encoder_activity_time(), sync_timer_read32());
        return JIGGLE_TICK_MS;
    }

    float u  = (float)timer_elapsed32(jig.move_started) / jig.duration;
    if (u > 1) u = 1;
    float s  = u * u * (3 - 2 * u); // ease in and out
    float dx = jig.to_x - jig.from_x, dy = jig.to_y - jig.from_y;
    // Quadratic Bezier curve: the control point sits to one side of the midpoint, which bows the path into an arc.
    float cx = dx / 2 - dy * jig.bend, cy = dy / 2 + dx * jig.bend;
    int   want_x = jig.from_x + (int)(2 * (1 - s) * s * cx + s * s * dx);
    int   want_y = jig.from_y + (int)(2 * (1 - s) * s * cy + s * s * dy);

    // A report holds -127..127 per axis; anything left over after a late tick is caught up on the next one.
    report_mouse_t report = {.x = clamp(want_x - jig.x, -127, 127), .y = clamp(want_y - jig.y, -127, 127)};
    jig.x += report.x;
    jig.y += report.y;
    if (report.x || report.y) host_mouse_send(&report);

    if (u < 1 || jig.x != jig.to_x || jig.y != jig.to_y) return JIGGLE_TICK_MS;

    // Arrived: pause, usually 0.3-1.8 s and one time in five up to 8 s longer.
    jig.duration = 0;
    return rand_between(300, 1800) + (rand() % 5 == 0 ? rand_between(0, 8000) : 0);
}

// While the jiggler runs, flash every LED red: half a second on, half a second off.
bool rgb_matrix_indicators_user(void) {
    if (jiggle_token != INVALID_DEFERRED_TOKEN) {
        rgb_matrix_set_color_all(timer_elapsed32(jig.started) % 1000 < 500 ? RGB_MATRIX_MAXIMUM_BRIGHTNESS : 0, 0, 0);
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static deferred_token logoDelay = INVALID_DEFERRED_TOKEN;
    static char PROGMEM * text[4][5] = {
        { PSTR("ChatGPT"),       PSTR("Previous"),       PSTR("Play/Pause"),     PSTR("Next"),           PSTR("Mute") },
        { PSTR("Step In"),       PSTR("Step Out"),       PSTR("Step Over"),      PSTR("Shift+F9"),       PSTR("F9") },
        { PSTR("Definition"),    PSTR("Declaration"),    PSTR("Shift+Enter"),    PSTR("Ctrl+D"),         PSTR("Run") },
        { PSTR("Ctrl+-"),        PSTR("Shift+Ctrl+-"),   PSTR("Shift+Ctrl+B"),   PSTR("Shift+Ctrl+R"),   PSTR("Ctrl+U, K") },
    };

    // Display the text for the standard keys.
    if (record->event.key.row < 4 && record->event.key.col < 5) {
        display_text(text[record->event.key.row][record->event.key.col]);
    }

    switch (keycode) {
    case MS_JIGL:
        if (record->event.pressed) {
            if (jiggle_token == INVALID_DEFERRED_TOKEN) {
                srand(timer_read32()); // press time varies, so each run moves differently
                memset(&jig, 0, sizeof(jig));
                jig.started    = timer_read32();
                jig.rgb_was_on = rgb_matrix_is_enabled();
                rgb_matrix_enable_noeeprom(); // the red flash needs the LEDs on, even if they were switched off in VIA
                jiggle_token = defer_exec(JIGGLE_TICK_MS, jiggle, NULL);
            } else {
                cancel_deferred_exec(jiggle_token);
                jiggle_token = INVALID_DEFERRED_TOKEN;
                if (!jig.rgb_was_on) rgb_matrix_disable_noeeprom();
            }
        }
        display_text(jiggle_token == INVALID_DEFERRED_TOKEN ? PSTR("Stay Awake Off") : PSTR("Stay Awake On"));
        break;
    case KC_VOLU:
        display_text(PSTR("Volume Up"));
        break;
    case KC_VOLD:
        display_text(PSTR("Volume Down"));
        break;
    default:
        break;
    };

    if (logoDelay != INVALID_DEFERRED_TOKEN)
        cancel_deferred_exec(logoDelay);

    logoDelay = defer_exec(5000, display_logo, NULL);

    return true;
}

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    if (bDisplayChanged) {
        bDisplayChanged = false;
        oled_scroll_off();
        oled_clear();
        return false;
    }

    if (pDisplayText == NULL) {
        render_logo();
        oled_scroll_left();
        return false;
    }

    // Host Keyboard LED Status
    oled_write_P(PSTR("\n"), false);
    oled_write_P(pDisplayText, false);

    return false;
}
#endif

void keyboard_post_init_user(void) {
    gpio_set_pin_output(GP25);
    gpio_write_pin_high(GP25);
    rgb_matrix_enable();
}

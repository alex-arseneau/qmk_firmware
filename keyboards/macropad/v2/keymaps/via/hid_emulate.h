// Copyright 2026 Alex Arseneau
// SPDX-License-Identifier: GPL-2.0-or-later

// Synthetic HID activity: pointer moves, clicks, scrolling, Tab and Alt+Tab, mixed and paced
// like a person at a desk. Included by keymap.c.
//
// The pad cannot see the screen, so the "window" and its "elements" are imagined: a region around the pointer's
// starting position holding a handful of targets, re-laid on every Alt+Tab. The traffic on the wire looks the same.

#pragma once

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define HID_EMULATE_TICK_MS 8      // one mouse report per tick while moving, about the rate of a 125 Hz mouse
#define HID_EMULATE_TARGETS 10
#define HID_EMULATE_MAX_REST 180000 // ms; rests stay under 3 minutes so the pad still keeps the computer awake

enum { CH_MOVE, CH_SCROLL, CH_CLICK, CH_TAB, CH_SWITCH, CH_COUNT };

static struct {
    // Session plan, drawn once at switch-on.
    uint8_t weight[2][CH_COUNT]; // how often each channel is picked, per phase (0 = reading, 1 = interacting)
    float   pace;                // stretches or shrinks every pause and move time
    float   burst_med, rest_med; // median actions per burst, median ms of rest between bursts

    uint8_t  phase;
    uint16_t actions_left, bursts_left; // until the next rest, until the next phase change

    int16_t box_x, box_y, box_w, box_h; // imagined window: centre and half-size, relative to the start position
    struct {
        int16_t x, y, size;
    } target[HID_EMULATE_TARGETS];

    // Current action.
    uint8_t  channel, step, key, fix;
    uint16_t left; // scroll ticks, clicks or Tab presses still to do
    int8_t   dir;

    // Pointer.
    int16_t  x, y;           // position relative to where it was when the emulator was switched on
    int16_t  from_x, from_y; // start of the current stroke
    int16_t  to_x, to_y;     // end of the current stroke
    int16_t  end_x, end_y;   // where the whole move is meant to finish
    int16_t  width;          // size of what the move is aimed at; 30 and up is scrollable content
    float    bend;           // how far the path bows sideways, as a fraction of its length
    float    ease_a, ease_b; // shape of the speed curve, see stroke_tick()
    uint32_t move_started;
    uint16_t duration;
} emu;

static int clamp(int value, int low, int high) {
    return value < low ? low : value > high ? high : value;
}

static int rand_between(int low, int high) {
    return low + rand() % (high - low + 1);
}

static float frand(void) { // 0..1, never exactly 0
    return (rand() % 10000 + 1) / 10000.0f;
}

static float gauss(void) { // roughly normal, mean 0, deviation 1
    return (frand() + frand() + frand() + frand() - 2) * 1.732f;
}

// Skewed: most values land near the median, a few are several times larger.
static float lognorm(float median, float sigma) {
    return median * expf(sigma * gauss());
}

static void place_target(int i) {
    int kind = rand() % 20, w = emu.box_w, h = emu.box_h, x, y, size = rand_between(10, 28);
    if (kind < 5) { // toolbar button: small, along the top edge
        x = rand_between(-w, w);
        y = -h + rand_between(0, 30);
    } else if (kind < 9) { // panel edge: small, down one side
        x = (rand() % 2 ? w : -w) + rand_between(-20, 20);
        y = rand_between(-h, h);
    } else { // content: large, gathered toward the middle
        x    = clamp(gauss() * w / 2.5f, -w, w);
        y    = clamp(gauss() * h / 2.5f, -h, h);
        size = rand_between(30, 120);
    }
    emu.target[i].x    = emu.box_x + x;
    emu.target[i].y    = emu.box_y + y;
    emu.target[i].size = size;
}

// A different window: new region, new elements.
static void relayout(void) {
    emu.box_x = rand_between(-500, 500);
    emu.box_y = rand_between(-250, 250);
    emu.box_w = rand_between(300, 1000);
    emu.box_h = rand_between(180, 480);
    for (int i = 0; i < HID_EMULATE_TARGETS; i++) place_target(i);
}

static void stroke(int to_x, int to_y) {
    // Speed-curve shapes: ease in and out, sharper ease, quick start with a long settle, slow start,
    // near-linear, ballistic. One is drawn per stroke and its two exponents nudged.
    static const float profile[][2] = {{2, 2}, {3, 3}, {1.6f, 3}, {3, 1.7f}, {1.05f, 1.05f}, {1, 2.6f}};
    const float *p = profile[rand() % 6];
    emu.ease_a = p[0] * (0.85f + 0.3f * frand());
    emu.ease_b = p[1] * (0.85f + 0.3f * frand());
    emu.from_x = emu.x;
    emu.from_y = emu.y;
    emu.to_x   = to_x;
    emu.to_y   = to_y;
    emu.bend   = gauss() * 0.12f;
    // Fitts's law: far or small targets take longer.
    float dist       = hypotf(to_x - emu.x, to_y - emu.y);
    emu.duration     = clamp((120 + 130 * log2f(1 + dist / emu.width)) * lognorm(1, 0.25f) * emu.pace, 60, 4000);
    emu.move_started = timer_read32();
}

// Sends at most one mouse report; true once the stroke has arrived.
static bool stroke_tick(void) {
    float u = (float)timer_elapsed32(emu.move_started) / emu.duration;
    if (u > 1) u = 1;
    float up = powf(u, emu.ease_a), down = powf(1 - u, emu.ease_b);
    float s  = up / (up + down); // 0..1 along the path; the exponents set where it speeds up and slows down
    float dx = emu.to_x - emu.from_x, dy = emu.to_y - emu.from_y;
    // Quadratic Bezier curve: the control point sits to one side of the midpoint, which bows the path into an arc.
    float cx = dx / 2 - dy * emu.bend, cy = dy / 2 + dx * emu.bend;
    int   want_x = emu.from_x + (int)(2 * (1 - s) * s * cx + s * s * dx);
    int   want_y = emu.from_y + (int)(2 * (1 - s) * s * cy + s * s * dy);

    // A report holds -127..127 per axis; anything left over after a late tick is caught up on the next one.
    report_mouse_t report = {.x = clamp(want_x - emu.x, -127, 127), .y = clamp(want_y - emu.y, -127, 127)};
    emu.x += report.x;
    emu.y += report.y;
    if (report.x || report.y) host_mouse_send(&report);
    return u >= 1 && emu.x == emu.to_x && emu.y == emu.to_y;
}

// Each *_tick() does one step of its action and returns the ms until its next step, or 0 when the action is over.

static uint32_t move_tick(void) {
    if (emu.step == 1) { // mid-stroke
        if (!stroke_tick()) return HID_EMULATE_TICK_MS;
        if (!emu.fix) return 0;
        emu.fix--;
        emu.step = 2;
        return rand_between(80, 250); // notice the miss
    }
    if (emu.step == 2) { // correct: onto the target, or within a few counts of it if another correction follows
        stroke(emu.end_x + (emu.fix ? (int)(gauss() * 3) : 0), emu.end_y + (emu.fix ? (int)(gauss() * 3) : 0));
        emu.step = 1;
        return HID_EMULATE_TICK_MS;
    }

    if (rand() % 10 < 7) {
        // Aim at an element. Big and near ones are picked more often than small or far ones.
        uint32_t weight[HID_EMULATE_TARGETS], total = 0;
        for (int i = 0; i < HID_EMULATE_TARGETS; i++) {
            float dist = hypotf(emu.target[i].x - emu.x, emu.target[i].y - emu.y);
            total += weight[i] = emu.target[i].size * 1000 / (dist + 50) + 1;
        }
        int      i    = 0;
        uint32_t pick = rand() % total;
        while (pick >= weight[i]) pick -= weight[i++];
        emu.width = emu.target[i].size;
        emu.end_x = emu.target[i].x + gauss() * emu.width / 4;
        emu.end_y = emu.target[i].y + gauss() * emu.width / 4;
    } else {
        // Empty space: mostly short hops, now and then a long one, kept inside the window.
        float dist = lognorm(120, 0.9f), angle = frand() * 6.283f;
        emu.width = 60;
        emu.end_x = clamp(emu.x + dist * cosf(angle), emu.box_x - emu.box_w, emu.box_x + emu.box_w);
        emu.end_y = clamp(emu.y + dist * sinf(angle), emu.box_y - emu.box_h, emu.box_y + emu.box_h);
    }
    // The first stroke often misses: 15% overshoot, 25% fall short. One or two short strokes then correct it.
    int   r    = rand() % 100;
    float miss = r < 15 ? 0.04f + 0.08f * frand() : r < 40 ? -0.03f - 0.07f * frand() : 0;
    emu.fix    = miss ? 1 + (rand() % 5 == 0) : 0;
    stroke(emu.end_x + (emu.end_x - emu.x) * miss, emu.end_y + (emu.end_y - emu.y) * miss);
    emu.step = 1;
    return HID_EMULATE_TICK_MS;
}

static uint32_t scroll_tick(void) {
    if (emu.step == 0) {
        emu.step = 1;
        emu.left = clamp(lognorm(5, 0.7f), 1, 40);
        emu.dir  = rand() % 5 ? -1 : 1; // mostly down
    }
    if (emu.left-- == 0) return 0;
    if (rand() % 12 == 0) emu.dir = -emu.dir; // scrolled past it
    report_mouse_t report = {.v = emu.dir * (rand() % 6 ? 1 : 2)};
    host_mouse_send(&report);
    return 10 + lognorm(90, 0.5f) + (rand() % 8 ? 0 : rand_between(400, 2000)); // sometimes stop to read
}

static uint32_t click_tick(void) {
    if (emu.step == 0) emu.left = rand() % 12 ? 1 : 2; // now and then a double-click
    bool           press  = emu.step++ % 2 == 0;
    report_mouse_t report = {.buttons = press ? MOUSE_BTN1 : 0};
    host_mouse_send(&report);
    if (press) return 20 + lognorm(85, 0.3f);
    return --emu.left ? rand_between(70, 160) : 0;
}

// Plain Tab presses: focus steps through the window's controls. No other key is ever sent, so nothing gets typed.
static uint32_t tab_tick(void) {
    if (emu.step == 0) {
        emu.step = 1;
        emu.left = clamp(lognorm(2, 0.6f), 1, 8);
    }
    if (emu.key) {
        unregister_code(emu.key);
        emu.key = 0;
        if (--emu.left == 0) return 0;
        return 20 + lognorm(250, 0.5f) + (rand() % 10 ? 0 : rand_between(300, 1500)); // sometimes stop to look
    }
    emu.key = KC_TAB;
    register_code(emu.key);
    return 15 + lognorm(75, 0.3f);
}

static uint32_t switch_tick(void) {
    switch (emu.step++) {
        case 0:
            emu.left = rand() % 5 ? 1 : 2; // usually the previous window, sometimes the one before
            register_code(KC_LALT);
            return rand_between(60, 160);
        case 1:
            register_code(KC_TAB);
            return rand_between(50, 120);
        case 2:
            unregister_code(KC_TAB);
            if (--emu.left) emu.step = 1;
            return rand_between(80, 300);
        default:
            unregister_code(KC_LALT);
            relayout();
            return 0;
    }
}

// The last action is over: wait, then start another. Returns the wait in ms.
static uint32_t next_action(void) {
    // Median wait after each kind of action: a glance after a move, a read after a scroll, and so on.
    static const uint16_t pause_med[CH_COUNT] = {500, 1800, 900, 700, 1500};
    uint32_t              pause               = 30 + lognorm(pause_med[emu.channel], 0.7f) * emu.pace;

    if (emu.actions_left-- == 0) {
        // Burst over: rest, let the window drift and one element change, and after a few bursts change phase.
        emu.actions_left = lognorm(emu.burst_med, 0.6f);
        pause            = clamp(lognorm(emu.rest_med, 0.8f), 2000, HID_EMULATE_MAX_REST);
        emu.box_x        = clamp(emu.box_x + gauss() * 40, -700, 700);
        emu.box_y        = clamp(emu.box_y + gauss() * 25, -350, 350);
        place_target(rand() % HID_EMULATE_TARGETS);
        if (emu.bursts_left-- == 0) {
            emu.phase ^= 1;
            emu.bursts_left = lognorm(3, 0.6f);
        }
    }

    int total = 0, channel = 0;
    for (int i = 0; i < CH_COUNT; i++) total += emu.weight[emu.phase][i];
    for (int pick = rand() % total; pick >= emu.weight[emu.phase][channel]; channel++) pick -= emu.weight[emu.phase][channel];
    if (channel == CH_SCROLL && emu.width < 30) channel = CH_MOVE; // nothing under the pointer to scroll

    emu.channel = channel;
    emu.step    = 0;
    return pause;
}

static void hid_emulate_start(void) {
    //                                              move scroll click tab switch
    static const uint8_t base[2][CH_COUNT] = {{40, 45, 8, 2, 5},    // reading
                                              {35, 8, 30, 12, 5}};  // interacting
    memset(&emu, 0, sizeof(emu));
    for (int p = 0; p < 2; p++)
        for (int i = 0; i < CH_COUNT; i++) emu.weight[p][i] = 1 + base[p][i] * rand_between(50, 150) / 100;
    emu.pace         = lognorm(1, 0.2f);
    emu.burst_med    = rand_between(4, 14);
    emu.rest_med     = rand_between(8000, 40000);
    emu.phase        = rand() % 2;
    emu.actions_left = lognorm(emu.burst_med, 0.6f);
    emu.bursts_left  = lognorm(3, 0.6f);
    relayout(); // the first action is a move, as channel and step start at 0
}

// Call from a deferred executor: does one step and returns the ms until the next call, never 0.
static uint32_t hid_emulate_step(void) {
    static uint32_t (*const tick[CH_COUNT])(void) = {move_tick, scroll_tick, click_tick, tab_tick, switch_tick};
    uint32_t ms = tick[emu.channel]();
    return ms ? ms : next_action();
}

// Lets go of anything the emulator is holding down.
static void hid_emulate_stop(void) {
    report_mouse_t report = {0};
    host_mouse_send(&report);
    if (emu.key) unregister_code(emu.key);
    if (emu.channel == CH_SWITCH) {
        unregister_code(KC_TAB);
        unregister_code(KC_LALT);
    }
}

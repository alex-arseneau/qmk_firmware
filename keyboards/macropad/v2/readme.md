# blitzpad (MacroPad v2)

A personal 20-key macropad built on an RP2040.

* 4x5 key matrix
* Rotary encoder (volume by default)
* 128x32 SSD1306 OLED on I2C0 (GP20/GP21)
* 20 WS2812 per-key LEDs on GP19

## Building

    make macropad/v2:default
    make macropad/v2:via

Flash by copying the resulting `.uf2` onto the `RPI-RP2` drive, or:

    make macropad/v2:via:flash

## Keymaps

* `default`: fixed keymap with the ChatGPT and Ctrl+U, K macros built in.
* `via`: remappable with [VIA](https://usevia.app). The OLED shows the action of the last key pressed for 5 seconds. The top-left key toggles the HID interaction emulator, a source of realistic mouse and keyboard traffic for testing HID tooling; it flashes every LED red once a second while it is on. It appears in VIA as the custom key "HID Emu".

## HID interaction emulator

**It clicks and presses Tab and Alt+Tab on whatever has focus; it never types text. Only switch it on with the pad plugged into a test machine or with nothing on screen that a stray click or focus change could harm.**

The emulator (`keymaps/via/hid_emulate.h`) mixes five kinds of activity: pointer moves, scrolling, left clicks, Tab and Alt+Tab. Tab and Alt are the only keys it sends.

* **Session plan**: each switch-on draws its own mix, pace, burst length and rest length. Activity alternates between a reading phase (mostly moves and scrolling) and an interacting phase (more clicks and Tab presses).
* **Bursts and rests**: a handful of actions, then a rest of seconds to at most three minutes, short enough that the computer also stays awake. Roughly two thirds of a session is rest.
* **Moves**: each stroke draws one of six speed curves, a curved path and a duration that grows with distance and shrinks with target size. Some strokes overshoot or fall short and are corrected by one or two small follow-up strokes.
* **Targets**: the pad cannot see the screen, so it imagines a window around the pointer's starting position with ten elements in it: small ones along the top and sides, larger scrollable ones in the middle. Most moves end on an element, with large and near ones picked more often. The window drifts between bursts and is re-laid on every Alt+Tab.

## VIA

The pad is not in VIA's public keyboard list, so load `keyboards/macropad/via.json` once in VIA's Design tab.

VIA's lighting menu selects effects by number, so the effects enabled in `keyboard.json` must stay in the same order as that menu; a static assert in the `via` keymap checks this. The reactive and splash effects are left disabled because they did not work with VIA.

## Lighting

The LEDs turn off after 20 minutes without a key press or encoder turn (`rgb_matrix.timeout` in `keyboard.json`), and when the host suspends.

## Bootloader

* **Bootmagic reset**: hold the top-left key while plugging in the pad.
* **Double-tap reset**: press the reset button twice within one second.

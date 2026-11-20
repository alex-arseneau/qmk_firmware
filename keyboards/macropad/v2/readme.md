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
* `via`: remappable with [VIA](https://usevia.app). The OLED shows the action of the last key pressed for 5 seconds. The top-left key toggles a mouse jiggler, which nudges the pointer a few pixels every 1-5 seconds to keep the computer awake; it appears in VIA as the custom key "Jiggle".

## VIA

The pad is not in VIA's public keyboard list, so load `keyboards/macropad/via.json` once in VIA's Design tab.

VIA's lighting menu selects effects by number, so the effects enabled in `keyboard.json` must stay in the same order as that menu; a static assert in the `via` keymap checks this. The reactive and splash effects are left disabled because they did not work with VIA.

## Lighting

The LEDs turn off after 20 minutes without a key press or encoder turn (`rgb_matrix.timeout` in `keyboard.json`), and when the host suspends.

## Bootloader

* **Bootmagic reset**: hold the top-left key while plugging in the pad.
* **Double-tap reset**: press the reset button twice within one second.

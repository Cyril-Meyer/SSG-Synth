# SSG-Synth
YM2149F SSG audio synthesizer with midi interface, Arduino based.

We chose to use Arduino Uno in order to create a final product that is simple to setup and robust, thanks to the ability to easily stack shields.

### Roadmap
* 0.0 :heavy_check_mark: Create Arduino and GitHub project :smile:
* 0.1 :x: [hardware] setup and document project (YM2149F breadboard setup, midi interface)
* 0.2 :x: [code] control SSG chip using an Arduino Uno
* 0.3 :x: [code] receive midi signals with Arduino
* 0.4 :x: [code] control SSG chip using midi signals
* 0.5 :x: [hardware] from breadboard to custom shield
* 0.6 :x: [doc] examples and tutorial

# Schematics
### YM2149F setup
```
+-----------------+             +------------------+
|    +-------+    |             |  +------------+  |
|    |YM2149F|    |             |  |   ARDUINO  |  |
|    +-------+    |             |  | UNO / NANO |  |
|                 |             |  +------------+  |
|                 |             |                  |
|        D0 -> D7 +-------------+ PIN 5 -> 12      |
|                 |             |                  |
|    BC1,BC2,BDIR +-------------+ PIN 4,3,2        |
|                 |             |                  |
|      CHAN A,B,C +-- MIXER     |                  |
|                 |             |                  |
|                 |             |                  |
|          /RESET +-- 5V        |                  |
|                 |             |                  |
|            /SEL +-- GND       |                  |
|                 |             |                  |
|           CLOCK +------+      |                  |
|                 |      |      |                  |
+-----------------+      |      +------------------+
                         |
                         |
+---------------------+  |
| +-----------------+ |  |
| | 4Mhz oscillator | |  |
| ++----------------+ |  |
|                     |  |
| 5V              OUT +--+
|                 GND |
+---------------------+
```

### Passive audio mixer
```
           +----------------+
           |                |
           | +------+       |
    GND ---+-+ 100Ω |--+    |
             +------+  |    |
                       |    |
             +------+  |    |
 CHAN A -----+ 1k Ω +--+    +---- OUT (GND)
             +------+  |
                       +--------- OUT (SIGNAL)
             +------+  |
 CHAN B -----+ 1K Ω +--+
             +------+  |
                       |
             +------+  |
 CHAN C -----+ 1K Ω +--+
             +------+
```

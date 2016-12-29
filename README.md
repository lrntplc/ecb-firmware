# Foldable, Intel Edison powered, chess board project

This repo contains the source code for the firmware running on the Top Half,
Bottom Half and Command controllers.

## Requirements:
 * avr-gcc toolchain installed on your machine;
 * an AVR programmer (I used avrprog2, so the makefiles will use that for
   flashing). However, you can flash the hex files separately with your own
   programmer;

## Compilation:
 * go to the ecb-firmware root directory;
 * type `make`
 
The output hex files will be in the out/ directory.

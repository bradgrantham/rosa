# Rosa Emulator

This is the emulator, API headers and source, and initial apps for
the Rosa HW platform.

To run the apps, you'll probably need ROM files, floppy images, or cartridge images.

See application-development.md for information on building and running the emulator, but the Quick Start is:
* Install SDL2 development package including CMake config
* `cmake -Bbuild .`
* `(cd build ; make)`
* Put anything the app needs in `emu-root/`
* `./build/rosa-emu --root-dir emu-root`

## Caveat

It's not ready for production; user beware.  Some useful information may be printed on `stdout`.

## App Requirements Quick Start

Look in the individual app directories for README files with more detail.

### trs80

Put a Model III ROM file in `emu-root/model3.rom`.

### coleco

Put a Coleco ROM file in `emu-root/coleco/COLECO.ROM` and any cartridge images you'd like to run in `emu-root/coleco`/.

### apple2e

Put an Apple //e ROM file in `emu-root/apple2e.rom`.  If you want to run any floppy images, put those in `emu-root/floppies`/.

### mp3player

Put MP3 files to play in `emu-root/`.

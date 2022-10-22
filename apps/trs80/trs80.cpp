#include <stdio.h>
#include <cstring>
#include <chrono>
#include <map>
#include "rocinante.h"
#include "events.h"
#include "fonts.h"
#include "z80user.h"
#include "z80emu.h"

extern "C" {
int trs80_main(int argc, char **argv);
};

typedef long long clk_t;

constexpr int Trs80ColumnCount = 64;
constexpr int Trs80RowCount = 16;
constexpr int Trs80ScreenSize = Trs80ColumnCount*Trs80RowCount;
constexpr int Trs80ScreenBegin = 15*1024;
constexpr int Trs80ScreenEnd = Trs80ScreenBegin + Trs80ScreenSize;
constexpr int Trs80CharWidth = 8;
constexpr int Trs80CharHeight = 12;

// TODO check with Brad.
constexpr int NTSCWidth = 704;
constexpr int NTSCHeight = 240;

constexpr int Trs80PixelWidth = (Trs80ColumnCount*Trs80CharWidth);
constexpr int Trs80PixelHeight = (Trs80RowCount*Trs80CharHeight);

constexpr int Trs80HorizontalMargin = ((NTSCWidth - Trs80PixelWidth)/2);
constexpr int Trs80VerticalMargin = ((NTSCHeight - Trs80PixelHeight)/2);

// Handle keyboard mapping. The TRS-80 Model III keyboard has keys in different
// places, so we must occasionally fake a Shift key being up or down when it's
// really not.

// Whether to force a Shift key, and how.
enum ShiftState { ST_NEUTRAL, ST_FORCE_DOWN, ST_FORCE_UP };

// Keyboard is in several identical (mirrored) banks.
constexpr int Trs80KeyboardBankSize = 0x100;
constexpr int Trs80KeyboardBankCount = 4;
constexpr int Trs80KeyboardBegin = 0x3800;
constexpr int Trs80KeyboardEnd = Trs80KeyboardBegin + Trs80KeyboardBankSize*Trs80KeyboardBankCount;

struct KeyInfo {
    int byteIndex;
    int bitNumber;
    ShiftState shiftForce;
};

static std::map<int, KeyInfo> Trs80KeyboardMap;

// IRQs
// constexpr uint8_t M1_TIMER_IRQ_MASK = 0x80;
// constexpr uint8_t M3_CASSETTE_RISE_IRQ_MASK = 0x01;
// constexpr uint8_t M3_CASSETTE_FALL_IRQ_MASK = 0x02;
constexpr uint8_t M3_TIMER_IRQ_MASK = 0x04;
// constexpr uint8_t M3_IO_BUS_IRQ_MASK = 0x08;
// constexpr uint8_t M3_UART_SED_IRQ_MASK = 0x10;
// constexpr uint8_t M3_UART_RECEIVE_IRQ_MASK = 0x20;
// constexpr uint8_t M3_UART_ERROR_IRQ_MASK = 0x40;
// constexpr uint8_t CASSETTE_IRQ_MASKS = M3_CASSETTE_RISE_IRQ_MASK | M3_CASSETTE_FALL_IRQ_MASK;

// NMIs
constexpr uint8_t RESET_NMI_MASK = 0x20;
// constexpr uint8_t DISK_MOTOR_OFF_NMI_MASK = 0x40;
// constexpr uint8_t DISK_INTRQ_NMI_MASK = 0x80;

// Holds the state of the physical machine.
typedef struct Trs80Machine {
    Z80_STATE z80;
    uint8_t memory[MEMSIZE];

    // 8 bytes, each a bitfield of keys currently pressed.
    uint8_t keys[8];
    ShiftState shiftForce;

    // Which IRQs should be handled.
    uint8_t irqMask;
    // Which IRQs have been requested by the hardware.
    uint8_t irqLatch;
    // Which NMIs should be handled.
    uint8_t nmiMask;
    // Which NMIs have been requested by the hardware.
    uint8_t nmiLatch;
    // Whether we've seen this NMI and handled it.
    int nmiSeen;
    uint8_t modeImage;
} Trs80Machine;

static char *TextModeBuffer;

static uint8_t NTSCBlack, NTSCWhite;

static void Trs80TextModeClearDisplay()
{
    memset(TextModeBuffer, ' ', Trs80ColumnCount * Trs80RowCount);
}

static int Trs80TextModeInitVideoMemory(void *videoMemory, uint32_t size, uint8_t black, uint8_t white)
{
    auto reserve = [](void*& ptr, uint32_t& remaining, size_t rsv) {
        void *old = ptr;
        if (remaining < rsv) {
            // Out of memory.
            return (void *) 0;
        }
        ptr = static_cast<uint8_t*>(ptr) + rsv;
        remaining -= rsv;
        return old;
    };

    TextModeBuffer = static_cast<char*>(reserve(videoMemory, size, Trs80RowCount * Trs80ColumnCount));
    if (TextModeBuffer == 0) {
        // Out of memory.
        return 0;
    }
    NTSCBlack = black;
    NTSCWhite = white;
    Trs80TextModeClearDisplay();

    return 1;
}

__attribute__((hot,flatten)) void Trs80TextModeFillRowBuffer([[maybe_unused]] int frameIndex, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    int rowWithinTextArea = rowNumber - Trs80VerticalMargin;
    int charRow = rowWithinTextArea / Trs80FontHeight;

    if (rowWithinTextArea >= 0 && charRow < Trs80RowCount) {
        int charPixelY = (rowWithinTextArea % Trs80FontHeight);
        uint8_t *rowDst = rowBuffer + Trs80HorizontalMargin;

        for (int charCol = 0; charCol < Trs80ColumnCount; charCol++) {
            uint8_t character = TextModeBuffer[charRow * Trs80ColumnCount + charCol];
            uint8_t charRowBits = Trs80FontBits[character * Trs80FontHeight + charPixelY];

            for(int charPixelX = 0; charPixelX < Trs80FontWidth; charPixelX++) {
                int pixel = (charRowBits & (0x01 << charPixelX));
                *rowDst++ = pixel ? NTSCWhite : NTSCBlack;
            }
        }
    }
}

static int Trs80TextModeNeedsColorburst()
{
    return 0;
}

static void initializeKeyboardMap() {
    // Trs80KeyboardMap[KEYCAP_@] = { 0, 0, ST_FORCE_UP };
    // Trs80KeyboardMap[KEYCAP_`] = { 0, 0, ST_FORCE_DOWN };

    Trs80KeyboardMap[KEYCAP_A] = { 0, 1, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_B] = { 0, 2, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_C] = { 0, 3, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_D] = { 0, 4, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_E] = { 0, 5, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_F] = { 0, 6, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_G] = { 0, 7, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_H] = { 1, 0, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_I] = { 1, 1, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_J] = { 1, 2, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_K] = { 1, 3, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_L] = { 1, 4, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_M] = { 1, 5, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_N] = { 1, 6, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_O] = { 1, 7, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_P] = { 2, 0, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_Q] = { 2, 1, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_R] = { 2, 2, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_S] = { 2, 3, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_T] = { 2, 4, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_U] = { 2, 5, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_V] = { 2, 6, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_W] = { 2, 7, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_X] = { 3, 0, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_Y] = { 3, 1, ST_FORCE_DOWN };
    Trs80KeyboardMap[KEYCAP_Z] = { 3, 2, ST_FORCE_DOWN };

#if 0
    Trs80KeyboardMap[KEYCAP_a] = { 0, 1, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_b] = { 0, 2, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_c] = { 0, 3, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_d] = { 0, 4, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_e] = { 0, 5, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_f] = { 0, 6, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_g] = { 0, 7, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_h] = { 1, 0, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_i] = { 1, 1, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_j] = { 1, 2, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_k] = { 1, 3, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_l] = { 1, 4, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_m] = { 1, 5, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_n] = { 1, 6, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_o] = { 1, 7, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_p] = { 2, 0, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_q] = { 2, 1, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_r] = { 2, 2, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_s] = { 2, 3, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_t] = { 2, 4, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_u] = { 2, 5, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_v] = { 2, 6, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_w] = { 2, 7, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_x] = { 3, 0, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_y] = { 3, 1, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_z] = { 3, 2, ST_FORCE_UP };
#endif

    Trs80KeyboardMap[KEYCAP_0_CPAREN] = { 4, 0, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_1_EXCLAMATION] = { 4, 1, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_2_AT] = { 4, 2, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_3_NUMBER] = { 4, 3, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_4_DOLLAR] = { 4, 4, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_5_PERCENT] = { 4, 5, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_6_CARET] = { 4, 6, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_7_AMPERSAND] = { 4, 7, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_8_ASTERISK] = { 5, 0, ST_FORCE_UP };
    Trs80KeyboardMap[KEYCAP_9_OPAREN] = { 5, 1, ST_FORCE_UP };

    // keyMap.set("_", new KeyInfo(4, 0, ST_FORCE_DOWN)); // Simulate Shift-0, like trsemu.
    // Trs80KeyboardMap[KEYCAP_!] = { 4, 1, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_\] = { 4, 2, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_#] = { 4, 3, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_$] = { 4, 4, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_%] = { 4, 5, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_&] = { 4, 6, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_'] = { 4, 7, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_(] = { 5, 0, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_)] = { 5, 1, ST_FORCE_DOWN };

    // Trs80KeyboardMap[KEYCAP_:] = { 5, 2, ST_FORCE_UP };
    // Trs80KeyboardMap[KEYCAP_;] = { 5, 3, ST_FORCE_UP };
    // Trs80KeyboardMap[KEYCAP_,] = { 5, 4, ST_FORCE_UP };
    // Trs80KeyboardMap[KEYCAP_-] = { 5, 5, ST_FORCE_UP };
    // Trs80KeyboardMap[KEYCAP_.] = { 5, 6, ST_FORCE_UP };
    // Trs80KeyboardMap[KEYCAP_/] = { 5, 7, ST_FORCE_UP };

    // Trs80KeyboardMap[KEYCAP_*] = { 5, 2, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_+] = { 5, 3, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_<] = { 5, 4, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_=] = { 5, 5, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_>] = { 5, 6, ST_FORCE_DOWN };
    // Trs80KeyboardMap[KEYCAP_?] = { 5, 7, ST_FORCE_DOWN };

    Trs80KeyboardMap[KEYCAP_ENTER] = { 6, 0, ST_NEUTRAL };
    // Trs80KeyboardMap[KEYCAP_\\] = { 6, 1, ST_NEUTRAL };
    Trs80KeyboardMap[KEYCAP_ESCAPE] = { 6, 2, ST_NEUTRAL };
    Trs80KeyboardMap[KEYCAP_UP] = { 6, 3, ST_NEUTRAL };
    Trs80KeyboardMap[KEYCAP_DOWN] = { 6, 4, ST_NEUTRAL };
    Trs80KeyboardMap[KEYCAP_LEFT] = { 6, 5, ST_NEUTRAL };
    Trs80KeyboardMap[KEYCAP_BACKSPACE] = { 6, 5, ST_NEUTRAL };
    Trs80KeyboardMap[KEYCAP_RIGHT] = { 6, 6, ST_NEUTRAL };
    Trs80KeyboardMap[KEYCAP_SPACE] = { 6, 7, ST_NEUTRAL };
    Trs80KeyboardMap[KEYCAP_LEFTSHIFT] = { 7, 0, ST_NEUTRAL };
    Trs80KeyboardMap[KEYCAP_RIGHTSHIFT] = { 7, 0, ST_NEUTRAL };

#if 0
    KEYCAP_1_EXCLAMATION,
    KEYCAP_2_AT,
    KEYCAP_3_NUMBER,
    KEYCAP_4_DOLLAR,
    KEYCAP_5_PERCENT,
    KEYCAP_6_CARET,
    KEYCAP_7_AMPERSAND,
    KEYCAP_8_ASTERISK,
    KEYCAP_9_OPAREN,
    KEYCAP_0_CPAREN,
    KEYCAP_TAB,
    KEYCAP_HYPHEN_UNDER,
    KEYCAP_EQUAL_PLUS,
    KEYCAP_OBRACKET_OBRACE,
    KEYCAP_CBRACKET_CBRACE,
    KEYCAP_BACKSLASH_PIPE,
    KEYCAP_HASH_TILDE,
    KEYCAP_SEMICOLON_COLON,
    KEYCAP_SINGLEQUOTE_DOUBLEQUOTE,
    KEYCAP_GRAVE_TILDE,
    KEYCAP_COMMA_LESS,
    KEYCAP_PERIOD_GREATER,
    KEYCAP_SLASH_QUESTION,
#endif
}

// Release all keys.
static void clearKeyboard(Trs80Machine *machine) {
    memset(machine->keys, 0, sizeof(machine->keys));
    machine->shiftForce = ST_NEUTRAL;
}

// Read a byte from the keyboard memory bank. This is an odd system where
// bits in the address map to the various bytes, and you can read the OR'ed
// addresses to read more than one byte at a time. For the last byte we fake
// the Shift key if necessary.
static uint8_t readKeyboard(Trs80Machine *machine, uint16_t addr) {
    addr = (addr - Trs80KeyboardBegin) % Trs80KeyboardBankSize;
    uint8_t b = 0;

#if 0
    // Dequeue if necessary.
    if (clock > this.keyProcessMinClock) {
        const keyWasPressed = this.processKeyQueue();
        if (keyWasPressed) {
            this.keyProcessMinClock = clock + KEY_DELAY_CLOCK_CYCLES;
        }
    }
#endif

    // OR together the various bytes.
    for (int i = 0; i < 8; i++) {
        uint8_t keys = machine->keys[i];
        if ((addr & (1 << i)) != 0) {
            if (i == 7) {
                // Modify keys based on the shift force.
                switch (machine->shiftForce) {
                    case ST_NEUTRAL:
                        // Nothing.
                        break;

                    case ST_FORCE_UP:
                        // On the Model III the first two bits are left and right shift,
                        // though we don't handle the right shift anywhere.
                        keys &= ~0x03;
                        break;

                    case ST_FORCE_DOWN:
                        keys |= 0x01;
                        break;
                }
            }

            b |= keys;
        }
    }

    return b;
}

static void handleKeypress(Trs80Machine *machine, int key, int isPress) {
    auto itr = Trs80KeyboardMap.find(key);
    if (itr != Trs80KeyboardMap.end()) {
        KeyInfo &keyInfo = itr->second;
        machine->shiftForce = keyInfo.shiftForce;
        uint8_t bit = 1 << keyInfo.bitNumber;
        if (isPress) {
            machine->keys[keyInfo.byteIndex] |= bit;
        } else {
            machine->keys[keyInfo.byteIndex] &= ~bit;
        }
    }
}

/**
 * Set the mask for IRQ (regular) interrupts.
 */
static void setIrqMask(Trs80Machine *machine, uint8_t irqMask) {
    machine->irqMask = irqMask;
}

// Reset whether we've seen this NMI interrupt if the mask and latch no longer overlap.
static void updateNmiSeen(Trs80Machine *machine) {
    if ((machine->nmiLatch & machine->nmiMask) == 0) {
        machine->nmiSeen = 0;
    }
}

/**
 * Set the mask for non-maskable interrupts. (Yes.)
 */
static void setNmiMask(Trs80Machine *machine, uint8_t nmiMask) {
    // Reset is always allowed:
    machine->nmiMask = nmiMask | RESET_NMI_MASK;
    updateNmiSeen(machine);
}

static uint8_t interruptLatchRead(Trs80Machine *machine) {
    return ~machine->irqLatch;
}

// Set or reset the timer interrupt.
void setTimerInterrupt(Trs80Machine *machine, bool state) {
    if (state) {
        machine->irqLatch |= M3_TIMER_IRQ_MASK;
    } else {
        machine->irqLatch &= ~M3_TIMER_IRQ_MASK;
    }
}

// Set the state of the reset button interrupt.
void resetButtonInterrupt(Trs80Machine *machine, bool state) {
    if (state) {
        machine->nmiLatch |= RESET_NMI_MASK;
    } else {
        machine->nmiLatch &= ~RESET_NMI_MASK;
    }
    updateNmiSeen(machine);
}

// What to do when the hardware timer goes off.
static void handleTimer(Trs80Machine *machine) {
    setTimerInterrupt(machine, true);
}

static void resetMachine(Trs80Machine *machine) {
    machine->modeImage = 0x80;
    setIrqMask(machine, 0);
    setNmiMask(machine, 0);
    // resetCassette(machine);
    clearKeyboard(machine);
    setTimerInterrupt(machine, false);
    Z80Reset(&machine->z80);
}

uint8_t Trs80ReadByte(Trs80Machine *machine, uint16_t address) {
    if (address >= Trs80KeyboardBegin && address < Trs80KeyboardEnd) {
        return readKeyboard(machine, address);
    }

    return machine->memory[address];
}

void Trs80WriteByte(Trs80Machine *machine, uint16_t address, uint8_t value) {
    if (address >= ROMSIZE) {
        if (address >= Trs80ScreenBegin && address < Trs80ScreenEnd) {
            TextModeBuffer[address - Trs80ScreenBegin] = value;
        }
        machine->memory[address] = value;
    }
}

uint8_t Trs80ReadPort(Trs80Machine *machine, uint8_t address) {
    uint8_t value = 0xFF;

    switch (address) {
        case 0xE0:
            // IRQ latch read.
            value = interruptLatchRead(machine);
            break;

        case 0xE4:
            // NMI latch read.
            value = ~machine->nmiLatch;
            break;

        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
            // Acknowledge timer.
            setTimerInterrupt(machine, false);
            break;

        case 0xF8:
            // Printer status. Printer selected, ready, with paper, not busy.
            value = 0x30;
            break;

        case 0xFF:
            // Cassette and various flags.
            value = machine->modeImage & 0x7E;
            // value |= this.getCassetteByte();
            break;
    }

    return value;
}

void Trs80WritePort(Trs80Machine *machine, uint8_t address, uint8_t value) {
    switch (address) {
        case 0xE0:
            // Set interrupt mask.
            setIrqMask(machine, value);
            break;

        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7:
            // Set NMI state.
            setNmiMask(machine, value);
            break;

        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
            // Various controls.
            machine->modeImage = value;
            // this.setCassetteMotor((value & 0x02) != 0);
            // this.screen.setExpandedCharacters((value & 0x04) != 0);
            // this.screen.setAlternateCharacters((value & 0x08) == 0);
            break;
    }
}

int trs80_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    bool quit = false;
    Trs80Machine *machine = new Trs80Machine;
    memset(machine, 0, sizeof(*machine));

    // Set up the display.
    RoNTSCSetMode(0, Trs80TextModeInitVideoMemory, Trs80TextModeFillRowBuffer, Trs80TextModeNeedsColorburst);
    Trs80TextModeClearDisplay();
#if 0
    // For when debugging the character set.
    for (int i = 0; i < 256; i++) {
        TextModeBuffer[i] = i;
    }
#endif

    // Read the ROM.
    const char *romFilename = "model3.rom";
    FILE *fp = fopen(romFilename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "failed to open %s for reading\n", romFilename);
        exit(EXIT_FAILURE);
    }
    size_t romSize = fread(machine->memory, 1, ROMSIZE, fp);
    if (romSize != ROMSIZE) {
        fprintf(stderr, "ROM read from %s was unexpectedly short (%zd bytes)\n",
                romFilename, romSize);
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    initializeKeyboardMap();
    resetMachine(machine);

    clk_t tStateCount = 0;
    clk_t previousTimerClock = 0;
    clk_t clockHz = 2027520;
    clk_t timerHz = 30;

    auto emulationStartTime = std::chrono::system_clock::now();

    while (!quit) {
        clk_t cyclesToDo = 10000;

        // See if we should interrupt the emulator early for our timer interrupt.
        clk_t nextTimerClock = previousTimerClock + clockHz / timerHz;
        if (nextTimerClock >= tStateCount) {
            clk_t clocksUntilTimer = nextTimerClock - tStateCount;
            if (cyclesToDo > clocksUntilTimer) {
                cyclesToDo = clocksUntilTimer;
            }
        }

        // See if we should slow down if we're going too fast.
        auto now = std::chrono::system_clock::now();
        auto microsSinceStart = std::chrono::duration_cast<std::chrono::microseconds>(now - emulationStartTime);
        clk_t expectedClock = clockHz * microsSinceStart.count() / 1000000;
        if (expectedClock < tStateCount) {
            RoDoHousekeeping();
            continue;
        }

        // Emulate!
        tStateCount += Z80Emulate(&machine->z80, cyclesToDo, machine);

        // Handle non-maskable interrupts.
        if ((machine->nmiLatch & machine->nmiMask) != 0 && !machine->nmiSeen) {
            tStateCount += Z80NonMaskableInterrupt(&machine->z80, machine);
            machine->nmiSeen = true;

            // Simulate the reset button being released.
            resetButtonInterrupt(machine, false);
        }

        // Handle interrupts.
        if ((machine->irqLatch & machine->irqMask) != 0) {
            tStateCount += Z80Interrupt(&machine->z80, 0, machine);
        }

        // Set off a timer interrupt.
        if (tStateCount > nextTimerClock) {
            handleTimer(machine);
            previousTimerClock = tStateCount;
        }

        // Check user input.
        RoEvent ev;
        int haveEvent = RoEventPoll(&ev);

        if (haveEvent) {
            switch(ev.eventType) {
                case RoEvent::KEYBOARD_RAW: {
                    const struct KeyboardRawEvent raw = ev.u.keyboardRaw;
                    handleKeypress(machine, raw.key, raw.isPress);
                    break;
                }
                
                default:
                    // pass;
                    break;
            }
        }
        RoDoHousekeeping();
    }

    return 0;
}

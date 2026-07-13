# PowerBank Keeper

**Keep any USB power bank alive with a $2 ESP32-C3 board. No extra hardware required.**

Most USB power banks auto-shutoff after 30-60 seconds when connected devices draw too little current. This is a nightmare for:

- **Solar charging setups** — solar trickle charging can't overcome the bank's auto-off
- **Low-power IoT devices** — Raspberry Pi in idle, ESP32 sensors, e-ink displays
- **Pass-through charging** — the bank cuts out while you're trying to charge it AND power something

Companies like [Voltaic Systems](https://voltaicsystems.com) charge $100+ for power banks with an "always-on" mode that solves this. **PowerBank Keeper does the same thing with a $2 board and zero extra components.**

## How It Works

The firmware exploits the ESP32-C3's WiFi radio as a periodic load:

1. The board sleeps in deep sleep mode, drawing only ~10µA
2. Every 20 seconds, a hardware timer wakes the chip
3. WiFi fires up and performs a network scan, spiking current draw to 200-400mA for ~500ms
4. WiFi shuts down, board returns to deep sleep

This brief current spike resets the power bank's auto-off timer, keeping it permanently "awake" — just like Voltaic's always-on mode.

```
Deep Sleep (~10µA)  →  WiFi TX Burst (~300mA)  →  Deep Sleep (~10µA)
        |                        |                         |
   20 seconds              500ms pulse               20 seconds
```

## Hardware

**You need exactly one thing:**

- An ESP32-C3 SuperMini (or any ESP32-C3 dev board with USB)
  - [Amazon](https://www.amazon.com/s?k=ESP32-C3+supermini) — ~$2 each
  - [AliExpress](https://www.aliexpress.com/w/wholesale-ESP32-C3-supermini.html) — ~$1.50 each

That's it. No MOSFETs, no resistors, no capacitors, no breadboard. Just plug the board directly into the power bank's USB port.

## Flash It

### Option A: Arduino IDE

1. Install the [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
2. Open `powerbank_keeper.ino`
3. Select board: **ESP32C3 Dev Module**
4. Set Upload Mode to **USB-Serial/JTAG** (built-in)
5. Upload

### Option B: Command Line (arduino-cli)

```bash
# Install esp32 core
arduino-cli core install esp32:esp32

# Compile
arduino-cli compile \
  --fqbn "esp32:esp32:esp32c3:CDCOnBoot=default,CPUFreq=160,FlashMode=qio,FlashSize=4M,JTAGAdapter=builtin,DebugLevel=none" \
  powerbank_keeper.ino

# Flash
arduino-cli upload \
  --fqbn "esp32:esp32:esp32c3:CDCOnBoot=default,CPUFreq=160,FlashMode=qio,FlashSize=4M,JTAGAdapter=builtin,DebugLevel=none" \
  --port /dev/cu.usbmodem101 \
  powerbank_keeper.ino
```

### Option C: esptool (pre-built binary)

```bash
esptool --port /dev/cu.usbmodem101 write_flash 0x00000 powerbank_keeper.bin
```

## Usage

1. Flash the firmware
2. Plug the ESP32-C3 into your power bank
3. Done

The onboard LED will double-blink every 20 seconds to confirm each pulse. The power bank will never auto-shutoff again.

## Tuning

If your power bank is particularly aggressive (some cut off in 15 seconds), edit this line in the firmware:

```cpp
#define SLEEP_INTERVAL_US 20ULL * 1000000ULL  // Change to 15 or 10 seconds
```

## How It Compares

| Feature | Voltaic V50 ($129) | PowerBank Keeper ($2) |
|---|---|---|
| Always-on mode | Built-in | WiFi pulse trick |
| Works with solar | Yes | Yes |
| Works with low-power devices | Yes | Yes |
| Battery capacity | 13,200 mAh | Uses *your* power bank |
| Cost | $129 | ~$2 |

## How It Looks on a Power Bank

The ESP32-C3 SuperMini is tiny (22mm x 18mm / 0.86" x 0.7") — smaller than a postage stamp. It plugs flush into the USB port and draws negligible power between pulses.

## Technical Details

- **Chip:** ESP32-C3 (RISC-V single-core, 160MHz)
- **Sleep current:** ~10-15µA (deep sleep)
- **Pulse current:** 200-400mA (WiFi TX scan, ~500ms)
- **Pulse interval:** 20 seconds (configurable)
- **Flash size:** 4MB (firmware uses <1MB)
- **Interface:** USB-Serial/JTAG (no external programmer needed)

## License

MIT — do whatever you want with it.

## Credits

Built by [Eric Woodard](https://github.com/Snail3D) and [Biggy Snails](https://github.com/Snail3D) (AI agent).

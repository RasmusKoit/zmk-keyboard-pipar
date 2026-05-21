# zmk-keyboard-pipar

A [ZMK](https://zmk.dev) module providing the board definitions, shields, custom
drivers, snippets, and keymaps for the [Pipar](https://github.com/RasmusKoit/pipar)
keyboards.

## Contents

### Boards (`boards/piparkeyboards/`)

| Board | Description |
|-------|-------------|
| `pipar_point` | 40-key split wireless keyboard (nRF52840). Built per side via the `left` / `right` revision — e.g. `pipar_point@right`. The right half is the central and carries the trackpoint. |
| `pipar_chip` | 12-key wireless keypad (nRF52840). |
| `pipar_dev` | Bring-up / development board for prototyping (nRF52840). |

### Shields (`boards/shields/`)

| Shield | Description |
|--------|-------------|
| `pipar_sool` | 36-key split keyboard (Pipar & Sool), with central, dongle, and left/right configurations. |
| `pipar_flake` | 8+1-key keypad with a rotary encoder and an OLED display. |

### Drivers (`drivers/`)

Custom ZMK drivers, each gated behind a `CONFIG_PIPAR_*` Kconfig symbol:

- **`indicator/`** — LED indicators driven by ZMK events:
  - `PIPAR_CAPS_INDICATOR` — caps lock
  - `PIPAR_CONNECTION_INDICATOR` — BT/USB connection type
  - `PIPAR_BATTERY_INDICATOR` — low battery (npm1300 PMIC LED)
  - `PIPAR_BLE_CONNECTION_INDICATOR` — split peripheral BLE connection
  - `PIPAR_HID_INDICATOR` — configurable HID indicator bit (Num/Caps/Scroll Lock…)
- **`caps_word/`** (`PIPAR_CAPS_WORD_EVENT`) — overrides ZMK's caps-word behavior to
  emit state-change events that the indicators can subscribe to.
- **`watchdog/`** (`PIPAR_WATCHDOG_FEEDER`) — a recovery watchdog: feeds a task
  watchdog from the system workqueue so the device auto-resets if the workqueue
  hangs (e.g. a stuck I2C transfer).

### Snippets (`snippets/`)

- **`trackpoint`** — enables the PS/2 trackpoint on the Pipar Point central half.
- **`uart_con`** — routes the firmware console/logging over UART for debugging.

### Other

- `nrf-bat-profile/` — npm1300 PMIC battery profile (YDL375678 cell).
- `include/zmk/events/caps_word_state_changed.h` — public event header.

## Usage

This repository is a ZMK module. Add it to a ZMK build's `west.yml`:

```yaml
manifest:
  remotes:
    - name: rasmuskoit
      url-base: https://github.com/RasmusKoit
  projects:
    - name: zmk-keyboard-pipar
      remote: rasmuskoit
      revision: main
```

It declares its own dependencies (`zmk-unicode`, `kb_zmk_ps2_mouse_trackpoint_driver`)
through `zephyr/module.yml`, and registers itself as a board / snippet / DTS root.

## Building

Firmware for these keyboards is built from the parent
[`pipar`](https://github.com/RasmusKoit/pipar) repository, which wires this module
into a ZMK build. From that repo:

```bash
./build.sh right   # Pipar Point central (with trackpoint)
./build.sh left    # Pipar Point peripheral
```

See the `pipar` repo's `build.sh` for all available targets.

## More info

See [ZMK's modules documentation](https://zmk.dev/docs/features/modules) and the
[Zephyr modules page](https://docs.zephyrproject.org/latest/develop/modules.html)
for background on how ZMK modules work.

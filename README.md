# GTA5 Legacy Cheats – 3rr0r // Story Client

A native **ScriptHookV `.asi` cheat menu for GTA V Legacy (Story Mode)**, built on **Linux** with MinGW and running under **Steam / Proton**.
No .NET, no Mono, no ScriptHookVDotNet required.

**Made by 3rr0r303YT on TikTok**

> ⚠️ **Story Mode only.** Do **not** use this in GTA Online. Rockstar's anti-cheat will ban your account.
> This project is not affiliated with or endorsed by Rockstar Games or Take-Two Interactive.

---

## Features

**Menu**
- In-game cheat menu with the classic phone cheats (invincibility, weapons, super jump, explosive ammo, slow motion, low gravity, weather, …)
- Teleport to waypoint, repair vehicle, skyfall
- 20 vehicles with descriptions, including 6 military vehicles (tank, fighter jet, helicopter, …)
- Keyboard **and** controller support (Xbox / Nintendo Switch Pro via Steam Input)

**Performance**
- **FPS counter** (top right)
- **FPS Booster** with 4 levels: reduces traffic, lowers the model budget and removes distant ambient cars and pedestrians
- **Anti-Lag Protection**: detects stutter or low FPS and automatically turns off heavy cheats, clears your wanted level and switches the booster to ULTRA

---

## Controls

### Controller (hold **R / RB** first, then press the second button)

| Combo | Action |
|---|---|
| R + ← | Open / close menu |
| R + → | FPS Booster level |
| R + ↑ | FPS counter on/off |
| R + ↓ | Invincibility on/off |
| R + A | Max health & armor |
| R + B | Clear wanted level |
| R + X | All weapons & ammo |
| R + Y | Teleport to waypoint |
| R + ZR / RT | Spawn Comet (on foot) |
| R + ZL / LT | Super jump on/off (on foot) |
| R + Left stick click | Skyfall |
| R + Right stick click | Repair vehicle |

Inside the menu: **D-Pad ↑/↓** to navigate, **A** to select, **B** to close.

### Keyboard

| Key | Action |
|---|---|
| F4 | Open / close menu |
| F3 | FPS Booster level |
| F2 | FPS counter on/off |
| F5 | Run Anti-Lag now |
| ↑ / ↓ | Navigate |
| Enter | Select |
| Backspace | Close menu |

---

## Requirements

- GTA V **Legacy** (Steam), running via Proton (native Steam or Flatpak Steam)
- [ScriptHookV](https://www.dev-c.com/gtav/scripthookv/) (`ScriptHookV.dll` + `dinput8.dll`) matching your game version
- ScriptHookV **SDK** zip (same page) – only needed for building
- MinGW-w64 cross compiler
  - Gentoo: `sudo emerge --ask dev-util/mingw64-toolchain`
  - Arch: `sudo pacman -S mingw-w64-gcc`
  - Debian/Ubuntu: `sudo apt install g++-mingw-w64-x86-64`
  - Fedora: `sudo dnf install mingw64-gcc-c++`

---

## Installation

1. Copy `ScriptHookV.dll` and `dinput8.dll` into your GTA V folder.
2. Download the ScriptHookV SDK zip into `~/Downloads`.
3. Build and install:

```bash
git clone https://github.com/Real3rr0r303YT/Gta5LegacyCheats.git
cd Gta5LegacyCheats
./build.sh
```

The script finds your GTA V folder automatically (native and Flatpak Steam) and copies `3rr0rClient.asi` into it.
You can also pass the SDK path manually: `./build.sh /path/to/ScriptHookV_SDK.zip`

4. Steam → GTA V Legacy → Properties → Launch Options:

```
WINEDLLOVERRIDES="dinput8=n,b" %command%
```

5. In the Rockstar Launcher disable BattlEye and start **Story Mode**.
   You should see *"3rr0r Story Client loaded"* at the bottom of the screen.

---

## Extra FPS on weak GPUs (optional)

On integrated graphics, FSR upscaling with gamescope gives the biggest boost:

```bash
# Flatpak Steam
flatpak install -y flathub com.valvesoftware.Steam.Utility.gamescope
```

Launch options (set `-W/-H` to your monitor resolution):

```
WINEDLLOVERRIDES="dinput8=n,b" gamescope -W 1920 -H 1080 -w 1280 -h 720 -F fsr -f -- %command%
```

Then set the in-game resolution to 1280×720.

---

## Troubleshooting

| Problem | Fix |
|---|---|
| No "loaded" message, menu does not open | Check `ScriptHookV.log` in the GTA folder for `3rr0rClient.asi`. Make sure `dinput8.dll` is present and the launch option is set. |
| Game crashes on load | Your ScriptHookV version does not match your game version – update ScriptHookV. Remove ScriptHookVDotNet if installed. |
| A/B or X/Y swapped on a Switch controller | Steam → Settings → Controller → toggle "Use Nintendo Button Layout". |
| Controller combos don't work | Make sure Steam Input is enabled for the controller. Try F4 on the keyboard to check the menu works. |

---

## How it works

`ScriptHookV.dll` is compiled with MSVC, so its exported C++ functions use MSVC name mangling, which MinGW cannot link against.
`shim/main.h` replaces the SDK's `main.h` and resolves those exports at runtime with `GetProcAddress`.
The ScriptHookV SDK itself is **not** included in this repository – `build.sh` uses your local copy.

---

## License

This project is licensed under the **GNU General Public License v3.0** – see [LICENSE](LICENSE).

ScriptHookV and its SDK are © Alexander Blade and are **not** part of this project.

**Made by 3rr0r303YT on TikTok**

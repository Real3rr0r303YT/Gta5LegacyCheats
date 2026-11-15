#!/bin/bash
# GTA5 Legacy Cheats - performance optimizer for weak / integrated GPUs
# Fixes stutter while driving and flying (jets, helicopters).
# Made by 3rr0r303YT on TikTok - GPL-3.0-or-later
# Usage: ./tools/optimize.sh          apply
#        ./tools/optimize.sh restore  restore original settings
set -e

CFG=""
for d in "$HOME/.var/app/com.valvesoftware.Steam/.local/share/Steam" \
         "$HOME/.local/share/Steam" "$HOME/.steam/steam" "$HOME/.steam/root"; do
  f="$d/steamapps/compatdata/271590/pfx/drive_c/users/steamuser/Documents/Rockstar Games/GTA V/settings.xml"
  [ -f "$f" ] && CFG="$f" && break
done
[ -z "$CFG" ] && { echo "[!] settings.xml not found - start GTA V once, then run again."; exit 1; }
echo "[+] Settings: $CFG"

if pgrep -f GTA5.exe >/dev/null 2>&1; then
  echo "[!] Close GTA V first, otherwise the game overwrites the settings."; exit 1
fi

if [ "$1" = "restore" ]; then
  [ -f "$CFG.orig" ] && cp "$CFG.orig" "$CFG" && echo "[+] Original settings restored." || echo "[!] No backup found."
  exit 0
fi

[ -f "$CFG.orig" ] || { cp "$CFG" "$CFG.orig"; echo "[+] Backup: settings.xml.orig"; }

s() { sed -i "s|<$1 value=\"[^\"]*\"|<$1 value=\"$2\"|" "$CFG"; }
s HdStreamingInFlight false        # biggest fix for jet / heli stutter
s ExtendedDistanceScaling 0.000000
s LodScale 0.300000
s MaxLodScale 0.000000
s VehicleLodBias 0.000000
s PedLodBias 0.000000
s Shadow_Distance 1.000000
s Shadow_LongShadows false
s Shadow_SoftShadows 0
s TextureQuality 0
s GrassQuality 0
s ReflectionQuality 0
s Reflection_MSAA 0
s MSAA 0
s SSAO 0
s Tessellation 0
s PostFX 0
s WaterQuality 0
s ParticleQuality 0
s MotionBlurStrength 0.000000
s DoF false
echo "[+] Optimized for driving and flying."

VRAM=$(cat /sys/class/drm/card*/device/mem_info_vram_total 2>/dev/null | sort -n | tail -1)
if [ -n "$VRAM" ]; then
  MB=$((VRAM / 1024 / 1024))
  echo "[i] GPU memory: ${MB} MB"
  if [ "$MB" -lt 2048 ]; then
    echo "[!] Low GPU memory. On integrated AMD graphics set in BIOS:"
    echo "    Advanced > AMD CBS > NBIO > GFX Configuration > UMA Frame Buffer Size = 4G"
  fi
fi

echo
echo "Optional Steam launch option (steady 30 FPS):"
echo 'WINEDLLOVERRIDES="dinput8=n,b" MANGOHUD=1 MANGOHUD_CONFIG=fps_limit=30 %command%'
echo
echo "Restore: ./tools/optimize.sh restore"

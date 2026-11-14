#!/bin/bash
# GTA5 Legacy Cheats - build script
# Made by 3rr0r303YT on TikTok - GPL-3.0-or-later
# Usage: ./build.sh [path/to/ScriptHookV_SDK_x.zip]
set -e
cd "$(dirname "$0")"

CXX="${CXX:-/usr/lib/mingw64-toolchain/bin/x86_64-w64-mingw32-g++}"
command -v "$CXX" >/dev/null 2>&1 || CXX=x86_64-w64-mingw32-g++
command -v "$CXX" >/dev/null 2>&1 || { echo "[!] MinGW-w64 g++ not found. Install it first (see README)."; exit 1; }

SDK_ZIP="${1:-$(ls -t "$HOME"/Downloads/ScriptHookV_SDK*.zip 2>/dev/null | head -1)}"
if [ ! -f "$SDK_ZIP" ]; then
  echo "[!] ScriptHookV SDK not found."
  echo "    Download it from https://www.dev-c.com/gtav/scripthookv/"
  echo "    and run: ./build.sh /path/to/ScriptHookV_SDK_x.zip"
  exit 1
fi

echo "[+] Using SDK: $SDK_ZIP"
rm -rf build && mkdir -p build
unzip -oq "$SDK_ZIP" -d build/sdk
INC="$(dirname "$(find build/sdk -name natives.h | head -1)")"
cp -r "$INC" build/inc
cp shim/main.h build/inc/main.h

echo "[+] Compiling..."
"$CXX" -shared -O2 -std=gnu++17 -fpermissive -w -Ibuild/inc src/main.cpp \
  -static -static-libgcc -static-libstdc++ -o build/3rr0rClient.asi
echo "[+] Built: build/3rr0rClient.asi"

GTA=""
for d in "$HOME/.var/app/com.valvesoftware.Steam/.local/share/Steam" \
         "$HOME/.local/share/Steam" "$HOME/.steam/steam" "$HOME/.steam/root"; do
  if [ -f "$d/steamapps/common/Grand Theft Auto V/GTA5.exe" ]; then
    GTA="$d/steamapps/common/Grand Theft Auto V"; break
  fi
done

if [ -n "$GTA" ]; then
  cp build/3rr0rClient.asi "$GTA/"
  echo "[+] Installed to: $GTA"
else
  echo "[i] GTA V Legacy folder not found automatically."
  echo "    Copy build/3rr0rClient.asi into your GTA V folder manually."
fi

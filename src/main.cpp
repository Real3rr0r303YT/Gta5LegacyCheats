// GTA5 Legacy Cheats - 3rr0r // STORY CLIENT
// Made by 3rr0r303YT on TikTok
// License: GPL-3.0-or-later
// STORY MODE ONLY - do not use in GTA Online.

#include <windows.h>
#include <string>
#include <vector>
#include <functional>
#include <cstdio>
#include <map>
#include <cstring>
#include "main.h"
#include "natives.h"
#include "types.h"

// ---------------------------------------------------------------- state
struct Item { std::string name; bool toggle; bool on; std::function<void()> act; };
static std::vector<Item> items;
static bool menuOpen = false;
static int sel = 0, topI = 0;
static const int VIS = 15;

static int iFps, iBoost, iAntiLag, iLang;
static int lang = 0; // 0 = English, 1 = Deutsch
static int iGod, iRun, iSwim, iJump, iDrunk, iSlowAim, iGrav, iMelee, iExp, iFire, iSlide;

static int slowIdx = 0;
static float slowSteps[] = { 1.0f, 0.6f, 0.4f, 0.2f };
static int weatherIdx = 0;
static const char* weathers[] = { "EXTRASUNNY","CLEAR","CLOUDS","SMOG","FOGGY","OVERCAST","RAIN",
                                  "THUNDER","CLEARING","SNOW","BLIZZARD","SNOWLIGHT","XMAS" };
static int boost = 0;
static const char* boostNames[2][5] = { { "OFF","MEDIUM","HIGH","ULTRA","MAX" }, { "AUS","MITTEL","STARK","ULTRA","MAX" } };
// NPCs and traffic are never removed completely - the world stays alive
static float boostDens[] = { 1.0f, 0.75f, 0.55f, 0.35f, 0.30f };

static std::string msg;
static DWORD msgUntil = 0;
static float fpsSmooth = 60.0f;
static bool drunkApplied = false;
static bool prevKey[256];
static Vehicle lastSpawn = 0;
static int worldBuf[1024];

static float lagTime = 0.0f;
static DWORD lastFix = 0;
static int spikes = 0;
static DWORD spikeWin = 0;

// ---------------------------------------------------------------- language
static const std::map<std::string, std::string> DE = {
    {"FPS Counter","FPS-Anzeige"},
    {"Anti-Lag Protection","Anti-Lag Schutz"},
    {"Run Anti-Lag Now","Anti-Lag jetzt ausfuehren"},
    {"Language: English","Sprache: Deutsch"},
    {"Invincibility","Unsterblichkeit"},
    {"Max Health & Armor","Max Leben & Ruestung"},
    {"All Weapons & Ammo","Alle Waffen & Munition"},
    {"Recharge Special Ability","Spezialfaehigkeit aufladen"},
    {"Wanted Level +1","Fahndung +1"},
    {"Wanted Level -1","Fahndung -1"},
    {"Clear Wanted Level","Fahndung loeschen"},
    {"Fast Run","Schnell rennen"},
    {"Fast Swim","Schnell schwimmen"},
    {"Super Jump","Super-Sprung"},
    {"Parachute","Fallschirm"},
    {"Drunk Mode","Betrunken"},
    {"Skyfall","Skyfall"},
    {"Slow Motion (Steps)","Zeitlupe (Stufen)"},
    {"Slow Motion Aim","Zeitlupe beim Zielen"},
    {"Low Gravity","Niedrige Schwerkraft"},
    {"Explosive Melee","Explosive Nahkampfangriffe"},
    {"Explosive Ammo","Explosive Munition"},
    {"Flaming Ammo","Brandmunition"},
    {"Slidey Cars","Rutschige Autos"},
    {"Change Weather","Wetter wechseln"},
    {"Teleport to Waypoint","Teleport zum Wegpunkt"},
    {"Repair Vehicle","Fahrzeug reparieren"},
    {"BMX - Bicycle","BMX - Fahrrad"},
    {"Buzzard - Attack Helicopter","Buzzard - Kampfhubschrauber"},
    {"Caddy - Golf Cart","Caddy - Golfwagen"},
    {"Comet - Sports Car","Comet - Sportwagen"},
    {"Duster - Crop Duster Plane","Duster - Spruehflugzeug"},
    {"Mallard - Stunt Plane","Mallard - Stuntflugzeug"},
    {"PCJ-600 - Motorcycle","PCJ-600 - Motorrad"},
    {"Rapid GT - Sports Car","Rapid GT - Sportwagen"},
    {"Sanchez - Dirt Bike","Sanchez - Crossbike"},
    {"Stretch - Limousine","Stretch - Limousine"},
    {"Trashmaster - Garbage Truck","Trashmaster - Muellwagen"},
    {"Dodo - Seaplane","Dodo - Wasserflugzeug"},
    {"Duke O'Death - Armored Car","Duke O'Death - Panzerauto"},
    {"Kraken - Submarine","Kraken - U-Boot"},
    {"Rhino - Military Tank","Rhino - Militaer-Panzer"},
    {"Crusader - Military Jeep","Crusader - Militaer-Jeep"},
    {"Barracks - Military Truck","Barracks - Militaer-LKW"},
    {"Lazer - Military Fighter Jet","Lazer - Militaer-Kampfjet"},
    {"Annihilator - Military Heli","Annihilator - Militaer-Heli"},
    {"Titan - Military Cargo Plane","Titan - Militaer-Transporter"},
    {"Model not found","Modell nicht gefunden"},
    {"No waypoint set","Kein Wegpunkt gesetzt"},
    {"Teleported","Teleportiert"},
    {"Healed","Geheilt"},
    {"Weapons received","Waffen erhalten"},
    {"Recharged","Aufgeladen"},
    {"Wanted level cleared","Fahndung geloescht"},
    {"Vehicle repaired","Fahrzeug repariert"},
    {"Anti-Lag: heavy cheats off, wanted cleared, boost HIGH","Anti-Lag: Cheats aus, Fahndung weg, Boost STARK"},
    {"3rr0r Story Client loaded - R + Left / F4","3rr0r Story Client geladen - R + Links / F4"},
};
static std::string L(const std::string& s)
{
    if (lang == 0) return s;
    auto it = DE.find(s);
    return it != DE.end() ? it->second : s;
}
static const char* OnTxt(bool on) { return lang ? (on ? "AN" : "AUS") : (on ? "ON" : "OFF"); }

static void SaveLang()
{
    FILE* f = fopen("3rr0rClient.ini", "w");
    if (f) { fprintf(f, "lang=%s\n", lang ? "de" : "en"); fclose(f); }
}
static void LoadLang()
{
    FILE* f = fopen("3rr0rClient.ini", "r");
    if (!f) return;
    char b[32] = { 0 };
    if (fgets(b, sizeof(b), f)) lang = (strstr(b, "de") != nullptr) ? 1 : 0;
    fclose(f);
}

// ---------------------------------------------------------------- helpers
static void Notify(const std::string& s) { msg = L(s); msgUntil = GetTickCount() + 2500; }
static Ped Me() { return PLAYER::PLAYER_PED_ID(); }
static Hash H(const char* s) { return GAMEPLAY::GET_HASH_KEY((char*)s); }
static int AddA(const std::string& n, std::function<void()> a) { items.push_back({ n, false, false, a }); return (int)items.size() - 1; }
static int AddT(const std::string& n, std::function<void()> a = nullptr) { items.push_back({ n, true, false, a }); return (int)items.size() - 1; }
static bool On(int i) { return items[i].on; }

static void Txt(const char* t, float x, float y, float sc, int r, int g, int b, int font = 0, bool right = false, bool centre = false)
{
    UI::SET_TEXT_FONT(font);
    UI::SET_TEXT_SCALE(0.0f, sc);
    UI::SET_TEXT_COLOUR(r, g, b, 255);
    UI::SET_TEXT_OUTLINE();
    if (centre) UI::SET_TEXT_CENTRE(1);
    if (right) { UI::SET_TEXT_RIGHT_JUSTIFY(1); UI::SET_TEXT_WRAP(0.0f, x); }
    UI::_SET_TEXT_ENTRY((char*)"STRING");
    UI::_ADD_TEXT_COMPONENT_STRING((char*)t);
    UI::_DRAW_TEXT(x, y);
}

static void Rect(float x, float y, float w, float h, int r, int g, int b, int a)
{
    GRAPHICS::DRAW_RECT(x + w / 2, y + h / 2, w, h, r, g, b, a);
}

// ---------------------------------------------------------------- features
static void Spawn(const char* model, const std::string& label)
{
    Hash h = H(model);
    if (!STREAMING::IS_MODEL_IN_CDIMAGE(h)) { Notify("Model not found"); return; }
    STREAMING::REQUEST_MODEL(h);
    DWORD t = GetTickCount() + 3000;
    while (!STREAMING::HAS_MODEL_LOADED(h) && GetTickCount() < t) scriptWait(0);
    Ped p = Me();
    Vector3 c = ENTITY::GET_ENTITY_COORDS(p, 1);
    Vector3 f = ENTITY::GET_ENTITY_FORWARD_VECTOR(p);
    // remove the previously spawned vehicle unless the player is sitting in it
    if (lastSpawn && ENTITY::DOES_ENTITY_EXIST(lastSpawn) &&
        !(PED::IS_PED_IN_ANY_VEHICLE(p, 0) && PED::GET_VEHICLE_PED_IS_USING(p) == lastSpawn))
    {
        ENTITY::SET_ENTITY_AS_MISSION_ENTITY(lastSpawn, 1, 1);
        VEHICLE::DELETE_VEHICLE(&lastSpawn);
    }
    Vehicle v = VEHICLE::CREATE_VEHICLE(h, c.x + f.x * 6.0f, c.y + f.y * 6.0f, c.z + 0.5f, ENTITY::GET_ENTITY_HEADING(p), 0, 1);
    VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(v);
    PED::SET_PED_INTO_VEHICLE(p, v, -1);
    STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(h);
    lastSpawn = v;
    { Vehicle tmp = v; ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&tmp); }
    msg = (lang ? "Gespawnt: " : "Spawned: ") + L(label); msgUntil = GetTickCount() + 2500;
}

// MAX: debloats ambient extras that cost FPS but add little fun.
// NPCs, police, cars and chases all stay.
static void ApplyMax(bool on)
{
    VEHICLE::SET_GARBAGE_TRUCKS(!on);
    VEHICLE::SET_RANDOM_BOATS(!on);
    VEHICLE::SET_RANDOM_TRAINS(!on);
    VEHICLE::SET_FAR_DRAW_VEHICLES(!on);
    if (on) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST((char*)"CLEAR");
    else GAMEPLAY::CLEAR_WEATHER_TYPE_PERSIST();
}

static void CycleBoost()
{
    boost = (boost + 1) % 5;
    if (boost == 4) ApplyMax(true);
    else if (boost == 0) ApplyMax(false);
    msg = std::string("FPS Booster: ") + boostNames[lang][boost]; msgUntil = GetTickCount() + 2500;
}

static void TeleportWaypoint()
{
    if (!UI::IS_WAYPOINT_ACTIVE()) { Notify("No waypoint set"); return; }
    Blip b = UI::GET_FIRST_BLIP_INFO_ID(8);
    Vector3 c = UI::GET_BLIP_INFO_ID_COORD(b);
    Ped p = Me();
    Entity e = PED::IS_PED_IN_ANY_VEHICLE(p, 0) ? PED::GET_VEHICLE_PED_IS_USING(p) : p;
    float gz = 0.0f; bool found = false;
    for (float h = 0.0f; h < 1000.0f; h += 100.0f)
    {
        ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, c.x, c.y, h, 0, 0, 1);
        scriptWait(100);
        if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(c.x, c.y, h, &gz, 0)) { found = true; break; }
    }
    ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, c.x, c.y, found ? gz + 3.0f : 500.0f, 0, 0, 1);
    Notify("Teleported");
}

// Removes only EMPTY, abandoned ambient vehicles far away.
// NPCs are never deleted, and no vehicle with anyone inside is touched.
static void CleanWorld(float maxD)
{
    Ped me = Me();
    Vector3 c = ENTITY::GET_ENTITY_COORDS(me, 1);
    Vehicle myV = PED::IS_PED_IN_ANY_VEHICLE(me, 0) ? PED::GET_VEHICLE_PED_IS_USING(me) : 0;
    float md2 = maxD * maxD;

    int n = worldGetAllVehicles(worldBuf, 1024);
    for (int i = 0; i < n; i++)
    {
        Vehicle v = worldBuf[i];
        if (v == myV || v == lastSpawn || !ENTITY::DOES_ENTITY_EXIST(v)) continue;
        if (ENTITY::IS_ENTITY_A_MISSION_ENTITY(v)) continue;
        if (!VEHICLE::IS_VEHICLE_SEAT_FREE(v, -1)) continue;
        if (VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(v) > 0) continue;
        Vector3 q = ENTITY::GET_ENTITY_COORDS(v, 1);
        float dx = q.x - c.x, dy = q.y - c.y, dz = q.z - c.z;
        if (dx * dx + dy * dy + dz * dz < md2) continue;
        ENTITY::SET_ENTITY_AS_MISSION_ENTITY(v, 1, 1);
        VEHICLE::DELETE_VEHICLE(&v);
    }
}

static void DoAntiLag()
{
    lastFix = GetTickCount();
    int heavy[] = { iExp, iFire, iMelee, iSlide, iDrunk, iSlowAim, iGrav };
    for (int i : heavy)
        if (items[i].on) { items[i].on = false; if (items[i].act) items[i].act(); }
    slowIdx = 0;
    GAMEPLAY::SET_TIME_SCALE(1.0f);
    PLAYER::CLEAR_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID());
    if (boost < 2) boost = 2;
    Vector3 c = ENTITY::GET_ENTITY_COORDS(Me(), 1);
    FIRE::STOP_FIRE_IN_RANGE(c.x, c.y, c.z, 100.0f);
    Notify("Anti-Lag: heavy cheats off, wanted cleared, boost HIGH");
}

// Triggers on 5+ frame spikes (>100 ms) within 3 s, or below 30 FPS for 2 s
static void AntiLag(float ft)
{
    if (!On(iAntiLag) || ft <= 0.0f) return;
    DWORD now = GetTickCount();
    if (now - spikeWin > 3000) { spikeWin = now; spikes = 0; }
    if (ft > 0.1f) spikes++;
    if (1.0f / ft < 30.0f) lagTime += ft;
    else { lagTime -= ft * 0.5f; if (lagTime < 0.0f) lagTime = 0.0f; }
    if ((lagTime >= 2.0f || spikes >= 5) && now - lastFix > 10000)
    {
        lagTime = 0.0f; spikes = 0;
        DoAntiLag();
    }
}

// ---------------------------------------------------------------- menu
static void Build()
{
    // Performance
    iFps = AddT("FPS Counter"); items[iFps].on = true;
    iBoost = AddA("FPS Booster: OFF", [] { CycleBoost(); });
    iAntiLag = AddT("Anti-Lag Protection"); items[iAntiLag].on = true;
    AddA("Run Anti-Lag Now", [] { DoAntiLag(); });
    iLang = AddA("Language: English", [] { lang = 1 - lang; SaveLang(); });

    // Player
    iGod = AddT("Invincibility");
    AddA("Max Health & Armor", [] { Ped p = Me(); ENTITY::SET_ENTITY_HEALTH(p, ENTITY::GET_ENTITY_MAX_HEALTH(p)); PED::SET_PED_ARMOUR(p, 100); Notify("Healed"); });
    AddA("All Weapons & Ammo", [] {
        const char* w[] = { "WEAPON_KNIFE","WEAPON_NIGHTSTICK","WEAPON_HAMMER","WEAPON_BAT","WEAPON_CROWBAR","WEAPON_GOLFCLUB",
            "WEAPON_PISTOL","WEAPON_COMBATPISTOL","WEAPON_APPISTOL","WEAPON_PISTOL50","WEAPON_MICROSMG","WEAPON_SMG",
            "WEAPON_ASSAULTSMG","WEAPON_ASSAULTRIFLE","WEAPON_CARBINERIFLE","WEAPON_ADVANCEDRIFLE","WEAPON_MG","WEAPON_COMBATMG",
            "WEAPON_PUMPSHOTGUN","WEAPON_SAWNOFFSHOTGUN","WEAPON_ASSAULTSHOTGUN","WEAPON_BULLPUPSHOTGUN","WEAPON_STUNGUN",
            "WEAPON_SNIPERRIFLE","WEAPON_HEAVYSNIPER","WEAPON_GRENADELAUNCHER","WEAPON_RPG","WEAPON_MINIGUN","WEAPON_GRENADE",
            "WEAPON_STICKYBOMB","WEAPON_SMOKEGRENADE","WEAPON_MOLOTOV","WEAPON_FIREEXTINGUISHER","WEAPON_PETROLCAN" };
        for (auto n : w) WEAPON::GIVE_WEAPON_TO_PED(Me(), H(n), 9999, 0, 0);
        Notify("Weapons received");
    });
    AddA("Recharge Special Ability", [] { PLAYER::SPECIAL_ABILITY_FILL_METER(PLAYER::PLAYER_ID(), 1); Notify("Recharged"); });
    AddA("Wanted Level +1", [] { Player pl = PLAYER::PLAYER_ID(); int w = PLAYER::GET_PLAYER_WANTED_LEVEL(pl); if (w < 5) w++; PLAYER::SET_PLAYER_WANTED_LEVEL(pl, w, 0); PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(pl, 0); });
    AddA("Wanted Level -1", [] { Player pl = PLAYER::PLAYER_ID(); int w = PLAYER::GET_PLAYER_WANTED_LEVEL(pl); if (w > 0) w--; PLAYER::SET_PLAYER_WANTED_LEVEL(pl, w, 0); PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(pl, 0); });
    AddA("Clear Wanted Level", [] { PLAYER::CLEAR_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID()); Notify("Wanted level cleared"); });
    iRun = AddT("Fast Run", [] { PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER::PLAYER_ID(), On(iRun) ? 1.49f : 1.0f); });
    iSwim = AddT("Fast Swim", [] { PLAYER::SET_SWIM_MULTIPLIER_FOR_PLAYER(PLAYER::PLAYER_ID(), On(iSwim) ? 1.49f : 1.0f); });
    iJump = AddT("Super Jump");
    AddA("Parachute", [] { WEAPON::GIVE_WEAPON_TO_PED(Me(), H("GADGET_PARACHUTE"), 1, 0, 1); Notify("Parachute"); });
    iDrunk = AddT("Drunk Mode", [] {
        if (On(iDrunk)) { STREAMING::REQUEST_ANIM_SET((char*)"move_m@drunk@verydrunk"); CAM::SHAKE_GAMEPLAY_CAM((char*)"DRUNK_SHAKE", 1.0f); drunkApplied = false; }
        else { PED::RESET_PED_MOVEMENT_CLIPSET(Me(), 0.0f); CAM::STOP_GAMEPLAY_CAM_SHAKING(1); }
    });
    AddA("Skyfall", [] { Ped p = Me(); Vector3 c = ENTITY::GET_ENTITY_COORDS(p, 1); ENTITY::SET_ENTITY_COORDS_NO_OFFSET(p, c.x, c.y, c.z + 1000.0f, 0, 0, 1); });
    AddA("Slow Motion (Steps)", [] { slowIdx = (slowIdx + 1) % 4; GAMEPLAY::SET_TIME_SCALE(slowSteps[slowIdx]); char b[32]; sprintf(b, "Time scale: %.1f", slowSteps[slowIdx]); Notify(b); });
    iSlowAim = AddT("Slow Motion Aim", [] { GAMEPLAY::SET_TIME_SCALE(slowSteps[slowIdx]); });
    iGrav = AddT("Low Gravity", [] { GAMEPLAY::SET_GRAVITY_LEVEL(On(iGrav) ? 2 : 0); });
    iMelee = AddT("Explosive Melee");
    iExp = AddT("Explosive Ammo");
    iFire = AddT("Flaming Ammo");
    iSlide = AddT("Slidey Cars", [] { Ped p = Me(); if (!On(iSlide) && PED::IS_PED_IN_ANY_VEHICLE(p, 0)) VEHICLE::SET_VEHICLE_REDUCE_GRIP(PED::GET_VEHICLE_PED_IS_USING(p), 0); });
    AddA("Change Weather", [] { weatherIdx = (weatherIdx + 1) % 13; GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST((char*)weathers[weatherIdx]); Notify(std::string("Weather: ") + weathers[weatherIdx]); });
    AddA("Teleport to Waypoint", [] { TeleportWaypoint(); });
    AddA("Repair Vehicle", [] { Ped p = Me(); if (PED::IS_PED_IN_ANY_VEHICLE(p, 0)) { VEHICLE::SET_VEHICLE_FIXED(PED::GET_VEHICLE_PED_IS_USING(p)); Notify("Vehicle repaired"); } });

    // Vehicles
    struct V { const char* n; const char* m; };
    static V cars[] = {
        {"BMX - Bicycle","BMX"},
        {"Buzzard - Attack Helicopter","BUZZARD"},
        {"Caddy - Golf Cart","CADDY"},
        {"Comet - Sports Car","COMET2"},
        {"Duster - Crop Duster Plane","DUSTER"},
        {"Mallard - Stunt Plane","STUNT"},
        {"PCJ-600 - Motorcycle","PCJ"},
        {"Rapid GT - Sports Car","RAPIDGT"},
        {"Sanchez - Dirt Bike","SANCHEZ"},
        {"Stretch - Limousine","STRETCH"},
        {"Trashmaster - Garbage Truck","TRASH"},
        {"Dodo - Seaplane","DODO"},
        {"Duke O'Death - Armored Car","DUKES2"},
        {"Kraken - Submarine","SUBMERSIBLE2"},
        {"Rhino - Military Tank","RHINO"},
        {"Crusader - Military Jeep","CRUSADER"},
        {"Barracks - Military Truck","BARRACKS"},
        {"Lazer - Military Fighter Jet","LAZER"},
        {"Annihilator - Military Heli","ANNIHILATOR"},
        {"Titan - Military Cargo Plane","TITAN"} };
    for (auto& c : cars) { const char* m = c.m; std::string n = c.n; AddA(n, [m, n] { Spawn(m, n); }); }
}

static void Move(int d)
{
    int c = (int)items.size();
    sel = (sel + d + c) % c;
    if (sel < topI) topI = sel;
    if (sel >= topI + VIS) topI = sel - VIS + 1;
}

static void Activate(Item& it)
{
    if (it.toggle) { it.on = !it.on; msg = L(it.name) + ": " + OnTxt(it.on); msgUntil = GetTickCount() + 2500; }
    if (it.act) it.act();
}

static void RunItem(const char* n)
{
    for (auto& it : items) if (it.name == n) { Activate(it); return; }
}

// ---------------------------------------------------------------- input
static bool KeyJust(int vk)
{
    bool d = (GetAsyncKeyState(vk) & 0x8000) != 0;
    bool r = d && !prevKey[vk];
    prevKey[vk] = d;
    return r;
}
static bool PadP(int c) { return CONTROLS::IS_DISABLED_CONTROL_PRESSED(2, c) || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, c); }
static bool PadJ(int c) { return CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, c) || CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, c); }

// R (RB) + button shortcuts
static bool Combos()
{
    int block[] = { 21,22,23,24,25,26,36,45,140,141,142,143 };
    for (int c : block) CONTROLS::DISABLE_CONTROL_ACTION(0, c, 1);
    bool inVeh = PED::IS_PED_IN_ANY_VEHICLE(Me(), 0);
    if (PadJ(187)) { RunItem("Invincibility"); return true; }
    if (PadJ(201)) { RunItem("Max Health & Armor"); return true; }
    if (PadJ(202)) { RunItem("Clear Wanted Level"); return true; }
    if (PadJ(203)) { RunItem("All Weapons & Ammo"); return true; }
    if (PadJ(204)) { RunItem("Teleport to Waypoint"); return true; }
    if (!inVeh && PadJ(208)) { RunItem("Comet - Sports Car"); return true; }
    if (!inVeh && PadJ(207)) { RunItem("Super Jump"); return true; }
    if (PadJ(209)) { RunItem("Skyfall"); return true; }
    if (PadJ(210)) { RunItem("Repair Vehicle"); return true; }
    return false;
}

// ---------------------------------------------------------------- drawing
static void DrawMenu()
{
    float x = 0.02f, w = 0.26f, y = 0.08f, h = 0.032f;
    Rect(x, y, w, 0.062f, 8, 8, 8, 235);
    Txt("3rr0r // STORY CLIENT", x + 0.006f, y + 0.002f, 0.6f, 255, 30, 30, 4);
    Txt("Made by 3rr0r303YT on TikTok", x + 0.006f, y + 0.036f, 0.25f, 190, 190, 190);
    Rect(x, y + 0.060f, w, 0.002f, 255, 0, 0, 255);
    y += 0.062f;
    int end = (int)items.size(); if (end > topI + VIS) end = topI + VIS;
    for (int i = topI; i < end; i++)
    {
        bool s = (i == sel);
        Rect(x, y, w, h, s ? 190 : 0, 0, 0, s ? 230 : 210);
        std::string nm = (i == iBoost) ? std::string("FPS Booster: ") + boostNames[lang][boost] : L(items[i].name);
        Txt(nm.c_str(), x + 0.006f, y + 0.003f, 0.32f, 255, 255, 255);
        if (items[i].toggle)
        {
            std::string st = std::string("[") + OnTxt(items[i].on) + "]";
            Txt(st.c_str(), x + w - 0.006f, y + 0.003f, 0.32f,
                items[i].on ? 50 : 150, items[i].on ? 205 : 150, items[i].on ? 50 : 150, 0, true);
        }
        y += h;
    }
    Rect(x, y, w, 0.03f, 8, 8, 8, 235);
    char b[96];
    sprintf(b, lang ? "%d/%d   Steuerkreuz | A = OK | B = Zu" : "%d/%d   D-Pad | A = Select | B = Close",
            sel + 1, (int)items.size());
    Txt(b, x + 0.006f, y + 0.004f, 0.27f, 190, 190, 190);
}

// ---------------------------------------------------------------- main loop
static void Update()
{
    Player pl = PLAYER::PLAYER_ID();
    Ped p = Me();
    bool inVeh = PED::IS_PED_IN_ANY_VEHICLE(p, 0);

    // cheats
    PLAYER::SET_PLAYER_INVINCIBLE(pl, On(iGod));
    if (On(iJump)) GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(pl);
    if (On(iMelee)) GAMEPLAY::SET_EXPLOSIVE_MELEE_THIS_FRAME(pl);
    if (On(iExp)) GAMEPLAY::SET_EXPLOSIVE_AMMO_THIS_FRAME(pl);
    if (On(iFire)) GAMEPLAY::SET_FIRE_AMMO_THIS_FRAME(pl);
    if (On(iSlide) && inVeh) VEHICLE::SET_VEHICLE_REDUCE_GRIP(PED::GET_VEHICLE_PED_IS_USING(p), 1);
    if (On(iSlowAim) && slowIdx == 0) GAMEPLAY::SET_TIME_SCALE(CONTROLS::IS_CONTROL_PRESSED(0, 25) ? 0.4f : 1.0f);
    if (On(iDrunk) && !drunkApplied && STREAMING::HAS_ANIM_SET_LOADED((char*)"move_m@drunk@verydrunk"))
    {
        PED::SET_PED_MOVEMENT_CLIPSET(p, (char*)"move_m@drunk@verydrunk", 1.0f);
        drunkApplied = true;
    }

    // FPS booster
    if (boost > 0)
    {
        float d = boostDens[boost];
        PED::SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(d);
        PED::SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(d, d);
        VEHICLE::SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(d);
        VEHICLE::SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(d);
        VEHICLE::SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(boost == 4 ? 0.0f : d * 0.5f);
        STREAMING::SET_REDUCE_PED_MODEL_BUDGET(boost >= 2);
        STREAMING::SET_REDUCE_VEHICLE_MODEL_BUDGET(boost >= 2);
        static DWORD lastClean = 0;
        DWORD iv = boost == 4 ? 2000 : (boost == 3 ? 3000 : (boost == 2 ? 5000 : 10000));
        float maxD = boost == 4 ? 40.0f : (boost == 3 ? 50.0f : (boost == 2 ? 80.0f : 150.0f));
        if (GetTickCount() - lastClean > iv) { lastClean = GetTickCount(); CleanWorld(maxD); }
    }
    else
    {
        STREAMING::SET_REDUCE_PED_MODEL_BUDGET(0);
        STREAMING::SET_REDUCE_VEHICLE_MODEL_BUDGET(0);
    }

    if (menuOpen)
    {
        int block[] = { 27,172,173,174,175,176,177,19,21,22,23,140,45,76,74,85,37 };
        for (int c : block) CONTROLS::DISABLE_CONTROL_ACTION(0, c, 1);
    }

    // keyboard
    bool kF4 = KeyJust(VK_F4), kF3 = KeyJust(VK_F3), kF2 = KeyJust(VK_F2), kF5 = KeyJust(VK_F5);
    bool kUp = KeyJust(VK_UP) | KeyJust(VK_NUMPAD8);
    bool kDn = KeyJust(VK_DOWN) | KeyJust(VK_NUMPAD2);
    bool kOk = KeyJust(VK_RETURN) | KeyJust(VK_NUMPAD5);
    bool kBk = KeyJust(VK_BACK);
    if (kF4) menuOpen = !menuOpen;
    if (kF3) CycleBoost();
    if (kF2) items[iFps].on = !items[iFps].on;
    if (kF5) DoAntiLag();
    if (KeyJust(VK_F6)) { lang = 1 - lang; SaveLang(); Notify(lang ? "Sprache: Deutsch" : "Language: English"); }

    // controller: hold R (RB) + button
    bool rb = PadP(206);
    if (rb && !menuOpen && Combos()) {}
    else if (rb && PadJ(189)) menuOpen = !menuOpen;
    else if (rb && PadJ(190)) CycleBoost();
    else if (rb && PadJ(188) && !menuOpen) items[iFps].on = !items[iFps].on;
    else if (menuOpen)
    {
        if (kUp || PadJ(188)) Move(-1);
        if (kDn || PadJ(187)) Move(1);
        if (kOk || PadJ(201)) Activate(items[sel]);
        if (kBk || PadJ(202)) menuOpen = false;
    }

    // FPS + anti-lag
    float ft = GAMEPLAY::GET_FRAME_TIME();
    if (ft > 0.0f) fpsSmooth = fpsSmooth * 0.9f + (1.0f / ft) * 0.1f;
    AntiLag(ft);
    if (On(iFps))
    {
        char b[64];
        sprintf(b, "FPS %d  |  Boost %s", (int)fpsSmooth, boostNames[lang][boost]);
        int g = fpsSmooth >= 55 ? 205 : (fpsSmooth >= 30 ? 220 : 40);
        int r = fpsSmooth >= 55 ? 50 : 255;
        Txt(b, 0.995f, 0.005f, 0.32f, r, g, 40, 0, true);
    }

    if (menuOpen) DrawMenu();
    if (GetTickCount() < msgUntil) Txt(msg.c_str(), 0.5f, 0.85f, 0.45f, 255, 255, 255, 0, false, true);
}

static void ScriptMain()
{
    LoadLang();
    Build();
    Notify("3rr0r Story Client loaded - R + Left / F4");
    while (true) { Update(); scriptWait(0); }
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) scriptRegister(hInstance, ScriptMain);
    else if (reason == DLL_PROCESS_DETACH) scriptUnregister(hInstance);
    return TRUE;
}

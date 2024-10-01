# CS16Client
New cs16 client based on old FWGS repository (v1.33).

## Features & Planned
- [x] **255 HP HUD Limit Fix**
- [x] **Ported and redesigned C++ based mainui**
- [x] **Nightvision fix**
- [x] **HP & Money Display on Scoreboard**
- [x] **Default cvar values same with original CS 1.6**
- [x] Win Image (CT & T WIN)
- [x] Speedometer (Console string, HUD and Nexon's Floatingdamage)
- [x] CSO Scoreboard (Old & New) 
- [x] CS2 and CSO Crosshair
- [ ] HTML on MOTD
- [ ] VGUI2 (requires Physic3D engine)
- [ ] Gradle Android build
- [ ] CS2 HUD (https://youtu.be/wi6-yMox6pk)
- [ ] CSO Hud
- [ ] CSO Killmark(Done) Crossfire Killmark With 40 Different Killmarks(Done) and CS2 Killmark
- [ ] Hit indicator(https://cdn.discordapp.com/attachments/1018571696246100151/1250930219582226513/2024-06-14_00-49-15.mp4?ex=66bbd50d&is=66ba838d&hm=273b1b41e868ba87caee74cf949903fa2440c9ed8504e97a61d20c14bc5bf7dd&)
- [ ] Glow (Viewmodel, playermodel, Worldmodel)
## Have you ever checked Physic3D?
Need some cool features on Xash3D 0.19.x like fog, weapon inspect, xhair? Look at [Physic3D](https://github.com/Physic3D/physic3d).


## Commands
```py
hud_winimage 
hud_speedometer ( 1 Console String 2 HUD 3 Nexon's floatingdamage tga )
hud_scoreboard ( 1 Old Cso Scoreboard 2 New Cso Scoreboard )
hud_killmark ( 1 Old Cso Kill mark 2 New Cso Killmark 3 Crossfire Killmark )
hud_killicon_display_time
hud_killeffect_display_time
hud_crossfire_killmark_type ( 1 default, 2 15th anniversary, 3 Angelic Beast, 4 Armored Beast, 5 Blueneon, 6 Bornbeast2, 7 Bornbeast2NG, 8 CFS_2019, 9 CHAMPQ9, 10 christimas
	// 11 Christmas two, 12 Demonic Beast, 13 Disco, 14 Dominator, 15 Fury Beast, 16 Fury BeastNG,17 Gaming Glory, 18 Halloween, 19 Halloween 2, 20 Hero,
	//21 IronBeast2, 22 IronBeast2NG, 23 Kar98k Satelite, 24 M82A1 Angelic Beast, 25 M82A1 Demonic Beast, 26 New Year 27  Nightmare, 28 Rankmach19, 29 Rankmach19 two, 30 Rankmach25
	// 31 Rankmach25 two, 32 Rankmach26, 33 Rankmach26 Two, 34 Rankmach26 Three, 35 Reactive Armor Set, 36 Red Nano, 37 Summer, 38 Summer 2, 39 TianShen, 40 ValorBeast )

// Speedometer commands
hud_speedometer
hud_speedometer_x
hud_speedometer_y
hud_speedometer_red
hud_speedometer_green
hud_speedometer_blue
hud_speedometer_texture_width
hud_speedometer_texture_height
hud_speedometer_texture_type

//Crosshair Commands
cl_crosshair // 1 CSO (Counter Strike Online) 2 CS2 (Counter Strike 2)
cl_cs2_crosshair_type 
cl_cso_crosshair_type
cl_cs2_crosshairthickness
cl_cs2_crosshair_drawoutline
cl_cs2_crosshair_outlinethickness
cl_cs2_crosshairalpha
cl_cs2_crosshairusealpha
cl_cs2_crosshairdot
cl_cs2_crosshairgap
cl_cs2_crosshair_t
cl_cs2_crosshair_red
cl_cs2_crosshair_green
cl_cs2_crosshair_blue

```

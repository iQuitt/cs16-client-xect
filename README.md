# CS16Client
New cs16 client based on old FWGS repository (v1.33).

## Features & Planned
- [x] **255 HP HUD Limit Fix**
- [x] **Ported and redesigned C++ based mainui**
- [x] **Nightvision fix**
- [x] **HP, Money and Weapon Name Display on Scoreboard**
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


## Commands (Cvars)
<details>

| CVar                               | Default | Disable | Enable          | Description                                    |
| :--------------------------------- | :-----: | :-: | :----------: | :--------------------------------------------- |
| hud_winimage                      | 1       | 0   | 1            | Show the win image. |
| hud_speedometer                  | 1       | 0   | 1            | Draw your speed value ( 1 Console String 2 HUD 3 Nexon's floatingdamage tga ) |
| hud_scoreboard                          | 1     | 0 | 1            | Draw Topscoreboard ( 1 CSO old, 2 CSO new ). |
| hud_killfx                        | 1   | 0   | 1     |  Shows an image when kill the user ( 1 CSO old, 2 CSO new, 3 Crossfire |
| hud_killicon_display_time                  | 1.5       | 0 | 0               | Kill icon display time for CSO old killfx |
| hud_killeffect_display_time                       | 3       | 0  | 0             | Kill effect display time for CSO new and Crossfire killfx |
| hud_crossfire_killmark_type               | 1       |    | 1            | Draw different kill marks (MaxValue: 40)  |
| hud_speedometer_x            | 0       |    |             | Draw speedometer X Coord. |
| hud_speedometer_y                        | 300        |    |             | Draw speedometer Y Coord. |
| hud_speedometer_red               | 255      |    |             | Draw Speedometer Red color |
| hud_speedometer_green             | 255       |    |             | Draw Speedomter Green Color |
| hud_speedometer_blue           | 255       |    |             | Draw Speedometer Blue Color |
| hud_speedometer_texture_width                   | 25       | 0   | 1            | Draw Speedometer Texture Width |
| hud_speedometer_texture_height               | 25       | 0   | 1            | Draw Speedometer Texture Height |
| hud_speedometer_texture_type                  | 1       | 0   | 1            | Draw different Speedometer textures (MaxValue:37) |
| cl_crosshair                   | 0       | 0   | 1            | Draw crosshairs ( 1 Cso, 2 CS2)  |
| cl_cso_crosshair_type                   | 1       | 0   | 1            | Draw different crosshair styles from CSO |
| cl_cs2_crosshair_type                      | 1       | 0   | 1            | to activate different crosshair styles. The values are: 0 for the default dynamic style, 1 for a static default crosshair, 2 for a classic static crosshair with dynamic dots, 3 for classic dynamic, 4 for classic static, and 5 for a mix of static and dynamic styles of the classic crosshair. |
| cl_cs2_crosshairthickness                      | 0.5       |    |             |  crosshair thickness. The range of values is from -2 to 2. |
| cl_cs2_crosshair_drawoutline                        | 1       | 0   | 1            | a black outline appears around the crosshair, making it more notable on the screen. |
| cl_cs2_crosshair_outlinethickness                       | 0.1       | 0   | 1            | change how thick the outline around your CS2 crosshair is. 1 is the default value here, and the possible range is from 0.1 to 3. |
| cl_cs2_crosshairalpha                    | 200       | 0   | 1            | change the transparency of your crosshair. The default value is 200, and the possible range is from 10 |
| cl_cs2_crosshairusealpha                    | 1       | 0   | 1            | Use the alpha of crosshair |
| cl_cs2_crosshairdot                    | 1       | 0   | 1            | Draw a dot on crosshair |
| cl_cs2_crosshairgap                   | 1       | 0   | 1            | this command changes the gap size in the middle of the crosshair. The default value is 1, and the possible range is from -10 to 10. cl_fixedcrosshairgap 3 is an alternative command with the same range of values; use it with a fixed crosshair style. |
| cl_cs2_crosshair_t                    | 1       | 0   | 1            | activate a T style of crosshair |
| cl_cs2_crosshair_red                    | 255       | 0   | 1            | Draw Crosshair Red color |
| cl_cs2_crosshair_green                    | 255       | 0   | 1            | Draw Crosshair Green color |
| cl_cs2_crosshair_blue                   | 255       | 0   | 1            | Draw Crosshair Blue Color |

</details>

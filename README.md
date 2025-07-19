# CS16Client

> Modern Counter-Strike 1.6 client based on FWGS repository (v1.33) with enhanced features

![Counter-Strike 1.6](https://img.shields.io/badge/Counter--Strike-1.6-orange.svg)
![Status](https://img.shields.io/badge/Status-Active%20Development-green.svg)
![License](https://img.shields.io/badge/License-GPL--3.0-blue.svg)

---

## Table of Contents

- [Features](#features)
- [Planned Features](#planned-features)
- [Commands & Settings](#commands--settings)
- [Screenshots](#screenshots)

---

## Features

### **Core Improvements**
- **255 HP HUD Limit Fix** - Enhanced HUD for high health values
- **Ported and Redesigned C++ Based MainUI** - Modern interface system
- **Nightvision Fix** - Improved nightvision functionality
- **Original CS 1.6 Default Cvar Values** - Full compatibility with original game

### **HUD & Interface**
- **Enhanced Scoreboard** - HP, Money and Weapon Name display
- **Win Images** - CT & T team victory animations
- **Speedometer** - Console string, HUD and Nexon's floating damage styles
- **CSO Scoreboard** - Both old and new designs
- **CS2 and CSO Crosshairs** - Modern crosshair systems

---

## Planned Features

### **Web & Mobile**
- [ ] **HTML on MOTD** - Web-based message system
- [ ] **VGUI2** *(requires Physic3D engine)*
- [ ] **Gradle Android Build** - Android platform support

### **Visual Enhancements**
- [ ] **CS2 HUD** - [Preview](https://youtu.be/wi6-yMox6pk)
- [ ] **CSO HUD** - Counter-Strike Online interface
- [ ] **Advanced Killmark System**
  - CSO Killmark ✅
  - Crossfire Killmark (40 different types) ✅
  - CS2 Killmark
- [ ] **Hit Indicator** - [Demo](https://cdn.discordapp.com/attachments/1018571696246100151/1250930219582226513/2024-06-14_00-49-15.mp4)
- [ ] **Glow Effects** - Viewmodel, playermodel, worldmodel

---

## Commands & Settings

<details>
<summary><strong>Complete CVar List</strong></summary>

### **Visual Settings**

| CVar | Default | Disable | Enable | Description |
|:-----|:-------:|:-------:|:------:|:------------|
| `hud_winimage` | **1** | 0 | 1 | Show the win image |
| `hud_speedometer` | **1** | 0 | 1 | Draw speed value (1: Console String, 2: HUD, 3: Nexon's floatingdamage) |
| `hud_scoreboard` | **1** | 0 | 1 | Draw top scoreboard (1: CSO old, 2: CSO new) |

### **Kill Effects**

| CVar | Default | Disable | Enable | Description |
|:-----|:-------:|:-------:|:------:|:------------|
| `hud_killfx` | **1** | 0 | 1 | Show kill effect image (1: CSO old, 2: CSO new, 3: Crossfire) |
| `hud_killicon_display_time` | **1.5** | 0 | - | Kill icon display time for CSO old killfx |
| `hud_killeffect_display_time` | **3** | 0 | - | Kill effect display time for CSO new and Crossfire killfx |
| `hud_crossfire_killmark_type` | **1** | - | 1-40 | Draw different kill marks (Max Value: 40) |

### **Speedometer Settings**

| CVar | Default | Range | Description |
|:-----|:-------:|:-----:|:------------|
| `hud_speedometer_x` | **0** | - | Speedometer X coordinate |
| `hud_speedometer_y` | **300** | - | Speedometer Y coordinate |
| `hud_speedometer_red` | **255** | 0-255 | Speedometer red color value |
| `hud_speedometer_green` | **255** | 0-255 | Speedometer green color value |
| `hud_speedometer_blue` | **255** | 0-255 | Speedometer blue color value |
| `hud_speedometer_texture_width` | **25** | - | Speedometer texture width |
| `hud_speedometer_texture_height` | **25** | - | Speedometer texture height |
| `hud_speedometer_texture_type` | **1** | 1-37 | Different speedometer textures (Max Value: 37) |

### **Crosshair Settings**

#### General Crosshair
| CVar | Default | Options | Description |
|:-----|:-------:|:-------:|:------------|
| `cl_crosshair` | **0** | 0/1/2 | Draw crosshairs (1: CSO, 2: CS2) |
| `cl_cso_crosshair_type` | **1** | 1+ | Draw different crosshair styles from CSO |

#### CS2 Crosshair
| CVar | Default | Range | Description |
|:-----|:-------:|:-----:|:------------|
| `cl_cs2_crosshair_type` | **1** | 0-5 | Crosshair style (0: dynamic, 1: static default, 2: classic static with dynamic dots, etc.) |
| `cl_cs2_crosshairthickness` | **0.5** | -2 to 2 | Crosshair thickness |
| `cl_cs2_crosshair_drawoutline` | **1** | 0/1 | Black outline around crosshair |
| `cl_cs2_crosshair_outlinethickness` | **0.1** | 0.1-3 | Outline thickness |
| `cl_cs2_crosshairalpha` | **200** | 10+ | Crosshair transparency |
| `cl_cs2_crosshairusealpha` | **1** | 0/1 | Use alpha for crosshair |
| `cl_cs2_crosshairdot` | **1** | 0/1 | Draw dot in center |
| `cl_cs2_crosshairgap` | **1** | -10 to 10 | Gap size in middle of crosshair |
| `cl_cs2_crosshair_t` | **1** | 0/1 | Activate T-style crosshair |
| `cl_cs2_crosshair_red` | **255** | 0-255 | Crosshair red color |
| `cl_cs2_crosshair_green` | **255** | 0-255 | Crosshair green color |
| `cl_cs2_crosshair_blue` | **255** | 0-255 | Crosshair blue color |

</details>

---

## Screenshots

<details>
<summary><strong>View Screenshots</strong></summary>

### Counter Strike Online Scoreboard
**Old & New Designs**

<img width="1839" height="765" alt="CSO Scoreboard Old" src="https://github.com/user-attachments/assets/741123d9-1118-42cd-8614-22e52b739a18" />
<img width="1916" height="638" alt="CSO Scoreboard New" src="https://github.com/user-attachments/assets/66804c2a-b3f9-4e2e-9325-d2c36ef5c0e5" />

### Kill Effects
- **Crossfire Killmark (40 Types)** - [Watch Video](https://streamable.com/fi8nq4)
- **Counter Strike Online Killmark** - [Watch Video](https://streamable.com/07yodi)

### 255 HP HUD Fix
<img width="578" height="738" alt="image" src="https://github.com/user-attachments/assets/8ff82bb4-1845-4e43-96bb-5de264c83d62" />

### Teammate Weapons on Scoreboard
<img width="1086" height="139" alt="Teammate Weapons" src="https://github.com/user-attachments/assets/ee747917-57cf-4a21-8e0d-2554c682764e" />

### Speedometer Variants
**Console String, HUD and Nexon's Floatingdamage** - [Watch Video](https://streamable.com/kcvtjv)

<img width="1913" height="724" alt="Speedometer 1" src="https://github.com/user-attachments/assets/5e41811d-b3d4-4b4d-a618-8c4caefa5e2a" />
<img width="1907" height="826" alt="Speedometer 2" src="https://github.com/user-attachments/assets/8ecd867b-288c-436b-9c20-e1e575fb7814" />
<img width="1919" height="842" alt="Speedometer 3" src="https://github.com/user-attachments/assets/4b447794-0551-4824-8883-f888c626b10e" />
<img width="1916" height="685" alt="Speedometer 4" src="https://github.com/user-attachments/assets/49eaa72e-32b5-4aab-82cc-1fc62306d51d" />
<img width="1905" height="650" alt="Speedometer 5" src="https://github.com/user-attachments/assets/09f23e1d-9cd3-41cf-82ed-1bc4c49ce12e" />
<img width="1919" height="842" alt="Speedometer 6" src="https://github.com/user-attachments/assets/ba55c269-dd26-40f6-bf0a-1362fdec6193" />

### CS2 and CSO Crosshairs
<img width="1881" height="671" alt="CS2 Crosshair 1" src="https://github.com/user-attachments/assets/6d48885f-3f92-4872-bd80-b87fa2eb7edf" />
<img width="1919" height="471" alt="CSO Crosshair" src="https://github.com/user-attachments/assets/aa4dd0ed-bf1a-4903-ae34-f2f4df65c12a" />
<img width="580" height="268" alt="Crosshair Detail" src="https://github.com/user-attachments/assets/f68c2d1f-d79e-415c-b337-c03b99277e61" />
<img width="1916" height="457" alt="CS2 Crosshair 2" src="https://github.com/user-attachments/assets/4136fd26-2afc-401c-9063-ba7d3b198542" />

</details>

---

## About Physic3D

Looking for advanced features on Xash3D 0.19.x like fog, weapon inspect, custom crosshairs and more? Check out [**Physic3D**](https://github.com/Physic3D/physic3d) project!

---


---

## License

This project is distributed under the GPL-3.0 License. See `LICENSE` file for more information.

---


<div align="center">

**If you like this project, don't forget to give it a star!**

[Report Bug](https://github.com/iQuitt/CS16Client/issues) • [Request Feature](https://github.com/iQuitt/CS16Client/issues) • [Discussions](https://github.com/iQuitt/CS16Client/discussions)

</div>

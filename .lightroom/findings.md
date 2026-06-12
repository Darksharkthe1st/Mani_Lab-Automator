# Findings: Darksharkthe1st/Mani_Lab-Automator

## Overview

Mani_Lab-Automator is a Windows desktop automation suite for the Mani Lab (physics/materials research). It automates control of a Kepco magnet power supply and coordinates with a LabVIEW-based gaussmeter reader, replacing repetitive manual GUI clicks and typed values during experiments. The primary deliverable is a console C++ program that drives the Kepco Control Panel by simulating mouse clicks and keyboard input, executing user-defined magnet on/off/state sequences timed to lab-safe intervals (≥7.5 s).

The repository serves lab researchers running magnet calibration and timed field-switching experiments. It is not a web service or distributed system; it is local, hardware-adjacent automation built with Visual Studio C++ projects targeting Windows 10+.

## Stack

- **C++ (console applications)** — `Magnet Controller/Magnet Controller/New Magnet Code.cpp`, `Magnet Calibrator/Magnet Calibrator/Calibrating Magnets in C++.cpp`, `Experimentation with Windows/Experimentation with Windows/Window Experimentation in C++.cpp`
- **Windows API (`Windows.h`)** — input simulation via `SendInput`, `mouse_event`, `SetCursorPos`, `GetCursorPos`, `GetAsyncKeyState`; window management via `FindWindow`, `MoveWindow`, `ShowWindow`, `SetWindowPos` (see `New Magnet Code.cpp`, `Calibrating Magnets in C++.cpp`)
- **Visual Studio 2022 / MSVC v143** — `Magnet Controller/Magnet Controller/Magnet Controller.vcxproj` (`PlatformToolset>v143</PlatformToolset>`, `WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>`)
- **MSBuild / `.sln` + `.vcxproj`** — four solution roots: `Magnet Controller/Magnet Controller.sln`, `Magnet Calibrator/Magnet Calibrator.sln`, `Experimentation with Windows/Experimentation with Windows.sln`, `Analysis Automation/Analysis Automation.sln`
- **LabVIEW 2017 (external, launched by automation)** — `system("explorer C:\\Program Files\\National Instruments\\LabVIEW 2017\\LabVIEW.exe")` in `Calibrating Magnets in C++.cpp`
- **File-based IPC with gaussmeter script** — reads calibration output from `C:\Users\Mani Lab\Desktop\Calibrator Text File\calibrator text.txt` via `ifstream` in `runAndCollect()` (`Calibrating Magnets in C++.cpp`)
- **WinMM (legacy audio hook)** — `#pragma comment(lib, "Winmm.lib")` in controller/calibrator sources; MCI sound playback commented out in `Magnet Code.cpp`

**Not present:** Docker, cloud services, databases, web frameworks, CI/CD pipelines (`.github/` absent), automated test suites, REST/gRPC APIs.

## Architecture

Four Visual Studio projects sit at the repo root; three contain executable C++ logic and one is a scaffold:

```
┌─────────────────────────────────────────────────────────────────┐
│                     Mani Lab Windows PC                         │
├─────────────────────────────────────────────────────────────────┤
│  Magnet Controller (primary)                                    │
│    New Magnet Code.cpp → Kepco GUI automation (mouse/keyboard)  │
│    User records SET button (F12) → offset-based field clicks  │
│    Runs ternary sequence (0/1/2) over magnet states             │
├─────────────────────────────────────────────────────────────────┤
│  Magnet Calibrator                                            │
│    Calibrating Magnets in C++.cpp                               │
│    Binary search + fine-grained voltage sweep → target Gauss    │
│    Reads gaussmeter values from shared .txt file                │
├─────────────────────────────────────────────────────────────────┤
│  Experimentation with Windows (earlier integrated prototype)    │
│    Window Experimentation in C++.cpp                            │
│    Combines calibration loop + timed on/off cycles              │
├─────────────────────────────────────────────────────────────────┤
│  Analysis Automation — VS project only, no .cpp source files    │
└─────────────────────────────────────────────────────────────────┘
         │                              │
         ▼                              ▼
   Kepco Control Panel            LabVIEW 2017 + gaussmeter
   (GUI automation target)        (writes calibrator text.txt)
```

**Control flow (Magnet Controller):** User enters target Gauss, interval, and a ternary digit string. On F12, the console hides and `magnetTo()` clicks VSET/ISET/SET at coordinates derived from a one-time F12-recorded SET position, types voltage/current strings, dismisses dialogs, and sleeps for the configured interval. States `0`, `1`, and `2` map to zero-field, input-field, and ending-field settings.

**Control flow (Magnet Calibrator):** Launches LabVIEW and Kepco, positions windows, then uses `calibrateTo()` (binary search on voltage) and nested decimal refinement loops, polling `runAndCollect()` after each voltage change.

## User contributions (@Darksharkthe1st)

All **23 commits** on `main` are authored by **Farhan Kittur** (`kitturfarhan@gmail.com`; one commit uses `54246758+Darksharkthe1st@users.noreply.github.com`). @Darksharkthe1st is the **sole contributor** and appears to own the entire codebase.

| Area | Evidence | Commit themes |
|------|----------|---------------|
| **Magnet Controller (primary ownership)** | 18+ commits touching `Magnet Controller/Magnet Controller/New Magnet Code.cpp` | Iterative debugging ("It works!", "third time's the charm!"), binary → ternary state encoding (`6548d7c`), Gauss↔voltage conversion (`68bd97d`, `0a3c076`), F12-based coordinate automation, escape termination, timing tuning |
| **Legacy controller refactor** | `Magnet Controller/Magnet Controller/Magnet Code.cpp` (now fully commented out) | Early tab-key navigation prototype; superseded by coordinate-based `New Magnet Code.cpp` (`3c737a9`) |
| **Magnet Calibrator** | `Magnet Calibrator/Magnet Calibrator/Calibrating Magnets in C++.cpp` | Initial push (`804e241`); documentation/cleanup (`f660859`); calibration constant updates (`68bd97d`) |
| **Experimentation prototype** | `Experimentation with Windows/.../Window Experimentation in C++.cpp` | Shipped in initial full push (`804e241`); no subsequent commits |
| **Dev utilities** | `Magnet Controller/Magnet Controller/track mouse.cpp` | Mouse coordinate debugging tool (`b88c9aa`, `0c998c1`, `2174bc3`) |
| **Repo bootstrap** | All four `.sln` projects, `.gitignore` | Single "Full Code Push" (`804e241`) established the multi-project layout |

**Evolution narrative:** June 2025 — built and debugged keyboard-tab automation until working; introduced `New Magnet Code.cpp` with binary-string-driven sequences. June 2025–2026 — extended to ternary states, refined Gauss/voltage physics constants, switched to mouse-coordinate control with user-calibrated SET button offsets, and hardened operator controls (ESC kill, numpad restart). January 2026 — improved calibrator comments. June 2026 — burst of 14 commits refining controller reliability (delays, loops, dialog dismissal).

## Recruiter signals

| Signal | Confidence | Evidence paths |
|--------|------------|----------------|
| **C++ / native Windows development** | 0.95 | `Magnet Controller/Magnet Controller/New Magnet Code.cpp`, `Magnet Calibrator/Magnet Calibrator/Calibrating Magnets in C++.cpp`, all `.vcxproj` files |
| **Windows GUI automation / RPA** | 0.92 | `SendInput`, `mouse_event`, `SetCursorPos`, `clickAt()` in `New Magnet Code.cpp`; `changeVolts()` in `Calibrating Magnets in C++.cpp` |
| **Scientific lab instrumentation automation** | 0.88 | Kepco magnet supply control, Gauss targets, LabVIEW gaussmeter integration across calibrator and controller |
| **Search / calibration algorithms** | 0.82 | Binary search in `calibrateTo()` and iterative decimal refinement in `calibToZero()` — `Calibrating Magnets in C++.cpp` |
| **Visual Studio / MSVC toolchain** | 0.90 | `PlatformToolset>v143</PlatformToolset>`, `.sln`/`.vcxproj` in all four project dirs |
| **Hardware-adjacent systems programming** | 0.80 | Timed magnet cycling, emergency ESC shutdown (`terminateIfEsc()`), interval guards (≥7500 ms) |
| **LabVIEW integration (file IPC)** | 0.72 | LabVIEW launch + `runAndCollect()` file read in `Calibrating Magnets in C++.cpp` |
| **API design (REST/services)** | 0.05 | **Rejected** — "Controller" refers to magnet hardware GUI automation, not API layers; no HTTP/RPC/OpenAPI artifacts |
| **CI/CD** | 0.00 | No `.github/workflows`, Jenkins, or similar |
| **Automated testing** | 0.05 | No test framework; manual `system("Pause")` checkpoints only |
| **Cloud / containers / databases** | 0.00 | None detected |

## Notes

- **Resume-worthy framing:** "Built C++ Windows automation tooling for a university physics lab to control a Kepco magnet supply and gaussmeter, replacing manual GUI operations with scripted coordinate-based input simulation and binary-search calibration algorithms."
- **Domain keywords for recruiters:** instrumentation control, scientific computing, desktop automation, Windows API, real-time sequencing, calibration loops, research tooling.
- **Analysis Automation** is an empty Visual Studio shell (`Analysis Automation/Analysis Automation/Analysis Automation.vcxproj` has no `<ClCompile Include=...>` entries) — likely a planned but unimplemented component.
- **Legacy artifact:** `Magnet Code.cpp` is entirely commented out; active entry point is `New Magnet Code.cpp` (both listed in `.vcxproj`).
- **Repo hygiene:** Commits include Visual Studio cache/build artifacts (`.vs/`, `x64/Debug/`, `.exe`, `.pdb`); `.gitignore` only excludes one Browse DB path.
- **No README** — project purpose is inferred from code comments and structure only.

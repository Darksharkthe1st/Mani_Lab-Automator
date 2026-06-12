# Signal: Magnet Controller — Kepco GUI automation

## Summary

@Darksharkthe1st (Farhan Kittur, `kitturfarhan@gmail.com`) is the sole contributor to the Magnet Controller subsystem, with **22 commits** on `main` spanning June 2025–June 2026. They built a Windows C++ console application that automates the Kepco magnet power supply Control Panel by simulating mouse clicks and keyboard input, replacing repetitive manual GUI operations during lab experiments. The work evolved from a tab-key navigation prototype through binary/ternary state sequencing, F12-based coordinate calibration, dialog dismissal, and timing hardening—making it the strongest ownership and recruiter-relevant signal in the repository (legacy GUI automation for scientific hardware).

## Technical depth

**Architecture:** Single-process console app (`Magnet Controller/Magnet Controller/New Magnet Code.cpp`, ~269 lines) compiled via Visual Studio 2022 (MSVC v143, Windows 10 SDK) per `Magnet Controller/Magnet Controller/Magnet Controller.vcxproj`. No network layer, database, or test harness—local desktop automation only.

**Control flow:**
1. **Calibration:** User places cursor on the Kepco SET button and presses F12; `GetCursorPos(&setButtonPos)` records anchor coordinates (lines 55–65).
2. **Configuration:** User enters target Gauss, interval (enforced ≥7500 ms in prompts), a ternary sequence string (`0` = off, `1` = on, `2` = ending state), and final Gauss.
3. **Execution:** F12 hides the console (`ShowWindow(..., SW_HIDE)`), iterates the sequence calling `magnetTo(current, voltage)` for each character, then applies the ending state.

**Core automation — `magnetTo()` (lines 125–182):**
- Derives VSET/ISET/SET screen positions from `setButtonPos` using lab-measured pixel offsets (`VSET_OFFSET_X = -93`, `ISET_OFFSET_X = 99`).
- `clickAt(POINT)` uses `SetCursorPos` + `mouse_event(MOUSEEVENTF_LEFTDOWN/UP)` for precise GUI targeting.
- Field entry: Ctrl+A select, Delete clear, `sendString()` typing via `SendInput`/`VkKeyScanA`.
- `dismissDialog()` clicks a fixed dialog button at (690, 392) before and after SET.
- Timing: `sleepCut(int percent)` sleeps a percentage of the user interval and accumulates into `delay`; final `Sleep(interval - delay)` enforces lab-safe dwell time between state changes.

**Physics conversion:** `voltageFunction(int gauss)` applies lab calibration `(gauss - 21.1) / 252.34` (lines 42–44), replacing an earlier formula `(gauss + 9.0991) / 260.65`. Configurable constants: `zeroVoltage`, `zeroCurrent`, `defaultCurrent`.

**Safety / UX hotkeys:** `terminateIfEsc()` calls `terminate()` on Escape (injected at every step of `magnetTo` per commit `0c998c1`); Numpad 0 terminates, Numpad 2 restarts input loop, F12 triggers run.

**Legacy prototype:** `Magnet Controller/Magnet Controller/Magnet Code.cpp` (271 lines, fully commented out since `3c737a9`) used Tab/Return keyboard navigation (`clickKey('\t')`, 14-tab focus restore loop) instead of coordinate clicks—superseded by `New Magnet Code.cpp`.

**Dev utility:** `Magnet Controller/Magnet Controller/track mouse.cpp` — live `GetCursorPos` coordinate printer used to measure Kepco field offsets; renamed `main()` → `mains()` (`0c998c1`) to avoid duplicate entry points. Not listed in `.vcxproj`.

## Evidence

### Files

- `Magnet Controller/Magnet Controller/New Magnet Code.cpp` — Active deliverable: F12 calibration, ternary sequence runner, `magnetTo`/`clickAt`/`dismissDialog`, `voltageFunction`, `SendInput` keyboard helpers, interval-aware `sleepCut` timing.
- `Magnet Controller/Magnet Controller/Magnet Code.cpp` — Entire file commented out; preserves tab-key navigation prototype and MCI sound-effect hooks; shows architectural pivot to coordinate-based automation.
- `Magnet Controller/Magnet Controller/track mouse.cpp` — Mouse coordinate debugging utility (`GetCursorPos` loop); supports offset calibration workflow documented in `New Magnet Code.cpp` comments (SET at X=732, VSET at X=639, ISET at X=831).
- `Magnet Controller/Magnet Controller/Magnet Controller.vcxproj` — VS 2022 project: `PlatformToolset>v143`, `WindowsTargetPlatformVersion>10.0`, console subsystem, compiles `Magnet Code.cpp` + `New Magnet Code.cpp`.
- `Magnet Controller/Magnet Controller.sln` — Visual Studio 17 solution wrapping the controller project.

### Commits

- `804e241`: **Full Code Push** — Initial repository bootstrap including stub `Magnet Code.cpp`.
- `8037244`: **Testing** — Early `Magnet Code.cpp` development with coordinate helpers (`clickAt`, `calibrateTo`) later removed.
- `0587790`: **third time's the charm!** — Major simplification of `Magnet Code.cpp`; stripped calibration/LabVIEW integration, established tab-key `magnetTo` pattern.
- `6825ab0`: **It works!** — First working tab-navigation magnet toggle; disabled MCI sound on startup.
- `c87ca5e`: **Compiled** — Build milestone for legacy controller.
- `3c737a9`: **Working Binary Style** — Created `New Magnet Code.cpp` with binary (`0`/`1`) string iteration over `magnetTo`; commented out entire legacy `Magnet Code.cpp`.
- `ab278b9`: **Small Change** — Iterative tuning on new controller.
- `81a008b`: **smaller change** — Further iteration.
- `6548d7c`: **Binary => Ternary** — Extended sequence encoding to three states (`0`/`1`/`2`); added `endingGauss`/`endingVoltage`; final state set via `magnetTo(inputCurrent, endingVoltage)` instead of zero-off.
- `68bd97d`: **Add gaussFunction, update zeroVoltage and zeroCurrent, remove sound comments** — Updated lab constants (`zeroVoltage = "-0.085"`, `zeroCurrent = "4.2"`); added inverse calibration helper (later replaced).
- `0a3c076`: **switch to voltage function, add new defaultCurrent variable** — Introduced `voltageFunction(gauss)` and `defaultCurrent` config variable.
- `44a87aa`: **Update New Magnet Code.cpp** — Timing overhaul: replaced 1000 ms sleeps with 16 ms; added `sleepCut(int percent)` and interval-aware delay capping.
- `857691f`: **Update New Magnet Code.cpp** — Migrated `magnetTo` sleeps from `countSleep` to `sleepCut(16)` for proportional timing.
- `b88c9aa`: **Create track mouse.cpp** — Added live mouse coordinate utility for GUI calibration.
- `9103b19`: **hihi** — Magnet Controller area commit (no `New Magnet Code.cpp` diff).
- `2174bc3`: **remove while** — Temporary control-flow experiment removing the F12 wait loop.
- `98a9619`: **Claude's fix** — Major refactor: F12 SET-button calibration, pixel-offset field targeting (`VSET_OFFSET_X`/`ISET_OFFSET_X`), `clickAt`/`SetCursorPos` replacing tab navigation in `magnetTo`, `GetAsyncKeyState` bitmask checks.
- `8f1578d`: **add while loop back, add break cmd** — Restored F12 wait loop with `break` after sequence completion.
- `373a9d4`: **Update New Magnet Code.cpp** — Added `dismissDialog()` with pre/post-SET dialog clicks (initial Y typo 3922).
- `0c998c1`: **add escape termination** — Injected `terminateIfEsc()` at every `magnetTo` step; renamed `track mouse.cpp` entry to `mains()`.
- `7588d93`: **up the delay** — Switched `magnetTo` timing to `sleepCut(12)` (12% of interval per step).
- `d76e565`: **fix** — Corrected dialog button Y coordinate from 3922 to 392.

## Resume bullet candidates

1. Built a Windows C++ console automation tool that drives a Kepco magnet power supply GUI via `SendInput` and mouse simulation, executing user-defined ternary on/off/state sequences with lab-safe timing (≥7.5 s) for physics research workflows.
2. Iterated a legacy tab-key prototype into F12-calibrated coordinate automation with offset-based field targeting, Gauss-to-voltage conversion, dialog dismissal, and escape-key safety checks across 18+ commits as sole contributor.

## Confidence

**0.94** — Strong, file-backed evidence: complete source in `New Magnet Code.cpp`, clear commit arc from prototype to hardened automation, sole authorship (Farhan Kittur / @Darksharkthe1st). Slightly below 0.95 because the coordinate refactor landed in a commit titled "Claude's fix" (AI-assisted) and the app has no tests or CI to corroborate runtime claims.

## Gaps

- **AI-assisted refactor:** Commit `98a9619` ("Claude's fix") introduced the F12 calibration and coordinate-based `magnetTo`; authorship is still @Darksharkthe1st, but the assist source should be disclosed if asked in interviews.
- **Not in build:** `track mouse.cpp` is a standalone dev utility (not in `.vcxproj`); `mains()` prevents accidental dual-`main` linkage but it is not shipped.
- **Hardcoded lab constants:** Dialog click position (690, 392) and field offsets are machine/GUI-layout specific; no config file or abstraction layer.
- **Stale UI copy:** Prompt still says "binary string (1's and 0's only)" though ternary `2` is supported since `6548d7c`.
- **No automated tests or CI:** Behavior verified manually in lab; no `.github/` workflows or test projects.
- **Legacy dead code:** Commented-out `Magnet Code.cpp` remains in the vcxproj compile list; `Magnet Code.cpp` and `New Magnet Code.cpp` both included though only one is active.

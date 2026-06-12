# Signal: Magnet calibration — binary search & iterative refinement

## Summary

@Darksharkthe1st (Farhan Kittur) built a Windows C++ magnet calibrator that automatically finds Kepco power-supply voltages to hit target magnetic field strengths (Gauss), using closed-loop feedback from a LabVIEW gaussmeter script via file-based IPC. The core is a two-stage numerical method: binary search over a ±30 V voltage bracket, then nested decimal-place sweeps (2nd and 3rd decimal digits) with early termination when within 1 Gauss — plus a separate zero-field calibration path that coarse-scans ±0.2 V before the same refinement loop. This is non-trivial algorithmic work applied to real hardware timing and safety constraints (3 s settle delays, ESC emergency stop).

## Technical depth

**Architecture:** Console C++ app (`Magnet Calibrator/Magnet Calibrator/Calibrating Magnets in C++.cpp`) orchestrates three external systems without direct APIs: (1) Kepco Control Panel via `SendInput`/`mouse_event` keyboard and mouse simulation (`changeVolts`, `clickAt`, `sendString`), (2) LabVIEW 2017 gaussmeter reader launched at startup, and (3) file IPC at `C:\Users\Mani Lab\Desktop\Calibrator Text File\calibrator text.txt` read by `runAndCollect()`.

**`calibrateTo(double Gauss)` — binary search phase:**
- Initializes search bounds `top = 30`, `bottom = -30`.
- Each iteration: sets voltage to midpoint `(top + bottom) / 2` via `changeVolts()`, waits 3000 ms for hardware settle, reads measured Gauss from `runAndCollect(filename)`.
- Narrows bracket: if `value < Gauss` then `bottom = midpoint`, else `top = midpoint`.
- Loop condition: `abs(value - Gauss) > 0.1 && abs(top - bottom) > 0.001` — converges to ~0.1 Gauss tolerance with 1 mV voltage resolution floor.

**`calibrateTo(double Gauss)` — iterative refinement phase:**
- If not within 1 Gauss after binary search, runs nested loops: `digits` from 2 to 3, `x` from -10 to 10.
- Probes `prevMin + pow(0.1, digits) * x` volts (±0.01 V then ±0.001 V around best candidate).
- Tracks `minDiff = abs(value - Gauss)` and `minimumVoltage`; early-returns when `abs(value - Gauss) < 1`.

**`calibToZero()` — zero-field calibration:**
- Coarse linear sweep: `x` from -2 to 2, testing `changeVolts(x * 0.1)` (±0.2 V around 0).
- Selects voltage minimizing `abs(value)` against target 0 Gauss.
- Reuses the same 2nd/3rd decimal nested sweep (`digits` 2→3, `x` -10→10) with early exit when `abs(value) < 1`.

**`main()` dispatch:** User enters target Gauss; `abs(setPoint) <= 1` routes to `calibToZero()`, otherwise `calibrateTo(setPoint)`.

**Hardware constraints encoded in algorithm:** `Sleep(1000)`/`Sleep(3000)` between probe steps; `terminateIfEsc()` polled throughout loops for emergency abort; `doubleToStr(4, inputVolts)` formats voltages for GUI entry.

**Scale:** ~516 lines in the production calibrator file; calibration logic spans ~215 lines across `calibToZero()` (lines 129–239) and `calibrateTo()` (lines 242–344). A related prototype with variant refinement logic exists in `Experimentation with Windows/.../Window Experimentation in C++.cpp` (shipped same initial commit, never updated).

## Evidence

### Files

- `Magnet Calibrator/Magnet Calibrator/Calibrating Magnets in C++.cpp` — Production calibrator implementing `calibrateTo()`, `calibToZero()`, `runAndCollect()`, and `changeVolts()`; contains explicit binary-search comment at line 275 and nested `pow(0.1, digits)` refinement loops in both calibration functions.
- `Magnet Calibrator/Magnet Calibrator/Magnet Calibrator.vcxproj` — MSVC v143 console project compiling the calibrator source (`ClCompile Include="Calibrating Magnets in C++.cpp"`).
- `Experimentation with Windows/Experimentation with Windows/Window Experimentation in C++.cpp` — Earlier prototype with the same binary-search `calibrateTo()` loop and decimal refinement, but a different `calibToZero()` using gradient-style `stopper` logic and fewer coarse-sweep steps; evidence of iterative algorithm design before the production calibrator.

### Commits

- `804e241`: **Full Code Push** — Introduces the entire `Calibrating Magnets in C++.cpp` (437 lines) with binary search in `calibrateTo()` (`if (value < Gauss) bottom = (top + bottom) / 2; else top = ...`) and nested `for (int digits = 2; digits <= 3; digits++)` / `for (int x = -10; x <= 10; x++)` refinement in both `calibrateTo()` and `calibToZero()`; establishes file-based gaussmeter feedback via `runAndCollect()`. Sole initial authorship of the calibration algorithms.
- `f660859`: **Improve comments and clean up unused code** — Documents the algorithm explicitly (adds `//Using binary search, go up if value too low, go down if value too high`; annotates coarse sweep, decimal-place refinement, and gaussmeter file reads); comments out hardcoded test calls to `calibToZero()`/`calibrateTo(3000)` in `main()`. No algorithm logic changes — documentation and cleanup only.
- `68bd97d`: **Add gaussFunction, update zeroVoltage and zeroCurrent, remove sound comments** — Updates calibrator constants `current` and `zeroCurrent` from `"8"`/`"4"` to `"4.2"`; tunes hardware parameters used by `changeVolts()` during calibration probes. Algorithm structure unchanged; `gaussFunction` added only in `Magnet Controller/New Magnet Code.cpp`, not the calibrator.

## Resume bullet candidates

1. Implemented closed-loop magnet calibration in C++ using binary search over ±30 V to converge on target Gauss values, then nested decimal-place voltage sweeps (0.01 V / 0.001 V) with feedback from a LabVIEW gaussmeter via file-based IPC and Windows GUI automation of a Kepco power supply.
2. Built a two-stage numerical calibration system (bracketing binary search + iterative voltage refinement) for a physics-lab electromagnet, including a dedicated zero-field calibration path with coarse ±0.2 V sweep, hardware settle delays, and emergency-stop hooks for safe unattended operation.

## Confidence

**0.88** — Binary search, nested decimal refinement, and zero-calibration loops are clearly implemented and fully attributable to @Darksharkthe1st (sole repo author; algorithms present in initial `804e241` commit). Slightly below 0.90 because algorithm logic shipped in a single monolithic push with no subsequent commits refining the search/refinement logic, and the explicit "binary search" comment was added later in a documentation-only commit.

## Gaps

- No commit history showing incremental development of the calibration algorithms — the full binary-search and refinement implementation appears complete in `804e241` with only documentation (`f660859`) and constant tuning (`68bd97d`) afterward.
- `68bd97d` commit message references `gaussFunction`, but that function lives in `Magnet Controller/New Magnet Code.cpp`, not the calibrator; it does not extend the calibrator's search algorithms.
- `0c998c1` (escape termination) and other listed controller commits do not touch the calibrator file; ESC handling was already present in `804e241`.
- No automated tests, benchmarks, or logged calibration accuracy metrics in the repository.
- Minor code issues (e.g., `abs(value - Gauss <= 1)` missing a closing parenthesis at line 292) suggest the refinement exit condition may not behave as intended, though the overall algorithm structure is clear.
- `zeroVoltage` / `zeroCurrent` string constants are declared but not wired into the search loops — the algorithms compute voltage dynamically rather than using those presets.

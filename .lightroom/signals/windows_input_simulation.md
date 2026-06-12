# Signal: Windows API input & window control

## Summary

@Darksharkthe1st (Farhan Kittur) built a reusable Win32 input-simulation layer in C++ console apps that automates a Kepco magnet power-supply GUI and coordinates LabVIEW/Kepco window layout for a physics lab. The work combines `SendInput` keyboard synthesis, `mouse_event`/`SetCursorPos` clicking, `GetAsyncKeyState` hotkey control, and `FindWindow`/`MoveWindow`/`ShowWindow` window management—patterns that map directly to desktop/systems automation and test-engineering interviews.

## Technical depth

**Architecture:** Three Visual Studio C++ projects share the same low-level automation primitives (not a formal library, but duplicated helper functions with identical signatures). The primary production path is `New Magnet Code.cpp`, which drives timed magnet on/off/ternary state sequences by programmatically filling VSET/ISET fields and clicking SET in the Kepco Control Panel.

**Input simulation pattern (repeated across files):**
- `clickKey(char)` — builds `INPUT` with `INPUT_KEYBOARD`, maps characters via `VkKeyScanA`, sends key-down/key-up pairs through `SendInput(1, &Input, sizeof(Input))`
- `keyDown` / `keyUp` / `keyTap` — modifier and special-key handling (e.g., `VK_CONTROL`, `VK_DELETE`, `VK_RETURN`, `VK_TAB`)
- `sendString(string)` — iterates characters with 10 ms delays to avoid UI overload
- `clickAt(POINT)` — `SetCursorPos(p.x, p.y)` followed by `mouse_event(MOUSEEVENTF_LEFTDOWN/LEFTUP)` for left-click synthesis
- `changeVolts(double)` (calibrator/experimentation) — full form-fill workflow: click field → Ctrl+A → Delete → type voltage → Tab → type current → Tab → Enter

**Hotkey / safety control:**
- `GetAsyncKeyState(VK_F12)` — records SET button position via `GetCursorPos(&setButtonPos)` at startup; later triggers automation run while hiding console with `ShowWindow(GetConsoleWindow(), SW_HIDE/SW_SHOW)`
- `GetAsyncKeyState(VK_ESCAPE)` — polled in `terminateIfEsc()` at every step of `magnetTo()` for emergency abort (`terminate()`)
- Additional bindings: `VK_NUMPAD0` (exit), `VK_NUMPAD2` (restart), `VK_ESCAPE & 0x8000` debounce mask on F12/hotkeys in the controller

**Window management (calibrator + experimentation):**
- `FindWindow(NULL, L"Kepco Control Panel")` to obtain Kepco HWND
- `MoveWindow(handle, 600, 0, 700, 450, false)` to reposition supply UI
- `ShowWindow` cycle (`SW_MINIMIZE` → `SW_RESTORE` → `SW_HIDE` → `SW_SHOW`) to force window refresh
- `SetWindowPos(GetConsoleWindow(), 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER)` to move console out of automation path during calibration

**Controller evolution (most attributable iterative work):**
- **Tab-key prototype** (`3c737a9`) — early `magnetTo()` used keyboard-only navigation (Ctrl+A, Tab, Enter) without mouse coordinates
- **Coordinate-based automation** (`98a9619`) — F12-calibrated `setButtonPos` with pixel offsets (`VSET_OFFSET_X = -93`, `ISET_OFFSET_X = 99`) to derive VSET/ISET/SET click targets; replaced brittle 14-tab navigation
- **Dialog dismissal** (`373a9d4`, fixed `d76e565`) — `dismissDialog()` clicks a fixed dialog OK button at (690, 392) before and after SET
- **Timing model** (`44a87aa`, `857691f`) — `sleepCut(int percent)` scales delays as a fraction of user-configured `interval` (≥7500 ms lab-safe minimum) while tracking cumulative `delay` for interval compliance
- **Safety hardening** (`0c998c1`) — `terminateIfEsc()` inserted at each step inside `magnetTo()`

**Scale:** ~270 lines in the controller, ~430–520 lines each in calibrator/experimentation prototypes; 18+ commits touching the controller alone. Sole contributor across all 23 repo commits (emails: `kitturfarhan@gmail.com`, `54246758+Darksharkthe1st@users.noreply.github.com`).

## Evidence

### Files

- `Magnet Controller/Magnet Controller/New Magnet Code.cpp` — Primary deliverable: F12 coordinate calibration (`GetCursorPos`), offset-derived field clicking (`clickAt`), `SendInput` keyboard helpers, `mouse_event` clicks, `GetAsyncKeyState` hotkey loop, console hide/show during runs, `dismissDialog()` for modal dialogs, interval-aware timing via `sleepCut`/`countSleep`
- `Magnet Calibrator/Magnet Calibrator/Calibrating Magnets in C++.cpp` — Full shared input stack plus `FindWindow`/`MoveWindow`/`ShowWindow`/`SetWindowPos` for Kepco and console layout; `changeVolts()` drives supply GUI during binary-search calibration loops; `getPosOnClick()` uses F12 + `GetCursorPos` for coordinate discovery
- `Experimentation with Windows/Experimentation with Windows/Window Experimentation in C++.cpp` — Earlier prototype with identical Win32 input/window patterns; adds on/off cycle loop calling `changeVolts()` with interval timing; unchanged since initial push
- `Magnet Controller/Magnet Controller/track mouse.cpp` — Debug utility polling `GetCursorPos` in a loop to discover click coordinates (supports coordinate-based automation development)
- `Magnet Controller/Magnet Controller/Magnet Code.cpp` — Legacy tab-key controller (fully commented out); shows superseded `SendInput`/`GetAsyncKeyState`/`ShowWindow` approach replaced by `New Magnet Code.cpp`

### Commits

- `804e241`: Full Code Push — Initial commit shipping `Calibrating Magnets in C++.cpp` and `Window Experimentation in C++.cpp` with complete `SendInput`, `mouse_event`, `FindWindow`, `MoveWindow`, `ShowWindow`, and `GetAsyncKeyState` implementations
- `3c737a9`: Working Binary Style — Creates `New Magnet Code.cpp` with `SendInput`-based `magnetTo()`, F12-triggered runs, `ShowWindow` console hide/show, and `GetAsyncKeyState` hotkeys (tab-key navigation prototype)
- `6548d7c`: Binary => Ternary — Extends hotkey-driven state machine to ternary encoding (`'0'`, `'1'`, `'2'`) while retaining input simulation core
- `44a87aa`: Update New Magnet Code.cpp — Adds `sleepCut(int percent)` and retunes inter-step delays from fixed 1000 ms to interval-proportional timing for faster, interval-compliant automation
- `857691f`: Update New Magnet Code.cpp — Replaces `countSleep(16)` with `sleepCut(16)` throughout `magnetTo()` so keyboard/mouse step delays scale with user interval
- `98a9619`: Claude's fix — Major refactor to coordinate-based automation: F12 records `setButtonPos` via `GetCursorPos`, introduces `clickAt(POINT)` with `SetCursorPos`+`mouse_event`, replaces tab-key navigation with offset-derived VSET/ISET/SET clicks; adds `0x8000` debounce on `GetAsyncKeyState`
- `373a9d4`: Update New Magnet Code.cpp — Adds `dismissDialog()` using `clickAt` at fixed coordinates to dismiss Kepco modal dialogs before/after SET
- `0c998c1`: add escape termination — Inserts `terminateIfEsc()` (Escape via `GetAsyncKeyState`) at every step of `magnetTo()` for mid-sequence emergency stop
- `d76e565`: fix — Corrects `dismissDialog()` Y coordinate from 3922 to 392, fixing dialog click target
- `b88c9aa`: Create track mouse.cpp — Adds `GetCursorPos` polling tool for mouse coordinate debugging
- `f660859`: Improve comments and clean up unused code — Documents calibrator input/window functions (comments only; underlying Win32 calls unchanged)

## Resume bullet candidates

1. Built C++ Win32 desktop automation for a research magnet power supply, synthesizing keyboard input via `SendInput`, mouse clicks via `SetCursorPos`/`mouse_event`, and hotkey control via `GetAsyncKeyState` to execute timed field-switch sequences without manual GUI interaction.
2. Engineered reusable Windows API input and window-management patterns (`FindWindow`, `MoveWindow`, F12 coordinate calibration, Escape abort) across console tools that automated Kepco supply control and LabVIEW-adjacent lab workflows.

## Confidence

0.90 — Strong, file-backed evidence across three projects with a clear commit trail on the primary controller; all 23 commits authored by @Darksharkthe1st. Slightly below 0.95 because coordinate/dialog logic relies on hardcoded pixels, one commit (`98a9619`) is externally attributed in message, and the experimentation prototype has no follow-up commits.

## Gaps

- `Window Experimentation in C++.cpp` has no commits after `804e241`; it is a shipped prototype, not iteratively maintained.
- `98a9619` ("Claude's fix") message suggests partial external assistance on the coordinate refactor—file authorship is still @Darksharkthe1st, but the commit message weakens sole-design attribution for that specific change.
- Mouse clicking uses legacy `mouse_event` rather than `SendInput` with `INPUT_MOUSE`; keyboard path is modern but mouse path is older Win32 API.
- Automation depends on hardcoded window title (`"Kepco Control Panel"`), fixed pixel coordinates, and screen layout—brittle, not DPI-aware or multi-monitor safe.
- `track mouse.cpp` defines `int mains()` instead of `main()`; it is a debug aid, not a polished utility.
- No automated tests, CI, or abstraction layer—the helpers are copy-pasted across files rather than packaged as a library.

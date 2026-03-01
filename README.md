# GuitarFX (JUCE VST3)

## Release Status

This project is designated as **Version 1.2 (V1.2)**.

Reference label for future work:

- `GuitarFX V1`

GuitarFX is a Windows-focused VST3 guitar multi-effects plugin built with JUCE and CMake for use in hosts like Ableton Live 12.

## What It Does

GuitarFX processes stereo audio through this order:

1. Input Gain
2. Noise Gate
3. Compressor
4. Drive
5. Amp Sim
6. Cab Sim
7. Delay
8. Reverb
9. Output Gain
10. Thirty One Band Equalizer (post-effects)

Each effect block (except Input/Output) has a bypass switch, additional pedal-style controls, and dedicated input/output meters.
The equalizer is fed from the effects chain output and includes a real-time waveform display.

## Checkbox Behavior (Important)

Bypass checkboxes are **bypass toggles**:

- Unchecked: effect is active (enabled)
- Checked: effect is bypassed (disabled)

So checking `Delay` disables delay processing.

## DSP Overview

- Input/Output Gain:
  - `juce::dsp::Gain<float>` with short gain ramps for smoother automation.
- Noise Gate:
  - Envelope follower + threshold + open/close smoothing.
- Compressor:
  - `juce::dsp::Compressor<float>` with threshold/ratio/attack/release controls.
- Drive:
  - Soft clipping using `tanh()` waveshaping.
- Amp Sim:
  - Additional nonlinear saturation plus low-pass tone shaping.
- Cab Sim:
  - High-pass + low-pass filtering to approximate cabinet band-limiting.
- Delay:
  - Stereo delay lines with feedback and wet/dry mix.
- Reverb:
  - `juce::dsp::Reverb` with room/damping/wet controls.
- Per-section metering:
  - Every stage publishes input and output RMS levels converted to dBFS with smoothing.
  - UI displays IN/OUT meters for: Input, Gate, Comp, Drive, Amp, Cab, Delay, Reverb, Output.

## Main Parameters Exposed

- Input:
  - Gain (dB), High Pass (Hz)
- Gate:
  - Threshold (dB), Release (ms), Range (dB), Bypass
- Compressor:
  - Threshold (dB), Ratio, Attack (ms), Release (ms), Mix, Makeup (dB), Bypass
- Drive:
  - Drive, Tone (Hz), Level (dB), Blend, Bypass
- Amp:
  - Gain, Tone (Hz), Presence, Level (dB), Bypass
- Cab:
  - High Pass (Hz), Low Pass (Hz), Resonance, Bypass
- Delay:
  - Time (ms), Feedback, Mix, Tone (Hz), Bypass
- Reverb:
  - Room, Damp, Wet, Width, Bypass
- Output:
  - Gain (dB)

## UI Summary (V1.2)

- Professional themed custom UI with grouped section cards.
- Additional controls per section (pedal-style layout).
- Clear active vs bypassed visual state for each effect block.
- Procedurally generated custom background texture.
- Live IN/OUT meters for every stage in the chain.
- Bottom equalizer section with:
  - Real-time waveform graph (equalizer input signal).
  - Thirty one vertical slider bands.
  - Equalizer bypass and info button.

## Build Requirements

- Windows 11
- Visual Studio 2022 Build Tools (C++ workload)
- CMake
- JUCE checked out locally (example: `D:\dev\JUCE`)

## Configure and Build

From project root (`D:\dev\GuitarFX`):

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR=D:\dev\JUCE
cmake --build build --config Release
```

Built plugin path:

`D:\dev\GuitarFX\build\GuitarFX_artefacts\Release\VST3\GuitarFX.vst3`

## Install to VST3 Folder

Copy the built plugin bundle to:

`C:\Program Files\Common Files\VST3\`

Example (Admin PowerShell):

```powershell
Copy-Item "D:\dev\GuitarFX\build\GuitarFX_artefacts\Release\VST3\GuitarFX.vst3" "C:\Program Files\Common Files\VST3\" -Recurse -Force
```

Then rescan plugins in Ableton Live 12.

## State and Automation

- Plugin settings are saved/restored via `AudioProcessorValueTreeState`.
- Parameters are host-visible and automatable.

## Current Limitations (V1.2)

- No oversampling in nonlinear stages (Drive/Amp).
- Cab simulation is filter-based (not IR convolution yet).
- Delay is free-time only (no host tempo sync yet).
- No preset browser/manager yet.

## Project Structure

- `CMakeLists.txt` - JUCE plugin target and build settings
- `source/PluginProcessor.h/.cpp` - DSP chain, parameters, state
- `source/PluginEditor.h/.cpp` - plugin UI and APVTS attachments

## Recommended Next Enhancements

1. Add oversampling around Drive/Amp stages.
2. Add IR cab loader (`juce::dsp::Convolution`).
3. Add tempo-synced delay note divisions.
4. Add preset browser and patch management workflow.
5. Add optional simplified/compact UI mode.

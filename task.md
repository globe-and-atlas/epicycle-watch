---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-08T10:19:54-06:00"
---
# Build Epicycle

Purpose: an analog clock made from one linked chain, with readable numbers at its moving joints. Approved concept: center→hour→minute→seconds; each hand keeps its ordinary absolute clock angle.

Targets: Time2/Emery200×228 and Round2/Gabbro260×260. New standalone project, no changes to published faces. User confirmed numbered labels beside the hands/joints like Origin/Vector.

## Contract
- Directives: directives/build_watchface.md.
- Execution: execution/build.py, execution/verify.py, execution/capture_native.py.
- Artifacts: dist/Epicycle-0.1.0.pbw and native previews under media/.
- Store publication authorized below. No Git commit or push requested. No location, network service, Health, or plugin feature needed.

## Validation
- [x] Emery binary builds.
- [x] Gabbro binary builds.
- [x] The hour pivot is the screen midpoint.
- [x] The minute pivot equals the hour tip.
- [x] The second pivot equals the minute tip.
- [x] Every hand angle remains relative to screen-up.
- [x] Six named palettes are selectable.
- [x] Custom background color is selectable.
- [x] Custom hour color is selectable.
- [x] Custom minute color is selectable.
- [x] Custom second color is selectable.
- [x] Forced12h labels use1–12.
- [x] Forced24h labels use0–23.
- [x] Watch-default time format follows the watch preference.
- [x] All three joint labels are visible when enabled.
- [x] Joint labels can be disabled.
- [x] Joint labels do not intersect hands.
- [x] Joint labels do not intersect other labels.
- [x] Joint labels stay inside the screen.
- [x] A digital readout can be toggled.
- [x] Date can be Off, ISO, US, or European.
- [x] Settings survive restart.
- [x] Settings Save updates the native face.
- [x] Settings Cancel preserves saved values.
- [x] Geometry and placement tests pass.
- [x] Real Clay integration tests pass.
- [x] Native Emery previews are inspected.
- [x] Native Gabbro previews are inspected.
- [x] Production bundle excludes fixture controls.
- [x] Fresh independent review passes.

## State
Scaffold created with the canonical project-template script, using --skip-git --skip-venv --local-tmp. Scaffold health check failed before implementation; actual project checks replace template checks. No secrets are needed.

Completed: clean production build, host tests, both native capture matrices, and independent review pass. Initial implementation did not publish; release authorized below.

## Store release — 2026-11-08
User authorized public Epicycle publication with animated motion/color preview first and both screen shapes.
- [x] Emery preview GIF contains moving native hands.
- [x] Gabbro preview GIF contains moving native hands.
- [x] Six palettes appear in each GIF.
- [x] Time 2 still screenshots are uploaded.
- [x] Round 2 still screenshots are uploaded.
- [x] Public listing places a GIF first for Emery.
- [x] Public listing places a GIF first for Gabbro.
- [x] Public PBW matches the reviewed production artifact.
- [x] Independent release review passes.

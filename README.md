---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-08T12:00:00-06:00"
---
# Epicycle

A native Pebble watchface with a single linked chain: hour from the center, minute from the hour tip, seconds from the minute tip. Each segment keeps its normal absolute clock angle.

Supports Pebble Time 2 (Emery, 200×228) and Round 2 (Gabbro, 260×260). Settings offer six palettes, custom background/hand colors, watch-default/12/24-hour numbers, optional joint labels, optional digital time, and optional date formats. Analog hour geometry stays on a conventional 12-hour cycle.

Build: `python3 execution/build.py`. Host checks: `python3 execution/build.py --host-only`. Bundle verification: `python3 execution/verify.py`. Install `dist/Epicycle-0.1.0.pbw` using the Pebble app or SDK.

Native capture fixtures are separate from production. Run `execution/build_fixture.py`, then `execution/capture_native.py --platform emery` or `--platform gabbro`. Emulator resets require exclusive ownership; the capture script backs up emulator flash first. Rebuild production after fixtures. Temporary reports remain in `.tmp/`; actual emulator screenshots are in `media/`.

Published: [Epicycle on the Pebble app store](https://apps.repebble.com/d5c85c5d05cd4cbab4717d60). Physical-watch behavior and battery life remain unmeasured.

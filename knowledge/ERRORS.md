# Errors

Record deterministic errors, root causes, and fixes here.

## 2026-11-08 — Successful install with stale foreground
The first fixture installs reported success but the screen showed an old Hi-Fi not-responding modal. The Epicycle foreground marker was absent, so no capture was accepted. Back up SDKflash and use the owned emulator reset/wipe workflow before reinstalling. This is an emulator-state issue, not proof of an Epicycle crash.

## 2026-11-08 — SDK global writes under workspace sandbox
SDK clean/build writes settings.json and pending_analytics.json outside the project. Default sandbox execution fails before compilation. Run the authorized SDKbuild/nativecapture commands with automatic permission review; host tests and source edits need no escalation.

## 2026-11-08 — create response omits screenshotResults
The new-app POST succeeded without screenshotResults. A verifier that assumes this release-update response field fails after publication. Fix: collect both platform gallery image URLs from the public UI, compare ten assets, and retain the proof. No repeat upload needed. Graduated to procedural/publish_store.md.

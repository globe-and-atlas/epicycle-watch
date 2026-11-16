---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-08T12:00:00-06:00"
---
# Native verification

Run host tests before SDK builds. Clean production builds remove fixture environment variables. `execution/verify.py` compares both packaged binaries to build outputs and checks identity, settings keys, targets, and fixture symbol exclusion.

Fixture captures use a magenta identity marker and green valid-placement marker before saving clean images. Capture checks each palette, custom settings, label modes, crowded poses, date modes, Clay Save, restart persistence, and invalid setting rejection. A stale foreground app must fail the marker check; double installation alone is insufficient. Back up flash before an owned-emulator reset.

Do not run an SDK build concurrently with emulator capture. SDK analytics/settings writes require sandbox escalation on this machine.

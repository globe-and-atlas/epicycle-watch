---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-08T12:00:00-06:00"
---
# Decisions

- Use native Pebble C for rendering; share a Pebble-independent integer model with host tests.
- Keep absolute clock headings for each segment. 24-hour mode changes the numeric value, not the analog cycle.
- Place color-matched numbers beside the respective joints. Avoid hands, pivots, other numbers, and the circular boundary. Retain valid relative offsets to reduce movement; use a compact joint assignment for crowded poses.
- Use one-second ticks, without a continuous animation timer.
- Pin Clay 1.1.0 for settings. Python capture tooling uses Pillow 12.3.0. No external runtime service is required.
- Exclude fixture controls from production and verify their absence in both ELF symbol tables.

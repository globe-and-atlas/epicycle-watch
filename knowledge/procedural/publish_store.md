---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-08T11:20:00-06:00"
---
# First publication

Build native media using execution/build_store_media.py separately for Emery and Gabbro after build_fixture.py. Native GIFs use 36 actual rendered frames (2-second clock increments shown every500ms), advancing70seconds in18seconds across six palettes. Preserve platform prefix and ordering; GIF is first.

Rebuild clean production afterwards, run verify.py, then use the installed Pebble tool Python3 to run execution/publish_store.py validate/publish/verify. Publishing uploads the exact verified dist PBW; it does not build. Authentication uses the existing SDK account. A retained inflight guard prevents retrying an ambiguous write.

The first-app creation response can omit screenshotResults despite successful gallery uploads. Verify through public UI device selection: collect image src URLs in platform order, then compare each public GIF byte-for-byte and each still pixel-for-pixel. The public PBW must hash-match production. Public proof and gallery URL evidence are retained under release/. Do not repeat a release to repair a verification-only error.

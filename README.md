# akz26

z26 Atari 2600 emulator, tweaked to run with my Emuhost framework.

Original source was acquired here: https://www.whimsey.com/z26/

...first try, splitting up for a piecemeal build. disaster.
There are some very tricky bits here, like static members shared across C files, and arrays where we need to determine the length.

Go back to building it all in one pass, work from there.
- OK it's working, and it's exactly as written except we link in a separate pass.
- [ ] Eliminate z26 main().
- [ ] Replace SDL with Emuhost.
- [ ] Remove unused files.
- [ ] linux_startup_audio_kludge, what's this all about?

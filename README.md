# akz26

z26 Atari 2600 emulator, tweaked to run with my Emuhost framework.

Original source was acquired here: https://www.whimsey.com/z26/

...first try, splitting up for a piecemeal build. disaster.
There are some very tricky bits here, like static members shared across C files, and arrays where we need to determine the length.

Go back to building it all in one pass, work from there.
- OK it's working, and it's exactly as written except we link in a separate pass.
- [x] Eliminate z26 main().
- [x] Replace SDL with Emuhost.
- [x] Remove unused files.
- [x] linux_startup_audio_kludge, what's this all about?
- [x] Where's the audio?
- [x] c_soundq.c resamples from 31400 to 43960. We can eliminate this and output at the natural 31400.
- [x] Right-player input
- [x] Console switches (difficulty, color)
- [ ] Audio is pretty bad. Look at c_soundq.c:QueueSoundBytes(), I bet we're coming up a little short.

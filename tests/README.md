# Scan stability regression

Build `scanboundary_test.cpp` as a standalone C++17 console program, then run it.
For MSVC (Developer Command Prompt):

```
cl /EHsc /std:c++17 tests\scanboundary_test.cpp /Fe:scanboundary_test.exe
scanboundary_test.exe
```

The test covers startup midway through a scan, complete revolutions, wraparound,
out-of-order blocks on either side of the wrap, invalid angles, packet gaps and
reset. A scan with a gap larger than 20 degrees is not published. This policy
assumes full 360-degree scanning, matching the receiver's original behavior.

## Live verification

Build the Qt application with the Qt Concurrent module available. Connect the
Fairy sensor and compare detection enabled/disabled with the same scene:

1. Point-cloud density should remain consistent across revolutions. A heavily
   incomplete revolution should leave the last complete scan visible.
2. Rotate/zoom while detection is active; clustering should not block interaction.
3. Toggle detection rapidly and change its parameters during processing. Old
   worker results must not reintroduce boxes after disabling or clearing.
4. Stop and restart reception. Startup partial scans must not be displayed.

Only one detection job runs at a time. Frames arriving during processing replace
the pending input instead of creating an unbounded processing queue. Boxes may
lag the newest cloud by one processing interval. No sensor recording is included
in the repository, so actual packet-loss rate, frame rate and flicker reduction
require live verification.

#include "../scanboundary.h"
#include <cassert>
#include <limits>
#include <iostream>

int main() {
    ScanBoundary scan;
    // Starting midway through a scan must not replace a complete display frame.
    for (int az = 180; az < 360; ++az) assert(scan.observe(float(az)).accept);
    auto result = scan.observe(0);
    assert(result.boundary && !result.complete);
    for (int az = 1; az < 360; ++az) assert(scan.observe(float(az)).accept);
    result = scan.observe(0);
    assert(result.boundary && result.complete);
    assert(scan.observe(1).accept);
    assert(!scan.observe(359).accept); // delayed previous-scan block
    assert(!scan.observe(0).accept);
    scan.reset();
    scan.observe(0); scan.observe(10);
    assert(!scan.observe(9).accept);
    assert(!scan.observe(-1).accept);
    assert(!scan.observe(360).accept);
    assert(!scan.observe(std::numeric_limits<float>::quiet_NaN()).accept);
    for (int az = 11; az < 360; ++az) scan.observe(float(az));
    assert(scan.observe(0).complete);
    scan.reset(); scan.observe(0); scan.observe(100);
    for (int az = 101; az < 360; ++az) scan.observe(float(az));
    assert(!scan.observe(0).complete);
    std::cout << "scan boundary regression tests passed\n";
}

#ifndef SCANBOUNDARY_H
#define SCANBOUNDARY_H

#include <cmath>

// A backwards/reordered block must never masquerade as a full revolution.
class ScanBoundary {
public:
    struct Result { bool accept; bool boundary; bool complete; };
    Result observe(float azimuth) {
        if (!std::isfinite(azimuth) || azimuth < 0.f || azimuth >= 360.f)
            return {false, false, false};
        if (!started) {
            started = true; last = azimuth;
            return {true, false, false};
        }
        const bool wraps = last > 300.f && azimuth < 60.f;
        const float delta = azimuth - last + (wraps ? 360.f : 0.f);
        if (delta < 0.f || delta > 180.f) return {false, false, false};
        if (wraps) {
            const bool complete = coverage + delta >= 350.f && !largeGap && delta <= 20.f;
            coverage = 0.f; largeGap = false; last = azimuth;
            return {true, true, complete};
        }
        coverage += delta;
        largeGap = largeGap || delta > 20.f;
        last = azimuth;
        return {true, false, false};
    }
    void reset() { started = false; coverage = 0.f; largeGap = false; last = 0.f; }
private:
    bool started = false;
    bool largeGap = false;
    float last = 0.f;
    float coverage = 0.f;
};
#endif

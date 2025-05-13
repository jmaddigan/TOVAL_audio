#ifndef CONVERSIONFN_H
#define CONVERSIONFN_H

#include <cmath>  // For pow and log10

// Convert linear amplitude to decibels (dB)
inline float linearToDB(float linearValue) {
    return (linearValue > 0.0f) ? 20.0f * log10(linearValue) : -INFINITY;
}

// Convert decibels (dB) to linear amplitude
inline float dbToLinear(float dbValue) {
    return pow(10.0f, dbValue / 20.0f);
}

inline float stepResponse(float target, float current, float alpha) {
    return (1 - alpha) * target + alpha * current;
}

#endif // CONVERSIONFN_H
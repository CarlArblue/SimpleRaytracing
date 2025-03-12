//
// Created by alex on 3/12/25.
//

// SpectralData.cpp
#include "SpectralData.h"
#include <algorithm>

Spectrum::Spectrum() {
    samples.fill(0.0f);
}

Spectrum::Spectrum(float value) {
    samples.fill(value);
}

// Approximate RGB to spectrum conversion
// Note: A more accurate implementation would use pre-computed color matching functions
Spectrum Spectrum::fromRGB(const glm::vec3& rgb) {
    Spectrum result;

    // Very simplified conversion - in a real renderer you would use
    // more sophisticated methods based on color science
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        float wavelength = MIN_WAVELENGTH + (MAX_WAVELENGTH - MIN_WAVELENGTH) *
                          (static_cast<float>(i) / (SPECTRAL_SAMPLES - 1));

        // Simple RGB to spectrum mapping
        if (wavelength < 490.0f) {
            // Blue-dominant region
            result.samples[i] = 0.2f * rgb.r + 0.3f * rgb.g + 0.5f * rgb.b;
        } else if (wavelength < 580.0f) {
            // Green-dominant region
            result.samples[i] = 0.2f * rgb.r + 0.6f * rgb.g + 0.2f * rgb.b;
        } else {
            // Red-dominant region
            result.samples[i] = 0.6f * rgb.r + 0.3f * rgb.g + 0.1f * rgb.b;
        }
    }

    return result;
}

// Convert spectrum to RGB for display
glm::vec3 Spectrum::toRGB() const {
    // This is a simplified spectral to RGB conversion
    // A real implementation would use color matching functions and
    // proper integration over the spectral curve

    float r = 0.0f, g = 0.0f, b = 0.0f;
    float totalWeight = 0.0f;

    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        float wavelength = MIN_WAVELENGTH + (MAX_WAVELENGTH - MIN_WAVELENGTH) *
                          (static_cast<float>(i) / (SPECTRAL_SAMPLES - 1));
        float value = samples[i];

        // Simple spectral response functions
        if (wavelength >= 380 && wavelength < 490) {
            b += value;
        }
        if (wavelength >= 450 && wavelength < 620) {
            g += value;
        }
        if (wavelength >= 580 && wavelength <= 780) {
            r += value;
        }

        totalWeight += 1.0f;
    }

    // Normalize
    float scale = SPECTRAL_SAMPLES / totalWeight;
    return glm::vec3(r * scale, g * scale, b * scale);
}

Spectrum Spectrum::operator+(const Spectrum& other) const {
    Spectrum result;
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        result.samples[i] = samples[i] + other.samples[i];
    }
    return result;
}

Spectrum Spectrum::operator*(const Spectrum& other) const {
    Spectrum result;
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        result.samples[i] = samples[i] * other.samples[i];
    }
    return result;
}

Spectrum Spectrum::operator*(float scalar) const {
    Spectrum result;
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        result.samples[i] = samples[i] * scalar;
    }
    return result;
}

Spectrum& Spectrum::operator+=(const Spectrum& other) {
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        samples[i] += other.samples[i];
    }
    return *this;
}

Spectrum& Spectrum::operator*=(float scalar) {
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        samples[i] *= scalar;
    }
    return *this;
}

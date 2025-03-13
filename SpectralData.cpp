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
Spectrum Spectrum::fromRGB(const glm::vec3& rgb) {
    Spectrum result;

    // More accurate RGB to spectral conversion
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        float wavelength = MIN_WAVELENGTH + (MAX_WAVELENGTH - MIN_WAVELENGTH) *
                          (static_cast<float>(i) / (SPECTRAL_SAMPLES - 1));

        // Initialize sample to zero
        result.samples[i] = 0.0f;

        // Red component contribution (peaks around 650nm)
        if (rgb.r > 0.0f) {
            float redResponse = std::max(0.0f, 1.0f - std::abs((wavelength - 650.0f) / 100.0f));
            result.samples[i] += rgb.r * redResponse;
        }

        // Green component contribution (peaks around 550nm)
        if (rgb.g > 0.0f) {
            float greenResponse = std::max(0.0f, 1.0f - std::abs((wavelength - 550.0f) / 80.0f));
            result.samples[i] += rgb.g * greenResponse;
        }

        // Blue component contribution (peaks around 450nm)
        if (rgb.b > 0.0f) {
            float blueResponse = std::max(0.0f, 1.0f - std::abs((wavelength - 450.0f) / 80.0f));
            result.samples[i] += rgb.b * blueResponse;
        }

        // Normalize to prevent over-bright samples
        result.samples[i] = std::min(1.0f, result.samples[i]);
    }

    return result;
}

// Convert spectrum to RGB for display
glm::vec3 Spectrum::toRGB() const {
    // More accurate conversion using proper color matching functions
    // These are simplified versions of the CIE standard observer functions
    float r = 0.0f, g = 0.0f, b = 0.0f;

    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        float wavelength = MIN_WAVELENGTH + (MAX_WAVELENGTH - MIN_WAVELENGTH) *
                         (static_cast<float>(i) / (SPECTRAL_SAMPLES - 1));
        float value = samples[i];

        // Better approximation of RGB response curves
        // R curve peaks around 650nm
        if (wavelength >= 580.0f)
            r += value * (1.0f - std::abs((wavelength - 650.0f) / 75.0f));

        // G curve peaks around 550nm
        if (wavelength >= 490.0f && wavelength <= 620.0f)
            g += value * (1.0f - std::abs((wavelength - 550.0f) / 70.0f));

        // B curve peaks around 450nm
        if (wavelength <= 490.0f)
            b += value * (1.0f - std::abs((wavelength - 450.0f) / 70.0f));
    }

    // Normalize
    return glm::vec3(
        std::min(1.0f, r * 3.0f / SPECTRAL_SAMPLES),
        std::min(1.0f, g * 3.0f / SPECTRAL_SAMPLES),
        std::min(1.0f, b * 3.0f / SPECTRAL_SAMPLES)
    );
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

Spectrum Spectrum::operator/(float scalar) const {
    Spectrum result;
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        result.samples[i] = samples[i] / scalar;
    }
    return result;
}

Spectrum& Spectrum::operator/=(float scalar) {
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        samples[i] /= scalar;
    }
    return *this;
}

bool Spectrum::operator>(float scalar) const {
    for (int i = 0; i < SPECTRAL_SAMPLES; i++) {
        if (samples[i] > scalar)
            return true;
    }
    return false;
}

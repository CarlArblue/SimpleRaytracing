//
// Created by alex on 3/12/25.
//

#ifndef SPECTRALDATA_H
#define SPECTRALDATA_H

#include <array>
#include <vector>
#include <glm/glm.hpp>

// Number of wavelength samples
constexpr int SPECTRAL_SAMPLES = 32;

// Wavelength range (visible spectrum: ~380-780nm)
constexpr float MIN_WAVELENGTH = 380.0f;
constexpr float MAX_WAVELENGTH = 780.0f;

// Spectrum class for wavelength-based color representation
class Spectrum {
public:
    std::array<float, SPECTRAL_SAMPLES> samples;

    Spectrum();
    explicit Spectrum(float value); // Uniform spectrum

    // Convert RGB to spectrum (approximate)
    static Spectrum fromRGB(const glm::vec3& rgb);

    // Convert spectrum to RGB for display
    glm::vec3 toRGB() const;

    // Spectrum operations
    Spectrum operator+(const Spectrum& other) const;
    Spectrum operator*(const Spectrum& other) const;
    Spectrum operator*(float scalar) const;
    Spectrum& operator+=(const Spectrum& other);
    Spectrum& operator*=(float scalar);
    Spectrum operator/(float scalar) const;
    Spectrum& operator/=(float scalar);
};

#endif // SPECTRALDATA_H
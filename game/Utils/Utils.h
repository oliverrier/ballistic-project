#pragma once
#include "tools/PerlinNoise.h"

template<typename T>
T MapValue(T value, T fromLow, T fromHigh, T toLow, T toHigh) {
    // First, calculate the range of the input value.
    T range1 = fromHigh - fromLow;

    // Calculate the corresponding range of the output value.
    T range2 = toHigh - toLow;

    // Calculate the value as a fraction of the input range.
    T fraction = (value - fromLow) / range1;

    // Multiply the fraction by the output range and add the lower output value.
    T result = (fraction * range2) + toLow;

    // Return the mapped value.
    return result;
}

constexpr float frequency = 150.f;
constexpr int octaves = 4;
constexpr float fx = (frequency / static_cast<float>(800));
constexpr float min = -50.f;
constexpr float max = 100.f;
constexpr float n = 10;


inline void GenerateFloorVertex(Vec2 start, Vec2 end, int number_of_points, std::vector<Vec2>& vertices)
{
    vertices.reserve(number_of_points + 2);
    vertices.emplace_back(0.f, 0.f);
    vertices.emplace_back(end.x, 0.f);
    vertices.emplace_back(end.x, -end.y);

    srand((unsigned)time(NULL));
    int my_rand = rand();

    const siv::PerlinNoise perlin{ static_cast<siv::BasicPerlinNoise<double>::seed_type>(my_rand) };

    for (int i = number_of_points - 2; i >= 1; --i)
    {
        const float value = perlin.octave1D(fx * static_cast<float>(i), octaves);
        float y = MapValue(value, -1.f, 1.f, min, max);

        vertices.emplace_back(end.x / number_of_points * i, -start.y - y);
    }

    vertices.emplace_back(0.f, -start.y);

}

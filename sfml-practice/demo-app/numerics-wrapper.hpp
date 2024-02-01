#pragma once

#include <limits>
#include <numbers>
#include <random>

#include <SFML/Config.hpp>

using U8 = sf::Uint8;
using U16 = sf::Uint16;
using U32 = sf::Uint32;
using U64 = sf::Uint64;

using I8 = sf::Int8;
using I16 = sf::Int16;
using I32 = sf::Int32;
using I64 = sf::Int64;

using F32 = float;
using F64 = double;

namespace Numbers {
  namespace {
    // Create a random device for seeding the generator
    static std::random_device rand_dev{};
    // Initialize the Mersenne Twister engine with the random device
    static std::mt19937 engine(rand_dev());

    template<class T>
      concept Number = std::is_integral<T>::value || std::is_floating_point<T>::value;
  }

  constexpr F32 PI = std::numbers::pi_v<F32>;

  template<Number T>
    T randNum(const T min = std::numeric_limits<T>::min(), const T max = std::numeric_limits<T>::max()) {
      // Define the value min/max range for the uniform numeric distribution and apply engine
      return std::uniform_int_distribution<T>(min,max)(engine);
    }
}

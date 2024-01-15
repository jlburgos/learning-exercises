#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <list>
#include <numbers>
#include <random>
#include <ranges>

#include <SFML/Graphics.hpp>

using U8 = sf::Uint8;
using U16 = sf::Uint16;
using U32 = sf::Uint32;
using U64 = sf::Uint32;

using I8 = sf::Int8;
using I16 = sf::Int16;
using I32 = sf::Int32;
using I64 = sf::Int64;

using F32 = float;
using F64 = double;

const sf::Vector2f transform(const F32 periodMS, const F32 timeMS) {
    const F32 ellipse_width = 400.f;
    const F32 ellipse_height = 400.f;
    const F32 a = ellipse_width / 2.f;
    const F32 b = ellipse_height / 2.f;
    const F32 tau = 2.f * std::numbers::pi_v<F32>;
    const F32 period_ms = periodMS;
    const F32 x = (std::fmodf(timeMS, period_ms) / period_ms) * tau;
    return sf::Vector2f(a * std::cos(x), b * std::sin(x));
}

namespace RandomNumbers {
  namespace {
    // Create a random device for seeding the generator
    static std::random_device rand_dev;
    // Initialize the Mersenne Twister engine with the random device
    static std::mt19937 engine(rand_dev());

    template<class T>
      concept Integral = std::is_integral<T>::value;
  }

  template<Integral T>
    T randNum(const T min = std::numeric_limits<T>::min(), const T max = std::numeric_limits<T>::max()) {
      // Define the value min/max range for the uniform integer distribution and apply engine
      return std::uniform_int_distribution<T>(min,max)(engine);
    }
}

void handleEvents(sf::RenderWindow& window) {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Escape) {
        window.close();
      }
    }
    if (event.type == sf::Event::Closed) {
      window.close();
    }
  }
}

I32 main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(1000, 1000), "SFML Works!", sf::Style::Default, settings);
  window.setFramerateLimit(60);

  sf::CircleShape shape(100.f);
  sf::CircleShape centeredCircle(100.f);

  std::list<sf::CircleShape> afterImages;
  U32 maxAfterImages = 100;

  // Clocks
  sf::Clock clock;
  sf::Clock clockColor;

  while (window.isOpen()) {
    // Handle events
    handleEvents(window);

    const F32 periodMS = 3000.0f;

    // Set position according to time passed
    const sf::Vector2i posMouse = sf::Mouse::getPosition(window);
    const sf::Vector2f posCenter = sf::Vector2f(posMouse.x - shape.getRadius(), posMouse.y - shape.getRadius());
    //const sf::Vector2f posChange = transform(static_cast<F32>(clock.getElapsedTime().asMilliseconds()));
    //shape.setPosition(posCenter + posChange);
    shape.setPosition(posCenter);

    // Change color at a configurable rate
    U32 elapsedMS = static_cast<U32>(clockColor.getElapsedTime().asMilliseconds());
    if (elapsedMS > (1000 / maxAfterImages)) {
      clockColor.restart();
      //std::cout << "Num afterImages: " << afterImages.size() << std::endl;
      // Generate a random number within the specified range
      const U8 r = RandomNumbers::randNum<U8>();
      const U8 g = RandomNumbers::randNum<U8>();
      const U8 b = RandomNumbers::randNum<U8>();
      shape.setFillColor(sf::Color(r,g,b,255));

      // Manage after images
      if (afterImages.size() > maxAfterImages) {
        afterImages.pop_back();
      }
      afterImages.push_front(shape);
      for (auto& afterImage : afterImages) {
        sf::Color color = afterImage.getFillColor();
        color.a = static_cast<U8>(color.a - (255 / maxAfterImages));
        afterImage.setFillColor(color);
        afterImage.setRadius(afterImage.getRadius() * 1.005f);
      }
    }

    elapsedMS = static_cast<U32>(clock.getElapsedTime().asMilliseconds());
    if (elapsedMS > periodMS) {
      clock.restart();
    }

    // Centered circle
    centeredCircle.setPosition(posCenter);
    centeredCircle.setFillColor(sf::Color::Green);

    // Redraw things
    window.clear();
    std::ranges::for_each(afterImages.rbegin(), afterImages.rend(), [&window](const sf::CircleShape& afterImage) { window.draw(afterImage); });
    window.draw(shape);
    window.draw(centeredCircle);
    window.display();
  }

  return EXIT_SUCCESS;
}

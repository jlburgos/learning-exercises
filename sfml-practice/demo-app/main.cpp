#include <iostream>
#include <cstdlib>
#include <cmath>

#include <list>
#include <random>
#include <ranges>

#include <SFML/Graphics.hpp>

const sf::Vector2f transform(const float t) {
    const float ellipse_width = 400.f;
    const float ellipse_height = 400.f;
    const float a = ellipse_width / 2.f;
    const float b = ellipse_height / 2.f;
    const float pi = 3.141592653589f;
    const float tau = 2.f * pi;
    const float period_ms = 3000.f; // 5 seconds
    const float x = (std::fmodf(t, period_ms) / period_ms) * tau;
    return sf::Vector2f(a * std::cos(x), b * std::sin(x));
}

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(1000, 1000), "SFML Works!", sf::Style::Default, settings);
  window.setFramerateLimit(60);

  sf::CircleShape shape(100.f);
  sf::CircleShape centeredCircle(100.f);

  // Create a random device for seeding the generator
  std::random_device rand_dev;
  // Initialize the Mersenne Twister engine with the random device
  std::mt19937 gen(rand_dev());
  // Define the range [0, 255] for the uniform distribution
  std::uniform_int_distribution<sf::Uint8> distr(0, 255);

  std::list<sf::CircleShape> afterImages;
  int maxAfterImages = 100;
  for (int i = 0; i < maxAfterImages; ++i) {
    afterImages.push_front(shape);
  }

  // Clocks
  sf::Clock clock;
  sf::Clock clockColor;

  /*
  std::cout << "Click any key to continue!" << std::endl;
  while (window.isOpen()) {
    bool stay = true;
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          stay = false;
        }
      }
    }
    if (!stay) {
      break;
    }
  }
  */

  while (window.isOpen()) {
    // Handle events
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    // Change color at a configurable rate
    if (clockColor.getElapsedTime().asMilliseconds() > (1000 / maxAfterImages)) {
      //std::cout << "Num afterImages: " << afterImages.size() << std::endl;
      // Generate a random number within the specified range
      const sf::Uint8 r = distr(gen);
      const sf::Uint8 g = distr(gen);
      const sf::Uint8 b = distr(gen);
      shape.setFillColor(sf::Color(r,g,b,255));
      clockColor.restart();

      // Manage after images
      afterImages.pop_back();
      afterImages.push_front(shape);
      std::ranges::for_each(afterImages, [&](sf::CircleShape& afterImage) { 
          sf::Color color = afterImage.getFillColor();
          color.a = static_cast<sf::Uint8>(color.a - (255 / maxAfterImages));
          afterImage.setFillColor(color);
          afterImage.setRadius(afterImage.getRadius() * 1.005f);
          });
    }

    // Set position according to time passed
    const sf::Vector2i posMouse = sf::Mouse::getPosition(window);
    const sf::Vector2f posCenter = sf::Vector2f(posMouse.x - shape.getRadius(), posMouse.y - shape.getRadius());
    //const sf::Vector2f posChange = transform(static_cast<float>(clock.getElapsedTime().asMilliseconds()));
    //shape.setPosition(posCenter + posChange);
    shape.setPosition(posCenter);

    // Centered circle
    centeredCircle.setPosition(posCenter);
    centeredCircle.setFillColor(sf::Color::Green);

    // Redraw things
    window.clear();
    window.draw(centeredCircle);
    window.draw(shape);
    std::ranges::for_each(afterImages.begin(), afterImages.end(), [&](sf::CircleShape afterImage) { window.draw(afterImage); });
    window.display();
  }

  return EXIT_SUCCESS;
}

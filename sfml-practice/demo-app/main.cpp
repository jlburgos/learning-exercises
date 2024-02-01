#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <ranges>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "ellipse.hpp"
#include "numerics-wrapper.hpp"

class Layer {
  private:
    sf::RenderWindow* const window;
    std::vector<const sf::Drawable*> drawables;
  public:
    Layer(sf::RenderWindow* _window) : window(_window) {}
    void addToLayer(const sf::Drawable* drawable) {
      drawables.push_back(drawable);
    }
    void draw() {
      for (const sf::Drawable* const drawable : drawables) {
        window->draw(*drawable);
      }
    }
};

class AfterImagesContainer : public sf::Drawable {
  private:
    const U8 maxAfterImages;
    std::list<std::unique_ptr<sf::Drawable>> drawables;
  public:
    AfterImagesContainer(const U8 _maxAfterImages) : maxAfterImages(_maxAfterImages) {
    }

    std::size_t size() { return drawables.size(); }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
      for (const std::unique_ptr<sf::Drawable>& drawable : drawables) {
        sf::Drawable* shape = drawable.get();
        target.draw(*shape, states);
      }
    }

    void update(std::unique_ptr<sf::Drawable> newAfterImage) {
      drawables.push_front(std::move(newAfterImage));
      if (drawables.size() > maxAfterImages) {
        drawables.pop_back();
      }
      for (std::unique_ptr<sf::Drawable>& drawable : drawables) {
        EllipseShape* ellipse = nullptr;
        sf::CircleShape* circle = nullptr;
        sf::RectangleShape* rectangle = nullptr;
        if ((ellipse = dynamic_cast<EllipseShape*>(drawable.get()))) {
          sf::Color color = ellipse->getFillColor();
          color.a = static_cast<U8>(color.a - (255 / maxAfterImages));
          ellipse->setFillColor(color);
          ellipse->setRadius(sf::Vector2f(ellipse->getRadius() * 1.005f));
        } else if ((circle = dynamic_cast<sf::CircleShape*>(drawable.get()))) {
          sf::Color color = circle->getFillColor();
          color.a = static_cast<U8>(color.a - (255 / maxAfterImages));
          circle->setFillColor(color);
          circle->setRadius(circle->getRadius() * 1.005f);
        } else if ((rectangle = dynamic_cast<sf::RectangleShape*>(drawable.get()))) {
          sf::Color color = rectangle->getFillColor();
          color.a = static_cast<U8>(color.a - (255 / maxAfterImages));
          rectangle->setFillColor(color);
          rectangle->setScale(sf::Vector2f(rectangle->getScale() * 1.005f));
        } else {
          std::cout << "Encountered a non-ellipse shape! " << drawables.size() << std::endl;
        }
        
      }
    }
};

namespace Text {
  const char* ARIAL_TTF="/System/Library/Fonts/Supplemental/Arial Unicode.ttf";
}

const sf::Vector2f transform(const F32 periodMS, const F32 timeMS) {
    const F32 ellipse_width = 400.f;
    const F32 ellipse_height = 400.f;
    const F32 a = ellipse_width / 2.f;
    const F32 b = ellipse_height / 2.f;
    const F32 tau = 2.f * Numbers::PI;
    const F32 radians = tau * std::fmodf(timeMS, periodMS) / periodMS;
    return sf::Vector2f(a * std::cos(radians), b * std::sin(radians));
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
  window.setFramerateLimit(120);

  sf::CircleShape shape(100.f);
  sf::CircleShape centeredCircle(100.f);

  //std::list<EllipseShape> afterImages;
  U8 maxAfterImages = 20;
  AfterImagesContainer afterImages(maxAfterImages);

  // Clocks
  sf::Clock clock;
  sf::Clock clockColor;

  // Create a font for text rendering
  sf::Font font;
  if (!font.loadFromFile(Text::ARIAL_TTF)) {
    // Handle font loading error
    std::cout << "Failed to load font " << Text::ARIAL_TTF << std::endl;
    return EXIT_FAILURE;
  }

  // Create a text object for displaying frame rate
  sf::Text frameRateText;
  frameRateText.setFont(font);
  frameRateText.setCharacterSize(20);
  frameRateText.setFillColor(sf::Color::White);
  frameRateText.setPosition(600, 10); // Adjust the position as needed

  // Initialize layer
  Layer layer(&window);
  layer.addToLayer(&centeredCircle);
  layer.addToLayer(&shape);
  layer.addToLayer(&afterImages);
  layer.addToLayer(&frameRateText);

  U32 frames = 0;

  while (window.isOpen()) {
    // Handle events
    handleEvents(window);

    const F32 periodMS = 3000.0f;

    // Set position according to time passed
    const sf::Vector2i posMouse = sf::Mouse::getPosition(window);
    const sf::Vector2f posCenter = sf::Vector2f(posMouse.x - shape.getRadius(), posMouse.y - shape.getRadius());
    const sf::Vector2f posChange = transform(periodMS, static_cast<F32>(clock.getElapsedTime().asMilliseconds()));
    shape.setPosition(posCenter + posChange);
    //shape.setPosition(posCenter);

    // Change color at a configurable rate
    U32 elapsedMS = static_cast<U32>(clockColor.getElapsedTime().asMilliseconds());
    if (elapsedMS > (1000.0f / maxAfterImages)) {
      clockColor.restart();
      // Generate a random number within the specified range
      const U8 r = Numbers::randNum<U8>();
      const U8 g = Numbers::randNum<U8>();
      const U8 b = Numbers::randNum<U8>();
      shape.setFillColor(sf::Color(r,g,b,255));

      // Manage after images
      const sf::Vector2f radius(sf::Vector2f(shape.getRadius(), shape.getRadius()));

      // TODO :: Clean this up!
      const U8 selector = Numbers::randNum<U8>(1,3);
      switch(selector) {
        case 1: {
                  EllipseShape someShape(radius);
                  someShape.setFillColor(shape.getFillColor());
                  someShape.setPosition(posCenter + posChange);
                  afterImages.update(std::make_unique<EllipseShape>(someShape));
                  break;
                }
        case 2: {
                  sf::CircleShape someShape(shape.getRadius());
                  someShape.setFillColor(shape.getFillColor());
                  someShape.setPosition(posCenter + posChange);
                  afterImages.update(std::make_unique<sf::CircleShape>(someShape));
                  break;
                }
        default: { // case 3
                   sf::RectangleShape someShape(radius);
                   someShape.setFillColor(shape.getFillColor());
                   someShape.setPosition(posCenter + posChange);
                   afterImages.update(std::make_unique<sf::RectangleShape>(someShape));
                 }
      }

      // Shape that will be ignored
      //afterImages.update(std::make_unique<sf::CircleShape>(sf::CircleShape(200.0f)));
    }

    elapsedMS = static_cast<U32>(clock.getElapsedTime().asMilliseconds());
    const F32 time = static_cast<F32>(clock.getElapsedTime().asSeconds());
    const F32 frameRate = static_cast<F32>(++frames) / time;

    std::stringstream ss;
    ss << "[ Frame Rate: " << frameRate << " FPS ]";
    frameRateText.setString(ss.str());

    if (elapsedMS > periodMS) {
      clock.restart();
      frames = 0;
    }

    // Centered circle
    centeredCircle.setPosition(posCenter);
    centeredCircle.setFillColor(sf::Color::Green);

    // Redraw things
    window.clear();
    layer.draw();
    window.display();
  }

  return EXIT_SUCCESS;
}

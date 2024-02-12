#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <numbers>
#include <random>
#include <ranges>
#include <sstream>
#include <string>
#include <queue>

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

using U8 = sf::Uint8;   // 1 byte // unsigned char
using U16 = sf::Uint16; // 2 byte // unsigned short
using U32 = sf::Uint32; // 4 byte // unsigned int
using U64 = sf::Uint64; // 8 byte // unsigned long long

using I8 = sf::Int8;   // 1 byte // signed char
using I16 = sf::Int16; // 2 byte // signed short
using I32 = sf::Int32; // 4 byte // signed int
using I64 = sf::Int64; // 8 byte // signed long long

using F32 = float;  // 4 byte
using F64 = double; // 8 byte

namespace Numbers {
  namespace {
    // Create a random device for seeding the generator
    // Initialize the Mersenne Twister engine with the random device
    std::mt19937 engine(std::random_device{}());

    template<class T>
      concept NumberType = std::is_integral<T>::value || std::is_floating_point<T>::value;
  }

  constexpr F32 PI = std::numbers::pi_v<F32>;

  template<NumberType T>
    T randNum(const T min, const T max) {
      // Apply the value min/max range for the uniform numeric distribution and apply engine
      return std::uniform_int_distribution<T>(min,max)(engine);
    }

  template<NumberType T>
    constexpr T min() {
      return std::numeric_limits<T>::min();
    }

  template<NumberType T>
    constexpr T max() {
      return std::numeric_limits<T>::max();
    }

  template<NumberType T>
    T randNum() {
      // Define the value min/max range for the uniform numeric distribution and apply engine
      return randNum<T>(min<T>(), max<T>());
    }
}

struct EllipseShape : public sf::Shape {
    sf::Vector2f m_radius;
    F32 dx;
    F32 dy;

    EllipseShape(const sf::Vector2f& radius) : m_radius(radius) {
      dx = 1.0f;
      dy = 1.0f;
      update();
    }

    virtual std::size_t getPointCount() const override {
      return 30; // fixed, but could be an attribute of the class if needed
                 //return 3; // fixed, but could be an attribute of the class if needed
    }

    virtual sf::Vector2f getPoint(const unsigned long index) const override {
      const F32 angle = index * 2 * Numbers::PI / getPointCount() - Numbers::PI / 2;
      const F32 x = std::cos(angle) * m_radius.x * dx;
      const F32 y = std::sin(angle) * m_radius.y * dy;
      return sf::Vector2f(m_radius.x + x, m_radius.y + y);
    }

    void setRadius() {
      update();
    }

    sf::Vector2f& getRadius() {
      return m_radius;
    }
};

class Layer {
  private:
    sf::RenderWindow* const window;
    std::vector<const sf::Drawable*> drawables;
  public:
    Layer(sf::RenderWindow* _window) : window(_window) {
    }

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

    std::size_t size() const {
      return drawables.size();
    }

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

        if ((ellipse = dynamic_cast<EllipseShape*>(drawable.get()))) { // ellipse
          sf::Color color = ellipse->getFillColor();
          color.a = static_cast<U8>(color.a - (255 / maxAfterImages));
          ellipse->setFillColor(color);
          if (color.a > 255 / 2) {
            ellipse->dx = ellipse->dx * 1.05f;
          } else {
            ellipse->dx = ellipse->dx * 0.95f;
            ellipse->dy = ellipse->dy * 1.05f;
          }
          ellipse->setRadius();
        } else if ((circle = dynamic_cast<sf::CircleShape*>(drawable.get()))) { // circle
          sf::Color color = circle->getFillColor();
          color.a = static_cast<U8>(color.a - (255 / maxAfterImages));
          circle->setFillColor(color);
          circle->setRadius(circle->getRadius() * 1.005f);
        } else if ((rectangle = dynamic_cast<sf::RectangleShape*>(drawable.get()))) { // rectangle
          sf::Color color = rectangle->getFillColor();
          color.a = static_cast<U8>(color.a - (255 / maxAfterImages));
          rectangle->setFillColor(color);
          rectangle->setScale(sf::Vector2f(rectangle->getScale() * 1.005f));
        } else {
          std::cout << "Encountered some unknown shape! " << std::endl;
        }

      }
    }
};

namespace Text {
  constexpr const char* ARIAL_TTF="/System/Library/Fonts/Supplemental/Arial Unicode.ttf";
}

const sf::Vector2f transform(const F32 periodMS, const F32 timeMS) {
  const F32 ellipse_width = 400.0f;
  const F32 ellipse_height = 400.0f;
  const F32 a = ellipse_width / 2.0f;
  const F32 b = ellipse_height / 2.0f;
  const F32 tau = 2.0f * Numbers::PI;
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
  window.setFramerateLimit(60);

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
  //layer.addToLayer(&shape);
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
      const U8 selector = Numbers::randNum<U8>(1,1);
      switch(selector) {
        case 1:
          {
            EllipseShape someShape(radius);
            //EllipseShape someShape({radius.x, radius.y});
            someShape.setFillColor(shape.getFillColor());
            someShape.setPosition(posCenter + posChange);
            afterImages.update(std::make_unique<EllipseShape>(someShape));
            break;
          }
        case 2:
          {
            sf::CircleShape someShape(shape.getRadius());
            someShape.setFillColor(shape.getFillColor());
            someShape.setPosition(posCenter + posChange);
            afterImages.update(std::make_unique<sf::CircleShape>(someShape));
            break;
          }
        default:
          { // case 3
            sf::RectangleShape someShape({2 * radius.x, 2 * radius.y});
            someShape.setFillColor(shape.getFillColor());
            someShape.setPosition(posCenter + posChange);
            afterImages.update(std::make_unique<sf::RectangleShape>(someShape));
          }
      }
    }

    elapsedMS = static_cast<U32>(clock.getElapsedTime().asMilliseconds());
    const F32 time = static_cast<F32>(clock.getElapsedTime().asSeconds());
    const U32 frameRate = static_cast<U32>(++frames / time);

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

#include <cstdlib>
#include <cstdint>

#include <SFML/Graphics/Shape.hpp>

class EllipseShape : public sf::Shape {
private :
    sf::Vector2f m_radius;

public :
    explicit EllipseShape(const sf::Vector2f& radius = sf::Vector2f(0, 0)) : m_radius(radius) {
        update();
    }

    void setRadius(const sf::Vector2f& radius) {
        m_radius = radius;
        update();
    }

    const sf::Vector2f& getRadius() const {
        return m_radius;
    }

    virtual std::size_t getPointCount() const override {
        //return 30; // fixed, but could be an attribute of the class if needed
        return 3; // fixed, but could be an attribute of the class if needed
    }

    virtual sf::Vector2f getPoint(unsigned long index) const override {
        static const float pi = 3.141592654f;

        float angle = index * 2 * pi / getPointCount() - pi / 2;
        float x = std::cos(angle) * m_radius.x;
        float y = std::sin(angle) * m_radius.y * 1.5f; // stretch it along the y-axis!

        return sf::Vector2f(m_radius.x + x, m_radius.y + y);
    }
};

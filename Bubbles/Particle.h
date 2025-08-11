#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float lifetime = 1.0f;
    float age = 0.0f;

    Particle(sf::Vector2f position, sf::Texture* a_Texture) {
        shape.setTexture(a_Texture);
    	shape.setRadius(20.0f);
        shape.setPosition(position);
        
        // Random velocity (burst in all directions)
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.f * 3.14159f;
        float speed = 50.f + static_cast<float>(rand()) / RAND_MAX * 100.f;
        velocity = sf::Vector2f(std::cos(angle), std::sin(angle)) * speed;
    }

    bool update(float dt) {
        age += dt;
        float t = age / lifetime;

        // Move particle
        shape.move(velocity * dt);

        // Fade out
        sf::Color c = shape.getFillColor();
        c.a = static_cast<uint8_t>(255 * (1.f - t));
        shape.setFillColor(c);

        return age >= lifetime;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

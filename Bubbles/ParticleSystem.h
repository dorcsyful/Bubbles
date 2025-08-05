#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "Particle.h"

class ParticleSystem {
    std::vector<Particle> m_Particles;

public:
    void SpawnBurst(sf::Vector2f a_Position, int a_Count, sf::Color a_Color) {
        for (int i = 0; i < a_Count; ++i) {
            m_Particles.emplace_back(a_Position, a_Color);
        }
    }

    void Update(float a_Dt) {
        for (auto it = m_Particles.begin(); it != m_Particles.end();) {
            if (it->update(a_Dt))
                it = m_Particles.erase(it);
            else
                ++it;
        }
    }

    void Draw(sf::RenderWindow& a_Window) {
        for (auto& p : m_Particles) {
            p.draw(a_Window);
        }
    }
};

#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "FilePaths.h"
#include "Particle.h"

class ParticleSystem {
    std::vector<Particle> m_Particles;
    std::unique_ptr<sf::Texture> m_Texture;

public:

    ParticleSystem()
    {
        m_Texture = std::make_unique<sf::Texture>();
        m_Texture->loadFromFile(COMBO_STAR);
    }

    void SpawnBurst(sf::Vector2f a_Position, int a_Count, sf::Color a_Color) {
        for (int i = 0; i < a_Count; ++i) {
            Particle temp = Particle(a_Position, m_Texture.get());
            m_Particles.emplace_back(temp);
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

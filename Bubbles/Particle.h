#pragma once
#include <vector>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class Particle : public sf::Drawable, public sf::Transformable
{
public:

    Particle(const unsigned int a_Count) :
        m_Particles(a_Count),
        m_Vertices(sf::PrimitiveType::Points,a_Count),
        m_Lifetime(sf::seconds(3.f)),
        m_emitter(0.f, 0.f)
    {
    }

    void SetEmitter(const sf::Vector2f a_Position)
    {
        m_emitter = a_Position;
    }

    void Update(const sf::Time a_Elapsed)
    {
        for (std::size_t i = 0; i < m_Particles.size(); ++i)
        {
            // update the particle lifetime
            SingularParticle& p = m_Particles[i];
            p.m_Lifetime -= a_Elapsed;

            // if the particle is dead, respawn it
            if (p.m_Lifetime <= sf::Time::Zero)
                ResetParticle(i);

            // update the position of the corresponding vertex
            m_Vertices[i].position += p.m_Velocity * a_Elapsed.asSeconds();

            // update the alpha (transparency) of the particle according to its lifetime
            float ratio = p.m_Lifetime.asSeconds() / m_Lifetime.asSeconds();
            m_Vertices[i].color.a = static_cast<uint8_t>(ratio * 255);
        }
    }

private:
    void draw(sf::RenderTarget& a_Target, sf::RenderStates a_States) const override
    {
        // apply the transform
        a_States.transform *= getTransform();

        // our particles don't use a texture
        a_States.texture = nullptr;

        // draw the vertex array
        a_Target.draw(m_Vertices, a_States);
    }

private:

    struct SingularParticle
    {
        sf::Vector2f m_Velocity;
        sf::Time m_Lifetime;
    };

    void ResetParticle(const std::size_t a_Index)
    {
        // give a random velocity and lifetime to the particle
        float angle = static_cast<float>(std::rand() % 360) * 3.14f / 180.f;
        float speed = static_cast<float>(std::rand() % 50) + 50.f;
        m_Particles[a_Index].m_Velocity = sf::Vector2f(cosf(angle) * speed, sinf(angle) * speed);
        m_Particles[a_Index].m_Lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

        // reset the position of the corresponding vertex
        m_Vertices[a_Index].position = m_emitter;
    }

    std::vector<SingularParticle> m_Particles;
    sf::VertexArray m_Vertices;
    sf::Time m_Lifetime;
    sf::Vector2f m_emitter;
};

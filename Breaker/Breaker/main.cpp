#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

// Components-----------------------
struct Transform2D {
    sf::Vector2f position;
    sf::Vector2f velocity;
};

struct Render {
    sf::RectangleShape rect;
    sf::CircleShape circle;
    bool destroyed = false; 
};

// Entities--------------------
class Entity {
public:
    int id;
    Transform2D& transform;
    Render& render;

    // Entity constructor
    Entity(int id, Transform2D& trans, Render& ren)
        : id(id), transform(trans), render(ren) {}
};

// Systems---------------------------
class Behavior {
public:
    void move(Entity& entity, const sf::RenderWindow& window) {
        entity.transform.position.x += entity.transform.velocity.x;
        entity.transform.position.y += entity.transform.velocity.y;

        // X movement
        if (entity.transform.position.x <= 0) {
            entity.transform.position.x = 0;
            entity.transform.velocity.x = -entity.transform.velocity.x;
        }

        if (entity.transform.position.x + entity.render.rect.getSize().x >= window.getSize().x) {
            entity.transform.position.x = window.getSize().x - entity.render.rect.getSize().x;
            entity.transform.velocity.x = -entity.transform.velocity.x;
        }

        // Y movement
        if (entity.transform.position.y <= 0) {
            entity.transform.position.y = 0;
            entity.transform.velocity.y = -entity.transform.velocity.y;
        }

        if (entity.transform.position.y + entity.render.rect.getSize().y >= window.getSize().y) {
            entity.transform.position.y = window.getSize().y - entity.render.rect.getSize().y;
            entity.transform.velocity.y = -entity.transform.velocity.y;
        }
    }

    void input(Entity& entity, const sf::RenderWindow& window) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            entity.transform.position.x += entity.transform.velocity.x;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            entity.transform.position.x -= entity.transform.velocity.x;
        }

        //border collision
        if (entity.transform.position.x <= 0) {
            entity.transform.position.x = 0;
        }

        if (entity.transform.position.x + entity.render.rect.getSize().x >= window.getSize().x) {
            entity.transform.position.x = window.getSize().x - entity.render.rect.getSize().x;
        }


    }

    // Bounce function for ball's collision with window borders
    void bounce(Entity& entity, const sf::RenderWindow& window) {
        entity.transform.position.x += entity.transform.velocity.x;
        entity.transform.position.y += entity.transform.velocity.y;

        // X 
        if (entity.transform.position.x <= 0) {
            entity.transform.position.x = 0;
            entity.transform.velocity.x = -entity.transform.velocity.x;
        }
        if (entity.transform.position.x + entity.render.circle.getRadius() * 2 >= window.getSize().x) {
            entity.transform.position.x = window.getSize().x - entity.render.circle.getRadius() * 2;
            entity.transform.velocity.x = -entity.transform.velocity.x;
        }

        // Y 
        if (entity.transform.position.y <= 0) {
            entity.transform.position.y = 0;
            entity.transform.velocity.y = -entity.transform.velocity.y;
        }



    }

    void reset(Entity& circle, Entity& rect, const sf::RenderWindow& window) {

        //reset
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || circle.transform.position.y >= window.getSize().y) {
            rect.transform.position.x = window.getSize().x / 2 - 50;

            circle.transform.position.x = window.getSize().x / 2 - circle.render.circle.getRadius();
            circle.transform.position.y = 500;

            if (circle.transform.velocity.y < 0){
            circle.transform.velocity.y = -circle.transform.velocity.y;
            }
            else {
                circle.transform.velocity.y = +circle.transform.velocity.y;
            }

        }

    }

    void collideBall(Entity& circle, Entity& rect) {
        //ball hitbox
        sf::FloatRect circleHitbox(
            circle.transform.position.x,
            circle.transform.position.y,
            circle.render.circle.getRadius() * 2,
            circle.render.circle.getRadius() * 2
        );

        sf::FloatRect rectBounds = rect.render.rect.getGlobalBounds();

        if (circleHitbox.intersects(rectBounds) && !rect.render.destroyed) {   
            circle.transform.velocity.y = -circle.transform.velocity.y;
        }
    }

    void destroy(Entity& circle, Entity& rect) {
        //ball hitbox
        sf::FloatRect circleHitbox(
            circle.transform.position.x,
            circle.transform.position.y,
            circle.render.circle.getRadius() * 2,
            circle.render.circle.getRadius() * 2
        );

        sf::FloatRect rectBounds = rect.render.rect.getGlobalBounds();

        if (circleHitbox.intersects(rectBounds) && !rect.render.destroyed) {    
            circle.transform.velocity.y = -circle.transform.velocity.y;
            rect.render.destroyed = true;
        }
    }
};

class Rendering {
public:
    void draw(Entity& entity, sf::RenderWindow& window) {
        // Draw rect
        if (!entity.render.destroyed && entity.render.rect.getSize().x > 0) {
            entity.render.rect.setPosition(entity.transform.position);
            window.draw(entity.render.rect);
        }

        // Draw circle
        if (entity.render.circle.getRadius() > 0) {
            entity.render.circle.setPosition(entity.transform.position);
            window.draw(entity.render.circle);
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Breaker", sf::Style::Titlebar);
    vector<Entity> bricks;

    // Components for player
    Transform2D transformSquare{ sf::Vector2f(window.getSize().x / 2 - 50, 750), sf::Vector2f(2, 0)};
    Render renderSquare;
    renderSquare.rect.setSize(sf::Vector2f(100, 20));
    renderSquare.rect.setFillColor(sf::Color::White);

    // Components for circle
    Transform2D circlePosition{ sf::Vector2f(window.getSize().x / 2 - 30, 500), sf::Vector2f(1, 1.5) };
    Render circleRender;
    circleRender.circle.setRadius(15);
    circleRender.circle.setFillColor(sf::Color::White);

    // Components for bricks
    int row = 4;
    int verticalGap = 100;
    for (int i = 0; i <= row; i++) {
        for (int i = 0; i < 8; i++) {
            Transform2D* transformBrick = new Transform2D{ sf::Vector2f(25 + i * 95, verticalGap), sf::Vector2f(0, 0) };
            Render* renderBricks = new Render;
            renderBricks->rect.setSize(sf::Vector2f(80, 20));
            renderBricks->rect.setFillColor(sf::Color::White);

            bricks.emplace_back(i, *transformBrick, *renderBricks);
        }
        verticalGap += 30;
    }

    // Create entities
    Entity square(1, transformSquare, renderSquare);
    Entity circle(2, circlePosition, circleRender);

    // Systems
    Behavior system;
    Rendering rendering;

    // Main loop------
    while (window.isOpen()) {
        // Key input
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // System application
        system.input(square, window);
        system.bounce(circle, window);
        system.collideBall(circle, square);
        system.reset(circle, square, window);

        
        for (auto& brick : bricks) {
            system.destroy(circle, brick);
        }


        window.clear();
        rendering.draw(square, window);
        rendering.draw(circle, window);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        
            for (auto& brick : bricks) {
                brick.render.destroyed = false;
            }
        }


        for (auto& brick : bricks) {
            rendering.draw(brick, window);
        }

        window.display();

        sf::sleep(sf::milliseconds(5));
    }

    return 0;
}

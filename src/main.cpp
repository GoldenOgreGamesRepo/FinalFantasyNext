#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "My RPG - SFML 3.0");

    while (window.isOpen())
    {
        // SFML 3.0: pollEvent returns std::optional<sf::Event>
        while (auto event = window.pollEvent())
        {
            // Access the event using event-> (because it's an optional)
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);
        window.display();
    }

    return 0;
}

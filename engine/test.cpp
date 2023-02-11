#include "test.h"

namespace engine {
    sf::RenderWindow getWindow(std::string name) {
        return sf::RenderWindow(sf::VideoMode(800, 600), name);

    }
}
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#pragma once

template<typename T>
struct UIFactory
{
public:

    // Create a new UI element
    template<typename... Args>
    std::shared_ptr<T> create(Args&&... args) {
        std::shared_ptr<T> newElement = std::make_unique<T>(std::forward<Args>(args)...);
        m_elements.push_back(std::move(newElement));

        if(!m_elements.empty()){
            return m_elements.back();
        }
        else {
            return nullptr;
        }

    };

    // Get a vector of created UI elements
    std::vector<std::shared_ptr<T>>& getElements() {
        return m_elements;
    };

    std::vector<std::shared_ptr<T>> m_elements;
};
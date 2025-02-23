#include "Starfield.h"

#include <optional>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>

Starfield::Starfield(const std::string& title,
                     GameData&          gameData) : title(title),
                                                    gameData(gameData) {
}

Starfield::~Starfield() {
  if (window != nullptr) {
    stop();
  }
}

void Starfield::run() {
  window = new sf::RenderWindow(sf::VideoMode::getFullscreenModes()[0], title, sf::State::Fullscreen);

  window->setFramerateLimit(60);
  // sf::View view = window->getDefaultView();
  // view.setRotation(180);
  // window->setView(view);

  std::optional<sf::Event> event;

  shouldStop = false;

  while ((window->isOpen()) && (!shouldStop)) {
    while ((event = window->pollEvent()) != std::nullopt) {
      if (eventHandler != nullptr) {
        eventHandler->newEvent(event.value());
      }
    }

    window->clear();
    gameData.draw(*window);
    window->display();
  }

  delete window;
}

void Starfield::stop() {
  shouldStop = true;
}

bool Starfield::isStopped() {
  return shouldStop;
}

void Starfield::addEventHandler(EventHandler* eventHandler) {
  this->eventHandler = eventHandler;
}

#include "Starfield.h"

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
  window = new sf::RenderWindow(sf::VideoMode::getFullscreenModes()[0], title, sf::Style::Fullscreen);

  // sf::View view = window->getDefaultView();
  // view.setRotation(180);
  // window->setView(view);

  sf::Event event;

  shouldStop = false;

  while ((window->isOpen()) && (!shouldStop)) {
    while (window->pollEvent(event)) {
      if (eventHandler != nullptr) {
        eventHandler->newEvent(event);
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

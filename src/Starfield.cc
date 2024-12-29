#include "SFMLStub.h"

#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>

SFMLStub::SFMLStub(const std::string& title,
                   GameData&          gameData) : title(title),
                                                  gameData(gameData) {
}

SFMLStub::~SFMLStub() {
  if (window != nullptr) {
    stop();
  }
}

void SFMLStub::run() {
  window = new sf::RenderWindow(sf::VideoMode::getFullscreenModes()[0], title, sf::Style::Fullscreen);

  // sf::View view = window->getDefaultView();
  // view.setRotation(180);
  // window->setView(view);

  sf::Event event;

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

  shouldStop = false;
}

void SFMLStub::stop() {
  shouldStop = true;
}

void SFMLStub::draw(const GameData& data) {
  data.draw(*window);
}

void SFMLStub::addEventHandler(EventHandler* eventHandler) {
  this->eventHandler = eventHandler;
}

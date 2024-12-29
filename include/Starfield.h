#pragma once

#include <string>

#include "EventHandler.h"
#include "GameData.h"

#include <SFML/Graphics/RenderWindow.hpp>

class Starfield {
 public:
  Starfield(const std::string& title,
            GameData& gameData);

  ~Starfield();

  void run();

  void addEventHandler(EventHandler* eventHandler);

  void stop();

  bool isStopped();

 private:
  const std::string title;

  GameData& gameData;

  sf::RenderWindow* window = nullptr;

  EventHandler* eventHandler = nullptr;

  bool shouldStop = false;
};

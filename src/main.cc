#include <SFML/Graphics/CircleShape.hpp>
#include <cstdlib>
#include <iostream>

#include "EventHandler.h"
#include "GameData.h"
#include "SFMLStub.h"

class EV : public EventHandler {
 public:
  EV(SFMLStub& sfmlStub) : sfmlStub(sfmlStub) {}

  void newEvent(const sf::Event& event) override {
    if (event.type == sf::Event::KeyReleased) {
      if (event.key.code == sf::Keyboard::Escape) {
        sfmlStub.stop();
      }
    }
  }

 private:
  SFMLStub& sfmlStub;
};

class GD : public GameData {
 public:
  GD() {
    circle.setRadius(50);
    circle.setPosition(100, 100);
  }

  void draw(sf::RenderWindow& window) const override {
    window.draw(circle);
  }

 private:
  sf::CircleShape circle;
};

int main(const int    argc,
         const char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: sfmlStub windowName" << std::endl;

    std::exit(EXIT_FAILURE);
  }

  GD gd;

  SFMLStub sfmlStub(*(argv + 1), gd);

  EV ev(sfmlStub);
  sfmlStub.addEventHandler(&ev);

  sfmlStub.run();
}

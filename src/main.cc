#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <unistd.h>

#include "EventHandler.h"
#include "GameData.h"
#include "Starfield.h"

#define MAX_SIZE 5
#define NUM_STARS 100000

struct Star {
 public:
  float startSize;
  float currentX;
  float currentY;
  float currentSize;
  sf::Color colour;
};

class GD : public GameData {
 public:
  GD() {
    std::srand(std::time(nullptr));

    for (auto& star : stars) {
      star = new Star();
      star->currentX = std::rand() % sf::VideoMode::getFullscreenModes()[0].size.x;
      star->currentY = std::rand() % sf::VideoMode::getFullscreenModes()[0].size.y;
      star->colour = sf::Color((std::uint8_t) (std::rand() % 255),
                               (std::uint8_t) (std::rand() % 255),
                               (std::uint8_t) (std::rand() % 255),
                               (std::uint8_t) (std::rand() % 255));

      star->startSize = 1;
    }
  }

  void update() {
    mutex.lock();

    for (auto& star : stars) {
      star->currentX += (star->currentX - sf::VideoMode::getFullscreenModes()[0].size.x * 0.5) * xIncrement;
      star->currentY += (star->currentY - sf::VideoMode::getFullscreenModes()[0].size.y * 0.5) * yIncrement;
      star->currentSize = std::min(star->currentSize * 1.01, (double) MAX_SIZE);

      if ((star->currentX < 0 - (star->currentSize * 0.5)) ||
          (star->currentX > sf::VideoMode::getFullscreenModes()[0].size.x + (star->currentSize * 0.5)) ||
          (star->currentY < 0 - (star->currentSize * 0.5)) ||
          (star->currentY > sf::VideoMode::getFullscreenModes()[0].size.y + (star->currentSize * 0.5))) {
        star->currentX = std::rand() % sf::VideoMode::getFullscreenModes()[0].size.x;
        star->currentY = std::rand() % sf::VideoMode::getFullscreenModes()[0].size.y;
        star->currentSize = star->startSize;
      }
    }

    mutex.unlock();
  }

  void draw(sf::RenderWindow& window) override {
    mutex.lock();

    sf::VertexArray vertexArray(sf::PrimitiveType::Triangles, 3 * NUM_STARS);

    for (std::size_t index = 0; index < NUM_STARS; ++index) {
      vertexArray[index * 3 + 0].position = sf::Vector2f(stars[index]->currentX - (stars[index]->currentSize * 0.5), stars[index]->currentY - (stars[index]->currentSize * 0.5));
      vertexArray[index * 3 + 0].color = stars[index]->colour;
      vertexArray[index * 3 + 1].position = sf::Vector2f(stars[index]->currentX, stars[index]->currentY + (stars[index]->currentSize * 0.5));
      vertexArray[index * 3 + 1].color = stars[index]->colour;
      vertexArray[index * 3 + 2].position = sf::Vector2f(stars[index]->currentX + (stars[index]->currentSize * 0.5), stars[index]->currentY - (stars[index]->currentSize * 0.5));
      vertexArray[index * 3 + 2].color = stars[index]->colour;
    }

    mutex.unlock();

    window.draw(vertexArray);
  }

  void plusX() {
    xIncrement += 0.01;
  }

  void minusX() {
    xIncrement -= 0.01;
  }

  void plusY() {
    yIncrement += 0.01;
  }

  void minusY() {
    yIncrement -= 0.01;
  }

 private:
  std::array<Star*, NUM_STARS> stars;
  std::mutex mutex;
  float xIncrement = 0.01;
  float yIncrement = 0.01;
};

class EV : public EventHandler {
 public:
  EV(Starfield& starfield,
     GD& gd) : starfield(starfield),
               gd(gd) {}

  void newEvent(const sf::Event& event) override {
    if (event.is<sf::Event::KeyReleased>()) {
      const sf::Event::KeyReleased* krEvent = event.getIf<sf::Event::KeyReleased>();
      if (krEvent->code == sf::Keyboard::Key::Escape) {
        starfield.stop();
      } else if (krEvent->code == sf::Keyboard::Key::Right) {
        gd.plusX();
      } else if (krEvent->code == sf::Keyboard::Key::Left) {
        gd.minusX();
      } else if (krEvent->code == sf::Keyboard::Key::Up) {
        gd.plusY();
      } else if (krEvent->code == sf::Keyboard::Key::Down) {
        gd.minusY();
      }
    }
  }

 private:
  Starfield& starfield;
  GD& gd;
};

int main(const int    argc,
         const char** argv) {
  GD gd;

  Starfield starfield("starfield", gd);

  EV ev(starfield, gd);
  starfield.addEventHandler(&ev);

  std::thread thread([&starfield] {
    starfield.run();
  });

  while (!starfield.isStopped()) {
    gd.update();

    usleep(10000);
  }

  thread.join();
}

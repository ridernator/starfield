#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
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

class EV : public EventHandler {
 public:
  EV(Starfield& starfield) : starfield(starfield) {}

  void newEvent(const sf::Event& event) override {
    if (event.type == sf::Event::KeyReleased) {
      if (event.key.code == sf::Keyboard::Escape) {
        starfield.stop();
      }
    }
  }

 private:
  Starfield& starfield;
};

class GD : public GameData {
 public:
  GD() {
    std::srand(std::time(nullptr));

    for (auto& star : stars) {
      star = new Star();
      star->currentX = std::rand() % sf::VideoMode::getFullscreenModes()[0].width;
      star->currentY = std::rand() % sf::VideoMode::getFullscreenModes()[0].height;
      star->colour = sf::Color((std::uint8_t) (std::rand() % 255),
                               (std::uint8_t) (std::rand() % 255),
                               (std::uint8_t) (std::rand() % 255),
                               (std::uint8_t) (std::rand() % 255));

      star->startSize = 1;
    }
  }

  void update() {
    float increment = 0.01;

    mutex.lock();

    for (auto& star : stars) {
      star->currentX += (star->currentX - sf::VideoMode::getFullscreenModes()[0].width * 0.5) * increment;
      star->currentY += (star->currentY - sf::VideoMode::getFullscreenModes()[0].height * 0.5) * increment;
      star->currentSize = std::min(star->currentSize * 1.01, (double) MAX_SIZE);

      if ((star->currentX < 0 - (star->currentSize * 0.5)) ||
          (star->currentX > sf::VideoMode::getFullscreenModes()[0].width + (star->currentSize * 0.5)) ||
          (star->currentY < 0 - (star->currentSize * 0.5)) ||
          (star->currentY > sf::VideoMode::getFullscreenModes()[0].height + (star->currentSize * 0.5))) {
        star->currentX = std::rand() % sf::VideoMode::getFullscreenModes()[0].width;
        star->currentY = std::rand() % sf::VideoMode::getFullscreenModes()[0].height;
        star->currentSize = star->startSize;
      }
    }

    mutex.unlock();
  }

  void draw(sf::RenderWindow& window) override {
    mutex.lock();

    sf::VertexArray vertexArray(sf::PrimitiveType::Quads, 4 * NUM_STARS);

    for (std::size_t index = 0; index < NUM_STARS; ++index) {
      vertexArray[index * 4 + 0].position = sf::Vector2f(stars[index]->currentX - (stars[index]->currentSize * 0.5), stars[index]->currentY - (stars[index]->currentSize * 0.5));
      vertexArray[index * 4 + 0].color = stars[index]->colour;
      vertexArray[index * 4 + 1].position = sf::Vector2f(stars[index]->currentX - (stars[index]->currentSize * 0.5), stars[index]->currentY + (stars[index]->currentSize * 0.5));
      vertexArray[index * 4 + 1].color = stars[index]->colour;
      vertexArray[index * 4 + 2].position = sf::Vector2f(stars[index]->currentX + (stars[index]->currentSize * 0.5), stars[index]->currentY + (stars[index]->currentSize * 0.5));
      vertexArray[index * 4 + 2].color = stars[index]->colour;
      vertexArray[index * 4 + 3].position = sf::Vector2f(stars[index]->currentX + (stars[index]->currentSize * 0.5), stars[index]->currentY - (stars[index]->currentSize * 0.5));
      vertexArray[index * 4 + 3].color = stars[index]->colour;
    }

    mutex.unlock();

    window.draw(vertexArray);
  }

 private:
  std::array<Star*, NUM_STARS> stars;
  std::mutex mutex;
};

int main(const int    argc,
         const char** argv) {
  GD gd;

  Starfield starfield("starfield", gd);

  EV ev(starfield);
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

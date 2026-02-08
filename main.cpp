#include "gameoflife.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>

class TileMap : public sf::Drawable, public sf::Transformable {
public:
  TileMap(sf::Vector2u windowSize, int screenWidth,
          int screenHeight) {

    width = screenWidth;
    height = screenHeight;
    sf::Vector2u tileSize = sf::Vector2u(
        {windowSize.x / width, windowSize.y / height});
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(width * height * 6);

    setPos(windowSize);
  }
  sf::Vector2i globalToLocal(sf::Vector2i global,
                             sf::Vector2u windowSize) {

    sf::Vector2f tileSize = sf::Vector2f(
        {static_cast<float>(windowSize.x) / width,
         static_cast<float>(windowSize.y) / height});
    return {static_cast<int>(global.x / tileSize.x),
            static_cast<int>(global.y / tileSize.y)};
  }
  void setColor(std::vector<bool> *state) {

#pragma omp parallel for
    for (int i = 0; i < width * height; i++) {
      sf::Vertex *triangles = &m_vertices[(i) * 6];
      for (int j = 0; j < 6; j++) {
        if ((*state)[i] == true) {
          if (j % 2) {
            triangles[j].color = sf::Color::White;
          } else {

            triangles[j].color = sf::Color::White;
          }
        } else {
          if (j % 2) {
            triangles[j].color = sf::Color::Black;
          } else {

            triangles[j].color = sf::Color::Black;
          }
        }
      }
    }
  }

  void setPos(sf::Vector2u windowSize) {

    sf::Vector2f tileSize = sf::Vector2f(
        {static_cast<float>(windowSize.x) / width,
         static_cast<float>(windowSize.y) / height});

#pragma omp parallel for
    for (unsigned int i = 0; i < width; ++i) {
      for (unsigned int j = 0; j < height; ++j) {

        sf::Vertex *triangles =
            &m_vertices[(i + j * width) * 6];

        // define the 6 corners of the two triangles
        triangles[0].position =
            sf::Vector2f(i * tileSize.x, j * tileSize.y);
        triangles[1].position =
            sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
        triangles[2].position =
            sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
        triangles[3].position =
            sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
        triangles[4].position =
            sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
        triangles[5].position = sf::Vector2f(
            (i + 1) * tileSize.x, (j + 1) * tileSize.y);
        if (i * width + j == 899) {
          std::cout << triangles[5].position.x << " "
                    << triangles[5].position.y << "\n tilezise"
                    << tileSize.x << " " << tileSize.y << "\n";
        }
      }
    }
  }

private:
  void draw(sf::RenderTarget &target,
            sf::RenderStates states) const override {
    // apply the transform
    states.transform *= getTransform();

    // draw the vertex array
    target.draw(m_vertices, states);
  }
  int width, height;

  sf::VertexArray m_vertices;
  sf::Texture m_tileset;
};

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cout
        << "Unexpected Arguments use: gol width height time\n";
    return -1;
  }
  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  int sleep = atoi(argv[3]);
  gol::Grid grid = gol::Grid(width, height);
  sf::RenderWindow window(sf::VideoMode({200, 200}),
                          "SFML works!");

  TileMap map(window.getSize(), width, height);

  grid.Print();
  bool isRunning = false;
  bool isStep = false;
  for (int i = 10; i < width - 10; i++) {
    grid.SetCell(i, height / 2, true);
  }
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (const auto *keyPressed =
              event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Space) {

          isRunning = !isRunning;
        }
        if (keyPressed->code == sf::Keyboard::Key::P) {
          isStep = true;
        }
      }

      else if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (const auto *resized =
                     event->getIf<sf::Event::Resized>()) {
        float w = static_cast<float>(resized->size.x);
        float h = static_cast<float>(resized->size.y);
        std::cout << "new width: " << resized->size.x
                  << std::endl;
        std::cout << "new height: " << resized->size.y
                  << std::endl;
        sf::View view(sf::FloatRect({0, 0}, {w, h}));
        window.setView(view);
        map.setPos(window.getSize());
      } else if (const auto *mb =
                     event->getIf<
                         sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
          sf::Vector2i globalPos = mb->position;
          auto size = window.getSize();
          auto localPos = map.globalToLocal(globalPos, size);

          grid.toggleCell(localPos.x, localPos.y);
        }
      }
    }

    map.setColor(&grid.states);
    window.clear();
    window.draw(map);
    window.display();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
    }
    if (isRunning or isStep) {
      isStep = false;
      sf::sleep(sf::seconds(sleep));
      for (int i = 0; i < 1; i++) {
        grid.Step();
      }
    }
  }
}

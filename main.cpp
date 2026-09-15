#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

sf::Vector3f cube[8] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                        {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};

int edges[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                    {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

int faces[6][4] = {
    {0, 3, 2, 1}, {4, 5, 6, 7}, {0, 4, 7, 3},
    {1, 2, 6, 5}, {0, 1, 5, 4}, {3, 7, 6, 2},
};

sf::Vector3f rotate(sf::Vector3f v, float ay, float ax) {
  float x = v.x * std::cos(ay) + v.z * std::sin(ay);
  float z = -v.x * std::sin(ay) + v.z * std::cos(ay);
  float y = v.y * std::cos(ax) - z * std::sin(ax);
  z = v.y * std::sin(ax) + z * std::cos(ax);
  return {x, y, z};
}

sf::Vector2f project(sf::Vector3f v, float f, sf::Vector2f c) {
  return {c.x + f * v.x / v.z, c.y - f * v.y / v.z};
}

float dot(sf::Vector3f a, sf::Vector3f b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}
sf::Vector3f cross(sf::Vector3f a, sf::Vector3f b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
sf::Vector3f normalize(sf::Vector3f v) { return v / std::sqrt(dot(v, v)); }

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "ASCII RENDERING CUBE");
  window.setFramerateLimit(60);

  sf::Clock clock;
  clock.start();

  sf::Font font("Courier.ttf");
  const unsigned fontSize = 16;
  const float cw = font.getGlyph('M', fontSize, false).advance;
  const float ch = font.getLineSpacing(fontSize);
  const int cols = static_cast<int>(800 / cw),
            rows = static_cast<int>(600 / ch);

  sf::RenderTexture rt({800u, 600u});
  sf::Text text(font, "", fontSize);
  const std::string ramp = " .'-:;=+*xX%#8&@";

  while (window.isOpen()) {
    while (const auto event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }
    rt.clear(sf::Color::Black);

    float t = clock.getElapsedTime().asSeconds();
    sf::Vector3f cam[8];
    sf::Vector2f p[8];
    for (int i = 0; i < 8; ++i) {
      cam[i] = rotate(cube[i], t, t * 0.7f);
      cam[i].z += 4.f;
      p[i] = project(cam[i], 400.f, {400.f, 300.f});
    }

    sf::Vector3f lightDir = normalize({-1.f, 1.f, -1.f});
    for (auto &f : faces) {
      sf::Vector3f a = cam[f[0]], b = cam[f[1]], c = cam[f[2]];
      sf::Vector3f n = normalize(cross(b - a, c - a));

      if (dot(n, a) > 0)
        continue;

      float k = 0.2f + 0.8f * std::max(0.f, dot(n, lightDir));
      auto g = static_cast<std::uint8_t>(k * 255);

      sf::ConvexShape quad(4);
      for (int i = 0; i < 4; ++i)
        quad.setPoint(i, p[f[i]]);
      quad.setFillColor({g, g, g});
      rt.draw(quad);
    }

    sf::Image img = rt.getTexture().copyToImage();
    std::string frame;
    frame.reserve((cols + 1) * rows);
    for (int r = 0; r < rows; ++r) {
      int y0 = int(r * ch), y1 = int((r + 1) * ch);
      for (int c = 0; c < cols; ++c) {
        int x0 = int(c * cw), x1 = int((c + 1) * cw);
        float sum = 0;
        for (int y = y0; y < y1; ++y)
          for (int x = x0; x < x1; ++x)
            sum += img.getPixel({unsigned(x), unsigned(y)}).r;
        float avg = sum / ((x1 - x0) * (y1 - y0) * 255.f);
        frame += ramp[int(avg * (ramp.size() - 1) + 0.5f)];
      }
      frame += '\n';
    }

    window.clear(sf::Color::Black);
    text.setString(frame);
    window.draw(text);
    window.display();
  }

  return 0;
}

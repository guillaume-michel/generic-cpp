#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

struct Shape {
  virtual void draw() = 0;
  virtual float perimeter() = 0;
  virtual ~Shape() = default;
};

struct Circle: Shape {
  Circle(float radius): radius(radius) {}

  virtual ~Circle() = default;

  virtual void draw() override {
    std::cout << "Draw Circle" << std::endl;
  }

  virtual float perimeter() override {
    return 2 * (float)M_PI * radius;
  }

  float radius;
};

struct Square: Shape {
  Square(float length): length(length) {}

  virtual ~Square() = default;

  virtual void draw() override {
    std::cout << "Draw Square" << std::endl;
  }

  virtual float perimeter() override {
    return 4 * length;
  }

  float length;
};

int main(int argc, char* argv[]) {

  std::vector<std::unique_ptr<Shape>> shapes;
  shapes.push_back(std::make_unique<Circle>(1.0f));
  shapes.push_back(std::make_unique<Square>(1.0f));

  for (auto& shape : shapes) {
    shape->draw();
    std::cout << "perimeter: " << shape->perimeter() << std::endl;
  }

  return 0;
}

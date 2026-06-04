#include "vector2d.h"
#include <cmath>

// Aquí es donde se le da el cuerpo real a cada función usando Vector2D::
Vector2D::Vector2D() : x(0.0f), y(0.0f) {}

Vector2D::Vector2D(float _x, float _y) : x(_x), y(_y) {}

Vector2D Vector2D::operator+(const Vector2D& otro) const {
    return Vector2D(x + otro.x, y + otro.y);
}

Vector2D Vector2D::operator-(const Vector2D& otro) const {
    return Vector2D(x - otro.x, y - otro.y);
}

Vector2D Vector2D::operator*(float escalar) const {
    return Vector2D(x * escalar, y * escalar);
}

float Vector2D::punto(const Vector2D& otro) const {
    return (x * otro.x) + (y * otro.y);
}

float Vector2D::magnitud() const {
    return std::sqrt((x * x) + (y * y));
}

Vector2D Vector2D::normalizar() const {
    float mag = magnitud();
    if (mag > 0.0f) {
        return Vector2D(x / mag, y / mag);
    }
    return Vector2D(0.0f, 0.0f);
}
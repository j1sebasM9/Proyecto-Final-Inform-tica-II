#ifndef VECTOR2D_H
#define VECTOR2D_H


class Vector2D {
public:
    float x;
    float y;

    // Solo la "promesa" de que existen (terminan en punto y coma)
    Vector2D();
    Vector2D(float _x, float _y);

    Vector2D operator+(const Vector2D& otro) const;
    Vector2D operator-(const Vector2D& otro) const;
    Vector2D operator*(float escalar) const;
    float punto(const Vector2D& otro) const;
    float magnitud() const;
    Vector2D normalizar() const;
};

#endif // VECTOR2D_H

#ifndef OBSTACULO_H
#define OBSTACULO_H


#include <QGraphicsItem>
#include <QPixmap>
#include "Vector2D.h"

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class Combatiente;

enum TipoObstaculo {
    HIELO,
    FUEGO,
    RAYO_MODERADOR,
    RAYO_ELECTRICO
};

class Obstaculo : public QGraphicsItem {
private:
    TipoObstaculo tipo;
    Vector2D posicion;
    float radio;
    int daño;
    bool activo;
    bool modoSumo;

    float cronometroRayo;
    Vector2D zonaObjetivo;
    bool advertenciaActiva;
    float tiempoAviso;
    Vector2D velocidadMovimiento;

    // Sprites
    QPixmap spriteActivo;
    QPixmap spriteAdvertencia;

    // Método encapsulado para limpieza del constructor
    void cargarSprites();

public:
    Obstaculo(TipoObstaculo t, Vector2D pos, float r, float _tiempoAviso = 0.0f, bool _modoSumo = false);

    // Métodos de Qt
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setVelocidadMovimiento(const Vector2D& v) { velocidadMovimiento = v; }

    // Físicas y lógica
    void actualizarObstaculo(float dt);
    void aplicarEfecto(Combatiente* afectado, Combatiente* rival);

    // Getters
    bool isActivo() const;
    TipoObstaculo getTipo() const;
    float getRadio() const;
    Vector2D getPosicion() const;
};

#endif // OBSTACULO_H

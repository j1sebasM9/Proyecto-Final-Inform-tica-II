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

    float friccionModificada;
    float cronometroRayo;
    Vector2D zonaObjetivo;
    bool advertenciaActiva;
    float tiempoAviso;
    Vector2D velocidadMovimiento;

    QPixmap spriteActivo;
    QPixmap spriteAdvertencia;

    void cargarSprites();

public:
    Obstaculo(TipoObstaculo t, Vector2D pos, float r, float tiempoAviso = 0.0f);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setVelocidadMovimiento(const Vector2D& v) { velocidadMovimiento = v; }

    void actualizarObstaculo(float dt);
    void aplicarEfecto(Combatiente* afectado, Combatiente* rival);

    bool isActivo() const;
    TipoObstaculo getTipo() const;
    float getRadio() const;
    Vector2D getPosicion() const;
};

#endif

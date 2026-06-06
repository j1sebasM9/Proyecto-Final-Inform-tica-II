#ifndef COMBATIENTE_H
#define COMBATIENTE_H


#include <QGraphicsEllipseItem>
#include <QColor>
#include <QPixmap>   // ¡NUEVO! Para manejar imágenes
#include <QPainter>  // ¡NUEVO! Para redibujar el sprite
#include "Vector2D.h"

class Combatiente : public QGraphicsEllipseItem {
protected:
    Vector2D posicion;
    Vector2D posicionOrigen;
    Vector2D velocidad;
    float masa;
    float radio;
    float friccionSuperficie;
    float anguloOrientacion;
    float velocidadAngular;
    bool estaVivo;

    // Reglas de negocio
    int vidas;
    int puntos;

    // ¡NUEVO! Variable para almacenar la imagen del Pokémon
    QPixmap pixmapSprite;
    int direccionVisual; // 1 derecha, -1 izquierda

public:
    // MODIFICADO: Cambiamos 'QColor color' por 'QString rutaSprite'
    Combatiente(float _x, float _y, float _r, float _m, QString rutaSprite);

    // Destructor virtual
    virtual ~Combatiente() = default;

    // ¡NUEVO! Interceptamos el método paint para que pinte el Sprite en vez del círculo base
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    void cambiarSprite(const QString& rutaSprite);
    void setDireccionVisual(int direccion);

    // Métodos polimórficos puros obligatorios
    virtual void mover(int accion) = 0;
    virtual void aplicarFisicas(float dt) = 0;

    // Métodos de impacto físico
    void recibirImpulsoLineal(const Vector2D& J);
    void recibirImpulsoAngular(float torque);

    // Reglas de negocio
    void perderVida(int cantidad = 1) {
        vidas -= cantidad;
        if (vidas <= 0) {
            vidas = 0;
            estaVivo = false;
        }
    }

    void regresarAlOrigen();
    void ganarPunto();
    void resetVidas();

    // Getters y Setters
    int getVidas() const;
    int getPuntos() const;
    void setPuntos(int p);
    const Vector2D& getPosicion() const;
    void setPosicion(const Vector2D& p);
    const Vector2D& getVelocidad() const;
    void setVelocidad(const Vector2D& v);
    float getRadio() const;
    float getMasa() const;
    void setFriccionSuperficie(float f);
};
#endif // COMBATIENTE_H

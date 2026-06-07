#include "Combatiente.h"
#include <QPen>
#include <QBrush>
#include <QDebug> // ¡NUEVO! Para ver alertas en consola si una imagen no carga

// Constructor actualizado
Combatiente::Combatiente(float _x, float _y, float _r, float _m, QString rutaSprite)
    : QGraphicsEllipseItem(-_r, -_r, _r * 2, _r * 2), // El área de colisión sigue siendo circular
    posicion(_x, _y),
    posicionOrigen(_x, _y),
    velocidad(0.0f, 0.0f),
    masa(_m),
    radio(_r),
    friccionSuperficie(0.15f),
    anguloOrientacion(0.0f),
    velocidadAngular(0.0f),
    estaVivo(true),
    vidas(3),
    puntos(0),
    direccionVisual(1)
{
    setPos(_x, _y);

    // --- INTENTAR CARGAR EL SPRITE DEL PERSONAJE ---
    if (!rutaSprite.isEmpty()) {
        pixmapSprite.load(rutaSprite);
        if (pixmapSprite.isNull()) {
            qDebug() << "ALERTA: No se pudo cargar el sprite en:" << rutaSprite;
        }
    }

    // Hacemos que el borde original de QGraphicsEllipseItem sea invisible
    // para que no interfiera con nuestra imagen
    setPen(QPen(Qt::transparent, 0));
}

// ¡NUEVO MÉTODO MAGIC! Controla cómo se ve el personaje en pantalla
QRectF Combatiente::boundingRect() const {
    return QRectF(-radio * 1.5f,
                  -radio * 1.9f,
                  radio * 3.0f,
                  radio * 3.1f);
}

void Combatiente::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!pixmapSprite.isNull()) {
        painter->save();

        if (direccionVisual < 0) {
            painter->scale(-1, 1);
        }

        QRectF zonaDibujo(-radio * 1.45f,
                          -radio * 1.80f,
                          radio * 2.90f,
                          radio * 2.90f);

        painter->drawPixmap(zonaDibujo, pixmapSprite, pixmapSprite.rect());

        painter->restore();
    }
    else {
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(brush());
        painter->drawEllipse(-radio, -radio, radio * 2, radio * 2);
    }
}

void Combatiente::cambiarSprite(const QString& rutaSprite) {
    if (rutaSprite.isEmpty()) return;

    QPixmap nuevoSprite;
    nuevoSprite.load(rutaSprite);

    if (nuevoSprite.isNull()) {
        qDebug() << "ALERTA: No se pudo cargar el sprite en:" << rutaSprite;
        return;
    }

    pixmapSprite = nuevoSprite;
    update();
}

void Combatiente::setDireccionVisual(int direccion) {
    direccionVisual = direccion < 0 ? 1 : -1;
    update();
}

void Combatiente::regresarAlOrigen() {
    posicion = posicionOrigen;
    velocidad = Vector2D(0.0f, 0.0f);
    velocidadAngular = 0.0f;

    setPos(posicion.x, posicion.y);
    update();
}

void Combatiente::recibirImpulsoLineal(const Vector2D& J) {
    velocidad = velocidad + (J * (1.0f / masa));
}

void Combatiente::recibirImpulsoAngular(float torque) {
    velocidadAngular += torque;
}

void Combatiente::ganarPunto() {
    puntos++;
}

void Combatiente::resetVidas() {
    vidas = 3;
    estaVivo = true;
}

int Combatiente::getVidas() const {
    return vidas;
}

int Combatiente::getPuntos() const {
    return puntos;
}

void Combatiente::setPuntos(int p) {
    puntos = p;
}

const Vector2D& Combatiente::getPosicion() const {
    return posicion;
}

void Combatiente::setPosicion(const Vector2D& p) {
    posicion = p;
    setPos(p.x, p.y);
}

const Vector2D& Combatiente::getVelocidad() const {
    return velocidad;
}

void Combatiente::setVelocidad(const Vector2D& v) {
    velocidad = v;
}

float Combatiente::getRadio() const {
    return radio;
}

float Combatiente::getMasa() const {
    return masa;
}

void Combatiente::setFriccionSuperficie(float f) {
    friccionSuperficie = f;
}

#include "Obstaculo.h"
#include "Combatiente.h"
#include "PeleadorElectrico.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <cmath>

Obstaculo::Obstaculo(TipoObstaculo t, Vector2D pos, float r, float _tiempoAviso)
    : tipo(t),
    posicion(pos),
    radio(r),
    daño(0),
    activo(true),
    friccionModificada(0.01f),
    cronometroRayo(_tiempoAviso),
    zonaObjetivo(pos),
    advertenciaActiva(t == RAYO_MODERADOR || t == FUEGO),
    tiempoAviso(_tiempoAviso),
    velocidadMovimiento(0.0f, 0.0f)
{
    if (t == FUEGO) {
        daño = 2;
    }
    else if (t == RAYO_MODERADOR) {
        daño = 1;
    }
    else {
        daño = 0;
    }

    cargarSprites();

    setPos(posicion.x, posicion.y);
    setZValue(2);
}

void Obstaculo::cargarSprites() {
    QString rutaBase = "C:/Users/USER/Documents/videojuego_POO_sumo/videojuego_POO_sumo/spriters/obstaculos/";

    if (tipo == HIELO) {
        spriteActivo.load(rutaBase + "hielo.png");
    }
    else if (tipo == FUEGO) {
        spriteActivo.load(rutaBase + "fuego.png");
        spriteAdvertencia.load(rutaBase + "fuego_aviso.png");
    }
    else if (tipo == RAYO_MODERADOR) {
        spriteActivo.load(rutaBase + "rayo_moderador.png");
        spriteAdvertencia.load(rutaBase + "rayo_aviso.png");
    }
    else if (tipo == RAYO_ELECTRICO) {
        spriteActivo.load(rutaBase + "rayo_electrico.png");
    }

    if (spriteActivo.isNull()) {
        qDebug() << "ALERTA: No se pudo cargar sprite de obstaculo. Tipo:" << tipo;
    }
}

QRectF Obstaculo::boundingRect() const {
    if (tipo == RAYO_ELECTRICO) {
        return QRectF(-radio * 2.0f,
                      -radio * 0.9f,
                      radio * 4.0f,
                      radio * 1.8f);
    }

    if (tipo == RAYO_MODERADOR) {
        return QRectF(-radio,
                      -radio * 1.6f,
                      radio * 2.0f,
                      radio * 3.2f);
    }

    return QRectF(-radio, -radio, radio * 2, radio * 2);
}

void Obstaculo::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *option,
                      QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    if (advertenciaActiva) {
        if (!spriteAdvertencia.isNull()) {
            QRectF zonaAviso(-radio, -radio, radio * 2.0f, radio * 2.0f);
            painter->drawPixmap(zonaAviso, spriteAdvertencia, spriteAdvertencia.rect());
        }
        else {
            painter->setBrush(Qt::NoBrush);

            if (tipo == FUEGO) {
                painter->setPen(QPen(QColor(255, 80, 0), 3, Qt::DashLine));
            }
            else {
                painter->setPen(QPen(Qt::yellow, 3, Qt::DashLine));
            }

            painter->drawEllipse(-radio, -radio, radio * 2, radio * 2);
        }

        return;
    }

    if (!spriteActivo.isNull()) {
        if (tipo == RAYO_ELECTRICO) {
            painter->save();

            /*
             * IMPORTANTE:
             * Cambia este valor si tu sprite del rayo mira hacia la derecha.
             *
             * true  = el sprite original mira hacia la izquierda.
             * false = el sprite original mira hacia la derecha.
             */
            const bool SPRITE_RAYO_MIRA_IZQUIERDA = true;

            bool rayoSeMueveDerecha = velocidadMovimiento.x >= 0.0f;
            bool debeVoltearse = false;

            if (SPRITE_RAYO_MIRA_IZQUIERDA && rayoSeMueveDerecha) {
                debeVoltearse = true;
            }
            else if (!SPRITE_RAYO_MIRA_IZQUIERDA && !rayoSeMueveDerecha) {
                debeVoltearse = true;
            }

            if (debeVoltearse) {
                painter->scale(-1, 1);
            }

            QRectF zonaRayo(-radio * 2.0f,
                            -radio * 0.9f,
                            radio * 4.0f,
                            radio * 1.8f);

            painter->drawPixmap(zonaRayo, spriteActivo, spriteActivo.rect());

            painter->restore();
            return;
        }

        if (tipo == RAYO_MODERADOR) {
            QRectF zonaRayoModerador(-radio * 0.75f,
                                     -radio * 1.5f,
                                     radio * 1.5f,
                                     radio * 3.0f);

            painter->drawPixmap(zonaRayoModerador, spriteActivo, spriteActivo.rect());
            return;
        }

        QRectF zonaNormal(-radio, -radio, radio * 2.0f, radio * 2.0f);
        painter->drawPixmap(zonaNormal, spriteActivo, spriteActivo.rect());
        return;
    }

    if (tipo == FUEGO) {
        painter->setBrush(QBrush(QColor(255, 69, 0, 130)));
        painter->setPen(QPen(Qt::red, 3));
        painter->drawEllipse(-radio, -radio, radio * 2, radio * 2);
    }
    else if (tipo == HIELO) {
        painter->setBrush(QBrush(QColor(0, 191, 255, 90)));
        painter->setPen(QPen(Qt::darkBlue, 2));
        painter->drawEllipse(-radio, -radio, radio * 2, radio * 2);
    }
    else if (tipo == RAYO_MODERADOR) {
        painter->setBrush(QBrush(Qt::yellow));
        painter->setPen(QPen(QColor(255, 165, 0), 2));
        painter->drawRect(-15, -radio, 30, radio * 2);
    }
    else if (tipo == RAYO_ELECTRICO) {
        painter->setBrush(QBrush(Qt::cyan));
        painter->setPen(QPen(Qt::blue, 2));
        painter->drawEllipse(-radio, -radio, radio * 2, radio * 2);
    }
}

void Obstaculo::actualizarObstaculo(float dt) {
    if (!activo) return;

    if (tipo == RAYO_ELECTRICO) {
        posicion = posicion + (velocidadMovimiento * dt);
        setPos(posicion.x, posicion.y);

        if (std::abs(posicion.x) > 600.0f || std::abs(posicion.y) > 400.0f) {
            activo = false;
            setVisible(false);
        }
    }

    if (tipo == FUEGO) {
        if (advertenciaActiva) {
            tiempoAviso -= dt;

            if (tiempoAviso <= 0.0f) {
                advertenciaActiva = false;
                tiempoAviso = 15.0f;
                update();
            }
        }
        else {
            tiempoAviso -= dt;

            if (tiempoAviso <= 0.0f) {
                activo = false;
                setVisible(false);
            }
        }
    }

    if (tipo == RAYO_MODERADOR) {
        if (advertenciaActiva) {
            tiempoAviso -= dt;

            if (tiempoAviso <= 0.0f) {
                advertenciaActiva = false;
                tiempoAviso = 0.5f;
                update();
            }
        }
        else {
            tiempoAviso -= dt;

            if (tiempoAviso <= 0.0f) {
                activo = false;
                setVisible(false);
            }
        }
    }
}

void Obstaculo::aplicarEfecto(Combatiente* afectado, Combatiente* rival) {
    if (!activo || !afectado) return;

    if (tipo == HIELO) {
        afectado->setFriccionSuperficie(0.0f);

        Vector2D velActual = afectado->getVelocidad();

        if (velActual.magnitud() > 1.0f) {
            Vector2D direccionMovimiento = velActual.normalizar();
            float fuerzaImpulsoHielo = 70.0f;
            afectado->recibirImpulsoLineal(direccionMovimiento * fuerzaImpulsoHielo);
        }
    }
    else if (tipo == FUEGO) {
        if (!advertenciaActiva) {
            afectado->perderVida(1);

            if (rival) {
                rival->ganarPunto();
            }

            afectado->regresarAlOrigen();
            activo = false;
            setVisible(false);
        }
    }
    else if (tipo == RAYO_MODERADOR) {
        if (!advertenciaActiva) {
            afectado->perderVida(1);

            if (rival) {
                rival->ganarPunto();
            }

            afectado->regresarAlOrigen();
            activo = false;
            setVisible(false);
        }
    }
    else if (tipo == RAYO_ELECTRICO) {
        if (dynamic_cast<PeleadorElectrico*>(afectado) != nullptr) {
            return;
        }

        float masaProyectil = 6.0f;
        float masaAfectado = 1.0f;

        Vector2D vProyectil = velocidadMovimiento;
        Vector2D vAfectado = afectado->getVelocidad();

        float masaTotal = masaProyectil + masaAfectado;

        Vector2D vFinal = (vProyectil * masaProyectil + vAfectado * masaAfectado) * (1.0f / masaTotal);
        Vector2D deltaV = vFinal - vAfectado;

        afectado->recibirImpulsoLineal(deltaV * 20.0f);

        activo = false;
        setVisible(false);
    }
}

bool Obstaculo::isActivo() const {
    return activo;
}

TipoObstaculo Obstaculo::getTipo() const {
    return tipo;
}

float Obstaculo::getRadio() const {
    return radio;
}

Vector2D Obstaculo::getPosicion() const {
    return posicion;
}

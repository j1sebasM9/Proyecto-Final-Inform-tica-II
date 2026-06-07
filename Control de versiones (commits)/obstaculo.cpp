#include "Obstaculo.h"
#include "Combatiente.h"
#include "PeleadorElectrico.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <cmath>

Obstaculo::Obstaculo(TipoObstaculo t, Vector2D pos, float r, float _tiempoAviso, bool _modoSumo)
    : tipo(t)
    , posicion(pos)
    , radio(r)
    , daño(0)
    , activo(true)
    , cronometroRayo(_tiempoAviso)
    , zonaObjetivo(pos)
    , advertenciaActiva(t == RAYO_MODERADOR || t == FUEGO)
    , tiempoAviso(_tiempoAviso)
    , velocidadMovimiento(0.0f, 0.0f)
    , modoSumo(_modoSumo)
{
    if      (t == FUEGO)         daño = 2;
    else if (t == RAYO_MODERADOR) daño = 1;

    cargarSprites();
    setPos(posicion.x, posicion.y);
    if (t == HIELO) {
        setZValue(-0.5);           // Siempre en el piso
    } else if (_modoSumo) {
        setZValue(8);              // Nivel 1 (vista cenital): encima de jugadores
    } else {
        setZValue(3);              // Nivel 2 (vista lateral): detrás de jugadores
    }
}

// =============================================================================
//  SPRITES — carpeta obstaculos/ separada para nivel 2
// =============================================================================

void Obstaculo::cargarSprites() {
    const QString baseN1 = "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/obstaculos_nivel_1/";
    const QString baseN2 = "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/obstaculos_nivel_2/";

    if (tipo == HIELO) {
        spriteActivo.load(baseN1 + "hielo.png");
    }
    else if (tipo == FUEGO) {
        spriteActivo.load(baseN2 + "fuego.png");
        spriteAdvertencia.load(baseN2 + "fuego_aviso.png");
    }
    else if (tipo == RAYO_MODERADOR) {
        spriteActivo.load(baseN2 + "rayo_moderador.png");
        spriteAdvertencia.load(baseN2 + "rayo_aviso.png");
    }
    else if (tipo == RAYO_ELECTRICO) {
        if (modoSumo) {
            spriteActivo.load(baseN1 + "rayo_pikachu.png");
        } else {
            spriteActivo.load(baseN2 + "rayo_electrico.png");
        }
    }

    if (spriteActivo.isNull()) {
        qDebug() << "ALERTA: No se pudo cargar sprite de obstáculo tipo:" << tipo;
    }

    int diametro = int(radio * 2);
    if (!spriteActivo.isNull()) {
        spriteActivo = spriteActivo.scaled(diametro, diametro, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (!spriteAdvertencia.isNull()) {
        spriteAdvertencia = spriteAdvertencia.scaled(diametro, diametro, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
}

// =============================================================================
//  COLISIÓN
// =============================================================================

QRectF Obstaculo::boundingRect() const {
    if (tipo == RAYO_ELECTRICO) {
        if (modoSumo) {
            // Factor 1.5f → 50% más grande que antes
            return QRectF(-radio * 1.5f, -radio * 1.5f, radio * 3.0f, radio * 3.0f);
        } else {
            return QRectF(-radio * 2.0f, -radio * 0.9f, radio * 4.0f, radio * 1.8f);
        }
    }
    if (tipo == RAYO_MODERADOR) {
        // Más ancho (1.5f) y más alto (2.2f) que antes
        return QRectF(-radio * 1.5f, -radio * 2.2f, radio * 3.0f, radio * 4.4f);
    }
    if (tipo == FUEGO) {
        // Factor 1.5f → 50% más grande que el default anterior
        return QRectF(-radio * 1.5f, -radio * 1.5f, radio * 3.0f, radio * 3.0f);
    }

    // Default — Hielo y cualquier otro sin cambios
    return QRectF(-radio, -radio, radio * 2.0f, radio * 2.0f);
}

// =============================================================================
//  PINTURA
// =============================================================================

void Obstaculo::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option); Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing);

    // --- Fase de advertencia ---
    if (advertenciaActiva) {
        if (!spriteAdvertencia.isNull()) {
            painter->drawPixmap(-radio, -radio, spriteAdvertencia);
        } else {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(tipo == FUEGO ? QColor(255, 80, 0) : Qt::yellow, 3, Qt::DashLine));
            painter->drawEllipse(-radio, -radio, radio * 2, radio * 2);
        }
        return;
    }

    // --- Sprite activo ---
    if (!spriteActivo.isNull()) {

        // 1. Dibujado especial para el Rayo de Pikachu
        if (tipo == RAYO_ELECTRICO) {
            painter->save();
            if (velocidadMovimiento.x >= 0.0f) painter->scale(-1, 1); // Voltear si va a la derecha

            if (modoSumo) {
                // Factor 1.5f — debe coincidir con el boundingRect de arriba
                float factor = 1.5f;
                QRectF zona(-radio * factor, -radio * factor,
                            radio * factor * 2, radio * factor * 2);
                painter->drawPixmap(zona, spriteActivo, spriteActivo.rect());
            } else {
                QRectF zona(-radio * 2.0f, -radio * 0.9f, radio * 4.0f, radio * 1.8f);
                painter->drawPixmap(zona, spriteActivo, spriteActivo.rect());
            }

            painter->restore();
            return;
        }

        // 2. Dibujado especial para el Rayo Moderador
        if (tipo == RAYO_MODERADOR) {
            // factorAncho y factorAlto deben coincidir con el boundingRect de arriba
            float factorAncho = 1.5f;
            float factorAlto  = 2.2f;
            QRectF zona(-radio * factorAncho, -radio * factorAlto,
                        radio * factorAncho * 2, radio * factorAlto * 2);
            painter->drawPixmap(zona, spriteActivo, spriteActivo.rect());
            return;
        }

        // 3. Dibujado especial para el Fuego
        if (tipo == FUEGO) {
            // Factor 1.5f — debe coincidir con el boundingRect de arriba
            float factor = 1.5f;
            QRectF zona(-radio * factor, -radio * factor,
                        radio * factor * 2, radio * factor * 2);
            painter->drawPixmap(zona, spriteActivo, spriteActivo.rect());
            return;
        }

        // 4. Dibujado normal para todos los demás (Hielo, etc) — sin cambios
        painter->drawPixmap(-radio, -radio, spriteActivo);
        return;
    }

    // --- Geometría de respaldo (Por si falla la carga de imágenes) ---
    if (tipo == FUEGO) {
        painter->setBrush(QBrush(QColor(255, 69, 0, 130)));
        painter->setPen(QPen(Qt::red, 3));
    } else if (tipo == HIELO) {
        painter->setBrush(QBrush(QColor(0, 191, 255, 90)));
        painter->setPen(QPen(Qt::darkBlue, 2));
    } else if (tipo == RAYO_MODERADOR) {
        painter->setBrush(QBrush(Qt::yellow));
        painter->setPen(QPen(QColor(255, 165, 0), 2));
    } else {
        painter->setBrush(QBrush(Qt::cyan));
        painter->setPen(QPen(Qt::blue, 2));
    }
    painter->drawEllipse(-radio, -radio, radio * 2, radio * 2);
}

// =============================================================================
//  ACTUALIZACIÓN
// =============================================================================

void Obstaculo::actualizarObstaculo(float dt) {
    if (!activo) return;

    if (tipo == RAYO_ELECTRICO) {
        posicion = posicion + (velocidadMovimiento * dt);
        setPos(posicion.x, posicion.y);
        if (std::abs(posicion.x) > 450.0f || std::abs(posicion.y) > 450.0f) {
            activo = false;
            setVisible(false);
        }
        return;
    }

    // FUEGO y RAYO_MODERADOR comparten el mismo ciclo: aviso → activo → expirar
    if (tipo == FUEGO || tipo == RAYO_MODERADOR) {
        tiempoAviso -= dt;
        if (tiempoAviso <= 0.0f) {
            if (advertenciaActiva) {
                advertenciaActiva = false;
                tiempoAviso = (tipo == FUEGO) ? 15.0f : 0.5f;
                update();
            } else {
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
            Vector2D direccion = velActual.normalizar();
            float fuerzaImpulsoHielo = 70.0f;
            afectado->recibirImpulsoLineal(direccion * fuerzaImpulsoHielo);
        }
        return;
    }

    if (tipo == FUEGO || tipo == RAYO_MODERADOR) {
        if (!advertenciaActiva) {
            afectado->perderVida(1);
            if (rival) rival->ganarPunto();
            afectado->regresarAlOrigen();
            activo = false;
            setVisible(false);
        }
        return;
    }

    if (tipo == RAYO_ELECTRICO) {
        // El electrico es inmune a sus propios rayos
        if (dynamic_cast<PeleadorElectrico*>(afectado)) return;

        // Física de impulso arcade (valores de la versión original del nivel 2)
        const float masaProyectil  = 85.0f;
        const float masaAfectado   = afectado->getMasa();
        const float masaTotal      = masaProyectil + masaAfectado;
        const float multiplicador  = 3.0f;

        Vector2D vFinal  = (velocidadMovimiento * masaProyectil + afectado->getVelocidad() * masaAfectado)
                          * (1.0f / masaTotal);
        Vector2D deltaV  = vFinal - afectado->getVelocidad();

        afectado->recibirImpulsoLineal(deltaV * masaAfectado * multiplicador);

        activo = false;
        setVisible(false);
    }
}

// =============================================================================
//  GETTERS
// =============================================================================

bool          Obstaculo::isActivo()    const { return activo;   }
TipoObstaculo Obstaculo::getTipo()     const { return tipo;     }
float         Obstaculo::getRadio()    const { return radio;    }
Vector2D      Obstaculo::getPosicion() const { return posicion; }
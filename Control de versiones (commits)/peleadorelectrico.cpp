#include "PeleadorElectrico.h"
#include <cmath>
#include <QGraphicsScene>

const float PI = 3.14159265f;

// CAMBIADO: El constructor ahora acepta 'QString rutaSprite' y se lo manda a 'Combatiente(...)'
PeleadorElectrico::PeleadorElectrico(float _x, float _y, QString rutaSprite, std::vector<Obstaculo*>& listaObstaculos)
    : Combatiente(_x, _y, 22.0f, 20.0f, rutaSprite), enElSuelo(false), direccionMirada(1), modoSumo(true)
{
    referenciaObstaculosNivel = &listaObstaculos;
    friccionSuperficie = 0.10f; // Equilibrio estándar
}

void PeleadorElectrico::mover(int accion) {
    if (modoSumo) {
        if (accion == 1) { // Salir disparado
            float dirX = std::cos(anguloOrientacion);
            float dirY = std::sin(anguloOrientacion);
            Vector2D vectorDirector(dirX, dirY);
            recibirImpulsoLineal(vectorDirector * 15000.0f);
        }
        else if (accion == 2) { // Girar
            velocidadAngular += 5.0f;
        }
        else if(accion == 3){
            lanzarRayoAtaque();
        }
    }
    else {
        float fuerza = 200.0f;
        if (accion == 1) {
            direccionMirada = -1;
            recibirImpulsoLineal(Vector2D(-fuerza, 0));
        }
        else if (accion == 2) {
            direccionMirada = 1;
            recibirImpulsoLineal(Vector2D(fuerza, 0));
        }
        else if (accion == 3 && enElSuelo) {
            velocidad.y = -500.0f;
            enElSuelo = false;
        }
        else if (accion == 4) { // Tecla de habilidad especial
            lanzarRayoAtaque();
        }
    }
}

void PeleadorElectrico::lanzarRayoAtaque() {
    Vector2D direccion(1.0f, 0.0f);
    float distanciaAparicion = radio + 15.0f;
    Vector2D posAtaque;

    if (modoSumo) {
        direccion = Vector2D(std::cos(anguloOrientacion), std::sin(anguloOrientacion));
        posAtaque = posicion + (direccion * distanciaAparicion);
    } else {
        direccion = Vector2D((float)direccionMirada, 0.0f);
        posAtaque = posicion + (direccion * distanciaAparicion);
    }

    Obstaculo* rayo = new Obstaculo(RAYO_ELECTRICO, posAtaque, 30.0f, 0.0f);
    rayo->setVelocidadMovimiento(direccion * 550.0f);
    referenciaObstaculosNivel->push_back(rayo);

    if (this->scene()) {
        this->scene()->addItem(rayo);
    }
}

void PeleadorElectrico::aplicarFisicas(float dt) {
    if (modoSumo) {
        anguloOrientacion += velocidadAngular * dt;
        if (anguloOrientacion > 2 * PI) anguloOrientacion -= 2 * PI;
        setRotation((anguloOrientacion * (180.0f / PI))  - 90.0f);

        float friccionAngular = 0.005f;
        velocidadAngular = velocidadAngular * (1.0f - friccionAngular);

        posicion = posicion + (velocidad * dt);
        velocidad = velocidad * (1.0f - friccionSuperficie);
    }
    else {
        if (!enElSuelo) {
            velocidad.y += 1000.0f * dt;
        }
        posicion = posicion + (velocidad * dt);
        velocidad.x = velocidad.x * (1.0f - friccionSuperficie);
    }

    setPos(posicion.x, posicion.y);
}

void PeleadorElectrico::setEnElSuelo(bool estado) { enElSuelo = estado; }
void PeleadorElectrico::setModoSumo(bool estado) { modoSumo = estado; }
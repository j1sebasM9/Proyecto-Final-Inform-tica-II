#include "Fortachon.h"
#include <cmath> // Necesario para std::cos y std::sin del modo Sumo

const float PI = 3.14159265f;

// CAMBIADO: El constructor ahora recibe 'QString rutaSprite' y se lo pasa a 'Combatiente(...)'
Fortachon::Fortachon(std::string _nom, float _x, float _y, float _r, float _m, QString rutaSprite)
    : Combatiente(_x, _y, _r, _m, rutaSprite), nombre(_nom), enElSuelo(false), fuerzaEmpujeExtra(400.0f), modoSumo(true) {
    // Al ser pesado, tiene más fricción base
    friccionSuperficie = 0.25f;
}

void Fortachon::mover(int accion) {
    if (modoSumo) {
        // --- COMPORTAMIENTO NIVEL 1: SUMO (Trompo) ---
        if (accion == 1) {
            // ACCIÓN 1: Salir disparado
            float dirX = std::cos(anguloOrientacion);
            float dirY = std::sin(anguloOrientacion);
            Vector2D vectorDirector(dirX, dirY);

            // Al ser fortachón, su empuje sumo es un poco más fuerte que el estándar
            float fuerzaImpulso = 18000.0f;
            recibirImpulsoLineal(vectorDirector * fuerzaImpulso);
        }
        else if (accion == 2) {
            // ACCIÓN 2: Dar un impulso de rotación (Gira más lento por ser pesado)
            velocidadAngular += 2.5f;
        }
    }
    else {
        // --- COMPORTAMIENTO NIVEL 2: PLATAFORMAS ---
        float fuerzaLateral = fuerzaEmpujeExtra; // Se mueve más lento pero empuja fuerte

        if (accion == 1) {
            recibirImpulsoLineal(Vector2D(-fuerzaLateral, 0));
        }
        else if (accion == 2) {
            recibirImpulsoLineal(Vector2D(fuerzaLateral, 0));
        }
        else if (accion == 3 && enElSuelo) {
            // Salto muy corto/pesado
            velocidad.y = -300.0f;
            enElSuelo = false;
        }
    }
}

void Fortachon::aplicarFisicas(float dt) {
    if (modoSumo) {
        // --- FÍSICAS NIVEL 1: SUMO ---
        anguloOrientacion += velocidadAngular * dt;
        if (anguloOrientacion > 2 * PI) anguloOrientacion -= 2 * PI;
        setRotation((anguloOrientacion * (180.0f / PI))  - 90.0f);

        // Fricción angular alta porque es pesado
        float friccionAngular = 0.08f;
        velocidadAngular = velocidadAngular * (1.0f - friccionAngular);

        posicion = posicion + (velocidad * dt);
        velocidad = velocidad * (1.0f - friccionSuperficie);
    }
    else {
        // --- FÍSICAS NIVEL 2: LATERALES (Plataformas) ---
        if (!enElSuelo) {
            float gravedad = 980.0f; // Misma gravedad del mundo
            velocidad.y += gravedad * dt;
        }
        posicion = posicion + (velocidad * dt);
        velocidad.x = velocidad.x * (1.0f - friccionSuperficie);
    }

    setPos(posicion.x, posicion.y);
}

void Fortachon::setEnElSuelo(bool estado) { enElSuelo = estado; }
bool Fortachon::isEnElSuelo() const { return enElSuelo; }
void Fortachon::setModoSumo(bool estado) { modoSumo = estado; }
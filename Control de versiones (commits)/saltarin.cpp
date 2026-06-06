#include "Saltarin.h"
#include <cmath> // Necesario para std::cos y std::sin del modo Sumo

// Constante matemática para evitar problemas con M_PI en Qt
const float PI = 3.14159265f;

// CAMBIADO: El constructor ahora recibe 'QString rutaSprite' y se lo envía a 'Combatiente(...)'
Saltarin::Saltarin(std::string _nom, float _x, float _y, float _r, float _m, QString rutaSprite)
    : Combatiente(_x, _y, _r, _m, rutaSprite), nombre(_nom), enElSuelo(false), fuerzaSalto(600.0f), modoSumo(true) {
    // Al ser un personaje ligero, modificamos su fricción base (es muy resbaladizo)
    friccionSuperficie = 0.05f;
}

// Implementación del Movimiento (Teclado)
void Saltarin::mover(int accion) {
    if (modoSumo) {
        // --- COMPORTAMIENTO NIVEL 1: SUMO (Trompo) ---
        if (accion == 1) {
            // ACCIÓN 1: Salir disparado
            float dirX = std::cos(anguloOrientacion);
            float dirY = std::sin(anguloOrientacion);
            Vector2D vectorDirector(dirX, dirY);

            float fuerzaImpulso = 15000.0f; // Fuerza del empuje sumo
            recibirImpulsoLineal(vectorDirector * fuerzaImpulso);
        }
        else if (accion == 2) {
            // ACCIÓN 2: Dar un impulso de rotación
            velocidadAngular += 4.0f;
        }
    }
    else {
        // --- COMPORTAMIENTO NIVEL 2: PLATAFORMAS ---
        float fuerzaLateral = 150.0f; // Se mueve rápido pero con poca fuerza de empuje

        if (accion == 1) {
            recibirImpulsoLineal(Vector2D(-fuerzaLateral, 0));
        }
        else if (accion == 2) {
            recibirImpulsoLineal(Vector2D(fuerzaLateral, 0));
        }
        else if (accion == 3 && enElSuelo) {
            // Para saltar (ir hacia arriba), la velocidad Y debe ser negativa.
            velocidad.y = -fuerzaSalto;
            enElSuelo = false;
        }
    }
}

// Implementación de las Físicas
void Saltarin::aplicarFisicas(float dt) {
    if (modoSumo) {
        // --- FÍSICAS NIVEL 1: SUMO ---
        // 1. Rotación con inercia
        anguloOrientacion += velocidadAngular * dt;
        if (anguloOrientacion > 2 * PI) anguloOrientacion -= 2 * PI;
        setRotation((anguloOrientacion * (180.0f / PI))  - 90.0f); // Sincroniza rotación en Qt

        // Fricción angular (Evita que gire eternamente)
        float friccionAngular = 0.05f;
        velocidadAngular = velocidadAngular * (1.0f - friccionAngular);

        // 2. Desplazamiento lineal
        posicion = posicion + (velocidad * dt);

        // 3. Fricción de la superficie de la arena
        velocidad = velocidad * (1.0f - friccionSuperficie);
    }
    else {
        // --- FÍSICAS NIVEL 2: LATERALES (Plataformas) ---
        // 1. Aplicar Gravedad constantemente si no está en el suelo
        if (!enElSuelo) {
            float gravedad = 980.0f; // Pixeles por segundo al cuadrado
            velocidad.y += gravedad * dt;
        }

        // 2. Integración de Euler para actualizar posición
        posicion = posicion + (velocidad * dt);

        // 3. Fricción horizontal (rozamiento del aire/suelo)
        velocidad.x = velocidad.x * (1.0f - friccionSuperficie);
    }

    // 4. Sincronizar la matemática con el ítem gráfico de Qt
    setPos(posicion.x, posicion.y);
}

void Saltarin::setEnElSuelo(bool estado) { enElSuelo = estado; }
bool Saltarin::isEnElSuelo() const { return enElSuelo; }
void Saltarin::setModoSumo(bool estado) { modoSumo = estado; }
#ifndef ENTRENADORMAESTROARENA_H
#define ENTRENADORMAESTROARENA_H

#include <vector>
#include "Vector2D.h"
#include "Obstaculo.h"


#include <vector>
#include "Vector2D.h"

// Declaración previa: Le decimos al compilador "Confía en mí, Obstaculo es una clase".
// Esto evita incluir "Obstaculo.h" aquí y aligera el proyecto.
class Obstaculo;

class EntrenadorMaestroArena {
private:
    int estrategiaActual; // 1: Castigar, 2: Romper Estancamiento
    std::vector<Vector2D> zonasFrecuentesJ1;
    std::vector<Vector2D> zonasFrecuentesJ2;
    int contadorEmpujesJ1;
    int contadorEmpujesJ2;
    float temporizadorInactividad;
    float tiempoParaSiguienteAccion;
    float temporizadorFuego;

public:
    // Constructor
    EntrenadorMaestroArena();

    // 1. PERCEPCIÓN
    void registrarPercepcion(const Vector2D& posJ1, const Vector2D& posJ2, bool huboColision, float dt);

    // 2. RAZONAMIENTO Y APRENDIZAJE
    Vector2D calcularZonaCritica(int idJugador);

    // 3. ACCIÓN
    void ejecutarAccionModeradora(std::vector<Obstaculo*>& obstaculosGlobales, float dt);

    // Mantenimiento
    void registrarEmpuje(int idJugador);
};
#endif // ENTRENADORMAESTROARENA_H

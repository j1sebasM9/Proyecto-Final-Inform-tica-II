#include "EntrenadorMaestroArena.h"
#include "Obstaculo.h"
#include <cstdlib>
#include <cmath>

// Implementación del Constructor
EntrenadorMaestroArena::EntrenadorMaestroArena()
    : estrategiaActual(1), contadorEmpujesJ1(0), contadorEmpujesJ2(0),
    temporizadorInactividad(0.0f), tiempoParaSiguienteAccion(8.0f),
    temporizadorFuego(15.0f) // El primer fuego avisará a los 15 segundos de iniciar
{
}

// 1. PERCEPCIÓN: Recolección de datos
void EntrenadorMaestroArena::registrarPercepcion(const Vector2D& posJ1, const Vector2D& posJ2, bool huboColision, float dt) {
    zonasFrecuentesJ1.push_back(posJ1);
    if (zonasFrecuentesJ1.size() > 30) {
        zonasFrecuentesJ1.erase(zonasFrecuentesJ1.begin());
    }

    zonasFrecuentesJ2.push_back(posJ2);
    if (zonasFrecuentesJ2.size() > 30) {
        zonasFrecuentesJ2.erase(zonasFrecuentesJ2.begin());
    }

    if (!huboColision) temporizadorInactividad += dt;
    else temporizadorInactividad = 0.0f;
}

// 2. RAZONAMIENTO: Promedio matemático (Centroide)
Vector2D EntrenadorMaestroArena::calcularZonaCritica(int idJugador) {
    const std::vector<Vector2D>& historial = (idJugador == 1) ? zonasFrecuentesJ1 : zonasFrecuentesJ2;

    if (historial.empty()) {
        return Vector2D(0, 0);
    }

    Vector2D suma(0, 0);
    for (const auto& pos : historial) {
        suma = suma + pos;
    }

    return suma * (1.0f / historial.size());
}

// 3. ACCIÓN: Despliegue de obstáculos separados con ritmo controlado
void EntrenadorMaestroArena::ejecutarAccionModeradora(std::vector<Obstaculo*>& obstaculosGlobales, float dt) {

    // --- EVENTO 1: FUEGO ALEATORIO CON AVISO Y CICLO DE AUSENCIA ---
    temporizadorFuego -= dt;
    if (temporizadorFuego <= 0.0f) {

        const float RADIO_PISTA = 250.0f;
        const float RADIO_FUEGO = 75.0f;
        float distanciaMaxima = RADIO_PISTA - RADIO_FUEGO;

        // Geometría polar para asegurar que caiga dentro del círculo centralizado en (0,0)
        float angulo = (rand() % 360) * (3.141592f / 180.0f);
        float factorR = sqrt(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        float distancia = factorR * distanciaMaxima;

        float xAleatorio = distancia * cos(angulo);
        float yAleatorio = distancia * sin(angulo);

        // ¡NUEVO! Creamos el fuego pasándole 2.5 segundos como tiempo de aviso inicial
        float tiempoDeAviso = 2.5f;
        Obstaculo* fuego = new Obstaculo(FUEGO, Vector2D(xAleatorio, yAleatorio), RADIO_FUEGO, tiempoDeAviso);
        obstaculosGlobales.push_back(fuego);

        // MATEMÁTICA DEL CICLO TOTAL:
        // 2.5s (Aparece el círculo de advertencia)
        // + 15.0s (Se convierte en fuego real activo)
        // + 15.0s (Tiempo que debe permanecer desaparecido antes del próximo aviso)
        // Total = 32.5 segundos para reiniciar el evento.
        temporizadorFuego = tiempoDeAviso + 15.0f + 15.0f;
    }

    // --- EVENTO 2: RAYO MODERADOR PREDICTIVO EQUITATIVO ---
    tiempoParaSiguienteAccion -= dt;
    if (tiempoParaSiguienteAccion <= 0.0f) {

        int objetivoAleatorio = (rand() % 2) + 1;
        Vector2D zonaAtaque = calcularZonaCritica(objetivoAleatorio);

        if (zonaAtaque.magnitud() > 1.0f) {
            Obstaculo* rayo = new Obstaculo(RAYO_MODERADOR, zonaAtaque, 40.0f, 2.5f);
            obstaculosGlobales.push_back(rayo);
        }

        tiempoParaSiguienteAccion = 8.0f;
    }
}

// Registro de empujes
void EntrenadorMaestroArena::registrarEmpuje(int idJugador) {
    if (idJugador == 1) contadorEmpujesJ1++;
    else contadorEmpujesJ2++;
}
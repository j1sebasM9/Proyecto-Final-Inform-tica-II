#include "DirectorJuego.h"
#include "Nivel1.h"
#include "Nivel2.h"
#include <QMessageBox>

DirectorJuego::DirectorJuego(QGraphicsView* viewView)
    : vistaGrafica(viewView), nivelActual(nullptr), puntosAcumuladosJ1(0), puntosAcumuladosJ2(0), faseJuego(1) {}

DirectorJuego::~DirectorJuego() {
    if (nivelActual) delete nivelActual;
}

void DirectorJuego::iniciarFlujo() {
    // 1. Instanciar el Nivel 1 dinámicamente
    nivelActual = new Nivel1();

    // 2. Montar el nivel en la vista existente (mismo widget)
    vistaGrafica->setScene(nivelActual);
    vistaGrafica->setRenderHint(QPainter::Antialiasing);

    // 3. Encender el QTimer interno del nivel
    nivelActual->iniciarNivel();
}

void DirectorJuego::verificarCambioNivel() {
    // Esta función la llamaremos de forma síncrona o podemos conectarla.
    // Por simplicidad, el Director monitorea la salud de los combatientes.

    // Si estamos en Nivel 1 y algún jugador se quedó sin vidas, pasamos al Nivel 2
    /*
    if (faseJuego == 1 && (nivelActual->obtenerVidasJ1() <= 0 || nivelActual->obtenerVidasJ2() <= 0)) {
        nivelActual->pausarNivel();

        // SALVAGUARDAR PUNTOS (Regla de negocio: se mantienen)
        puntosAcumuladosJ1 = nivelActual->obtenerPuntosJ1();
        puntosAcumuladosJ2 = nivelActual->obtenerPuntosJ2();

        // BORRAR NIVEL 1 DE LA MEMORIA
        delete nivelActual;

        // CARGAR NIVEL 2 EN EL MISMO WIDGET
        faseJuego = 2;
        nivelActual = new Nivel2();

        // INYECTAR PUNTOS A LOS NUEVOS PERSONAJES
        nivelActual->cargarPuntosIniciales(puntosAcumuladosJ1, puntosAcumuladosJ2);

        vistaGrafica->setScene(nivelActual);
        nivelActual->iniciarNivel();
    }
    */
}

void DirectorJuego::terminarJuego() {
    // Desplegar un cuadro de diálogo nativo de Qt informando el ganador absoluto
    QString ganador = (puntosAcumuladosJ1 > puntosAcumuladosJ2) ? "Jugador 1" : "Jugador 2";
    if (puntosAcumuladosJ1 == puntosAcumuladosJ2) ganador = "Empate";

    QMessageBox::information(nullptr, "Fin del Juego", "El ganador absoluto es: " + ganador);
}
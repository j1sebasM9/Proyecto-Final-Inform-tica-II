#ifndef DIRECTORJUEGO_H
#define DIRECTORJUEGO_H

#include <QObject>
#include <QGraphicsView>
#include "NivelBase.h"

class DirectorJuego : public QObject {
    Q_OBJECT

private:
    QGraphicsView* vistaGrafica; // El único widget contenedor en pantalla
    NivelBase* nivelActual;      // Puntero polimórfico al escenario activo

    int puntosAcumuladosJ1;      // Persistencia de datos inter-nivel
    int puntosAcumuladosJ2;      // Persistencia de datos inter-nivel
    int faseJuego;               // 1: Nivel 1, 2: Nivel 2, 3: Fin

public:
    DirectorJuego(QGraphicsView* viewView);
    ~DirectorJuego();

    void iniciarFlujo();
    void verificarCambioNivel();
    void terminarJuego();
};

#endif // DIRECTORJUEGO_H

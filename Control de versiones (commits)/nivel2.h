#ifndef NIVEL2_H
#define NIVEL2_H


#include "NivelBase.h"
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QString>
#include <string>

class Combatiente;

class Nivel2 : public NivelBase {
    Q_OBJECT

private:
    // Nodos de la escena Qt
    QGraphicsRectItem* puentePlataforma;
    QGraphicsPixmapItem* fondoNivel;
    QGraphicsPixmapItem* spritePlataforma;

    // Dimensiones y geometria
    float centroEscenaX;
    float centroEscenaY;
    float anchoEscena;
    float altoEscena;
    float nivelDelSueloY;
    float xIzquierdaPuente;
    float xDerechaPuente;
    float altoPuente;
    float limiteCaidaY;

    // Datos de jugadores
    int     tipoJ1;
    int     tipoJ2;
    QString nombreJ1;
    QString nombreJ2;

    // Estado del hardware (Controles)
    bool j1Izquierda;
    bool j1Derecha;
    bool j2Izquierda;
    bool j2Derecha;

    // Temporizadores
    float cooldownRayoJ1;
    float cooldownRayoJ2;
    float duracionCooldownRayo;
    float tiempoInicioObstaculos;
    float retrasoActivacionObstaculos;
    bool  obstaculosActivados;

public:
    explicit Nivel2(int eleccionJ1, int eleccionJ2, QString nomJ1, QString nomJ2, QObject* parent = nullptr);
    virtual ~Nivel2() = default;

    // Concreciones del patron Template Method
    void configurarEscenario() override;
    void verificarBordes()     override;

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    // Creacion y gestion visual
    Combatiente* crearPersonaje(int tipo, const std::string& nombre, float x, float y);
    QString      rutaSpriteLateral(int tipo) const;
    void         aplicarAparienciaTemporal(Combatiente* jugador, int tipo);
    void         activarModoPlataforma(Combatiente* jugador);
    void         marcarEnSuelo(Combatiente* jugador, bool estado);
    void         crearObstaculosNivel2();
    void         ordenarCapasVisuales();

    // Controles continuos y habilidades
    void aplicarControlesContinuos();
    void activarRayoJugador(int indice);

    // Fisicas concretas del motor lateral
    void resolverColisionPlataforma(int indice);
    void verificarCaida(int indice);
    void respawnJugador(int indice);
    void verificarAplastamiento();
    void procesarAplastamiento(int atacante, int victima);
};

#endif // NIVEL2_H
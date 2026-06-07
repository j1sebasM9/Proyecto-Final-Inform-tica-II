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

    // =========================================================================
    // ESCENARIO VISUAL
    // =========================================================================
private:
    QGraphicsRectItem*   puentePlataforma;
    QGraphicsPixmapItem* fondoNivel;
    QGraphicsPixmapItem* spritePlataforma;

    // ✅ CORRECCIÓN BUG 1: Se eliminan textoJ1, textoJ2, textoJ3 de aquí.
    // NivelBase NO los declara, por lo que eran punteros basura que causaban
    // crash al llamar textoJ3->setVisible() y textoJ1->setZValue().
    // Los paneles de UI se crean con crearPanelEstadisticas() (heredado).

    // =========================================================================
    // DIMENSIONES Y GEOMETRÍA DEL NIVEL
    // =========================================================================
    float centroEscenaX;
    float centroEscenaY;
    float anchoEscena;
    float altoEscena;
    float nivelDelSueloY;       // Y del borde superior de la plataforma
    float xIzquierdaPuente;     // Límite izquierdo de la plataforma
    float xDerechaPuente;       // Límite derecho de la plataforma
    float altoPuente;           // Grosor lógico de la plataforma
    float limiteCaidaY;         // Si el jugador baja de aquí, cae al vacío

    // =========================================================================
    // DATOS DE JUGADORES
    // =========================================================================
    int     tipoJ1;
    int     tipoJ2;
    QString nombreJ1;
    QString nombreJ2;

    // =========================================================================
    // CONTROLES (estado de teclas presionadas)
    // =========================================================================
    bool j1Izquierda;
    bool j1Derecha;
    bool j2Izquierda;
    bool j2Derecha;

    // =========================================================================
    // HABILIDADES Y OBSTÁCULOS
    // =========================================================================
    float cooldownRayoJ1;
    float cooldownRayoJ2;
    float duracionCooldownRayo;
    float tiempoInicioObstaculos;
    float retrasoActivacionObstaculos;
    bool  obstaculosActivados;

public:
    explicit Nivel2(int eleccionJ1,
                    int eleccionJ2,
                    QString nomJ1,
                    QString nomJ2,
                    QObject* parent = nullptr);
    virtual ~Nivel2() = default;

    // Implementación obligatoria de métodos puros de NivelBase
    void configurarEscenario() override;
    void verificarBordes()     override;

protected:
    void keyPressEvent(QKeyEvent*   event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    // =========================================================================
    // CREACIÓN Y CONFIGURACIÓN
    // =========================================================================
    Combatiente* crearPersonaje(int tipo, const std::string& nombre, float x, float y);
    QString      rutaSpriteLateral(int tipo) const;
    void         aplicarAparienciaTemporal(Combatiente* jugador, int tipo);
    void         activarModoPlataforma(Combatiente* jugador);
    void         marcarEnSuelo(Combatiente* jugador, bool estado);
    void         crearObstaculosNivel2();

    // =========================================================================
    // CONTROLES Y HABILIDADES
    // =========================================================================
    void aplicarControlesContinuos();
    void activarRayoJugador(int indice);

    // =========================================================================
    // FÍSICA DEL NIVEL
    // =========================================================================
    void resolverColisionPlataforma(int indice);
    void verificarCaida(int indice);
    void respawnJugador(int indice);
    void verificarAplastamiento();
    void procesarAplastamiento(int atacante, int victima);

    // =========================================================================
    // CAPAS VISUALES
    // =========================================================================
    void ordenarCapasVisuales();
};

#endif // NIVEL2_H
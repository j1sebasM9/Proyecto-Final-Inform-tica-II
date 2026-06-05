#ifndef NIVELBASE_H
#define NIVELBASE_H


#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <array>
#include <vector>

// Declaraciones previas (Forward Declarations)
class Combatiente;
class Obstaculo;
class EntrenadorMaestroArena;

class NivelBase : public QGraphicsScene {
    Q_OBJECT // Macro indispensable para los Slots (No borrar)

protected:
    std::array<Combatiente*, 3> jugadores = {nullptr, nullptr, nullptr}; // Preparado para los 3 jugadores
    std::vector<Obstaculo*> obstaculos;
    EntrenadorMaestroArena* agenteIA;

    QTimer* gameTimer;
    float dt; // Delta time

    // UI (Textos en pantalla)
    QGraphicsTextItem* textoJ1;
    QGraphicsTextItem* textoJ2;
    QGraphicsTextItem* textoJ3; // UI para el jugador eléctrico

public:
    // Constructor y Destructor
    explicit NivelBase(QObject *parent = nullptr);
    virtual ~NivelBase();

    // Controles del ciclo de juego
    void iniciarNivel();
    void pausarNivel();

    // Métodos polimórficos puros (El Nivel 1 y 2 deben implementarlos)
    virtual void verificarBordes() = 0;
    virtual void configurarEscenario() = 0;

public slots:
    // EL CORAZÓN DEL JUEGO: Se ejecuta 60 veces por segundo
    void gameLoop();

protected:
    // Métodos de ayuda
    void dibujarUI();

    // ¡MODIFICADO! Ahora recibe como parámetros a los dos combatientes que chocaron
    // Esto nos permite calcular colisiones dinámicas entre cualquier pareja (J1vsJ2, J2vsJ3, J1vsJ3)
    void resolverColisionSumo(Combatiente* j1, Combatiente* j2);

    // Qt: Captura de Teclas
    void keyPressEvent(QKeyEvent *event) override;
};
#endif // NIVELBASE_H

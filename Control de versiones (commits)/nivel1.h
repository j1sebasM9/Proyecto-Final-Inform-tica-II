#include "NivelBase.h"
#include "Vector2D.h"

class QGraphicsEllipseItem;

class Nivel1 : public NivelBase {
private:
    float radioArena;
    Vector2D centroArena;
    QGraphicsEllipseItem* anilloVisual;

    Vector2D posInicialJ1;
    Vector2D posInicialJ2;
    int tipoJ1;
    int tipoJ2;
    QString nombreJ1;
    QString nombreJ2;

public:
    explicit Nivel1(int eleccionJ1 = 1, int eleccionJ2 = 2, QString nomJ1 = "JUGADOR 1", QString nomJ2 = "JUGADOR 2", QObject *parent = nullptr);
    virtual ~Nivel1() = default;

    // Concreciones del patron Template Method de NivelBase
    void configurarEscenario() override;
    void verificarBordes() override;
};
#ifndef PELEADORELECTRICO_H
#define PELEADORELECTRICO_H


#include "Combatiente.h"
#include "Obstaculo.h"
#include <vector>
#include <QString> // ¡NUEVO! Para manejar la ruta de la imagen

class PeleadorElectrico : public Combatiente {
private:
    bool enElSuelo;
    int direccionMirada; // -1: Izquierda, 1: Derecha
    std::vector<Obstaculo*>* referenciaObstaculosNivel; // Para inyectar sus rayos al nivel

    // Interruptor para saber en qué nivel estamos
    bool modoSumo;

public:
    // CAMBIADO: Sustituimos 'QColor color' por 'QString rutaSprite'
    PeleadorElectrico(float _x, float _y, QString rutaSprite, std::vector<Obstaculo*>& listaObstaculos);

    void mover(int accion) override;
    void aplicarFisicas(float dt) override;
    void lanzarRayoAtaque();

    void setEnElSuelo(bool estado);
    void setModoSumo(bool estado);
};

#endif // PELEADORELECTRICO_H

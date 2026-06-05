#ifndef SALTARIN_H
#define SALTARIN_H


#include "Combatiente.h"
#include <string>
#include <QString> // ¡NUEVO! Para manejar la ruta del sprite del pokémon ágil

class Saltarin : public Combatiente {
private:
    std::string nombre;
    bool enElSuelo; // Crucial para la vista lateral: saber si puede saltar
    float fuerzaSalto;

    // El interruptor para saber qué físicas aplicar
    bool modoSumo;

public:
    // CAMBIADO: Sustituimos 'QColor color' por 'QString rutaSprite'
    Saltarin(std::string _nom, float _x, float _y, float _r, float _m, QString rutaSprite);

    // Sobreescritura de los métodos polimórficos
    void mover(int accion) override;
    void aplicarFisicas(float dt) override;

    // Métodos propios
    void setEnElSuelo(bool estado);
    bool isEnElSuelo() const;
    void setModoSumo(bool estado);
};
#endif // SALTARIN_H

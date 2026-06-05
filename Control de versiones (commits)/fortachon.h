#ifndef FORTACHON_H
#define FORTACHON_H


#include "Combatiente.h"
#include <string>
#include <QString> // ¡NUEVO! Necesario para manejar la ruta del sprite

class Fortachon : public Combatiente {
private:
    std::string nombre;
    bool enElSuelo;
    float fuerzaEmpujeExtra;

    // El interruptor para saber qué físicas aplicar
    bool modoSumo;

public:
    // CAMBIADO: Sustituimos 'QColor color' por 'QString rutaSprite'
    Fortachon(std::string _nom, float _x, float _y, float _r, float _m, QString rutaSprite);

    void mover(int accion) override;
    void aplicarFisicas(float dt) override;

    void setEnElSuelo(bool estado);
    bool isEnElSuelo() const;
    void setModoSumo(bool estado);
};

#endif // FORTACHON_H

#include "Nivel1.h"
#include "Saltarin.h"
#include "Fortachon.h"
#include "PeleadorElectrico.h"
#include "Obstaculo.h"
#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QPen>
#include <QRandomGenerator>
#include "EntrenadorMaestroArena.h"

Nivel1::Nivel1(int eleccionJ1, int eleccionJ2, QString nomJ1, QString nomJ2, QObject *parent)
    : NivelBase(parent)
    , radioArena(205.0f)
    , centroArena(0.0f, 0.0f)
    , posInicialJ1(-150.0f, 0.0f)
    , posInicialJ2(150.0f, 0.0f)
    , tipoJ1(eleccionJ1)
    , tipoJ2(eleccionJ2)
    , nombreJ1(nomJ1)
    , nombreJ2(nomJ2)
{
    configurarEscenario();
}

void Nivel1::configurarEscenario()
{
    setSceneRect(-400, -400, 800, 800);

    // 1. Carga y renderizado del fondo del ring (Modo Sumo)
    QString rutaArena = "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/arena.png";
    QImage imgArena(rutaArena);

    if (imgArena.isNull()) {
        qDebug() << "ALERTA: No cargo la imagen del ring en:" << rutaArena;
        setBackgroundBrush(QBrush(Qt::darkGray));
    } else {
        QImage imgEscalada = imgArena.scaled(800, 800, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QGraphicsPixmapItem* fondo = new QGraphicsPixmapItem(QPixmap::fromImage(imgEscalada));
        fondo->setPos(-400, -400);
        fondo->setZValue(-1);
        addItem(fondo);
    }

    // 2. Anillo de calibracion vectorial (Guia visual del limite fisico)
    anilloVisual = new QGraphicsEllipseItem(-radioArena, -radioArena, radioArena * 2, radioArena * 2);
    anilloVisual->setBrush(Qt::NoBrush);
    anilloVisual->setPen(QPen(Qt::red, 3, Qt::DashLine));
    anilloVisual->setZValue(0);
    addItem(anilloVisual);

    // 3. Generacion parametrizada de obstaculos ambientales (Hielo aleatorio)
    float radioArenaSeguro = 205.0f;
    float radioHielo = 55.0f;
    float distanciaMaxima = radioArenaSeguro - radioHielo;

    float angulo = (QRandomGenerator::global()->generateDouble()) * 2.0f * 3.14159265f;
    float factorR = std::sqrt(QRandomGenerator::global()->generateDouble());
    float distancia = factorR * distanciaMaxima;

    float hieloX = distancia * std::cos(angulo);
    float hieloY = distancia * std::sin(angulo);

    obstaculos.push_back(new Obstaculo(HIELO, Vector2D(hieloX, hieloY), radioHielo, 0.0f, true));

    // 4. Factoria lambda de instanciacion polimorfica de jugadores
    auto crearPersonaje = [&](int tipo, std::string nombre, float x, float y) -> Combatiente* {
        QString rutaBase = "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/";

        if (tipo == 1) return new Fortachon(nombre, x, y, 35.0f, 30.0f, rutaBase + "fortachon.png");
        if (tipo == 2) return new Saltarin(nombre, x, y, 25.0f, 10.0f, rutaBase + "saltarin.png");
        if (tipo == 3) return new PeleadorElectrico(x, y, rutaBase + "electrico.png", obstaculos);

        return new Fortachon(nombre, x, y, 35.0f, 30.0f, rutaBase + "fortachon.png");
    };

    jugadores[0] = crearPersonaje(tipoJ1, "J1", posInicialJ1.x, posInicialJ1.y);
    jugadores[1] = crearPersonaje(tipoJ2, "J2", posInicialJ2.x, posInicialJ2.y);
    jugadores[2] = nullptr;

    // Calibracion de simetria inicial del Jugador 2
    jugadores[1]->setDireccionVisual(-1);
    jugadores[1]->setAnguloOrientacion(3.14159f);

    jugadores[0]->setZValue(5);
    jugadores[1]->setZValue(5);

    addItem(jugadores[0]);
    addItem(jugadores[1]);

    // 5. Carga dinamica de paneles de estadisticas (HUD)
    auto obtenerRutaPanel = [](int tipo) -> QString {
        if (tipo == 1) return "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/puntaje_de_fortachon.png";
        if (tipo == 2) return "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/puntaje_de_saltarin.png";
        if (tipo == 3) return "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/puntaje_de_electrico.png";
        return "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/puntaje_de_electrico.png";
    };

    crearPanelEstadisticas(1, obtenerRutaPanel(tipoJ1), nombreJ1, jugadores[0]->getVidas(), -400, -400);
    crearPanelEstadisticas(2, obtenerRutaPanel(tipoJ2), nombreJ2, jugadores[1]->getVidas(), 180, -400);

    // 6. Inyeccion de estado ambiental para el gestor de IA
    if (agenteIA) {
        agenteIA->setZonasSeguras(posInicialJ1, posInicialJ2);
        agenteIA->setRadioPista(205.0f);
        agenteIA->setModoPlataforma(false);
    }
}

void Nivel1::verificarBordes()
{
    for (int i = 0; i < 3; i++) {
        if (jugadores[i] && jugadores[i]->getVidas() > 0) {
            Vector2D posActual = jugadores[i]->getPosicion();

            // Calculo de descalificacion mediante distancia polar
            float distanciaAlCentro = (posActual - centroArena).magnitud();

            if (distanciaAlCentro > radioArena) {
                // Sancion mecanica al perdedor del round
                jugadores[i]->perderVida();

                // Adjudicacion de recompensa al rival
                if (i == 0 && jugadores[1]) {
                    jugadores[1]->ganarPunto();
                } else if (i == 1 && jugadores[0]) {
                    jugadores[0]->ganarPunto();
                }

                Vector2D respawnPos;
                if (i == 0) respawnPos = posInicialJ1;
                else if (i == 1) respawnPos = posInicialJ2;
                else respawnPos = Vector2D(0.0f, 200.0f);

                // Reset de estado cinematico y espacial
                jugadores[i]->setVelocidad(Vector2D(0.0f, 0.0f));
                jugadores[i]->setPosicion(respawnPos);
                jugadores[i]->setPos(respawnPos.x, respawnPos.y);
            }
        }
    }
}
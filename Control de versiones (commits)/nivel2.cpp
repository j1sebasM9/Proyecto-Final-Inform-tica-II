#include "Nivel2.h"
#include "Combatiente.h"
#include "Saltarin.h"
#include "Fortachon.h"
#include "PeleadorElectrico.h"
#include "Obstaculo.h"
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QDebug>
#include <QPixmap>
#include "EntrenadorMaestroArena.h"

Nivel2::Nivel2(int eleccionJ1, int eleccionJ2, QString nomJ1, QString nomJ2, QObject* parent)
    : NivelBase(parent)
    , puentePlataforma(nullptr)
    , fondoNivel(nullptr)
    , spritePlataforma(nullptr)
    , centroEscenaX(0.0f)
    , centroEscenaY(0.0f)
    , anchoEscena(800.0f)
    , altoEscena(800.0f)
    , nivelDelSueloY(80.0f)
    , xIzquierdaPuente(-300.0f)
    , xDerechaPuente(300.0f)
    , altoPuente(35.0f)
    , limiteCaidaY(330.0f)
    , tipoJ1(eleccionJ1)
    , tipoJ2(eleccionJ2)
    , nombreJ1(nomJ1)
    , nombreJ2(nomJ2)
    , j1Izquierda(false), j1Derecha(false)
    , j2Izquierda(false), j2Derecha(false)
    , cooldownRayoJ1(0.0f), cooldownRayoJ2(0.0f)
    , duracionCooldownRayo(0.75f)
    , tiempoInicioObstaculos(0.0f)
    , retrasoActivacionObstaculos(6.0f)
    , obstaculosActivados(false)
{
    configurarEscenario();
}

void Nivel2::configurarEscenario()
{
    setSceneRect(centroEscenaX - anchoEscena / 2.0f, centroEscenaY - altoEscena / 2.0f, anchoEscena, altoEscena);

    // 1. Renderizado de escenario
    const QString rutaFondo = "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/escenario/fondo_nivel2.png";
    QPixmap pixmapFondo(rutaFondo);
    if (!pixmapFondo.isNull()) {
        fondoNivel = new QGraphicsPixmapItem(pixmapFondo.scaled(static_cast<int>(anchoEscena), static_cast<int>(altoEscena), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        fondoNivel->setPos(sceneRect().left(), sceneRect().top());
        fondoNivel->setZValue(-10);
        addItem(fondoNivel);
    } else {
        qDebug() << "ALERTA: fondo nivel 2 no encontrado:" << rutaFondo;
        setBackgroundBrush(QBrush(QColor(10, 10, 40)));
    }

    // 2. Colisionador basico de la plataforma principal
    const float anchoPuente = xDerechaPuente - xIzquierdaPuente;
    puentePlataforma = new QGraphicsRectItem(xIzquierdaPuente, nivelDelSueloY, anchoPuente, altoPuente);
    puentePlataforma->setBrush(Qt::NoBrush);
    puentePlataforma->setPen(Qt::NoPen);
    puentePlataforma->setZValue(-1);
    addItem(puentePlataforma);

    // 3. Texturizado del puente principal
    const QString rutaPlataforma = "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/escenario/plataforma_nivel2.png";
    QPixmap pixmapPlataforma(rutaPlataforma);
    if (!pixmapPlataforma.isNull()) {
        const float anchoVisual = anchoPuente + 90.0f;
        const float altoVisual  = anchoVisual * (static_cast<float>(pixmapPlataforma.height()) / static_cast<float>(pixmapPlataforma.width()));
        spritePlataforma = new QGraphicsPixmapItem(pixmapPlataforma.scaled(static_cast<int>(anchoVisual), static_cast<int>(altoVisual), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        spritePlataforma->setPos(xIzquierdaPuente - 45.0f, nivelDelSueloY - 95.0f);
        spritePlataforma->setZValue(0);
        addItem(spritePlataforma);
    } else {
        qDebug() << "ALERTA: plataforma nivel 2 no encontrada:" << rutaPlataforma;
        puentePlataforma->setBrush(QBrush(QColor(0, 110, 130)));
        puentePlataforma->setPen(QPen(Qt::white, 3));
    }

    // 4. Inyeccion de jugadores
    const float yInicio = nivelDelSueloY - 120.0f;
    jugadores[0] = crearPersonaje(tipoJ1, nombreJ1.toStdString(), -180.0f, yInicio);
    jugadores[1] = crearPersonaje(tipoJ2, nombreJ2.toStdString(),  180.0f, yInicio);
    jugadores[2] = nullptr;

    if (jugadores[0]) {
        jugadores[0]->setDireccionVisual(1);
        jugadores[0]->setModoSumo(false);
        jugadores[0]->setEnElSuelo(false);
        addItem(jugadores[0]);
    }
    if (jugadores[1]) {
        jugadores[1]->setDireccionVisual(-1);
        jugadores[1]->setModoSumo(false);
        jugadores[1]->setEnElSuelo(false);
        addItem(jugadores[1]);
    }

    // 5. Inyeccion del HUD
    auto rutaPanel = [](int tipo) -> QString {
        const QString base = "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/";
        if (tipo == 1) return base + "puntaje_de_fortachon.png";
        if (tipo == 2) return base + "puntaje_de_saltarin.png";
        return base + "puntaje_de_electrico.png";
    };

    if (jugadores[0]) crearPanelEstadisticas(1, rutaPanel(tipoJ1), nombreJ1, jugadores[0]->getVidas(), sceneRect().left(), sceneRect().top());
    if (jugadores[1]) crearPanelEstadisticas(2, rutaPanel(tipoJ2), nombreJ2, jugadores[1]->getVidas(), sceneRect().right() - 220.0f, sceneRect().top());

    ordenarCapasVisuales();

    // 6. Inyeccion de la Inteligencia Artificial de la Arena
    if (agenteIA) {
        agenteIA->setZonasSeguras(Vector2D(-180.0f, -60.0f), Vector2D(180.0f, -60.0f));
        agenteIA->setModoPlataforma(true);
    }
}

Combatiente* Nivel2::crearPersonaje(int tipo, const std::string& nombre, float x, float y)
{
    const QString base = "C:/Users/sm713/Documents/videojuego_POO_sumo/spriters/";
    QString rutaSprite;
    if      (tipo == 1) rutaSprite = base + "fortachon/idle.png";
    else if (tipo == 2) rutaSprite = base + "saltarin/idle.png";
    else if (tipo == 3) rutaSprite = base + "electrico/idle.png";

    Combatiente* nuevo = nullptr;
    if      (tipo == 1) nuevo = new Fortachon(nombre, x, y, 35.0f, 30.0f, rutaSprite);
    else if (tipo == 2) nuevo = new Saltarin(nombre, x, y, 25.0f, 10.0f, rutaSprite);
    else if (tipo == 3) nuevo = new PeleadorElectrico(x, y, rutaSprite, obstaculos);
    else                nuevo = new Fortachon(nombre, x, y, 35.0f, 30.0f, rutaSprite);

    if (nuevo) {
        if (tipo == 1) nuevo->setBrush(QBrush(Qt::red));
        else if (tipo == 2) nuevo->setBrush(QBrush(Qt::blue));
        else nuevo->setBrush(QBrush(Qt::yellow));
        nuevo->setPen(QPen(Qt::black, 2));
        nuevo->setZValue(5);
    }
    return nuevo;
}

void Nivel2::verificarBordes()
{
    aplicarControlesContinuos();

    if (!obstaculosActivados) {
        tiempoInicioObstaculos += dt;
        if (tiempoInicioObstaculos >= retrasoActivacionObstaculos) {
            crearObstaculosNivel2();
            obstaculosActivados = true;
        }
    }

    if (cooldownRayoJ1 > 0.0f) cooldownRayoJ1 = qMax(0.0f, cooldownRayoJ1 - dt);
    if (cooldownRayoJ2 > 0.0f) cooldownRayoJ2 = qMax(0.0f, cooldownRayoJ2 - dt);

    for (int i = 0; i < 2; i++) {
        if (!jugadores[i] || jugadores[i]->getVidas() <= 0) continue;
        resolverColisionPlataforma(i);
        verificarCaida(i);
    }

    verificarAplastamiento();
    ordenarCapasVisuales();
}

void Nivel2::aplicarControlesContinuos()
{
    if (jugadores[0] && jugadores[0]->getVidas() > 0) {
        if (j1Izquierda) jugadores[0]->mover(1);
        if (j1Derecha)   jugadores[0]->mover(2);
    }
    if (jugadores[1] && jugadores[1]->getVidas() > 0) {
        if (j2Izquierda) jugadores[1]->mover(1);
        if (j2Derecha)   jugadores[1]->mover(2);
    }
}

void Nivel2::resolverColisionPlataforma(int indice)
{
    Combatiente* jugador = jugadores[indice];
    if (!jugador) return;

    const Vector2D pos   = jugador->getPosicion();
    Vector2D       vel   = jugador->getVelocidad();
    const float    radio = jugador->getRadio();

    const bool dentroDelPuente = (pos.x + radio >= xIzquierdaPuente) && (pos.x - radio <= xDerechaPuente);
    const bool vieneCayendo    = vel.y >= 0.0f;
    const bool tocaDesdeArriba = (pos.y + radio >= nivelDelSueloY) && (pos.y <= nivelDelSueloY + radio * 0.65f);

    if (dentroDelPuente && vieneCayendo && tocaDesdeArriba) {
        Vector2D posCorregida = pos;
        posCorregida.y = nivelDelSueloY - radio;
        vel.y = 0.0f;
        jugador->setPosicion(posCorregida);
        jugador->setVelocidad(vel);
        jugador->setPos(posCorregida.x, posCorregida.y);
        jugador->setEnElSuelo(true);

        bool sobreHielo = false;
        for (const Obstaculo* obs : obstaculos) {
            if (obs && obs->getTipo() == HIELO && jugador->collidesWithItem(obs)) {
                sobreHielo = true;
                break;
            }
        }
        jugador->setEnHielo(sobreHielo);
    } else {
        jugador->setEnElSuelo(false);
        jugador->setEnHielo(false);
    }
}

void Nivel2::verificarCaida(int indice)
{
    Combatiente* jugador = jugadores[indice];
    if (!jugador || jugador->getVidas() <= 0) return;

    if (jugador->getPosicion().y - jugador->getRadio() > limiteCaidaY) {
        const int rival = (indice == 0) ? 1 : 0;
        jugador->perderVida(1);
        if (jugadores[rival]) jugadores[rival]->ganarPunto();
        if (jugador->getVidas() > 0) respawnJugador(indice);
    }
}

void Nivel2::respawnJugador(int indice)
{
    if (!jugadores[indice]) return;
    const float xRespawn = (indice == 0) ? -180.0f : 180.0f;
    const float yRespawn = nivelDelSueloY - 140.0f;
    jugadores[indice]->setPosicion(Vector2D(xRespawn, yRespawn));
    jugadores[indice]->setVelocidad(Vector2D(0.0f, 0.0f));
    jugadores[indice]->setPos(xRespawn, yRespawn);
    jugadores[indice]->setEnElSuelo(false);
}

void Nivel2::verificarAplastamiento()
{
    if (!jugadores[0] || !jugadores[1]) return;
    if (jugadores[0]->getVidas() <= 0 || jugadores[1]->getVidas() <= 0) return;
    if (!jugadores[0]->collidesWithItem(jugadores[1])) return;

    const Vector2D posJ1 = jugadores[0]->getPosicion();
    const Vector2D posJ2 = jugadores[1]->getPosicion();
    const Vector2D velJ1 = jugadores[0]->getVelocidad();
    const Vector2D velJ2 = jugadores[1]->getVelocidad();

    const bool j1Aplasta = (posJ1.y < posJ2.y - jugadores[1]->getRadio() * 0.35f) && (velJ1.y >= velJ2.y);
    const bool j2Aplasta = (posJ2.y < posJ1.y - jugadores[0]->getRadio() * 0.35f) && (velJ2.y >= velJ1.y);

    if      (j1Aplasta && !j2Aplasta) procesarAplastamiento(0, 1);
    else if (j2Aplasta && !j1Aplasta) procesarAplastamiento(1, 0);
}

void Nivel2::procesarAplastamiento(int atacante, int victima)
{
    if (!jugadores[atacante] || !jugadores[victima]) return;

    jugadores[atacante]->ganarPunto();
    jugadores[victima]->perderVida(1);

    Vector2D velAtacante = jugadores[atacante]->getVelocidad();
    Vector2D posAtacante = jugadores[atacante]->getPosicion();
    Vector2D posVictima  = jugadores[victima]->getPosicion();
    float radioAtacante = jugadores[atacante]->getRadio();
    float radioVictima  = jugadores[victima]->getRadio();
    float fuerzaDeCaida = velAtacante.y;

    // Efecto Trampolin (Rebote vertical)
    velAtacante.y = fuerzaDeCaida * -0.4f;

    // Resbalon por inercia (Rebote horizontal)
    float deltaX = posAtacante.x - posVictima.x;
    float sumaRadios = radioAtacante + radioVictima;
    float ratioImpacto = deltaX / sumaRadios;
    float multiplicadorHorizontal = fuerzaDeCaida * 0.35f;

    velAtacante.x = (velAtacante.x * 0.5f) + (ratioImpacto * multiplicadorHorizontal);

    jugadores[atacante]->setVelocidad(velAtacante);
    jugadores[atacante]->setEnElSuelo(false);

    if (jugadores[victima]->getVidas() > 0) respawnJugador(victima);
}

void Nivel2::crearObstaculosNivel2()
{
    Obstaculo* hielo = new Obstaculo(HIELO, Vector2D(0.0f, nivelDelSueloY - 12.0f), 55.0f);
    hielo->setOpacity(0.0);
    hielo->setZValue(2);
    obstaculos.push_back(hielo);
    addItem(hielo);

    Obstaculo* fuegoIzq = new Obstaculo(FUEGO, Vector2D(-170.0f, nivelDelSueloY - 12.0f), 38.0f, 2.0f);
    fuegoIzq->setZValue(3);
    obstaculos.push_back(fuegoIzq);
    addItem(fuegoIzq);

    Obstaculo* fuegoDer = new Obstaculo(FUEGO, Vector2D(170.0f, nivelDelSueloY - 12.0f), 38.0f, 2.0f);
    fuegoDer->setZValue(3);
    obstaculos.push_back(fuegoDer);
    addItem(fuegoDer);
}

void Nivel2::activarRayoJugador(int indice)
{
    if (indice < 0 || indice > 1) return;
    if (!jugadores[indice] || jugadores[indice]->getVidas() <= 0) return;

    PeleadorElectrico* electrico = dynamic_cast<PeleadorElectrico*>(jugadores[indice]);
    if (!electrico) return;

    float& cooldown = (indice == 0) ? cooldownRayoJ1 : cooldownRayoJ2;
    if (cooldown > 0.0f) return;

    electrico->lanzarRayoAtaque();
    electrico->mostrarAtaqueVisual();

    if (!obstaculos.empty()) {
        addItem(obstaculos.back());
    }
    cooldown = duracionCooldownRayo;
}

void Nivel2::ordenarCapasVisuales()
{
    if (fondoNivel)       fondoNivel->setZValue(-10);
    if (puentePlataforma) puentePlataforma->setZValue(-1);
    if (spritePlataforma) spritePlataforma->setZValue(0);

    for (Obstaculo* obs : obstaculos) {
        if (!obs) continue;
        obs->setZValue(obs->getTipo() == RAYO_ELECTRICO ? 6 : 3);
    }

    if (jugadores[0]) jugadores[0]->setZValue(5);
    if (jugadores[1]) jugadores[1]->setZValue(5);
}

void Nivel2::keyPressEvent(QKeyEvent* event)
{
    if (!jugadores[0] || !jugadores[1]) return;
    const bool repetida = event->isAutoRepeat();

    // Controles J1
    if (event->key() == Qt::Key_A) j1Izquierda = true;
    if (event->key() == Qt::Key_D) j1Derecha   = true;
    if (event->key() == Qt::Key_W && !repetida) jugadores[0]->mover(3);
    if (event->key() == Qt::Key_S) activarRayoJugador(0);

    // Controles J2
    if (event->key() == Qt::Key_Left)  j2Izquierda = true;
    if (event->key() == Qt::Key_Right) j2Derecha   = true;
    if (event->key() == Qt::Key_Up   && !repetida) jugadores[1]->mover(3);
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        activarRayoJugador(1);
    }
}

void Nivel2::keyReleaseEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;
    if (event->key() == Qt::Key_A)     j1Izquierda = false;
    if (event->key() == Qt::Key_D)     j1Derecha   = false;
    if (event->key() == Qt::Key_Left)  j2Izquierda = false;
    if (event->key() == Qt::Key_Right) j2Derecha   = false;
}
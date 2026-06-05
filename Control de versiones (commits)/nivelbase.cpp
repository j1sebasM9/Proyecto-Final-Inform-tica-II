#include "NivelBase.h"
#include "Combatiente.h"
#include "Obstaculo.h"
#include "EntrenadorMaestroArena.h"
#include "PeleadorElectrico.h" // Necesario para funciones específicas
#include <QString>

// Constructor
NivelBase::NivelBase(QObject *parent) : QGraphicsScene(parent), dt(0.016f) {
    agenteIA = new EntrenadorMaestroArena();

    textoJ1 = new QGraphicsTextItem();
    textoJ2 = new QGraphicsTextItem();
    textoJ3 = new QGraphicsTextItem();

    textoJ1->setDefaultTextColor(Qt::blue);
    textoJ2->setDefaultTextColor(Qt::red);
    textoJ3->setDefaultTextColor(Qt::darkYellow);

    addItem(textoJ1);
    addItem(textoJ2);
    addItem(textoJ3);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &NivelBase::gameLoop);
}

NivelBase::~NivelBase() {
    delete agenteIA;
    for (auto obs : obstaculos) {
        delete obs;
    }
}

void NivelBase::iniciarNivel() { gameTimer->start(16); }
void NivelBase::pausarNivel() { gameTimer->stop(); }

void NivelBase::gameLoop() {
    bool huboColision = false;

    for (int i = 0; i < 3; i++) {
        if (jugadores[i]) jugadores[i]->setFriccionSuperficie(0.15f);
    }

    for (int i = 0; i < 3; i++) {
        if (jugadores[i] && jugadores[i]->getVidas() > 0) {
            jugadores[i]->aplicarFisicas(dt);
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 3; j++) {
            if (jugadores[i] && jugadores[j] && jugadores[i]->getVidas() > 0 && jugadores[j]->getVidas() > 0) {
                if (jugadores[i]->collidesWithItem(jugadores[j])) {
                    huboColision = true;
                    resolverColisionSumo(jugadores[i], jugadores[j]);
                    agenteIA->registrarEmpuje(i + 1);
                }
            }
        }
    }

    agenteIA->registrarPercepcion(jugadores[0]->getPosicion(), jugadores[1]->getPosicion(), huboColision, dt);

    int cantidadAnterior = obstaculos.size();
    agenteIA->ejecutarAccionModeradora(obstaculos, dt);

    if (obstaculos.size() > cantidadAnterior) {
        addItem(obstaculos.back());
    }

    for (size_t i = 0; i < obstaculos.size(); i++) {
        if (obstaculos[i]->scene() == nullptr) addItem(obstaculos[i]);
        obstaculos[i]->actualizarObstaculo(dt);

        for (int j = 0; j < 3; j++) {
            if (jugadores[j] && jugadores[j]->collidesWithItem(obstaculos[i])) {
                int rival = (j == 0) ? 1 : 0;
                obstaculos[i]->aplicarEfecto(jugadores[j], jugadores[rival]);
            }
        }
    }

    verificarBordes();
    dibujarUI();

    for (auto it = obstaculos.begin(); it != obstaculos.end(); ) {
        if (!(*it)->isActivo()) {
            removeItem(*it);
            delete *it;
            it = obstaculos.erase(it);
        } else {
            ++it;
        }
    }

    if (jugadores[0]->getVidas() <= 0 || jugadores[1]->getVidas() <= 0 || (jugadores[2] && jugadores[2]->getVidas() <= 0)) {
        pausarNivel();
    }
}

void NivelBase::dibujarUI() {
    textoJ1->setPos(sceneRect().left() + 20, sceneRect().top() + 20);
    textoJ1->setPlainText(QString("JUGADOR 1\nVidas: %1\nPuntos: %2").arg(jugadores[0]->getVidas()).arg(jugadores[0]->getPuntos()));

    textoJ2->setPos(sceneRect().right() - 150, sceneRect().top() + 20);
    textoJ2->setPlainText(QString("JUGADOR 2\nVidas: %1\nPuntos: %2").arg(jugadores[1]->getVidas()).arg(jugadores[1]->getPuntos()));

    if (jugadores[2]) {
        textoJ3->setPos(sceneRect().left() + 20, sceneRect().bottom() - 60);
        textoJ3->setPlainText(QString("JUGADOR 3\nVidas: %1\nPuntos: %2").arg(jugadores[2]->getVidas()).arg(jugadores[2]->getPuntos()));
    }
}

void NivelBase::keyPressEvent(QKeyEvent *event) {
    // CONTROLES JUGADOR 1
    if (event->key() == Qt::Key_W) jugadores[0]->mover(2);
    if (event->key() == Qt::Key_E) jugadores[0]->mover(1);

    // CONTROLES JUGADOR 2
    if (event->key() == Qt::Key_8 || event->key() == Qt::Key_Up) jugadores[1]->mover(2);
    if (event->key() == Qt::Key_7) jugadores[1]->mover(1);

    // ¡CONTROLES JUGADOR 3! (IDÉNTICOS AL MODELO BASE)
    if (jugadores[2]) {
        if (event->key() == Qt::Key_K) {
            jugadores[2]->mover(2); // Acción 2: Rotar
        }
        if (event->key() == Qt::Key_L) {
            jugadores[2]->mover(1); // Acción 1: Dash / Disparado
        }
        if (event->key() == Qt::Key_P) {
            PeleadorElectrico* pe = dynamic_cast<PeleadorElectrico*>(jugadores[2]);
            if (pe) pe->lanzarRayoAtaque(); // Delega la creación de la bala a su propia clase
        }
    }
}

void NivelBase::resolverColisionSumo(Combatiente* j1, Combatiente* j2) {
    Vector2D normal = j2->getPosicion() - j1->getPosicion();
    float distancia = normal.magnitud();

    if (distancia == 0.0f) return;
    normal = normal.normalizar();

    float superposicion = (j1->getRadio() + j2->getRadio()) - distancia;
    if (superposicion > 0.0f) {
        j1->setPosicion(j1->getPosicion() - (normal * (superposicion * 0.5f)));
        j2->setPosicion(j2->getPosicion() + (normal * (superposicion * 0.5f)));
    }

    Vector2D velRelativa = j1->getVelocidad() - j2->getVelocidad();
    float velAproximacion = velRelativa.punto(normal);

    if (velAproximacion < 0.0f) return;

    float epsilon = 0.6f;
    float impulsoMag = (1.0f + epsilon) * velAproximacion / ((1.0f / j1->getMasa()) + (1.0f / j2->getMasa()));

    Vector2D vectorImpulso = normal * impulsoMag;
    float multJ1 = (dynamic_cast<PeleadorElectrico*>(j1)) ? 3.0f : 1.0f;
    float multJ2 = (dynamic_cast<PeleadorElectrico*>(j2)) ? 3.0f : 1.0f;

    j1->setVelocidad(j1->getVelocidad() - (vectorImpulso * (1.0f / j1->getMasa()) * multJ1));
    j2->setVelocidad(j2->getVelocidad() + (vectorImpulso * (1.0f / j2->getMasa()) * multJ2));
}
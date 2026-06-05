#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QBrush>
#include <QDebug>
#include "Nivel2.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. FIJAR EL TAMAÑO DE LA VENTANA (Un cuadrado de 800x800 es ideal)
    this->setFixedSize(800, 800);

    // Obligamos al lienzo (graphicsView) a ocupar exactamente esos 800x800
    ui->graphicsView->setGeometry(0, 0, 800, 800);

    // 2. Creamos la escena de ese mismo tamaño
    escenaMenu = new QGraphicsScene(this);
    escenaMenu->setSceneRect(0, 0, 800, 800);
    ui->graphicsView->setScene(escenaMenu);

    // 3. Cargamos la imagen original de 1254x1254
    QString rutaImagen = "C:/Users/USER/Documents/videojuego_POO_sumo/videojuego_POO_sumo/spriters/mainofgame.png";
    QImage imagenMenu(rutaImagen);

    if(imagenMenu.isNull()) {
        qDebug() << "¡ERROR FATAL! Qt no encuentra la imagen en esta ruta: " << rutaImagen;
    } else {
        qDebug() << "¡Imagen cargada con éxito!";

        // 4. ¡LA MAGIA DEL ESCALADO!
        // Reducimos la imagen a 800x800 manteniendo alta calidad (SmoothTransformation)
        QImage imagenEscalada = imagenMenu.scaled(800, 800, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 5. Aplicamos la imagen ya escalada como fondo
        ui->graphicsView->setBackgroundBrush(QBrush(imagenEscalada));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnStart_clicked()
{
    // 1. Guardamos las elecciones
    eleccionJ1 = ui->txtPersonajeJ1->text().toInt();
    eleccionJ2 = ui->txtPersonajeJ2->text().toInt();

    if(eleccionJ1 < 1 || eleccionJ1 > 3) eleccionJ1 = 1;
    if(eleccionJ2 < 1 || eleccionJ2 > 3) eleccionJ2 = 2;

    // 2. Ocultamos la interfaz
    ui->txtNombreJ1->hide();
    ui->txtPersonajeJ1->hide();
    ui->txtNombreJ2->hide();
    ui->txtPersonajeJ2->hide();
    ui->btnStart->hide();

    // --- ¡LA OPCIÓN NUCLEAR PARA EL FONDO PEGADO! ---
    // Borramos las hojas de estilo de TODAS las capas de la ventana
    this->setStyleSheet("");
    ui->centralwidget->setStyleSheet("");
    ui->graphicsView->setStyleSheet("");
    ui->graphicsView->setBackgroundBrush(Qt::NoBrush); // Limpiamos brochas viejas

    // 3. Elegir la ruta de la imagen
    QString rutaTransicion;
    if ((eleccionJ1 == 1 && eleccionJ2 == 2) || (eleccionJ1 == 2 && eleccionJ2 == 1)) {
        rutaTransicion = "C:/Users/USER/Documents/videojuego_POO_sumo/videojuego_POO_sumo/spriters/transicion_1vs2.png";
    }
    else if ((eleccionJ1 == 1 && eleccionJ2 == 3) || (eleccionJ1 == 3 && eleccionJ2 == 1)) {
        rutaTransicion = "C:/Users/USER/Documents/videojuego_POO_sumo/videojuego_POO_sumo/spriters/transicion_1vs3.png";
    }
    else if ((eleccionJ1 == 2 && eleccionJ2 == 3) || (eleccionJ1 == 3 && eleccionJ2 == 2)) {
        rutaTransicion = "C:/Users/USER/Documents/videojuego_POO_sumo/videojuego_POO_sumo/spriters/transicion_2vs3.png";
    }
    else {
        rutaTransicion = "C:/Users/USER/Documents/videojuego_POO_sumo/videojuego_POO_sumo/spriters/transicion_espejo.png";
    }

    // 4. CREAR Y VERIFICAR LA IMAGEN
    QImage imgTransicion(rutaTransicion);

    // ¡NUEVO DETECTOR DE ERRORES!
    if(imgTransicion.isNull()){
        qDebug() << "¡ALERTA! No se encontró la imagen de transición en:" << rutaTransicion;
        qDebug() << "Revisa si la extensión es .jpg o .png, o si el nombre está mal escrito.";
    }

    QGraphicsScene *escenaTransicion = new QGraphicsScene(this);
    escenaTransicion->setSceneRect(0, 0, 800, 800);
    QImage imgEscalada = imgTransicion.scaled(800, 800, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    escenaTransicion->setBackgroundBrush(QBrush(imgEscalada));

    ui->graphicsView->setScene(escenaTransicion);

    // 5. Esperar 3 segundos
    QTimer::singleShot(3000, this, &MainWindow::finalizarTransicion);
}


void MainWindow::finalizarTransicion()
{
    Nivel2 *nivel2 = new Nivel2(eleccionJ1, eleccionJ2);
    ui->graphicsView->setScene(nivel2);

    ui->graphicsView->centerOn(0, 0);

    nivel2->iniciarNivel();
}


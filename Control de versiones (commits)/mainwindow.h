#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btnStart_clicked();
    void finalizarTransicion();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *escenaMenu;

    int eleccionJ1;
    int eleccionJ2;
};
#endif // MAINWINDOW_H

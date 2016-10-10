#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <windows.h>

namespace Ui {
class game;
}

class game : public QWidget
{
    Q_OBJECT

public:
    explicit game(QWidget *parent = 0);
    ~game();

private:
    QGraphicsScene *scene; // сцена
    QGraphicsView *grview;
    QTimer *timer; // игровой таймер
    Ui::game *ui;

    QGraphicsRectItem *player; // игрок
    QGraphicsEllipseItem *ball; // мячик

    int player_height = 10;
    int player_width = 100;
    int ball_size = 10;

    int ball_x_speed = 5;
    int ball_y_speed = 5;
    int player_speed = 10;

private slots:
    void onTimer();
};

#endif // GAME_H

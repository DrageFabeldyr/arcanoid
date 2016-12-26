#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <windows.h>
#include <QHBoxLayout>
#include <QLabel>
#include<qmath.h>

namespace Ui {
class game;
}

class game : public QWidget
{
    Q_OBJECT

public:
    explicit game(QWidget *parent = 0);
    ~game();
    QGraphicsScene *scene;  // сцена

private:
    QGraphicsView *grview;
    QTimer *timer;          // игровой таймер перерисовки
    QTimer *pause_timeout_timer; // таймер задержки повторной обработки нажатия паузы
    Ui::game *ui;

    QGraphicsRectItem *player;  // игрок
    QGraphicsEllipseItem *ball; // мячик
    QList<QGraphicsRectItem*> bricks;

    // элементы отображения
    QVBoxLayout *game_layout;   // игра
    QVBoxLayout *stat_layout;   // настройки, параметры и очки
    QHBoxLayout *main_layout;   // основной
    QLabel *hello_text;           // текст
    QLabel *level_text;           // текст уровня
    QLabel *score_text;         // текст очков
    QLabel *speed_text;        // текст скорости шарика
    QLabel *bricks_text;        // текст оставшихся кирпичиков

    // переменные
    int player_height = 10;     // толщина игрока
    int player_width = 200;    // ширина игрока
    int ball_size = 12;           // диаметр шарика
    int brick_height = 10;      // толщина кирпичей
    int brick_width = 40;       // ширина кирпичей

    int speed = 1;                // переменная скорости обновления экрана
    int ball_speed = 2;         // смещение шарика (скорость)
    int ball_x_speed;           // горизонтальная скорость шарика
    int ball_y_speed;           // вертикальная скорость шарика
    int ball_x_speed_old;     // для сохранения горизонтальной скорости шарика
    int ball_y_speed_old;     // для сохранения вертикальной скорости шарика
    int player_speed = 10;   // скорость перемещения игрока

    int score_value = 0;       // количество заработанных очков
    int level_value = 1;        // номер уровня

    // флаги
    bool pause_timeout = false;

private slots:
    void onTimer(); // функция обновления экрана
    void can_pause(); // функция разрешения повторного нажатия кнопки паузы
    void create_level(QGraphicsScene *level_scene); // функция создания уровня
    void end_game(void);// функция выхода из игры
};

#endif // GAME_H

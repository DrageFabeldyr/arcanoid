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
#include <qmath.h>
#include <QKeyEvent>

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
    QTimer *timer;                         // игровой таймер перерисовки
    QTimer *keypress_timer;  // таймер задержки повторной обработки нажатия паузы
    QTimer *create_timer;                // таймер для выхода из процедуры создания формы и создания меню
    //Ui::game *ui;

    QGraphicsRectItem *player;  // игрок
    QGraphicsEllipseItem *ball; // мячик
    QList<QGraphicsRectItem*> bricks; // кирпичики

    // элементы отображения
    QVBoxLayout *game_layout;   // игра
    QVBoxLayout *stat_layout;   // настройки, параметры и очки
    QHBoxLayout *main_layout;   // основной
    QLabel *hello_text;           // текст
    QLabel *level_text;           // текст уровня
    QLabel *score_text;         // текст очков
    QLabel *speed_text;        // текст скорости шарика
    QLabel *bricks_text;        // текст оставшихся кирпичиков
    QFont f;

    // переменные
    int player_height = 10;       // толщина игрока
    int player_width = 200;      // ширина игрока
    int ball_size = 12;             // диаметр шарика
    int brick_height = 10;        // высота кирпичей
    int brick_h_spacing = 10;  // промежуток между кирпичами по вертикали
    int brick_width = 40;         // ширина кирпичей
    int brick_w_spacing = 10; // промежуток между кирпичами по горизонтали

    int speed = 1;                // переменная скорости обновления экрана
    int ball_speed = 2;         // смещение шарика (скорость)
    int ball_x_speed;           // горизонтальная скорость шарика
    int ball_y_speed;           // вертикальная скорость шарика
    int ball_x_speed_old = 0;     // для сохранения горизонтальной скорости шарика
    int ball_y_speed_old = 0;     // для сохранения вертикальной скорости шарика
    double player_speed = 0.1;   // скорость перемещения игрока

    int score_value = 0;       // количество заработанных очков
    int level_value = 1;        // номер уровня

    bool keyboard_state[6] = {0}; // массив состояний клавиш
    /* список клавиш управления (всего: 6):
     * 0 / 1 - вверх / вниз - для меню
     * 2 / 3 - влево / вправо - для игры
     * 4 - ввод - для переходов по меню
     * 5 - пауза
     */

    // флаги
    bool keypress_timeout = false;
    bool gameplay = false;  // 0 - меню, 1 - игра

private slots:
    void onTimer(); // функция обновления экрана
    void can_press_again(); // разрешения повторного нажатия кнопки
    void create_level(QGraphicsScene *level_scene); // функция создания уровня
    void start_menu(void); // функция вывода меню
    void start_game(void);// функция выхода из игры
    void end_game(void);// функция выхода из игры

protected:
    virtual bool event(QEvent *event); // обработчик событий
   //virtual void keyPressEvent(QKeyEvent *event);
};

#endif // GAME_H

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
#include <QVector>          // для перемещения стрелки по меню

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
    QTimer *timer;                         // игровой таймер перерисовки
    QTimer *keypress_timer;  // таймер задержки повторной обработки нажатия паузы
    QTimer *menu_timer;                // таймер для выхода из процедуры создания формы и создания меню
    //Ui::game *ui;

    //QGraphicsRectItem *player; // игрок
    QGraphicsEllipseItem *ball; // мячик

    struct upgraded_brick
    {
        QGraphicsRectItem *body;    // сам кирпич
        int x;                          // координаты
        int y;                          // координаты
        int level;                     // уровень прочности кирпича
        QString color;                    // цвет кирпича
        // enum Qt::GlobalColor
        // 5 - серый, 10 - голубой, 12 - жёлтый, 7 - красный
        bool just_had_collision;
    };

    QList<upgraded_brick*> bricks; // кирпичики
    upgraded_brick *player;

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
    // элементы меню
    QWidget *menu;             // окно меню
    QVector <QLabel*> menu_cursor_lines;
    QVector <QLabel*> menu_items_lines;
    QHBoxLayout *menu_layout;   // основной
    QVBoxLayout *menu_items_layout;
    QVBoxLayout *menu_cursor_layout;
    int menu_lines = 2; // количество строк меню
    int current_menu_line = 0; // текущий выбранный элемент меню

    // переменные
    int player_height = 10; // толщина игрока
    int player_width = 200; // ширина игрока
    int ball_size = 10;     // диаметр шарика
    int brick_height = 10;        // высота кирпичей
    int brick_h_spacing = 10;   // промежуток между кирпичами по вертикали
    int brick_width = 40;          // ширина кирпичей
    int brick_w_spacing = 10; // промежуток между кирпичами по горизонтали

    int speed = 1;                // переменная скорости обновления экрана
    //int ball_speed = 0;     // скорость шарика
    double ball_x_speed = 0;       // горизонатльная скорость шарика
    double ball_y_speed = 0;       // вертикальная скорость шарика
    double ball_x_speed_old = 0;     // для сохранения горизонтальной скорости шарика
    double ball_y_speed_old = 0;     // для сохранения вертикальной скорости шарика
    int player_speed = 10;  // скорость перемещения игрока
    
    int score_value = 0;       // количество заработанных очков
    int level_value = 1;        // номер уровня

    bool keyboard_state[7] = {0}; // массив состояний клавиш
    /* список клавиш управления (всего: 6):
     * 0 / 1 - вверх / вниз - для меню
     * 2 / 3 - влево / вправо - для игры
     * 4 - ввод - для переходов по меню
     * 5 - пауза
     * 6 - пробел, для запуска мячика
     */

    // флаги
    bool pause = false;
    bool pause_timeout = false;
    bool gameplay = false;  // 0 - меню, 1 - игра

private slots:
    void onTimer(); // функция обновления экрана
    void can_press_again(); // разрешения повторного нажатия кнопки
    void create_level(QGraphicsScene *level_scene, int level); // функция создания уровня
    void game_menu(void); // функция вывода меню
    void start_game(void);// функция выхода из игры
    void end_game(void);// функция выхода из игры
    QString set_brick_color (int brick_level);

protected:
    virtual bool event(QEvent *event); // обработчик событий
   //virtual void keyPressEvent(QKeyEvent *event);
};

#endif // GAME_H

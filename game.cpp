#include "game.h"
#include "ui_game.h"

game::game(QWidget *parent) : QWidget(parent), ui(new Ui::game)
{
    ball_x_speed = ball_speed;
    ball_y_speed = ball_speed;

    // установка сцены
    scene = new QGraphicsScene(QRectF(0, 0, 800, 600));
    grview = new QGraphicsView(scene, this);

    scene->setBackgroundBrush(QBrush(Qt::black)); // покраска сцены

    // добавляем "игрока"
    player = new QGraphicsRectItem;
    player = scene->addRect(0, 0, player_width, player_height, QPen(), QBrush(Qt::white));
    player->setPos((scene->width() - player_width) / 2, scene->height() - 3 * player_height);

    // добавляем "мячик"
    ball = new QGraphicsEllipseItem;
    ball = scene->addEllipse(0, 0, ball_size, ball_size, QPen(), QBrush(Qt::red));
    ball->setPos((scene->width() - ball_size) / 2, scene->height() - 3 * player_height - ball_size);

    // Создаем игровой таймер для перерисовки сцены
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->setInterval(1000 / 120); // скорость обновления экрана
    timer->start();

    grview->show();
}

game::~game()
{
    delete ui;
}

void game::onTimer()
{
    // обработка нажатия клавиш
    if (GetAsyncKeyState(VK_LEFT))
    {
         player->moveBy(-player_speed, 0);
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
         player->moveBy(player_speed, 0);
    }

    // Проверка столкновения с левой стенкой
    if (player->x() < 0)
    {
        player->setX(0);
    }
    // Проверка столкновения с правой стенкой
    if (player->x() + player->rect().width() + player_speed > scene->width())
    {
        player->setX(scene->width() - player->rect().width());
    }


    ball->moveBy(ball_x_speed, ball_y_speed); // двигать шарик

    // Проверка столкновения с левой стенкой
    if (ball->x() + ball_x_speed < 0)
    {
        ball_x_speed = -ball_x_speed;
    }
    // Проверка столкновения с правой стенкой
    if (ball->x() + ball->rect().width() + ball_x_speed > scene->width())
    {
        ball_x_speed = -ball_x_speed;
    }
    // Проверка столкновения с левой стенкой
    if (ball->y() + ball_y_speed < 0)
    {
        ball_y_speed = -ball_y_speed;
    }
    // Проверка столкновения с правой стенкой
    if (ball->y() + ball->rect().height() + ball_y_speed > scene->height())
    {
        ball_y_speed = -ball_y_speed;
    }
    if (ball->collidesWithItem(player)) // если ударяется в игрока - отлетает обратно
    {
        // если попадает сверху или снизу - меняется вертикальная скорость
        if (((ball->x() + ball_size / 2) > player->x()) && ((ball->x() + ball_size / 2) < (player->x() + player_width)))
            ball_y_speed = -ball_y_speed;
        // если сбоку - горизонтальная
        else
            ball_x_speed = -ball_x_speed;
    }
}

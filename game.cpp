#include "game.h"
#include "ui_game.h"

game::game(QWidget *parent) : QWidget(parent), ui(new Ui::game)
{
    setStyleSheet("background-color: MediumSpringGreen;"); // заливка окна - https://ru.wikipedia.org/wiki/Википедия:Таблица_цветов

    QFont f("Arial", 10, QFont::Bold);

    ball_x_speed = ball_speed;
    ball_y_speed = -ball_speed;

    // установка сцены
    scene = new QGraphicsScene(QRectF(0, 0, 800, 600));
    grview = new QGraphicsView(scene, this);

    scene->setBackgroundBrush(QBrush(Qt::black)); // покраска сцены

    // добавляем "игрока"
    player = new QGraphicsRectItem;
    player = scene->addRect(0, 0, player_width, player_height, QPen(Qt::green), QBrush(Qt::green, Qt::Dense5Pattern)); // размеры, цвет рамки, цвет заливки
    player->setPos((scene->width() - player_width) / 2, scene->height() - 2 * player_height);

    create_level(scene); // заполняем сцену кубиками

    // добавляем "мячик"
    ball = new QGraphicsEllipseItem;
    ball = scene->addEllipse(0, 0, ball_size, ball_size, QPen(Qt::red), QBrush(Qt::red));
    ball->setPos((scene->width() - ball_size) / 2, scene->height() - 2 * player_height - ball_size);

    // Создаем игровой таймер для перерисовки сцены
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->setInterval(1000 / (100 + 20 * speed)); // скорость обновления экрана
    timer->start();

    // Создаём таймер для регулировки частоты нажатия кнопки паузы
    pause_timeout_timer = new QTimer();
    connect(pause_timeout_timer, SIGNAL(timeout()), this, SLOT(can_pause()));

    game_layout = new QVBoxLayout;   // игра
    stat_layout = new QVBoxLayout;   // настройки, параметры и очки
    main_layout = new QHBoxLayout;   // основной

    game_layout->addWidget(grview);
    grview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    grview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    hello_text = new QLabel;
    level_text = new QLabel;
    score_text = new QLabel;
    speed_text = new QLabel;
    bricks_text = new QLabel;

    hello_text->setFont(f);
    level_text->setFont(f);
    score_text->setFont(f);
    speed_text->setFont(f);
    bricks_text->setFont(f);

    hello_text->setText("SuperArcanoid");
    level_text->setText(QString("Level: %1").arg(level_value));
    score_text->setText(QString("Score: %1").arg(score_value));
    speed_text->setText(QString("Speed: %1").arg(speed));
    bricks_text->setText(QString("Bricks: %1").arg(bricks.size()));

    stat_layout->setSpacing(30); // расстояние между элементами на расположении
    stat_layout->addWidget(hello_text);
    stat_layout->addWidget(level_text);
    stat_layout->addWidget(score_text);
    stat_layout->addWidget(speed_text);
    stat_layout->addWidget(bricks_text);
    stat_layout->addStretch(1); // чтобы сдвинуть всё наверх

    main_layout->addLayout(game_layout);
    main_layout->addLayout(stat_layout);
    setLayout(main_layout);
//    grview->show();
}

game::~game()
{
    delete ui;
}

void game::onTimer()
{
    // если уровень пройден
    if (bricks.size() <= 30)
    {
        create_level(scene);
        level_value++;
    }

    // обработка нажатия клавиш управления
    if (GetAsyncKeyState(VK_LEFT))
    {
         player->moveBy(-player_speed, 0);
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
         player->moveBy(player_speed, 0);
    }
    if (GetAsyncKeyState(VK_ESCAPE))
    {
        if (!pause_timeout)
        {
            pause_timeout = true;
            pause_timeout_timer->start(300); // допустимая частота нажатия клавиши паузы
            if (ball_x_speed == 0)
            {
                ball_x_speed = ball_x_speed_old;
                ball_y_speed = ball_y_speed_old;
            }
            else
            {
                ball_x_speed_old = ball_x_speed;
                ball_y_speed_old = ball_y_speed;
                ball_x_speed = 0;
                ball_y_speed = 0;
            }
        }
    }

    // Проверка столкновения игрока с левой стенкой
    if (player->x() < 0)
    {
        player->setX(1);
    }
    // Проверка столкновения игрока с правой стенкой
    if (player->x() + player->rect().width() + player_speed > scene->width())
    {
        player->setX(scene->width() - player->rect().width() - 1);
    }


    // обработка событий шарика
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
    // Проверка столкновения с верхней стенкой
    if (ball->y() + ball_y_speed < 0)
    {
        ball_y_speed = -ball_y_speed;
    }
    // Проверка столкновения с нижней стенкой
    if (ball->y() + ball->rect().height() + ball_y_speed > scene->height())
    {
        end_game();
    }
     // если ударяется в игрока - отлетает обратно
    if (ball->collidesWithItem(player))
    {
        // верхняя четверть дуги
        if (((ball->y() <= player->y() + player_height) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->y() + player_height)) &&
                ((ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->x()) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->x() + player_width)))
            ball_y_speed = -ball_y_speed;
        // нижняя четверть дуги
        if (((ball->y() + ball_size >= player->y()) && (ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->y())) &&
                ((ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->x()) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->x() + player_width)))
            ball_y_speed = -ball_y_speed;
        // правая четверть дуги
        if (((ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->y()) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->y() + player_height)) &&
                ((ball->x() + ball_size >= player->x()) && (ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->x())))
            ball_x_speed = -ball_x_speed;
        // левая четверть дуги
        if (((ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->y()) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->y() + player_height)) &&
                ((ball->x() <= player->x() + player_width) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->x() + player_width)))
            ball_x_speed = -ball_x_speed;
    }
     // если ударяется в кирпич - отлетает куда-то и он исчезает
    for (int i = 0; i < bricks.size(); i++)
    {
        if (ball->collidesWithItem(bricks.at(i)))
        {
            ball_x_speed_old = ball_x_speed;
            ball_y_speed_old = ball_y_speed;

            // верхняя четверть дуги
            if (((ball->y() <= bricks.at(i)->y() + brick_height) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->y() + brick_height)) &&
                    ((ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->x()) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->x() + brick_width)))
                ball_y_speed = -ball_y_speed;
            // нижняя четверть дуги
            if (((ball->y() + ball_size >= bricks.at(i)->y()) && (ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->y())) &&
                    ((ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->x()) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->x() + brick_width)))
                ball_y_speed = -ball_y_speed;
            // правая четверть дуги
            if (((ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->y()) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->y() + brick_height)) &&
                    ((ball->x() + ball_size >= bricks.at(i)->x()) && (ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->x())))
                ball_x_speed = -ball_x_speed;
            // левая четверть дуги
            if (((ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->y()) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->y() + brick_height)) &&
                    ((ball->x() <= bricks.at(i)->x() + brick_width) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->x() + brick_width)))
                ball_x_speed = -ball_x_speed;

            // "прогресс" засчитывается только при отражении - бывает, что шарик пролетает через самый край
            if ((ball_x_speed != ball_x_speed_old) || (ball_y_speed != ball_y_speed_old))
            {
                score_value += speed;
                scene->removeItem(bricks.at(i));
                bricks.removeAt(i);
            }
        }
    }

    // обновляем результаты
    level_text->setText(QString("Level: %1").arg(level_value));
    score_text->setText(QString("Score: %1").arg(score_value));
    speed_text->setText(QString("Speed: %1").arg(speed));
    bricks_text->setText(QString("Bricks: %1").arg(bricks.size()));
}

// функция разрешения повторного нажатия кнопки паузы
void game::can_pause()
{
    pause_timeout =  false;
}

// функция создания уровней
void game::create_level(QGraphicsScene *level_scene)
{
    int i, j = 0;

    while ((50 * j + brick_width) <  scene->width())
    {
        for (i = 0; i <= j; i++)
        {
            QGraphicsRectItem *brick = new QGraphicsRectItem;
            brick = level_scene->addRect(0, 0, brick_width, brick_height, QPen(Qt::cyan), QBrush(Qt::cyan)); // размеры, цвет рамки, цвет заливки
            brick->setPos(50 * i, 20 * j);
            bricks.append(brick);
        }
        j++;
    }
}

// обработка "game over"а
void game::end_game()
{
    exit(0);
}

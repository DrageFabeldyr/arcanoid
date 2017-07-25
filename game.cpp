#include "game.h"
#include "ui_game.h"

#include <QDebug>

game::game(QWidget *parent) : QWidget(parent)//, ui(new Ui::game)
{
    setStyleSheet("background-color: MediumSpringGreen;"); // заливка окна - https://ru.wikipedia.org/wiki/Википедия:Таблица_цветов
    f.setFamily("Arial");
    f.setBold(true);
    f.setPixelSize(13);

    // установка сцены
    scene = new QGraphicsScene(QRectF(0, 0, 800, 600));
    grview = new QGraphicsView(scene, this);


    // компоновка меню
    menu = new QWidget(grview);

    for (int i = 0; i < menu_lines; i++)
    {
        QLabel *menu_cursor_line = new QLabel;
        menu_cursor_line->setFont(f);
        menu_cursor_lines.append(menu_cursor_line);
        QLabel *menu_item_line = new QLabel;
        menu_item_line->setFont(f);
        menu_items_lines.append(menu_item_line);
    }

    menu_cursor_lines[0]->setText("➜");
    menu_cursor_lines[1]->setText("");
    menu_items_lines[1]->setText("ВЫХОД");

    menu_cursor_layout = new QVBoxLayout;
    menu_items_layout = new QVBoxLayout;
    menu_layout = new QHBoxLayout;

    menu_cursor_layout->setSpacing(20);
    for (int i = 0; i < menu_cursor_lines.size(); i++)
        menu_cursor_layout->addWidget(menu_cursor_lines[i]);
    menu_items_layout->setSpacing(20);
    for (int i = 0; i < menu_items_lines.size(); i++)
        menu_items_layout->addWidget(menu_items_lines[i]);
    menu_layout->addLayout(menu_cursor_layout);
    menu_layout->addLayout(menu_items_layout);
    menu->setLayout(menu_layout);
    menu->setStyleSheet("background-color: Grey;");

    scene->setBackgroundBrush(QBrush(Qt::black)); // покраска сцены

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
    main_layout->setSizeConstraint(QLayout::SetFixedSize); // запрет изменения размеров

    // Создаем таймер для выхода из отрисовки сцены и запуска меню
    menu_timer = new QTimer();
    connect(menu_timer, SIGNAL(timeout()), this, SLOT(game_menu()));
    menu_timer->setInterval(100); // время запуска игры
    menu_timer->start();
    this->setFocus(); // чтобы ловились стрелки
}

game::~game()
{
    //delete ui;
}

void game::onTimer()
{
    // если уровень пройден
    if (bricks.size() <= 10)
    {
        create_level(scene);
        level_value++;
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

        if (keyboard_state[2])
        {
            // Проверка столкновения игрока с левой стенкой
            if (player->x() - player_speed > -1)
                player->moveBy(-player_speed, 0);
        }
        if (keyboard_state[3])
        {
            // Проверка столкновения игрока с правой стенкой
            if (player->x() + player->rect().width() + player_speed < scene->width() + 1)
                player->moveBy(player_speed, 0);
        }
    }

    // обновляем результаты
    level_text->setText(QString("Level: %1").arg(level_value));
    score_text->setText(QString("Score: %1").arg(score_value));
    speed_text->setText(QString("Speed: %1").arg(speed));
    bricks_text->setText(QString("Bricks: %1").arg(bricks.size()));
}

// функция создания уровней
void game::create_level(QGraphicsScene *level_scene)
{
    int i, j = 0;

    while (((brick_width + brick_w_spacing) * j + brick_width) <  scene->width()) // пока можно всунуть ещё 1 кирпич по ширине
    {
        for (i = 0; i <= j; i++)
        {
            QGraphicsRectItem *brick = new QGraphicsRectItem;
            brick = level_scene->addRect(0, 0, brick_width, brick_height, QPen(Qt::cyan), QBrush(Qt::cyan)); // размеры, цвет рамки, цвет заливки
            brick->setPos((brick_width + brick_w_spacing) * i, (brick_height + brick_h_spacing) * j);
            bricks.append(brick);
        }
        j++;
    }
}
// работа с меню
void game::game_menu()
{
    // расположение меню рисуется здесь, потому что до создания виджетов все размеры равны нулю
    menu->setGeometry(grview->width()/2 - menu->width()/2, grview->height()/2 - menu->height()/2, menu->width(), menu->height());

    if (!pause)
        menu_items_lines[0]->setText("СТАРТ");
    else
        menu_items_lines[0]->setText("ДАЛЕЕ");


        /*
    if (keyboard_state[0] == true) // вверх
    {
        keyboard_state[0] = false;
        if (current_menu_line > 0) // если есть пункты выше
            current_menu_line--;    // смещаемся
        // перерисовываем стрелочку
        for (int i = 0; i < menu_lines; i++)
            menu_cursor_lines[i]->setText("");
        menu_cursor_lines[current_menu_line]->setText("➜");
    }
    if (keyboard_state[1] == true) // вниз
    {
        keyboard_state[1] = false;
        if (current_menu_line < menu_items_lines.size() - 1) // если есть пункты ниже
            current_menu_line++;    // смещаемся
        // перерисовываем стрелочку
        for (int i = 0; i < menu_lines; i++)
            menu_cursor_lines[i]->setText("");
        menu_cursor_lines[current_menu_line]->setText("➜");
    }
    if (keyboard_state[4] == true)
    {
        if (current_menu_line == 0)
            start_game();
        if (current_menu_line == 1)
            end_game();
    }
*/
}

// запуск игры
void game::start_game()
{
    gameplay = true;
    ball_x_speed = /*0;/*/ball_speed;
    ball_y_speed = /*0;/*/-ball_speed;

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
    keypress_timer = new QTimer();
    connect(keypress_timer, SIGNAL(timeout()), this, SLOT(can_press_again()));
}

// функция разрешения повторного нажатия кнопки паузы
void game::can_press_again()
{
    pause_timeout =  false;
}

// обработка "game over"а
void game::end_game()
{
    exit(0);
}

// обработка нажатий клавиш
bool game::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;

        qDebug() << keyEvent->key() << "key"; // вывод кода клавиши в окно вывода
        qDebug() << keyEvent->nativeScanCode() << "native"; // вывод кода клавиши в окно вывода

        if (keyEvent->key() == 0x01000013) // up
        {
            keyboard_state[0] = true;
            if (current_menu_line > 0) // если есть пункты выше
                current_menu_line--;    // смещаемся
            // перерисовываем стрелочку
            for (int i = 0; i < menu_lines; i++)
                menu_cursor_lines[i]->setText("");
            menu_cursor_lines[current_menu_line]->setText("➜");
        }
        if (keyEvent->key() == 0x01000015) // down
        {
            keyboard_state[1] = true;
            if (current_menu_line < menu_items_lines.size() - 1) // если есть пункты ниже
                current_menu_line++;    // смещаемся
            // перерисовываем стрелочку
            for (int i = 0; i < menu_lines; i++)
                menu_cursor_lines[i]->setText("");
            menu_cursor_lines[current_menu_line]->setText("➜");
        }
        if (keyEvent->key() == 0x01000012) // left
            keyboard_state[2] = true;
        if (keyEvent->key() == 0x01000014) // right
            keyboard_state[3] = true;
        if ((keyEvent->key() == 0x01000004) || (keyEvent->key() == 0x01000005)) // enter || num-enter
        {
            keyboard_state[4] = true;
            if (!gameplay)
            {
                if (current_menu_line == 0)
                {
                    menu->hide();
                    start_game();
                }
                if (current_menu_line == 1)
                    end_game();
            }

            if (pause)
            {
                if (current_menu_line == 0)
                {
                    menu->hide();
                    timer->start();
                    pause = !pause;
                    pause_timeout = true;            // устанавливаем запрещение на повторное нажатие, чтобы пауза случайно не "отжалась" сразу
                    keypress_timer->start(1000);      // интервал, достаточный для гарантированного нажатия
                }
                if (current_menu_line == 1)
                    end_game();
            }
        }
        if (keyEvent->key() == 0x01000008) // pause
        {
            if (!pause_timeout && gameplay)
            {
                if (!pause)
                {
                    timer->stop();
                    menu->show();
                    game_menu();
                }
                else
                {
                    timer->start();
                    menu->hide();
                }
                pause = !pause;
                pause_timeout = true;            // устанавливаем запрещение на повторное нажатие, чтобы пауза случайно не "отжалась" сразу
                keypress_timer->start(1000);      // интервал, достаточный для гарантированного нажатия
            }
            keyboard_state[5] = true;
        }
        qDebug() << "key pressed";
    }
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;

        if (keyEvent->key() == 0x01000013) // up
            keyboard_state[0] = false;
        if (keyEvent->key() == 0x01000015) // down
            keyboard_state[1] = false;
        if (keyEvent->key() == 0x01000012) // left

            keyboard_state[2] = false;
        if (keyEvent->key() == 0x01000014) // right
            keyboard_state[3] = false;
        if ((keyEvent->key() == 0x01000004) || (keyEvent->key() == 0x01000005)) // enter || num-enter
            keyboard_state[4] = false;
        if (keyEvent->key() == 0x01000008) // pause
            keyboard_state[5] = false;

        qDebug() << "key released";
    }

    return QWidget::event(event); // т.к. стандартная функция bool, надо что-то возвращать
}

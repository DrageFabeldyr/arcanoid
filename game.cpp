#include "game.h"
#include "ui_game.h"

#include <QDebug>
#include <QDir>

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
    if (bricks.size() < 1)
    {
        level_value++;
        create_level(scene, level_value);
        /*
        if (level_value > 1)
        {
            if (level_value % 2 != 0) // потихоньку поднимаем скорость каждый уровень
            {
                speed ++;
                timer->setInterval(1000 / (100 + 20 * speed)); // скорость обновления экрана
            }
            else // увеличиваем скорость самого мячика
            {
                if (abs(ball_x_speed) < 10)
                {
                    if (ball_x_speed > 0)
                        ball_x_speed++;
                    else
                        ball_x_speed--;
                }
                if (abs(ball_y_speed) < 10)
                {
                    if (ball_y_speed > 0)
                        ball_y_speed++;
                    else
                        ball_y_speed--;
                }
            }
        }
        */
    }

    // обработка событий шарика
    ball->moveBy(ball_x_speed, ball_y_speed); // двигать шарик

    // Проверка столкновения с левой стенкой
    if (ball->x() + ball_x_speed < 0)
    {
        for (int j = 0; j < bricks.size(); j++)
            bricks.at(j)->just_had_collision = false; // обнуляем показатели столкновения
        player->just_had_collision = false;
        ball_x_speed = -ball_x_speed;
    }
    // Проверка столкновения с правой стенкой
    if (ball->x() + ball->rect().width() + ball_x_speed > scene->width())
    {
        for (int j = 0; j < bricks.size(); j++)
            bricks.at(j)->just_had_collision = false; // обнуляем показатели столкновения
        player->just_had_collision = false;
        ball_x_speed = -ball_x_speed;
    }
    // Проверка столкновения с верхней стенкой
    if (ball->y() + ball_y_speed < 0)
    {
        for (int j = 0; j < bricks.size(); j++)
            bricks.at(j)->just_had_collision = false; // обнуляем показатели столкновения
        player->just_had_collision = false;
        ball_y_speed = -ball_y_speed;
    }
    // Проверка столкновения с нижней стенкой
    if (ball->y() + ball->rect().height() + ball_y_speed > scene->height())
    {
        end_game();
        /*
        for (int j = 0; j < bricks.size(); j++)
            bricks.at(j)->just_had_collision = false; // обнуляем показатели столкновения
        player->just_had_collision = false;
        ball_y_speed = -ball_y_speed;
        */
    }
     // если ударяется в игрока - отлетает обратно
    if ((ball->collidesWithItem(player->body)) &&  (!player->just_had_collision)) // чтобы мячик не залипал в ракетке
    {
        for (int j = 0; j < bricks.size(); j++)
            bricks.at(j)->just_had_collision = false; // обнуляем показатели столкновения
        player->just_had_collision = true;

        //int length_to_hit = abs((player->x() + player_width/2) - (ball->x() + ball_size/2)); // расстояние от центра ракетки до точки соприкосновения
        //int length_to_hit = (player->x() + player_width/2) - (ball->x() + ball_size/2); // расстояние от центра ракетки до точки соприкосновения, справа +, слева -
        //qDebug() <<  length_to_hit;

        // верхняя четверть дуги
        if (((ball->y() <= player->y + player_height) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->y + player_height)) &&
                ((ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->x) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->x + player_width)))
        {
            ball_y_speed = -ball_y_speed;
            /*
            if (ball_x_speed > 0)
            {
                ball_x_speed += ball_speed * (4 * length_to_hit / player_width);
                // т.к. расстояние до точки касания максимум половина, при которой скорость по оси должна увеличиваться вдвое
                ball_y_speed -= ball_y_speed * (1 - ball_speed/ball_x_speed);
            }
            else
            {
                ball_x_speed -= ball_speed * (4 * length_to_hit / player_width);
                // т.к. расстояние до точки касания максимум половина, при которой скорость по оси должна увеличиваться вдвое
                ball_y_speed -= ball_y_speed * (1 - ball_speed/ball_x_speed);
            }
            */
        }
        // нижняя четверть дуги
        else if (((ball->y() + ball_size >= player->y) && (ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->y)) &&
                ((ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->x) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->x + player_width)))
        {
            ball_y_speed = -ball_y_speed;
            /*
            if (ball_x_speed > 0)
            {
                ball_x_speed -= ball_speed * (4 * length_to_hit / player_width);
                // т.к. расстояние до точки касания максимум половина, при которой скорость по оси должна увеличиваться вдвое
                ball_y_speed -= ball_y_speed * (1 - ball_speed/ball_x_speed);
            }
            else
            {
                ball_x_speed += ball_speed * (4 * length_to_hit / player_width);
                // т.к. расстояние до точки касания максимум половина, при которой скорость по оси должна увеличиваться вдвое
                ball_y_speed -= ball_y_speed * (1 - ball_speed/ball_x_speed);
            }
            */
        }
        // правая четверть дуги
        else if (((ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->y) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->y + player_height)) &&
                ((ball->x() + ball_size >= player->x) && (ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->x)))
            ball_x_speed = -ball_x_speed;
        // левая четверть дуги
        else if (((ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->y) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= player->y + player_height)) &&
                ((ball->x() <= player->x + player_width) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= player->x + player_width)))
            ball_x_speed = -ball_x_speed;
        // если чётко в диагональ - отлетает назад
        else
        {
            qDebug() <<  "SHIT!!!! угол ракетки";
            ball_x_speed = -ball_x_speed;
            ball_y_speed = -ball_y_speed;
        }
        //qDebug() <<  ball_x_speed;
        //qDebug() <<  ball_y_speed;
    }
     // если ударяется в кирпич - отлетает куда-то и он исчезает
    for (int i = 0; i < bricks.size(); i++)
    {
        if ((ball->collidesWithItem(bricks.at(i)->body)) && (!bricks.at(i)->just_had_collision)) // чтобы не было многократных уменьшений уровня кирпичей
        {
            for (int j = 0; j < bricks.size(); j++)
                bricks.at(j)->just_had_collision = false; // обнуляем показатели столкновения
            player->just_had_collision = false;
            bricks.at(i)->just_had_collision = true;    // всем кроме столкнувшегося

            ball_x_speed_old = ball_x_speed;
            ball_y_speed_old = ball_y_speed;

            // верхняя четверть дуги
            if (((ball->y() <= bricks.at(i)->body->y() + brick_height) && (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->body->y() + brick_height)) &&
                    ((ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->body->x()) &&
                     (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->body->x() + brick_width)))
                ball_y_speed = -ball_y_speed;
            // нижняя четверть дуги
            else if (((ball->y() + ball_size >= bricks.at(i)->body->y()) && (ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->body->y())) &&
                    ((ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->body->x()) &&
                     (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->body->x() + brick_width)))
                ball_y_speed = -ball_y_speed;
            // правая четверть дуги
            else if (((ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->body->y()) &&
                      (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->body->y() + brick_height)) &&
                    ((ball->x() + ball_size >= bricks.at(i)->body->x()) && (ball->x() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->body->x())))
                ball_x_speed = -ball_x_speed;
            // левая четверть дуги
            else if (((ball->y() + ball_size / 2 + ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->body->y()) &&
                      (ball->y() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) <= bricks.at(i)->body->y() + brick_height)) &&
                    ((ball->x() <= bricks.at(i)->body->x() + brick_width) && (ball->x() + ball_size / 2 - ball_size / 2 * qSin(qDegreesToRadians(float(45))) >= bricks.at(i)->body->x() + brick_width)))
                ball_x_speed = -ball_x_speed;
            // если чётко в диагональ - отлетает назад
            else
            {
                qDebug() <<  "SHIT!!!! угол кирпича";
                ball_x_speed = -ball_x_speed;
                ball_y_speed = -ball_y_speed;
            }

            // "прогресс" засчитывается только при отражении - бывает, что шарик пролетает через самый край
            if ((ball_x_speed != ball_x_speed_old) || (ball_y_speed != ball_y_speed_old))
            {
                score_value += speed * bricks.at(i)->level;
                bricks.at(i)->level--;
                if (bricks.at(i)->level <= 0) // если кирпич первого уровня - удаляем
                {
                    scene->removeItem(bricks.at(i)->body);
                    bricks.removeAt(i);
                }
                else // иначе - снижаем уровень кирпича
                {
                    bricks.at(i)->color = set_brick_color(bricks.at(i)->level);
                    bricks.at(i)->body->setPen(QPen(bricks.at(i)->color.toStdString().c_str()));
                    bricks.at(i)->body->setBrush(QBrush(bricks.at(i)->color.toStdString().c_str()));
                    bricks.at(i)->just_had_collision = true;
                }
            }
        }
    }

    if (keyboard_state[2])
    {
        // Проверка столкновения игрока с левой стенкой
        if (player->x - player_speed > -1)
        {
            player->x -= player_speed;
            player->body->moveBy(-player_speed, 0);
        }
    }
    if (keyboard_state[3])
    {
        // Проверка столкновения игрока с правой стенкой
        if (player->x + player->body->rect().width() + player_speed < scene->width() + 1)
        {
            player->x += player_speed;
            player->body->moveBy(player_speed, 0);
        }
    }

    // обновляем результаты
    level_text->setText(QString("Level: %1").arg(level_value));
    score_text->setText(QString("Score: %1").arg(score_value));
    speed_text->setText(QString("Speed: %1").arg(speed));
    bricks_text->setText(QString("Bricks: %1").arg(bricks.size()));
}

// функция создания уровней
void game::create_level(QGraphicsScene *level_scene, int level)
{
    int i, j;
    bool level_data = false;
    QString str = "";
    QString level_str = "";

    int bricks_num = 16;
            /*
    int bricks_num =scene->width() / (brick_width + brick_w_spacing);  // количество кирпичей, влезающих по ширине
    if (bricks_num % 2 != 0) // если не чётный
        bricks_num--; // сделаем чётным, чтобы можно было рисовать зеркальные уровни
    if (bricks_num > 16)
        bricks_num = 16;
                */
    int border_spacing = (scene->width() - (brick_width + brick_w_spacing) * bricks_num + brick_w_spacing) / 2; // т.к. после последнего кубика отступ не нужен

    int bricks_map[bricks_num][bricks_num];  // карта уровня
    QString level_map_file = qApp->applicationDirPath() + QDir::separator() + "level_maps";
    QFile map_file(level_map_file);

    if (map_file.open(QIODevice::ReadOnly))
    {
        while (!map_file.atEnd())
        {
            str = map_file.readLine().simplified();
            if (level_data) // присвоение ставим раньше, чтобы избежать строки со словом level
                level_str += str;
            if (str.contains(QString("level%1").arg(level)))
            {
                level_data = true;
                level_str.clear();
            }
            if (str.isEmpty())
                level_data = false;
        }
        level_str.simplified();
        map_file.close();
    }

    if (!level_str.isEmpty())
    {
        for (j = 0; j < bricks_num; j++)
        {
            for (i = 0; i < bricks_num; i++)
            {
                bricks_map[j][i] = level_str.at(bricks_num * j + i).digitValue();
            }
        }
    }
    else // если карты уровня нет, тогда просто создаём зеркальную картинку
    {
        for (j = 0; j < bricks_num; j++)
        {
            for (i = 0; i < bricks_num / 2; i++)
            {
                bricks_map[j][i] = rand() % 5;
                bricks_map[j][bricks_num - i - 1] = bricks_map[j][i]; // зеркальное отражение левой половины
            }
        }
    }

    for (j = 0; j < bricks_num; j++)
    {
        for (i = 0; i < bricks_num; i++)
        {
            if (bricks_map[j][i] >= 1) // если кирпич есть
            {
                upgraded_brick *brick = new upgraded_brick;
                brick->level = bricks_map[j][i];
                brick->color = set_brick_color(brick->level);
                brick->body = level_scene->addRect(0, 0, brick_width, brick_height, QPen(brick->color.toStdString().c_str()), QBrush(brick->color.toStdString().c_str())); // размеры, цвет рамки, цвет заливки
                brick->x = (brick_width + brick_w_spacing) * i + border_spacing;
                brick->y = (brick_height + brick_h_spacing) * j + border_spacing; // сверху тоже сделаем отступ
                brick->body->setPos(brick->x, brick->y);
                brick->just_had_collision = false;
                bricks.append(brick);
            }
        }
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
}

// запуск игры
void game::start_game()
{
    gameplay = true;
    //ball_x_speed = /*0;/*/ball_speed;
    //ball_y_speed = /*0;/*/-ball_speed;

    create_level(scene, level_value); // заполняем сцену кубиками

    // добавляем "игрока"
    player = new upgraded_brick;
    player->body = scene->addRect(0, 0, player_width, player_height, QPen(Qt::green), QBrush(Qt::green, Qt::Dense5Pattern)); // размеры, цвет рамки, цвет заливки
    player->x = (scene->width() - player_width) / 2;
    player->y = scene->height() - 2 * player_height;
    player->body->setPos(player->x, player->y);

    // добавляем "мячик"
    ball = new QGraphicsEllipseItem;
    ball = scene->addEllipse(0, 0, ball_size, ball_size, QPen(Qt::red), QBrush(Qt::red));
    ball->setPos((scene->width() - ball_size) / 2, scene->height() - 2 * player_height - ball_size);

    // Создаем игровой таймер для перерисовки сцены
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->setInterval(1000 / (50 * speed)); // скорость обновления экрана
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
                    keypress_timer->start(100);      // интервал, достаточный для гарантированного нажатия
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
                    pause = !pause; // чтобы в game_menu() передавался правильный параметр
                    timer->stop();
                    menu->show();
                    game_menu();
                }
                else
                {
                    pause = !pause; // во второй ветке тоже должно быть
                    timer->start();
                    menu->hide();
                }
                pause_timeout = true;            // устанавливаем запрещение на повторное нажатие, чтобы пауза случайно не "отжалась" сразу
                keypress_timer->start(100);      // интервал, достаточный для гарантированного нажатия
            }
            keyboard_state[5] = true;
        }
        if (keyEvent->key() == 0x20) // пробел
            keyboard_state[6] = true;
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
        if (keyEvent->key() == 0x20) // пробел
        {
            keyboard_state[6] = false;
            if ((ball_x_speed  < 1) || (ball_y_speed  < 1))// если начало игры
            {
                ball_x_speed = 5;
                ball_y_speed = -5;
                player->just_had_collision = true;
            }
        }

        qDebug() << "key released";
    }

    return QWidget::event(event); // т.к. стандартная функция bool, надо что-то возвращать
}

QString game::set_brick_color(int brick_level)
{
    QString color;

    switch (brick_level)
    {
    case 1:
        color = "grey";
        break;
    case 2:
        color = "cyan";
        break;
    case 3:
        color = "yellow";
        break;
    case 4:
        color = "red";
        break;
    default:
        break;
    }
    return color;
}

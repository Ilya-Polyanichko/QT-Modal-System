#ifndef STAGE_TESTING_TWO_H
#define STAGE_TESTING_TWO_H

#include <QWidget>

#include <QVBoxLayout>
#include <QLabel>

class stage_testing_two : public QWidget
{
    Q_OBJECT

    QVBoxLayout *central_layout = new QVBoxLayout();
    QLabel *label = new QLabel("Приложение: Тестирование второго этапа");

public:
    stage_testing_two(QWidget *parent = nullptr);
    ~stage_testing_two();
};
#endif // STAGE_TESTING_TWO_H

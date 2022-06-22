#ifndef STAGE_TESTING_ONE_H
#define STAGE_TESTING_ONE_H

#include <QWidget>

#include <QVBoxLayout>
#include <QLabel>

class stage_testing_one : public QWidget
{
    Q_OBJECT

    QVBoxLayout *central_layout = new QVBoxLayout();
    QLabel *label = new QLabel("Приложение: Тестирование первого этапа");

public:
    stage_testing_one(QWidget *parent = nullptr);
    ~stage_testing_one();
};
#endif // STAGE_TESTING_ONE_H

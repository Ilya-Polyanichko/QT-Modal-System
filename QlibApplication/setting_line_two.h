#ifndef SETTING_LINE_TWO_H
#define SETTING_LINE_TWO_H

#include <QWidget>

#include <QVBoxLayout>
#include <QLabel>

class setting_line_two : public QWidget
{
    Q_OBJECT

    QVBoxLayout *central_layout = new QVBoxLayout();
    QLabel *label = new QLabel("Приложение: Наладка линии 2");

public:
    setting_line_two(QWidget *parent = nullptr);
    ~setting_line_two();
};
#endif // SETTING_LINE_TWO_H

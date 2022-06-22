#ifndef SETTING_LINE_ONE_H
#define SETTING_LINE_ONE_H

#include <QWidget>

#include <QVBoxLayout>
#include <QLabel>

class setting_line_one : public QWidget {

    Q_OBJECT

    QVBoxLayout *central_layout = new QVBoxLayout();
    QLabel *label = new QLabel("Приложение: Наладка линии 1");

public:
    setting_line_one(QWidget *parent = nullptr);
    ~setting_line_one();
};
#endif // SETTING_LINE_ONE_H

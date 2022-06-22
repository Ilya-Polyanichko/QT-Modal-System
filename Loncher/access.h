#ifndef ACCESS_H
#define ACCESS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QLineEdit>
#include <QKeyEvent>
#include <QCompleter>
#include <QComboBox>
#include <QPushButton>

#include "QlibApplication/qlibapplication.h"

class Access : public QWidget {

    Q_OBJECT

QComboBox *qComboBox_login = new QComboBox();
QPushButton *confirm = new QPushButton("Подтвердить");

QLineEdit *login = new QLineEdit();
QLineEdit *pass = new QLineEdit();
QList<QString> App_list;
QlibApplication App;

QList<QString> login_list;

public:
    explicit Access(QWidget *parent = 0);
    ~Access();

signals:
    void sendText(QString name, QString login, QList<QString> App_list);

private slots:
    void verification();
    void sorting(QString Roles, QList<QString> &App_list);
    void check_line();

private:
    QSqlDatabase StorageDB;

protected:
    void keyPressEvent(QKeyEvent *event) override;

};

#endif // ACCESS_H

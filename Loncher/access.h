#ifndef ACCESS_H
#define ACCESS_H

#include <QDialog>

#include <QSqlDatabase>
#include <QCompleter>
#include <QComboBox>
#include <QFontDatabase>
#include <QListView>


#include "QlibApplication/qlibapplication.h"

class Access : public QDialog {

    Q_OBJECT

public:

    explicit Access(QWidget *parent = 0);
    ~Access();

    QString name;
    QLineEdit *login = new QLineEdit();
    QList<QString> App_list;

private:

    QSqlDatabase StorageDB;

    QList<QString> login_list;

    QComboBox *qComboBox_login = new QComboBox();
    QLineEdit *pass = new QLineEdit();
    QPushButton *confirm = new QPushButton("Подтвердить");

    QlibApplication App;

private slots:

    void access_display_settings();
    void authentication();
    void authorization(QString ID);

    void search_application(QString Roles);
    void check_line();

};

#endif // ACCESS_H

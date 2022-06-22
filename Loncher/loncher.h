#ifndef LONCHER_H
#define LONCHER_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSplitter>


#include "access.h"

class Loncher : public QMainWindow
{
    Q_OBJECT

    QString login_users;

QMap<int, QMenu*> menu_list;
QWidget *central_widget = new QWidget();
QTabWidget *tabWidget = new QTabWidget();
QToolBar *toolBarMain = new QToolBar();
QLabel *lab = new QLabel();

public:
    Loncher(QWidget *parent = 0);
    ~Loncher();

private slots:
    void getText(QString name, QString login, QList<QString> App_list);
    void getToolBar(QToolBar* toolbar);
    void swapUsers();
    void addAction(int index_parent, QString name, QString objectName, QList<QString> &App_list);
    void addMenu(int index_parent, int index_menu, QString name, QString objectName, QList<QString> &App_list);
    bool check_empty(int ID, QList<QString> &App_list);
    void closeTabWidget();
    void openApp();

private:
    QSqlDatabase StorageDB;
    Access *Acc;

signals:
    void sendLogin(QString login, QString general_login);

};

#endif // LONCHER_H

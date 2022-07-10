#ifndef LONCHER_H
#define LONCHER_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSplitter>

#include <QMenuBar>
#include <QDebug>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QLayout>
#include <QSplitter>
#include <QTextEdit>
#include <QStatusBar>
#include <QMap>
#include <QTabBar>
#include <QToolBar>
#include <QGroupBox>
#include <QProcess>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>


#include "access.h"
#include "database.h"

class Loncher : public QMainWindow
{
    Q_OBJECT

public:
    Loncher(QWidget *parent = 0);
    ~Loncher();

public slots:

    void setAttribute(QString name, QString login, QList<QString> App_list);

private:
    QSqlDatabase StorageDB;
    Access *Acc;

    QString login_users;
    QString name_users;
    QList<QString> list_application;

    QMap<int, QMenu*> menu_list;
    QWidget *central_widget = new QWidget();

    QTabWidget *tabWidget = new QTabWidget();
    QToolBar *toolBarMain = new QToolBar();
    QToolBar *toolBarlast = new QToolBar();
    QMenu *menuBarlast = new QMenu();

    QMap<int, QMenu*> menubar_list;

    QSqlTableModel *admMenu_table_model = new QSqlTableModel();

    QList<int> generalGroupID;

    QWidget *welcome_widget = new QWidget();
    QMenu *menuLast = new QMenu();

private slots:
    void getToolBar(QToolBar* toolbar);
    void getMenu(QMenu* menu);
    void addAction(int index_parent, QString name, QString objectName);
    void addMenu(int index_parent, int index_menu, QString name, QString objectName);
    bool check_empty(int ID);
    void closeTabWidget(int index);
    void openApp();
    void swapUsers();

    void createMenuBar();


    void loncher_display_settings();
    void admin_mail_slot();

protected:
    void changeEvent(QEvent * event);

signals:
    void sendLogin(QString login, QString general_login);

};

#endif // LONCHER_H

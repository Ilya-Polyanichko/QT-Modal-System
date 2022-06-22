#include "loncher.h"

#include "QlibApplication/qlibapplication.h"
#include "qcoreapplication.h"

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

Loncher::Loncher(QWidget *parent) : QMainWindow(parent) {

    StorageDB = QSqlDatabase::addDatabase("QSQLITE");
    StorageDB.setDatabaseName("./StorageDB.db");
    StorageDB.open();

    Loncher::setMinimumSize(QSize(854, 480));
    Loncher::setFont(QFont( "Segoe UI Semibold",10 ));
    Loncher::setWindowIcon(QIcon("./Images/Img-003.png"));

    tabWidget -> setTabPosition(QTabWidget :: South);

    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabWidget()));
/*
    QPixmap bkgnd("./Images/Background.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    Loncher::setPalette(palette);
*/

    QPushButton *add_menu = new QPushButton(QIcon("./Images/Img-022.png"),"");

    lab -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    toolBarMain -> addWidget(lab);
    toolBarMain -> addWidget(add_menu);

    Access *Acc = new Access();
    connect(Acc,SIGNAL(sendText(QString, QString, QList<QString>)),this,SLOT(getText(QString, QString, QList<QString>)));
    Acc -> show();

}

Loncher::~Loncher() {

}


void Loncher::getText(QString name, QString login, QList<QString> App_list) {

    login_users = login;
    show();
    menuBar() -> clear();

    QSqlQuery query = QSqlQuery(StorageDB);

    query.exec("WITH RECURSIVE tr ( ID, Name, Parent, Object, lvl ) "
               "AS ( SELECT ID, Name, Parent, Object, 0 "
               "AS lvl FROM admMenu rt "
               "WHERE Parent IS NULL "
               "UNION SELECT nd.ID, nd.Name, nd.Parent, nd.Object, tr.lvl + 1 AS lvl "
               "FROM tr, admMenu nd "
               "WHERE tr.id = nd.Parent "
               "ORDER BY 5 DESC ) "
               "SELECT tr.* FROM tr;");


    while(query.next()) {

        if (query.value("lvl").toInt() < 1 && check_empty(query.value("ID").toInt(), App_list) == true) {

            QMenu *menu = new QMenu(query.value("Name").toString());
            menu -> setObjectName(query.value("Object").toString());
            menu_list.insert(query.value("ID").toInt(), menu);

            menuBar() -> addMenu(menu);

        } else if (query.value("Object").toString() == "group" && query.value("lvl").toInt() > 0) {

            addMenu(query.value("Parent").toInt(), query.value("ID").toInt(), query.value("Name").toString(), query.value("Object").toString(), App_list);

        } else if (query.value("Object").toString().split("_")[0] == "form") {

            addAction(query.value("Parent").toInt(), query.value("Name").toString(), query.value("Object").toString(), App_list);

        }
    }

    QMenuBar *bar = new QMenuBar(this);
    bar -> setLayoutDirection(Qt::RightToLeft);

    QMenu *userMenu = new QMenu(name, bar);

    userMenu -> addAction(QIcon("./Images/Img-014.png"), "Сменить пользователя", this, SLOT(swapUsers()));
    userMenu -> addAction(QIcon("./Images/Img-013.png"), "Выход",  qApp, SLOT(quit()));

    bar -> addMenu(userMenu);

    menuBar() -> setCornerWidget(bar);

    QGridLayout *gridLayout = new QGridLayout();

    gridLayout -> setContentsMargins(10, 10, 10, 10);
    central_widget -> setLayout(gridLayout);

    tabWidget -> setTabsClosable(true);

    gridLayout -> addWidget(tabWidget);

    central_widget -> hide();

    Loncher::setCentralWidget(central_widget);

}

void Loncher::addAction(int index_parent, QString name, QString objectName, QList<QString> &App_list) {


    if (menu_list.contains(index_parent) == true) {

        QAction *action = new QAction(name, menu_list[index_parent]);
        action -> setObjectName(QString(objectName + "||" + name));
        connect(action, SIGNAL(triggered()), SLOT(openApp()));

        foreach(QString it, App_list) if (it == objectName) menu_list[index_parent] -> addAction(action);

    }

}

void Loncher::addMenu(int index_parent, int index_menu, QString name, QString objectName, QList<QString> &App_list) {

    if (check_empty(index_menu, App_list)  == true) {

        QMenu *menu = new QMenu(name, menu_list[index_parent]);
        menu -> setObjectName(objectName);

        menu_list.insert(index_menu, menu);

        menu_list[index_parent] -> addMenu(menu);

    }
}

void Loncher::openApp() {

    //Loncher::setCentralWidget(central_widget);
    central_widget -> show();

    QString name_app = QObject::sender()->objectName();

    QlibApplication App;
    QWidget *add_app = new QWidget();
    add_app = App.getApp(name_app.split("||")[0]);

    connect(add_app, SIGNAL(sendToolBar(QToolBar*)), this, SLOT(getToolBar(QToolBar*)));
    connect(this, SIGNAL(sendLogin(QString, QString)), add_app, SLOT(getLogin(QString, QString)));
    emit sendLogin(login_users, "User_000");

    tabWidget -> addTab(add_app, QIcon("./Images/Img-012.png"), name_app.split("||")[1]);
    tabWidget -> setCurrentWidget(add_app);

}

void Loncher::swapUsers() {

    qApp -> quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());

}

bool Loncher::check_empty(int ID, QList<QString> &App_list) {

    QSqlQuery query = QSqlQuery(StorageDB);

    query.exec("SELECT * FROM admMenu");

    while(query.next()) {

        if (query.value("Parent") == ID) {

            if (query.value("Object").toString() == "group")

                if (check_empty(query.value("ID").toInt(), App_list) == true) return true;

            if (query.value("Object").toString().split("_")[0] == "form"  && App_list.contains(query.value("Object").toString()) == true) return true;
        }
    }

    return false;

}


void Loncher::closeTabWidget() {

    tabWidget -> removeTab(tabWidget -> currentIndex());

    if (tabWidget -> currentIndex() == -1) central_widget -> hide();
}

void Loncher::getToolBar(QToolBar* toolbar) {

    toolbar -> addWidget(toolBarMain);

    Loncher::addToolBar(toolBarMain);

}


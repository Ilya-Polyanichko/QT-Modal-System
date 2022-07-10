#include "loncher.h"

#include "QlibApplication/qlibapplication.h"
#include "qcoreapplication.h"

Loncher::Loncher(QWidget *parent) : QMainWindow(parent)
{
    loncher_display_settings();
}

Loncher::~Loncher() {}

void Loncher::setAttribute(QString name, QString login, QList<QString> App_list)
{
    login_users = login;
    name_users = name;
    list_application = App_list;

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


    while(query.next())
    {
        if (query.value("lvl").toInt() < 1 && check_empty(query.value("ID").toInt()) == true)
        {
            QMenu *menu = new QMenu(query.value("Name").toString());
            menu -> setObjectName(query.value("Object").toString());
            menu_list.insert(query.value("ID").toInt(), menu);
            generalGroupID.append(query.value("ID").toInt());
        }
        else if (query.value("Object").toString().split("_")[0] == "group" && query.value("lvl").toInt() > 0)
        {
            addMenu(query.value("Parent").toInt(), query.value("ID").toInt(), query.value("Name").toString(), query.value("Object").toString());
        }
        else if (query.value("Object").toString().split("_")[0] == "form")
        {
            addAction(query.value("Parent").toInt(), query.value("Name").toString(), query.value("Object").toString());
        }
    }

    createMenuBar();

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout -> setContentsMargins(10, 10, 10, 10);
    central_widget -> setLayout(gridLayout);
    tabWidget -> setTabsClosable(true);
    gridLayout -> addWidget(tabWidget);
}

void Loncher::addAction(int index_parent, QString name, QString objectName)
{
    if (menu_list.contains(index_parent) == true)
    {
        QAction *action = new QAction(name, menu_list[index_parent]);
        action -> setObjectName(QString(objectName + "||" + name));
        connect(action, SIGNAL(triggered()), SLOT(openApp()));
        foreach(QString it, list_application) if (it == objectName) menu_list[index_parent] -> addAction(action);
    }
}

void Loncher::addMenu(int index_parent, int index_menu, QString name, QString objectName)
{
    if (check_empty(index_menu) == true)
    {
        QMenu *menu = new QMenu(name, menu_list[index_parent]);
        menu -> setObjectName(objectName);

        menu_list.insert(index_menu, menu);
        menu_list[index_parent] -> addMenu(menu);
    }
}

void Loncher::openApp()
{
    Loncher::setCentralWidget(central_widget);
    central_widget -> show();

    QString name_app = QObject::sender()->objectName();

    QlibApplication App;
    QWidget *add_app = new QWidget();
    add_app = App.getApp(name_app.split("||")[0]);


    connect(add_app, SIGNAL(sendToolBar(QToolBar*)), this, SLOT(getToolBar(QToolBar*)));
    connect(add_app, SIGNAL(sendMenu(QMenu*)), this, SLOT(getMenu(QMenu*)));
    connect(this, SIGNAL(sendLogin(QString, QString)), add_app, SLOT(getLogin(QString, QString)));
    emit sendLogin(login_users, "senior_administrator");

    tabWidget -> addTab(add_app, QIcon("./Images/Img-012.png"), name_app.split("||")[1]);
    tabWidget -> setCurrentWidget(add_app);
}

void Loncher::swapUsers()
{
    Access *authentication = new Access();
    if( authentication -> exec() == QDialog::Rejected)
        return void();

    authentication -> deleteLater();

    Loncher *loncher = new Loncher();
    loncher -> setAttribute(authentication -> name,
                   authentication -> login -> text(),
                   authentication -> App_list);
    loncher -> show();
    Loncher::deleteLater();

}

bool Loncher::check_empty(int ID)
{
    QSqlQuery query = QSqlQuery(StorageDB);

    query.exec(QString("SELECT Object "
                       "FROM admMenu "
                       "WHERE parent = %1 "
                       "UNION "
                       "SELECT t1.Object "
                       "FROM admMenu t1 JOIN admMenu t2 "
                       "ON t1.parent = t2.id "
                       "WHERE t2.parent = %1").arg(ID));

    while (query.next())
        if (list_application.contains(query.value("Object").toString()) == true)
            return true;

    return false;
}

void Loncher::closeTabWidget(int index)
{
    tabWidget -> removeTab(index);

    if (tabWidget -> currentIndex() == -1)
    {
        central_widget -> hide();
        toolBarlast -> hide();
        createMenuBar();
    }
}

void Loncher::getToolBar(QToolBar* toolbar)
{
    toolbar -> addWidget(toolBarMain);

    toolBarlast -> hide();
    toolBarlast = toolbar;
    Loncher::addToolBar(toolbar);
    toolbar -> show();
}

void Loncher::getMenu(QMenu *menu)
{
    createMenuBar();
    menuLast = menu;
    Loncher::menuBar() -> addMenu(menu);
}

void Loncher::createMenuBar()
{
    if (Loncher::menuBar())
    {
        delete menuBar();
        QMenuBar *menubar = new QMenuBar();

        for (int it : generalGroupID) menubar -> addMenu(menu_list.value(it));

        QMenuBar *RightBar = new QMenuBar(this);
        RightBar -> setLayoutDirection(Qt::RightToLeft);

        QMenu *userMenu = new QMenu(name_users, RightBar);

        userMenu -> addAction(QIcon("./Images/Img-014.png"), "Сменить пользователя", this, SLOT(swapUsers()));
        userMenu -> addAction(QIcon("./Images/Img-013.png"), "Выход",  qApp, SLOT(quit()));

        RightBar -> addMenu(userMenu);
        menubar -> setCornerWidget(RightBar);
        Loncher::setMenuBar(menubar);
    }
}

void Loncher::loncher_display_settings()
{
    Loncher::setMinimumSize(QSize(854, 480));
    Loncher::setFont(QFont( "Segoe UI Semibold",10 ));
    Loncher::setWindowIcon(QIcon("./Images/Img-003.png"));

    QVBoxLayout *welcome_layout = new QVBoxLayout();
    QLabel *welcome_label_title = new QLabel("LONCHER - загрузчик модулей (приложений) \n\n");

    QLabel *welcome_label_central = new QLabel("Это приложение позволяет динамечески загружать необходимые модули\n(приложения) для конкретного пользователя\n"
                                               "Доступные приложения для пользователя определяются на через систему назначенных ролей\n"
                                               "Для роли могут быть доступны различные приложения, а для пользователя различные роли\n");

    welcome_label_title -> setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    welcome_label_title -> setFont(QFont("Rockin' Record", 15));
    welcome_label_title -> setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    welcome_label_central -> setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    welcome_label_central -> setFont(QFont("Segoe UI Semibold", 12));
    welcome_label_central -> setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    welcome_layout -> addWidget(welcome_label_title);
    welcome_layout -> addWidget(welcome_label_central);
    welcome_widget -> setLayout(welcome_layout);

    Loncher::setCentralWidget(welcome_widget);

    tabWidget -> setTabPosition(QTabWidget :: South);

    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabWidget(int)));

    QPushButton *admin_mail = new QPushButton(QIcon("./Images/Img-022.png"),"");
    connect(admin_mail, SIGNAL(clicked(bool)), this, SLOT(admin_mail_slot()));
    QLabel *label = new QLabel();
    label -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolBarMain -> addWidget(label);
    toolBarMain -> addWidget(admin_mail);
    Loncher::addToolBar(toolBarMain);

    admMenu_table_model -> setTable("admMenu");
    admMenu_table_model -> setEditStrategy(QSqlTableModel::OnManualSubmit);
    admMenu_table_model -> select();
}

void Loncher::admin_mail_slot()
{
    QMessageBox::information(this, "Пример", "Пример использования toolbar лончера");
}

void Loncher::changeEvent(QEvent* event)
{
    if (tabWidget -> currentIndex() != -1)
    {
        createMenuBar();
        Loncher::menuBar() -> addMenu(menuLast);
    }
}

/**************************************************************************************************
 *
 *  1. При создании окна авторизации вызывается функция access_display_settings(). В ней
 *     настраивается оформление окна. Реализован выпадающий список логинов, а также функция скрытия
 *     пароля.
 *
 *  2. После того как пользователь нажмет на кнопку подтвердить, будет вызван слот authentication().
 *     Он проверяет наличие записи пользователя в admUsers. Если пользователь найден, то вызывает
 *     функцию authorization(QString ID), если нет, то выводит на экран соответствующую ошибку
 *
 *  3. Функция authorization(QString ID) проверяет добавленны ли для ползователя приложения в DLL.
 *     Если приложения добавлены то формирует список объекных имен этих приложений, которые потом
 *     передаются в loncher.cpp. Приложения доступные для пользователя определяются исходя из
 *     доступных ролей. За поиск доступных приложений отвечает функция search_application(QString Roles)
 *
 *  4. Функция check_line() проверяет заполнены ли поля ввода логина и пароля. Если нет то блокирует
 *     кнопку 'Подтвердить'
 *
 **************************************************************************************************/

#include "access.h"

Access::Access(QWidget *parent) : QDialog(parent)
{
    access_display_settings();
}

Access::~Access() {}

void Access::authentication()
{
    QSqlQuery query = QSqlQuery(StorageDB);
    query.exec(QString("SELECT * FROM admUsers WHERE Login = '%1' AND Password = '%2'").arg(login -> text()).arg(pass -> text()));

    if (query.next())
    {
        name = query.value("Name").toString();
        authorization(query.value("ID").toString());
    }
    else
    {
        QMessageBox::information(this, "Ошибка аунтификации", "Логин или пароль были введены неверно. Попробуйте еще раз.");
    }
}

void Access::authorization(QString ID)
{
    QSqlQuery query = QSqlQuery(StorageDB);
    query.exec("SELECT admUsers.ID, admRoles.Roles "
               "FROM admUsers "
               "LEFT JOIN admUsersRoles "
               "ON admUsersRoles.admUsers_ID = admUsers.ID "
               "LEFT JOIN admRoles "
               "ON admUsersRoles.admRoles_ID = admRoles.ID");

    while(query.next())
        if (query.value("ID") == ID)
            search_application(query.value("Roles").toString());

    if (App_list.isEmpty() == false)
    {
        Access::accept();
    }
    else
    {
        QMessageBox::information(this, "Ошибка авторизации", "Для Вас еще не добавили приложения. Обратитель к Администратору.");
    }
}

void Access::search_application(QString Roles)
{
    QSqlQuery query = QSqlQuery(StorageDB);
    query.exec("SELECT admRoles.Roles, admMenu.Object "
               "FROM admRoles "
               "LEFT JOIN admRolesMenu "
               "ON admRolesMenu.admRoles_ID = admRoles.ID "
               "LEFT JOIN admMenu "
               "ON admRolesMenu.admMenu_ID = admMenu.ID");

    while(query.next())
        if (query.value("Roles") == Roles && App.check_connection(query.value("Object").toString()) == true)
            App_list.append(query.value("Object").toString());
}

void Access::check_line()
{
    if (login -> text().isEmpty() == false && pass -> text().isEmpty() == false)
    {
        confirm -> setEnabled(true);
    }
    else
    {
        confirm -> setEnabled(false);
    }
}

void Access::access_display_settings()
{
    QSqlQuery query = QSqlQuery(StorageDB);
    query.exec("SELECT * FROM admUsers");
    while(query.next()) login_list.append(query.value("Login").toString());

    Access::setFixedSize(QSize(500, 350));
    Access::setFont(QFont( "Segoe UI Semibold",12 ));
    Access::setWindowIcon(QIcon("./Images/Img-000.png"));

    QLabel *title = new QLabel("АВТОРИЗАЦИЯ");
    title -> setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    title -> setMinimumWidth(50);
    QFontDatabase::addApplicationFont("./Fonts/Fon-000.ttf");
    title -> setFont(QFont("Rockin' Record", 20));
    title -> setAlignment(Qt::AlignCenter);


    login -> addAction(QIcon("./Images/Img-011.png"), QLineEdit::LeadingPosition);
    login -> setPlaceholderText("Введите Ваш логин");

    qComboBox_login -> setDuplicatesEnabled(false);
    qComboBox_login -> setEditable(true);
    qComboBox_login -> setLineEdit(login);
    qComboBox_login -> completer() -> setFilterMode(Qt::MatchContains);
    qComboBox_login -> completer() -> setCompletionMode(QCompleter::PopupCompletion);
    qComboBox_login -> setMaxVisibleItems(8);

    QListView * listView = new QListView(qComboBox_login);
    for (const QString &it : login_list) qComboBox_login -> addItem(it);
    listView -> setStyleSheet("QListView::item { }");
    qComboBox_login->setView(listView);


    pass -> addAction(QIcon("./Images/Img-011.png"), QLineEdit::LeadingPosition);
    pass -> setPlaceholderText("Введите Ваш пароль");
    pass -> setEchoMode(QLineEdit::Password);

    QWidget *confirm_box_widget = new QWidget();
    confirm_box_widget -> setFixedHeight(45);
    QHBoxLayout *confirm_layout = new QHBoxLayout();
    confirm -> setFixedSize(QSize(250, 30));
    confirm -> setEnabled(false);
    confirm_layout -> addWidget(confirm);
    confirm_box_widget -> setLayout(confirm_layout);

    connect(confirm, SIGNAL(clicked()), this, SLOT(authentication()));
    connect(login, SIGNAL(textChanged(const QString &)), this, SLOT(check_line()));
    connect(pass, SIGNAL(textChanged(const QString &)), this, SLOT(check_line()));

    QGridLayout *gridLayout = new QGridLayout();

    gridLayout -> setContentsMargins(25, 0, 25, 0);
    gridLayout -> addWidget(title);
    gridLayout -> addWidget(qComboBox_login);
    gridLayout -> addWidget(pass);
    gridLayout -> addWidget(confirm_box_widget);

    login -> clear();

    setLayout(gridLayout);
}

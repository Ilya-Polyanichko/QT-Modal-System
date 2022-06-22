#include "access.h"

#include <QDebug>

#include <QMenuBar>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QLayout>
#include <QFontDatabase>
#include <QSqlError>
#include <QEvent>
#include <QCompleter>
#include <QListView>

Access::Access(QWidget *parent) : QWidget(parent) {

    StorageDB = QSqlDatabase::addDatabase("QSQLITE");
    StorageDB.setDatabaseName("./StorageDB.db");
    StorageDB.open();

    QSqlQuery query = QSqlQuery(StorageDB);

    query.exec("SELECT * FROM admUsers");

    while(query.next()) {
        login_list.append(query.value("Login").toString());
    }

    QFontDatabase::addApplicationFont("./Fonts/Fon-000.ttf");

    Access::setFixedSize(QSize(500, 350));
    Access::setFont(QFont( "Segoe UI Semibold",12 ));
    Access::setWindowIcon(QIcon("./Images/Img-000.png"));

    QLabel *title = new QLabel("АВТОРИЗАЦИЯ");
    title -> setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    title -> setMinimumWidth(50);
    title -> setFont(QFont("Rockin' Record", 20));
    title -> setAlignment(Qt::AlignCenter);

    Access::login -> addAction(QIcon("./Images/Img-011.png"), QLineEdit::LeadingPosition);
    Access::login -> setPlaceholderText("Введите Ваш логин");

    qComboBox_login -> setDuplicatesEnabled(false);
    qComboBox_login -> setEditable(true);

    qComboBox_login -> setLineEdit(login);
    qComboBox_login -> completer() -> setFilterMode(Qt::MatchContains);
    qComboBox_login -> completer() -> setCompletionMode(QCompleter::PopupCompletion);
    qComboBox_login -> setMaxVisibleItems(8);

    QListView * listView_1 = new QListView(qComboBox_login);

    for (const QString &it : login_list) qComboBox_login -> addItem(it);

    listView_1 -> setStyleSheet("QListView::item { }");
    qComboBox_login->setView(listView_1);


    Access::pass -> addAction(QIcon("./Images/Img-011.png"), QLineEdit::LeadingPosition);
    Access::pass -> setPlaceholderText("Введите Ваш пароль");
    pass -> setEchoMode(QLineEdit::Password);

    QWidget *confirm_widget = new QWidget();
    confirm_widget -> setFixedHeight(45);
    QHBoxLayout *confirm_box = new QHBoxLayout();
    confirm -> setFixedSize(QSize(250, 30));
    confirm_box -> addWidget(confirm);
    confirm_widget -> setLayout(confirm_box);

    connect(confirm, SIGNAL(clicked()), this, SLOT(verification()));

    connect(login, SIGNAL(textChanged(const QString &)), this, SLOT(check_line()));
    connect(pass, SIGNAL(textChanged(const QString &)), this, SLOT(check_line()));

    QGridLayout *gridLayout = new QGridLayout();

    gridLayout -> setContentsMargins(25, 0, 25, 0);
    gridLayout -> addWidget(title);
    gridLayout -> addWidget(qComboBox_login);
    gridLayout -> addWidget(pass);
    gridLayout -> addWidget(confirm_widget);

    login -> clear();
    pass -> clear();

    setLayout(gridLayout);
    Access::installEventFilter(this);
}

Access::~Access() {

}

void Access::verification() {



    QSqlQuery query = QSqlQuery(StorageDB);

    query.exec("SELECT admUsers.ID, admUsers.Name, admUsers.Login, admUsers.Password, admRoles.Roles "
               "FROM admUsers "
               "LEFT JOIN admUsersRoles "
               "ON admUsersRoles.admUsers_ID = admUsers.ID "
               "LEFT JOIN admRoles "
               "ON admUsersRoles.admRoles_ID = admRoles.ID");

    QString login = Access::login -> text();
    QString pass = Access::pass -> text();
    int check = 0;

    QString name;
    QlibApplication App;

    while(query.next()) {
        if (query.value("Login") == login && query.value("Password") == pass) {

            check++;
            sorting(query.value("Roles").toString(), App_list);
            name = query.value("Name").toString();
            login = query.value("Login").toString();

        }
    }
    if (check == 0) {
        QMessageBox::information(this, "Ошибка авторизации", "Логин или пароль были введены неверно. Попробуйте еще раз.");
    }

    if (App_list.isEmpty() == false) {

        close();
        emit sendText(name, login, App_list);

    } else if (App_list.isEmpty() == true && check != 0) {

        QMessageBox::information(this, "Ошибка авторизации", "Для Вас еще не добавили приложения. Обратитель к Администратору.");

    }
}

void Access::sorting(QString Roles, QList<QString> &App_list) {

    QSqlQuery query = QSqlQuery(StorageDB);
    query.exec("SELECT admRoles.ID, admRoles.Roles, admMenu.Name, admMenu.Object "
               "FROM admRoles "
               "LEFT JOIN admRolesMenu "
               "ON admRolesMenu.admRoles_ID = admRoles.ID "
               "LEFT JOIN admMenu "
               "ON admRolesMenu.admMenu_ID = admMenu.ID");

    while(query.next()) {

        if (query.value("Roles") == Roles && App.check_connection(query.value("Object").toString()) == true && App_list.contains(query.value("Object").toString()) == false) {

            App_list.append(query.value("Object").toString());

        }
    }

}

void Access::check_line() {

    if (login -> text().isEmpty() == false && pass -> text().isEmpty() == false) {

        confirm -> setEnabled(true);

    } else {

        confirm -> setEnabled(false);

    }
}

void Access::keyPressEvent(QKeyEvent *event) {

    //Access::keyPressEvent(event);

    if (event -> key() == Qt::Key_Enter) { login -> setText("dfgdfgg"); }

}


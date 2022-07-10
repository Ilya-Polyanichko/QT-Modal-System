/**************************************************************************************************
 *
 *  1. initialization_database() - открывает базу данных. В случае неудачи вызывает error_output().
 *     Далее проверяем наличие таблиц в базе данных. Если таблицы нет то создается функцией
 *     create_table_database(QSqlDatabase StorageDB, QString nameTable);
 *
 *  2. После проверяем запись старшего администратора. Принцип следующий отправляется запрос в бд
 *     на создание записи. Если запись существует, то вернется false, который не обрабатывается,
 *     если нет то создастся запись. После создания записи перепроверяется ее ID. Для выстраивания
 *     связей. Алгоритм отправки запросов:
 *
 *          а) Отправляем запрос на созадение учетной записи старшего админа
 *          б) Записываем ID в admUsers_ID
 *          б) Отправляем запрос на создание роли старшего администратора
 *          в) Записываем ID роли в admRoles_ID
 *          г) Проверяем есть ли связь admUsers_ID и admRoles_ID, если нет то создаем
 *          д) Отправляем запрос на создание группы Администрирование
 *          е) Записываем ID группы в admMenu_ID
 *          ж) Отправляем запрос на создание подгруппы Панель администратора
 *          з) Записываем ID группы в admMenu_ID
 *          и) Отправляем группу запросов на создание админских приложений
 *          й) Записываем ID приложений в admMenuApp_ID
 *          к) Проверяем наличие связей приложений и роли, если нет то создаем
 *
 **************************************************************************************************/

#include "database.h"

bool initialization_database()
{
    QSqlDatabase StorageDB;
    StorageDB = QSqlDatabase::addDatabase("QSQLITE");
    StorageDB.setDatabaseName("./StorageDB.db");

    if (StorageDB.open() == false)
    {
        error_output(StorageDB.lastError().text());
        return false;
    }

    QList<QString> admin_tables_database = { "admRoles", "admUsers", "admMenu", "admRolesMenu", "admUsersRoles" };

    for (QString it : admin_tables_database)
        if (StorageDB.tables().contains(it) == false) create_table_database(StorageDB, it);

    create_senior_admin_record(StorageDB);

    return true;
}

void create_table_database(QSqlDatabase StorageDB, QString nameTable)
{
    QSqlQuery query = QSqlQuery(StorageDB);

    QMap<QString, QString> QueryCreateTable;

    QueryCreateTable.insert("admMenu", "CREATE TABLE admMenu ( "
                                       "ID     INTEGER PRIMARY KEY, "
                                       "Name   TEXT    UNIQUE, "
                                       "Object TEXT    UNIQUE, "
                                       "Parent INTEGER REFERENCES admMenu (ID))");

    QueryCreateTable.insert("admRoles", "CREATE TABLE admRoles ( "
                                        "ID     INTEGER PRIMARY KEY, "
                                        "Roles  TEXT    UNIQUE, "
                                        "Object TEXT    UNIQUE, "
                                        "Info   TEXT)");

    QueryCreateTable.insert("admUsers", "CREATE TABLE admUsers ( "
                                        "ID       INTEGER PRIMARY KEY AUTOINCREMENT, "
                                        "Name     TEXT, "
                                        "Login    TEXT    UNIQUE, "
                                        "Password TEXT, "
                                        "INFO     TEXT)");

    QueryCreateTable.insert("admRolesMenu", "CREATE TABLE admRolesMenu ( "
                                            "ID          INTEGER PRIMARY KEY, "
                                            "admRoles_ID INTEGER REFERENCES admRoles (ID)"
                                            "ON DELETE CASCADE ON UPDATE CASCADE, "
                                            "admMenu_ID  INTEGER REFERENCES admMenu (ID)"
                                            "ON DELETE CASCADE ON UPDATE CASCADE)");

    QueryCreateTable.insert("admUsersRoles", "CREATE TABLE admUsersRoles ( "
                                             "ID          INTEGER PRIMARY KEY AUTOINCREMENT, "
                                             "admUsers_ID INTEGER REFERENCES admUsers (ID) "
                                             "ON DELETE CASCADE ON UPDATE CASCADE, "
                                             "admRoles_ID INTEGER REFERENCES admRoles (ID) "
                                             "ON DELETE CASCADE ON UPDATE CASCADE)");

    query.exec(QueryCreateTable.value(nameTable));
}

void create_senior_admin_record(QSqlDatabase StorageDB)
{
    QSqlQuery query = QSqlQuery(StorageDB);

    int admMenu_ID = 0;
    int admUsers_ID = 0;
    int admRoles_ID = 0;
    QList<int> admMenuApp_ID;


    query.exec("INSERT INTO admUsers (name, login, password, info) VALUES ('Admin', 'senior_administrator', '000', 'Запись старшего администратора')");
    admUsers_ID = searchID(StorageDB, "admUsers", "login", "senior_administrator");

    query.exec("INSERT INTO admRoles (roles, object, info) VALUES ('Старший администратор', 'senior_administrator', 'Роль старшего администратора')");
    admRoles_ID = searchID(StorageDB, "admRoles", "Object", "senior_administrator");

    query.exec(QString("SELECT * FROM admUsersRoles WHERE admUsers_ID = %1 AND admRoles_ID = %2").arg(admUsers_ID).arg(admRoles_ID));
    if (query.last() == false)
        query.exec(QString("INSERT INTO admUsersRoles (admUsers_ID, admRoles_ID) VALUES ('%1', '%2')").arg(admUsers_ID).arg(admRoles_ID));

    query.exec("INSERT INTO admMenu (name, object) VALUES ('Администрирование', 'group_administration')");
    admMenu_ID = searchID(StorageDB, "admMenu", "Object", "group_administration");

    query.exec(QString("INSERT INTO admMenu (name, object, parent) VALUES ('Панель администратора', 'group_admin_panel', %1)").arg(admMenu_ID));
    admMenu_ID = searchID(StorageDB, "admMenu", "Object", "group_admin_panel");

    query.exec(QString("INSERT INTO admMenu (name, object, parent) VALUES ('Управление меню', 'form_management_menu', %1)").arg(admMenu_ID));
    query.exec(QString("INSERT INTO admMenu (name, object, parent) VALUES ('Управление ролями', 'form_management_roles', %1)").arg(admMenu_ID));
    query.exec(QString("INSERT INTO admMenu (name, object, parent) VALUES ('Управление пользователями', 'form_management_users', %1)").arg(admMenu_ID));

    query.exec("SELECT * FROM admMenu");
    while (query.next())
        if (query.value("Object") == "form_management_menu" || query.value("Object") == "form_management_roles"
                                                            || query.value("Object") == "form_management_users")
            admMenuApp_ID.append(query.value("ID").toInt());

    for (int it : admMenuApp_ID)
    {
        query.exec(QString("SELECT * FROM admRolesMenu WHERE admMenu_ID = %1 AND admRoles_ID = %2").arg(it).arg(admRoles_ID));
        if (query.last() == false)
            query.exec(QString("INSERT INTO admRolesMenu (admRoles_ID, admMenu_ID) VALUES ('%1', '%2')")
                       .arg(admRoles_ID)
                       .arg(it));
    }
}

int searchID(QSqlDatabase StorageDB, QString nameTable, QString nameColumn, QString argument)
{
    QSqlQuery query = QSqlQuery(StorageDB);

    query.exec(QString("SELECT * FROM %1").arg(nameTable));
    while (query.next())
        if (query.value(nameColumn) == argument)
            return query.value("ID").toInt();

    return 0;
}

void error_output(QString text_error)
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("Ошибка инициализации базы данных");
    msgBox.setText(text_error);

    msgBox.addButton("Закрыть", QMessageBox::NoRole);
    msgBox.exec();
}

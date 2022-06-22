#ifndef MANAGEMENT_USERS_H
#define MANAGEMENT_USERS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <QTableView>
#include <QSqlTableModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QSqlQueryModel>
#include <QPushButton>
#include <QToolBar>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSqlRecord>
#include <QWidgetAction>
#include <QCheckBox>

class management_users : public QWidget
{
    Q_OBJECT

    QTableView *tableView = new QTableView();
    QSqlTableModel *admUsers_table_model = new QSqlTableModel();
    QSqlTableModel *admUsersRoles_table_model = new QSqlTableModel();

    /* Организация виджетов */

    QHBoxLayout *central_layout = new QHBoxLayout();
    QVBoxLayout *menu_layout = new QVBoxLayout();
    QVBoxLayout *general_layout = new QVBoxLayout();
    QHBoxLayout *search_admUsers_layout = new QHBoxLayout();


    /* Боковая панель меню */

    QMenu *menu = new QMenu();
    QMap<int, QString> roles_list;
    QMap<int, int> menu_list;
    QList<QString> management_users_update_list;
    QLabel *label = new QLabel("Доступные роли");


    /* Панель ToolBar */

    QToolBar *toolBar = new QToolBar();
    QPushButton *insert_row_admUsers = new QPushButton(QIcon("./Images/Img-010.png"), "Добавить");
    QPushButton *management_users_update = new QPushButton(QIcon("./Images/Img-019.png"), "Обновить");
    QPushButton *delete_row_admUsers = new QPushButton(QIcon("./Images/Img-004.png"),"Удалить");


    /* Виджеты для фильтра пользователей */

    QLineEdit *search_admUsers = new QLineEdit();
    QPushButton *search_admUsers_button = new QPushButton("Применить");
    QPushButton *resert_admUsers_button = new QPushButton("Сбросить");

    bool general_admin = false;
    int row_general_admin = -1;
    QString login_general_admin = "User_000";

public:
    management_users(QWidget *parent = nullptr);
    ~management_users();

private:
    QSqlDatabase StorageDB;

signals:
    void sendToolBar(QToolBar* toolbar);

protected:
      void showEvent(QShowEvent *event);

private slots:
    QSqlTableModel* create_model();
    void pressed_cell_admUsers();
    void set_users_roles();
    void management_users_update_slot();
    void insert_row_admUsers_slot();
    void delete_row_admUsers_slot();
    void search_admUsers_slot();
    void getLogin(QString login, QString general_login);
    void aboutToHide_slot();
    void menu_layout_create_clear(bool state);

};
#endif // MANAGEMENT_USERS_H

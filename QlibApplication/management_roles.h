#ifndef MANAGEMENT_ROLES_H
#define MANAGEMENT_ROLES_H

#include <QWidget>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>

#include <QTableView>
#include <QHeaderView>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>
#include <QToolBar>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>

class management_roles : public QWidget
{
    Q_OBJECT

    // Модель таблицы admRoles

    QTableView *tableView = new QTableView();
    QSqlTableModel *admRoles_table_model = new QSqlTableModel();
    QSqlTableModel *admRolesMenu_table_model = new QSqlTableModel();
    bool general_admin = false;
    int row_general_admin = -1;
    QString general_roles_name = "Старший администратор";

    /* Организация виджетов */

    QHBoxLayout *central_layout = new QHBoxLayout();
    QVBoxLayout *general_layout = new QVBoxLayout();


    /* Панель ToolBar */

    QToolBar *toolBar = new QToolBar();
    QPushButton *insert_row_admRoles = new QPushButton(QIcon("./Images/Img-010.png"), "Добавить");
    QPushButton *admRoles_table_model_update = new QPushButton(QIcon("./Images/Img-019.png"), "Обновить");
    QPushButton *admRoles_table_model_delete = new QPushButton(QIcon("./Images/Img-004.png"),"Удалить");

    /* Панель MenuBar */

    QMenu *menuBar = new QMenu("Управление ролями");
    QAction *action_insert = new QAction("Добавить", menuBar);
    QAction *action_update = new QAction("Обновить", menuBar);
    QAction *action_delete = new QAction("Удалить", menuBar);

    // Блок фильтра пользователей

    QLineEdit *search_admRoles = new QLineEdit();
    QHBoxLayout *search_admRoles_layout = new QHBoxLayout();
    QPushButton *search_admRoles_button = new QPushButton("Применить");
    QPushButton *resert_admRoles_button = new QPushButton("Сбросить");

    // Боковое меню перемещения папок и просмотра доступных ролей

    QMenu *menu = new QMenu();
    QMap<int, QString> menu_list;
    QMap<int, int> roles_list;
    QList<QString> management_roles_update_list;
    QVBoxLayout *menu_layout = new QVBoxLayout();
    QLabel *label = new QLabel("Доступные приложения");
    QList<QString> admin_app = {"Управление меню", "Управление пользователями", "Управление ролями"};

public:
    management_roles(QWidget *parent = nullptr);
    ~management_roles();

private:
    QSqlDatabase StorageDB;

private slots:

    QSqlTableModel* create_model();
    void pressed_cell_admRoles_tree_model();
    void admRoles_table_model_delete_slot();

    void insert_row_admRoles_slot();
    void admRoles_table_model_update_slot();

    void set_menu_roles();
    void search_admRoles_slot();

    void getLogin(QString login, QString general_login);
    void aboutToHide_slot();
    void menu_layout_create_clear(bool state);

protected:
      void showEvent(QShowEvent* event);

signals:
    void sendToolBar(QToolBar* toolbar);
    void sendMenu(QMenu* menuBar);

};
#endif // MANAGEMENT_ROLES_H

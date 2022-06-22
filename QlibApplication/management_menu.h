#ifndef MANAGEMENT_MENU_H
#define MANAGEMENT_MENU_H

#include <QWidget>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QSqlError>

#include <QTreeView>
#include <QStandardItemModel>
#include <QHeaderView>

#include <QVBoxLayout>
#include <QPushButton>
#include <QList>
#include <QMenu>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QToolBar>
#include <QLabel>


class management_menu : public QWidget
{
    Q_OBJECT

    // Иерархическое древо

    QStandardItemModel *admMenu_tree_model = new QStandardItemModel();
    QStringList name_column = { "Name", "Object", "CONNECT", "ID", "PARENT", "DELETE" };
    QTreeView *treeView = new QTreeView();
    QList<QString> menu_update_list;
    int admMenu_table_row_count;
    int root_model_row;


    // Блок редактирования имени и объектного имени приложения или папки

    QLineEdit *line_name_app = new QLineEdit();
    QLineEdit *line_object_name_app = new QLineEdit();
    QPushButton *save_line_name_app = new QPushButton("Применить");
    QHBoxLayout *line_name_app_layout = new QHBoxLayout();

    QString last_record_name = " ";
    QString last_record_object_name = " ";


    // Боковое меню перемещения папок и просмотра доступных ролей

    QMenu *menu = new QMenu();
    QVBoxLayout *menu_layout = new QVBoxLayout();

    QSqlQueryModel *admRolesMenu = new QSqlQueryModel();
    QSqlTableModel *admRolesMenu_table_model = new QSqlTableModel();
    QLabel *label = new QLabel();

    QMap<int, QString> roles_list_map;
    QMap<int, int> menu_list_map;
    QMap<int, QString> menu_map_name;
    QList<QString> obgect_name_list;


    // Организация ToolBar

    QToolBar *toolBar = new QToolBar();
    QPushButton *insert_root_folder_admMenu = new QPushButton(QIcon("./Images/Img-018.png"), "Добавить корневую папку");
    QPushButton *insert_folder_admMenu = new QPushButton(QIcon("./Images/Img-018.png"), "Добавить папку");
    QPushButton *insert_application_admMenu = new QPushButton(QIcon("./Images/Img-010.png"), "Добавить приложение");
    QPushButton *admMenu_tree_model_update = new QPushButton(QIcon("./Images/Img-019.png"), "Обновить");
    QPushButton *admMenu_tree_model_delete = new QPushButton(QIcon("./Images/Img-004.png"),"Удалить");


    // Организация centralLayout

    QLabel *label_info = new QLabel("На данный момент таблица admMenu недоступна, так как редактируется другим администратором");

    QHBoxLayout *central_layout = new QHBoxLayout();
    QVBoxLayout *general_layout = new QVBoxLayout();

public:
    management_menu(QWidget *parent = nullptr);
    ~management_menu();

private:
    QSqlDatabase StorageDB;

protected:
      void showEvent();

signals:
    void sendToolBar(QToolBar* toolbar);

private slots:

    QStandardItemModel* create_tree_model();
    void create_tree_model_recursion(QSqlTableModel* admMenu_model, QStandardItemModel* tree_model, QMap<int, QModelIndex> admMenu_parent_ID);
    void create_tree_model_decoration(QStandardItemModel* tree_model, QModelIndex index, QString object_name);
    QString access_application(QString object_name_app);

    void pressed_item_admMenu_tree_model();

    void save_line_name_app_slot();
    void line_name_app_change();

    void insert_row_admMenu_slot();
    void admMenu_tree_model_update_slot();
    void admMenu_tree_model_delete_slot();


    void set_parent_ID();
    void menu_layout_create_clear(bool state);
    QModelIndex check_parent_index(QModelIndex index);
    void aboutToHide_slot();

};
#endif // MANAGEMENT_MENU_H

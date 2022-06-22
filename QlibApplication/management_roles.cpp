/**************************************************************************************************
 *
 *   1. В конструкторе подключаем базу данных SQLITE - StorageDB и открываем ее.
 *
 *   2. Создаем иерархическое древо Menu - Bar. Древо создается в функции create_tree_model()
 *      Для создания древа выгружаю таблицу admMenu в модель admMenu_table_model (QSqlTableModel),
 *      после чего используя колонки ID и Parent рекурсивно собираю древо.
 *
 **************************************************************************************************/

#include "management_roles.h"

management_roles::management_roles(QWidget *parent) : QWidget(parent) {

    StorageDB = QSqlDatabase::addDatabase("QSQLITE");
    StorageDB.setDatabaseName("./StorageDB.db");
    StorageDB.open();

    admRoles_table_model = create_model();
    tableView -> setModel(admRoles_table_model);

    tableView -> horizontalHeader() -> setStretchLastSection(false);
    tableView -> horizontalHeader() -> setDefaultAlignment(Qt::AlignCenter);
    tableView -> hideColumn(0);
    for (int column = 0; column < 3; column ++) tableView -> horizontalHeader() -> resizeSection(column, 220);
    tableView -> horizontalHeader() -> setSectionResizeMode(3, QHeaderView::Stretch);

    connect(tableView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressed_cell_admRoles_tree_model()));

    connect(insert_row_admRoles, SIGNAL(clicked()), this, SLOT(insert_row_admRoles_slot()));
    connect(admRoles_table_model_update, SIGNAL(clicked()), this, SLOT(admRoles_table_model_update_slot()));
    connect(admRoles_table_model_delete, SIGNAL(clicked()), this, SLOT(admRoles_table_model_delete_slot()));

    toolBar -> addWidget(insert_row_admRoles);
    toolBar -> addWidget(admRoles_table_model_update);
    toolBar -> addWidget(admRoles_table_model_delete);


    search_admRoles -> setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    search_admRoles -> setMaximumHeight(30);
    search_admRoles -> addAction(QIcon("./Images/Img-023.png"), QLineEdit::LeadingPosition);
    search_admRoles -> setPlaceholderText("Введите название роли и примените фильтр");

    search_admRoles_button -> setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    search_admRoles_button -> setMaximumSize(QSize(75,30));

    resert_admRoles_button -> setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    resert_admRoles_button -> setMaximumSize(QSize(75,30));

    connect(search_admRoles_button, SIGNAL(clicked()), this, SLOT(search_admRoles_slot()));
    connect(resert_admRoles_button, SIGNAL(clicked()), this, SLOT(admRoles_table_model_update_slot()));

    menu -> setMinimumWidth(230);
    menu -> setMaximumWidth(230);
    menu -> setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    label -> setMaximumSize(QSize(230,30));
    label -> setAlignment(Qt::AlignCenter);
    label -> setStyleSheet("border: 1px solid black;");

    connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHide_slot()));

    search_admRoles_layout -> addWidget(search_admRoles);
    search_admRoles_layout -> addWidget(search_admRoles_button);
    search_admRoles_layout -> addWidget(resert_admRoles_button);


    general_layout -> addWidget(toolBar);
    general_layout -> addWidget(tableView);
    general_layout -> addLayout(search_admRoles_layout);

    general_layout -> addLayout(search_admRoles_layout);

    central_layout -> addLayout(general_layout);
    central_layout -> addLayout(menu_layout);

    setLayout(central_layout);

}

management_roles::~management_roles() {}

QSqlTableModel* management_roles::create_model() {

    QSqlTableModel *admRoles_model = new QSqlTableModel();

    admRoles_model->setTable("admRoles");

    admRoles_model -> setEditStrategy(QSqlTableModel::OnManualSubmit);

    admRoles_model->select();

    admRoles_model -> setHeaderData(1, Qt::Horizontal, "Название роли");
    admRoles_model -> setHeaderData(2, Qt::Horizontal, "Объектное имя");
    admRoles_model -> setHeaderData(3, Qt::Horizontal, "Описание роли");

    for (int row = 0; row < admRoles_model -> rowCount(); row ++)
        if (admRoles_model -> data(admRoles_model -> index(row, 1)).toString() == general_roles_name) row_general_admin = row;


    QSqlTableModel *admRolesMenu_model = new QSqlTableModel();

    admRolesMenu_model -> setTable("admRolesMenu");
    admRolesMenu_model -> select();

    admRolesMenu_table_model = admRolesMenu_model;

    QSqlQuery query = QSqlQuery(StorageDB);
    query.exec("SELECT * FROM admMenu");

    while (query.next()) if (query.value("Object") != "group") menu_list.insert(query.value("ID").toInt(), query.value("Name").toString());

    return admRoles_model;

}

void management_roles::pressed_cell_admRoles_tree_model() {

    QModelIndex index = tableView -> currentIndex();
    menu_layout_create_clear(false);
    int ID = admRoles_table_model -> data(admRoles_table_model -> index(index.row(), 0)).toInt();

    if (index.column() == 1 && menu_list.size() > 0) {

        QMap<int, QAction*> list_action;

        for( auto key : menu_list.keys()) {

            QAction *action = new QAction(menu_list.value(key));
            action -> setCheckable(true);
            action -> setObjectName(QString::number(key));
            connect(action, SIGNAL(triggered()), this, SLOT(set_menu_roles()));

            if (admin_app.contains(menu_list.value(key)) == true && general_admin == false)
                action -> setEnabled(false);

            menu -> addAction(action);
            list_action.insert(key, action);

        }


        for (int row = 0; row < admRolesMenu_table_model -> rowCount(); row ++) {

            QSqlRecord record = admRolesMenu_table_model -> record(row);

            if (ID == record.value("admRoles_ID")) {

                list_action.value(record.value("admMenu_ID").toInt()) -> setChecked(true);
                roles_list.insert(record.value("admMenu_ID").toInt(), ID);

            }

        }

        menu_layout_create_clear(true);
    }
}

void management_roles::set_menu_roles() {

    QModelIndex index = admRoles_table_model -> index(tableView -> currentIndex().row(), 0);

    for( auto & key : roles_list.keys()) if (key == sender() -> objectName().toInt()) {
        roles_list.remove(key);
        management_roles_update_list.append(QString("DELETE FROM admRolesMenu WHERE admRoles_ID = '%1' AND admMenu_ID = '%2'")
                                 .arg(admRoles_table_model -> data(index).toString())
                                 .arg(sender() -> objectName()));
    }

    roles_list.insert(sender() -> objectName().toInt(), admRoles_table_model -> data(index).toInt());
    management_roles_update_list.append(QString("INSERT INTO admRolesMenu (admRoles_ID, admMenu_ID) VALUES ('%1', '%2')")
                             .arg(admRoles_table_model -> data(index).toString())
                             .arg(sender() -> objectName()));

    QSize rec = management_roles::size();
    menu -> setMinimumHeight(rec.height());
    menu -> setMinimumHeight(0);

}

void management_roles::admRoles_table_model_update_slot() {

    if (admRoles_table_model -> isDirty() == true || management_roles_update_list.size() != 0) {

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Подтвердите изменения");
        msgBox.setText("Внимание! В таблицах admRoles и admRolesMenu были внесены изменения. Подтвердите, что хотите их сохранить");

        QPushButton *abortButton = msgBox.addButton("Отменить изменения", QMessageBox::NoRole);
        QPushButton *connectButton = msgBox.addButton("Применить изменения", QMessageBox::YesRole);

        msgBox.exec();

        if (msgBox.clickedButton() == connectButton) {

            QSqlQuery query = QSqlQuery(StorageDB);
            for (int i = 0; i < management_roles_update_list.size(); i ++) query.exec(management_roles_update_list[i]);

            admRoles_table_model -> submitAll();
            management_roles_update_list.clear();

            admRoles_table_model = create_model();
            tableView -> setModel(admRoles_table_model);

        } else if (msgBox.clickedButton() == abortButton) {}

    } else {

        admRoles_table_model = create_model();
        tableView -> setModel(admRoles_table_model);

    }

}

void management_roles::insert_row_admRoles_slot() {

    admRoles_table_model -> insertRow(admRoles_table_model -> rowCount());

}

void management_roles::admRoles_table_model_delete_slot() {

    if (tableView -> currentIndex().row() != row_general_admin) admRoles_table_model -> removeRow(tableView -> currentIndex().row());

}

void management_roles::search_admRoles_slot() {


    if (admRoles_table_model -> isDirty() == true && search_admRoles -> text().isEmpty() == false) {

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Подтвердите изменения");
        msgBox.setText("Внимание! В таблице имеются несохраненные изменения. Применение фильтра приведет к потере этих изменений.");

        msgBox.addButton("Отмена", QMessageBox::NoRole);
        QPushButton *connectButton = msgBox.addButton("Применить изменения", QMessageBox::YesRole);

        msgBox.exec();

        if (msgBox.clickedButton() == connectButton) {

            admRoles_table_model_update_slot();
            admRoles_table_model -> setFilter(QString("Roles = '%1'").arg(search_admRoles -> text()));

        }

    } else if (search_admRoles -> text().isEmpty() == false) {

        admRoles_table_model -> setFilter(QString("Roles = '%1'").arg(search_admRoles -> text()));

    }

}

void management_roles::getLogin(QString login, QString general_login) {

    (login == general_login) ? general_admin = true : general_admin = false;
    if (general_admin == false) tableView -> hideRow(row_general_admin);

}

void management_roles::aboutToHide_slot() { menu_layout_create_clear(true); }

void management_roles::menu_layout_create_clear(bool state) {

    if (state == false) {

        label -> setVisible(false);
        menu_layout -> removeWidget(menu);
        menu -> clear();

    } else {

        menu_layout -> addWidget(label);
        menu_layout -> addWidget(menu);
        label -> setVisible(true);
        menu -> show();

    }

}

void management_roles::showEvent() {

    emit sendToolBar(toolBar);

}

#include "management_users.h"

management_users::management_users(QWidget *parent) : QWidget(parent) {

    StorageDB = QSqlDatabase::addDatabase("QSQLITE");
    StorageDB.setDatabaseName("./StorageDB.db");
    StorageDB.open();

    admUsers_table_model = create_model();
    tableView -> setModel(admUsers_table_model);

    tableView -> horizontalHeader() -> setStretchLastSection(false);
    tableView -> horizontalHeader() -> setDefaultAlignment(Qt::AlignCenter);
    tableView -> hideColumn(0);
    for (int column = 0; column < 4; column ++) tableView -> horizontalHeader() -> resizeSection(column, 150);
    tableView -> horizontalHeader() -> setSectionResizeMode(4, QHeaderView::Stretch);

    connect(tableView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressed_cell_admUsers()));

    connect(insert_row_admUsers, SIGNAL(clicked()), this, SLOT(insert_row_admUsers_slot()));
    connect(management_users_update, SIGNAL(clicked()), this, SLOT(management_users_update_slot()));
    connect(delete_row_admUsers, SIGNAL(clicked()), this, SLOT(delete_row_admUsers_slot()));

    toolBar -> addWidget(insert_row_admUsers);
    toolBar -> addWidget(management_users_update);
    toolBar -> addWidget(delete_row_admUsers);

    search_admUsers -> setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    search_admUsers -> setMaximumHeight(30);
    search_admUsers -> addAction(QIcon("./Images/Img-023.png"), QLineEdit::LeadingPosition);
    search_admUsers -> setPlaceholderText("Введите логин пользователя и примените фильтр");

    search_admUsers_button -> setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    search_admUsers_button -> setMaximumSize(QSize(75,30));
    connect(search_admUsers_button, SIGNAL(clicked()), this, SLOT(search_admUsers_slot()));

    resert_admUsers_button -> setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    resert_admUsers_button -> setMaximumSize(QSize(75,30));
    connect(resert_admUsers_button, SIGNAL(clicked()), this, SLOT(management_users_update_slot()));

    menu -> setMinimumWidth(230);
    menu -> setMaximumWidth(230);

    label -> setMaximumSize(QSize(230,30));
    label -> setAlignment(Qt::AlignCenter);
    label -> setStyleSheet("border: 1px solid black;");

    connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHide_slot()));

    search_admUsers_layout -> addWidget(search_admUsers);
    search_admUsers_layout -> addWidget(search_admUsers_button);
    search_admUsers_layout -> addWidget(resert_admUsers_button);

    general_layout -> addWidget(toolBar);
    general_layout -> addWidget(tableView);
    general_layout -> addLayout(search_admUsers_layout);

    central_layout -> addLayout(general_layout);
    central_layout -> addLayout(menu_layout);

    setLayout(central_layout);

}

management_users::~management_users() {}

QSqlTableModel* management_users::create_model() {

    QSqlTableModel *admUsers_model = new QSqlTableModel();

    admUsers_model->setTable("admUsers");
    admUsers_model -> setEditStrategy(QSqlTableModel::OnManualSubmit);
    admUsers_model->select();

    admUsers_model -> setHeaderData(1, Qt::Horizontal, "Имя пользователя");
    admUsers_model -> setHeaderData(2, Qt::Horizontal, "Логин пользователя");
    admUsers_model -> setHeaderData(3, Qt::Horizontal, "Пароль пользователя");
    admUsers_model -> setHeaderData(4, Qt::Horizontal, "Дополнительная информация");

    for (int row = 0; row < admUsers_model -> rowCount(); row ++)
        if (admUsers_model -> data(admUsers_model -> index(row, 2)).toString() == login_general_admin) row_general_admin = row;

    QSqlTableModel *admUsersRoles_model = new QSqlTableModel();

    admUsersRoles_model -> setTable("admUsersRoles");
    admUsersRoles_model -> select();

    admUsersRoles_table_model = admUsersRoles_model;

    QSqlQuery query = QSqlQuery(StorageDB);
    query.exec("SELECT * FROM admRoles");
    while (query.next()) roles_list.insert(query.value("ID").toInt(), query.value("Roles").toString());

    return admUsers_model;

}

void management_users::pressed_cell_admUsers() {


    QModelIndex index = tableView -> currentIndex();
    menu_layout_create_clear(false);
    int ID = admUsers_table_model -> data(admUsers_table_model -> index(index.row(), 0)).toInt();

    if (index.column() == 1 && roles_list.size() > 0) {

        QMap<int, QAction*> list_action;

        for( auto key : roles_list.keys()) {

            QAction *action = new QAction(roles_list.value(key));
            action -> setCheckable(true);
            action -> setObjectName(QString::number(key));
            connect(action, SIGNAL(triggered()), this, SLOT(set_users_roles()));

            if (roles_list.value(key) == "Старший администратор")
                action -> setEnabled(false);

            if (roles_list.value(key) == "Администратор" && general_admin == false)
                action -> setEnabled(false);

            menu -> addAction(action);
            list_action.insert(key, action);

        }


        for (int row = 0; row < admUsersRoles_table_model -> rowCount(); row ++) {

            QSqlRecord record = admUsersRoles_table_model -> record(row);

            if (ID == record.value("admUsers_ID")) {

                list_action.value(record.value("admRoles_ID").toInt()) -> setChecked(true);
                menu_list.insert(record.value("admRoles_ID").toInt(), ID);

            }

        }

        menu_layout_create_clear(true);
    }
}

void management_users::set_users_roles() {

    QModelIndex index = admUsers_table_model -> index(tableView -> currentIndex().row(), 0);

    for( auto & key : menu_list.keys()) if (key == sender() -> objectName().toInt()) {
        menu_list.remove(key);
        management_users_update_list.append(QString("DELETE FROM admUsersRoles WHERE admUsers_ID = '%1' AND admRoles_ID = '%2'")
                                 .arg(admUsers_table_model -> data(index).toString())
                                 .arg(sender() -> objectName()));
    }

    menu_list.insert(sender() -> objectName().toInt(), admUsers_table_model -> data(index).toInt());
    management_users_update_list.append(QString("INSERT INTO admUsersRoles (admUsers_ID, admRoles_ID) VALUES ('%1', '%2')")
                             .arg(admUsers_table_model -> data(index).toString())
                             .arg(sender() -> objectName()));

    QSize rec = management_users::size();
    menu -> setMinimumHeight(rec.height());
    menu -> setMinimumHeight(0);

}

void management_users::management_users_update_slot() {


    if (admUsers_table_model -> isDirty() == true || management_users_update_list.size() != 0) {

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Подтвердите изменения");
        msgBox.setText("Внимание! В таблицах admUsers и admUsersRoles были внесены изменения. Подтвердите, что хотите их сохранить");

        QPushButton *abortButton = msgBox.addButton("Отменить изменения", QMessageBox::NoRole);
        QPushButton *connectButton = msgBox.addButton("Применить изменения", QMessageBox::YesRole);

        msgBox.exec();

        if (msgBox.clickedButton() == connectButton) {

            QSqlQuery query = QSqlQuery(StorageDB);
            for (int i = 0; i < management_users_update_list.size(); i ++) query.exec(management_users_update_list[i]);

            admUsers_table_model -> submitAll();
            management_users_update_list.clear();

            admUsers_table_model = create_model();

            tableView -> setModel(admUsers_table_model);

        } else if (msgBox.clickedButton() == abortButton) {}

    } else {

        admUsers_table_model = create_model();
        tableView -> setModel(admUsers_table_model);

    }

}

void management_users::insert_row_admUsers_slot() {

    admUsers_table_model -> insertRow(admUsers_table_model -> rowCount());

}

void management_users::delete_row_admUsers_slot() {

    if (tableView -> currentIndex().row() != row_general_admin) admUsers_table_model -> removeRow(tableView -> currentIndex().row());

}

void management_users::search_admUsers_slot() {

    if (admUsers_table_model -> isDirty() == true && search_admUsers -> text().isEmpty() == false) {

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Подтвердите изменения");
        msgBox.setText("Внимание! В таблице имеются несохраненные изменения. Применение фильтра приведет к потере этих изменений.");

        msgBox.addButton("Отмена", QMessageBox::NoRole);
        QPushButton *connectButton = msgBox.addButton("Применить изменения", QMessageBox::YesRole);

        msgBox.exec();

        if (msgBox.clickedButton() == connectButton) {

            management_users_update_slot();
            admUsers_table_model -> setFilter(QString("Login = '%1'").arg(search_admUsers -> text()));
        }

    } else if (search_admUsers -> text().isEmpty() == false) {

        admUsers_table_model -> setFilter(QString("Login = '%1'").arg(search_admUsers -> text()));

    }

}

void management_users::getLogin(QString login, QString general_login) {

    login_general_admin = general_login;
    (login == general_login) ? general_admin = true : general_admin = false;
    if (general_admin == false) tableView -> hideRow(row_general_admin);

}

void management_users::aboutToHide_slot() { menu_layout_create_clear(true); }

void management_users::menu_layout_create_clear(bool state) {

    if (state == false) {

        label -> setVisible(false);
        menu_layout -> removeWidget(menu);
        menu -> clear();
        menu_list.clear();

    } else {

        menu_layout -> addWidget(label);
        menu_layout -> addWidget(menu);
        label -> setVisible(true);
        menu -> show();

    }

}

void management_users::showEvent(QShowEvent* event) {

    emit sendToolBar(toolBar);

}

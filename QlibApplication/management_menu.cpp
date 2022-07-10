/*
 *
 *  1. В конструкторе подключаем базу данных SQLITE - StorageDB и открываем ее.
 *
 *  2. Создаем иерархическое древо Menu - Bar. Древо создается в функции create_tree_model()
 *     Для создания древа выгружаю таблицу admMenu в модель admMenu_table_model (QSqlTableModel),
 *     после чего используя колонки ID и Parent рекурсивно собираю древо.
 *
 *  3. Пока собираю дерево (create_tree_model() и create_tree_model_recursion()) через setData
 *     заполняю его данными. Для обхода колонок использую QList() с именами колонок и итератор.
 *     Функция create_tree_model_decoration() необходима для примененния к итемам определенных
 *     флагов, а также для установки иконок.
 *     Функция access_application() проверяет наличие приложения в DLL.
 *
 *  4. После отрисовки дерева создаю обработчик нажатий pressed_item_admMenu_tree_model().
 *     В нем реализованы следующие задачи:
 *
 *          а) Проверяется отмечен ли элемент древа для удаления и соответственно меняет текст
 *          кнопки (Удалить или восстановить). Проверяет по скрытому столбцу DELETE
 *
 *          б) Далее очищаем боковое меню функцией menu_layout_create_clear(bool)
 *
 *          в) Проверяем индекс колонки. Если == 0, то показываем доступные роли для данного
 *          приложения. Роли хранятся в roles_menu_map. Проверка произвадится по таблице связей
 *          admRolesMenu, которая сохранена в модели admRolesMenu_table_model. Если == 4, то
 *          отображаем в какие папки можно переместить выбранное приложения или папку (установка
 *          родителей). Проверяется чтобы папку нельзя было поместить саму в себя или в своих детей.
 *          Для этого использую два QMap: menu_list_map и menu_map_name. Первый хранит свой ID и
 *          ID родителя, второй свой ID и ID родителя. Устанавливает родителя в таблице функция
 *          set_parent_ID().
 *
 *          г) Далее показываем боковое меню функцией menu_layout_create_clear(bool)
 *
 *  5. Добавление новых строк и их удаление и обновление таблицы осуществляют функции:
 *
 *          insert_row_admMenu_slot()
 *          admMenu_tree_model_delete_slot()
 *          admMenu_tree_model_update_slot()
 *
 *     Вначале добавления, удаление происходит в модели за счет стандартных функций после чего
 *     формируется запрос в БД, который сохраняется в QList menu_update_list. При нажатии на кнопку
 *     обновить происходит отправка всех накопленных запросов в БД и потом. menu_update_list
 *     очищается.
 *
 *  6. Для редактирования названий приложений и папок, а также их объектных имен имеются специальные
 *     поля line_name_app и line_object_name_app, данные сохраняются после срабатывания слота
 *     save_line_name_app_slot(). Функция line_name_app_change() проверяет является ли выбранный
 *     объект пакпкой. если да то в поле line_object_name_app вписывается объектное имя "group"
 *     и оно запрещаается для редактирования, так как к этому полю обращается функция вставки строки
 *
 */


#include "management_menu.h"

management_menu::management_menu(QWidget *parent) : QWidget(parent)
{
    managment_menu_display_settings();
}

management_menu::~management_menu() {}

QStandardItemModel* management_menu::create_tree_model()
{
    QSqlQuery query = QSqlQuery(StorageDB);
    QSqlTableModel *admMenu_table_model = new QSqlTableModel();

    admMenu_table_model -> setTable("admMenu");
    admMenu_table_model -> setEditStrategy(QSqlTableModel::OnManualSubmit);
    admMenu_table_model -> select();

    QStandardItemModel *tree_model = new QStandardItemModel();
    tree_model -> setHorizontalHeaderLabels(QStringList() << "Иерархия Menu-Bar"
                                                          << "OBJECT"
                                                          << "CONNECT"
                                                          << "ID"
                                                          << "PARENT"
                                                          << "DELETE");

    QMap<int, QModelIndex> admMenu_parent_ID;
    int model_row = 0;

    for (int row = 0; row < admMenu_table_model -> rowCount(); row ++)
    {
        QSqlRecord admMenu_model_line = admMenu_table_model -> record(row);

        if (admMenu_model_line.value("Parent") == "")
        {
            tree_model -> insertRow(model_row);
            for (QString it : name_column)
            {
                tree_model -> setData(tree_model -> index(model_row, name_column.indexOf(it)),
                                      admMenu_model_line.value(it));

                create_tree_model_decoration(tree_model,
                                             tree_model -> index(model_row, name_column.indexOf(it)),
                                             admMenu_model_line.value("Object").toString().split("_")[0]);
            }

            admMenu_parent_ID.insert(admMenu_model_line.value("ID").toInt(),
                                     tree_model -> index(model_row, 0));

            menu_map_name.insert(admMenu_model_line.value("ID").toInt(), admMenu_model_line.value("Name").toString());
            model_row ++;
        }
    }

    root_model_row = model_row;
    admMenu_table_row_count = admMenu_table_model -> rowCount()+1;

    create_tree_model_recursion(admMenu_table_model, tree_model, admMenu_parent_ID);

    QSqlTableModel *admRolesMenu_model = new QSqlTableModel();

    admRolesMenu_model -> setTable("admRolesMenu");
    admRolesMenu_model -> select();

    admRolesMenu_table_model = admRolesMenu_model;

    query.exec("SELECT * FROM admRoles");
    while (query.next()) roles_list_map.insert(query.value("ID").toInt(), query.value("Roles").toString());

    return tree_model;
}

void management_menu::create_tree_model_recursion(QSqlTableModel *admMenu_model, QStandardItemModel *tree_model, QMap<int, QModelIndex> admMenu_parent_ID)
{
    int model_row;
    QMap<int, QModelIndex> admMenu_child_ID;

    for (int row = 0; row < admMenu_model -> rowCount(); row ++)
    {
        QSqlRecord admMenu_model_line = admMenu_model -> record(row);

        for(int & key : admMenu_parent_ID.keys())
        {
            if (admMenu_model_line.value("Parent") == key)
            {
                model_row = tree_model -> itemFromIndex(admMenu_parent_ID.value(key)) -> rowCount();
                tree_model -> insertRow(model_row, admMenu_parent_ID.value(key));
                if (model_row == 0) tree_model -> insertColumns(0, 6, admMenu_parent_ID.value(key));

                for (QString it : name_column)
                {
                    tree_model -> setData(tree_model -> index(model_row, name_column.indexOf(it), admMenu_parent_ID.value(key)),
                                          admMenu_model_line.value(it));

                    if (it == "CONNECT")
                        tree_model -> setData(tree_model -> index(model_row, name_column.indexOf(it), admMenu_parent_ID.value(key)),
                                                        access_application(admMenu_model_line.value("Object").toString()));

                    create_tree_model_decoration(tree_model,
                                                 tree_model -> index(model_row, name_column.indexOf(it), admMenu_parent_ID.value(key)),
                                                 admMenu_model_line.value("Object").toString().split("_")[0]);
                }

                admMenu_child_ID.insert(admMenu_model_line.value("ID").toInt(),
                                        tree_model -> index(model_row, 0, admMenu_parent_ID.value(key)));

                if (admMenu_model_line.value("Object").toString().split("_")[0] == "group")
                    menu_map_name.insert(admMenu_model_line.value("ID").toInt(), admMenu_model_line.value("Name").toString());
            }
        }
    }
    admMenu_parent_ID = admMenu_child_ID;
    if (admMenu_parent_ID.size() != 0) create_tree_model_recursion(admMenu_model, tree_model, admMenu_parent_ID);
}

void management_menu::create_tree_model_decoration(QStandardItemModel* tree_model, QModelIndex index, QString object_name)
{
    int column = index.column();

    switch (column)
    {
    case 0:
        tree_model -> itemFromIndex(index) -> setFlags(Qt::ItemIsEnabled);
        (object_name.split("_")[0] == "group") ? tree_model -> itemFromIndex(index) -> setIcon(QIcon("./Images/Img-021.png")) :
                                                 tree_model -> itemFromIndex(index) -> setIcon(QIcon("./Images/Img-003.png"));
        break;

    case 2:
    case 3:
        tree_model -> itemFromIndex(index) -> setFlags(Qt::ItemIsEditable);
        tree_model -> itemFromIndex(index) -> setTextAlignment(Qt::AlignCenter);

    case 4:
        tree_model -> itemFromIndex(index) -> setFlags(Qt::ItemIsEnabled);
        tree_model -> itemFromIndex(index) -> setTextAlignment(Qt::AlignCenter);

    default:
        break;
    }
}

void management_menu::admMenu_tree_model_update_slot()
{
    if (menu_update_list.size() != 0)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Подтвердите внесенные изменения");
        msgBox.setText("Внимание! В таблицах admMenu и admRolesMenu были внесены изменения. Подтвердите, что хотите их сохранить");

        msgBox.addButton("Отменить изменения", QMessageBox::NoRole);
        QPushButton *connectButton = msgBox.addButton("Применить изменения", QMessageBox::YesRole);

        msgBox.exec();

        if (msgBox.clickedButton() == connectButton)
        {
            QSqlQuery query = QSqlQuery(StorageDB);
            for (int i = 0; i < menu_update_list.size(); i ++) query.exec(menu_update_list[i]);

            menu_update_list.clear();

            admMenu_tree_model = create_tree_model();
            treeView -> setModel(admMenu_tree_model);
            treeView -> expandAll();
         }
    }
    else
    {
        admMenu_tree_model = create_tree_model();
        treeView -> setModel(admMenu_tree_model);
        treeView -> expandAll();
    }
}

QString management_menu::access_application(QString object_name_app) {

    if (object_name_app.split("_")[0] != "group")
    {
        int id = QMetaType::type(object_name_app.toStdString().c_str());
        if (id != QMetaType::UnknownType) { return "✅"; } else { return "❌"; }
    }
    else
    {
        return "";
    }

}

void management_menu::pressed_item_admMenu_tree_model(QModelIndex index)
{

    int ID = admMenu_tree_model -> itemFromIndex(admMenu_tree_model -> index(index.row(), 3, check_parent_index(index))) -> text().toInt();

    bool status =  admMenu_tree_model -> data(admMenu_tree_model -> index(index.row(), 5, check_parent_index(index))).toBool();

    (status == true) ? admMenu_tree_model_delete -> setText("Восстановить") :
                       admMenu_tree_model_delete -> setText("Удалить");

    menu_layout_create_clear(false);

    if (index.column() == 0 && line_object_name_app -> text() != "group" && roles_list_map.size() > 0)
    {
        for (int row = 0; row < admRolesMenu_table_model -> rowCount(); row ++)
        {
            QSqlRecord record = admRolesMenu_table_model -> record(row);

            if (ID == record.value("admMenu_ID"))
            {
                QAction *action = new QAction(QIcon("./Images/IMG-025.png"), roles_list_map.value(record.value("admRoles_ID").toInt()));
                menu -> addAction(action);
            }
        }

        label -> setText("Доступные роли");
        if (menu -> isEmpty() == false) menu_layout_create_clear(true);
    }


    if (index.column() == 4)
    {
        line_name_app -> clear();
        line_object_name_app -> clear();

        searchChildFolder(admMenu_tree_model -> index(index.row(), 0, check_parent_index(index)));

        for( auto & key : menu_map_name.keys())
        {
            if (childID.contains(key) == false && key != ID)
            {
                QAction *action = new QAction(QIcon("./Images/IMG-021.png"), menu_map_name.value(key));
                action -> setObjectName(QString::number(key));
                connect(action, SIGNAL(triggered(bool)), this, SLOT(set_parent_ID()));
                menu -> addAction(action);
            }
        }

        childID.clear();

        label -> setText("Доступные папки");
        if (menu -> isEmpty() == false) menu_layout_create_clear(true);
    }
}

void management_menu::insert_row_admMenu_slot() {

    if (last_record_name != "" && last_record_object_name != "")
    {
        QModelIndex index = treeView -> currentIndex();

        if (admMenu_tree_model -> itemFromIndex(index) != nullptr && line_object_name_app -> text().split("_")[0] == "group")
        {
            int ID = admMenu_tree_model -> itemFromIndex(admMenu_tree_model -> index(index.row(), 3, check_parent_index(index))) -> text().toInt();
            int model_row;

            model_row = admMenu_tree_model -> itemFromIndex(index) -> rowCount();
            admMenu_tree_model -> insertRow(model_row, index);
            if (model_row == 0) admMenu_tree_model -> insertColumns(0, 6, index);

            admMenu_tree_model -> setData(admMenu_tree_model -> index(model_row, 1, index), sender() -> objectName());
            admMenu_tree_model -> setData(admMenu_tree_model -> index(model_row, 3, index), admMenu_table_row_count);
            admMenu_tree_model -> setData(admMenu_tree_model -> index(model_row, 4, index), ID);

            for (QString it : name_column)create_tree_model_decoration(admMenu_tree_model,
                                                                       admMenu_tree_model -> index(model_row, name_column.indexOf(it), index),
                                                                       sender() -> objectName());

            admMenu_table_row_count ++;
            last_record_name = "";
            last_record_object_name = "";
            treeView -> expandAll();

            menu_update_list.append(QString("INSERT INTO admMenu (name, object, parent) VALUES ('', '%1', %2)")
                                    .arg(sender() -> objectName())
                                     .arg(ID));
        }

        if (sender() -> objectName() == "root_group")
        {
                    admMenu_tree_model -> insertRow(root_model_row);
                    if (root_model_row == 0) admMenu_tree_model -> insertColumns(0, 6);

                    admMenu_tree_model -> setData(admMenu_tree_model -> index(root_model_row, 1),
                                                  "group");

                    admMenu_tree_model -> setData(admMenu_tree_model -> index(root_model_row, 3),
                                                  admMenu_table_row_count);

                    for (QString it : name_column)create_tree_model_decoration(admMenu_tree_model,
                                                                               admMenu_tree_model -> index(root_model_row, name_column.indexOf(it)),
                                                                               "group");

                    menu_update_list.append("INSERT INTO admMenu (name, object) VALUES ('', 'group')");

                    admMenu_table_row_count ++;
                    last_record_name = "";
                    last_record_object_name = "";
                    treeView -> expandAll();
        }
    }
    else
    {
        QMessageBox::information(this, "Ошибка ввода", "Заполните данные в предыдущей строке");
    }

}

void management_menu::admMenu_tree_model_delete_slot() {

    QModelIndex index = treeView -> currentIndex();
    bool status =  admMenu_tree_model -> data(admMenu_tree_model -> index(index.row(), 5, check_parent_index(index))).toBool();
    int ID = admMenu_tree_model -> data(admMenu_tree_model -> index(index.row(), 3, check_parent_index(index))).toInt();

    if (status == true) {
        admMenu_tree_model_delete -> setText("Удалить");

        for (int column = 0; column < 5; column ++)
            admMenu_tree_model -> itemFromIndex(admMenu_tree_model -> index(index.row(), column, check_parent_index(index)))
                                                                   -> setBackground(QBrush(QColor(255, 255, 255)));

        menu_update_list.remove(menu_update_list.indexOf(QString("DELETE FROM admMenu WHERE ID = '%1'").arg(ID)));
        menu_update_list.remove(menu_update_list.indexOf(QString("DELETE FROM admRolesMenu WHERE admMenu_ID = '%1").arg(ID)));

        admMenu_tree_model -> itemFromIndex(admMenu_tree_model -> index(index.row(), 5, check_parent_index(index))) -> setText("0");

    } else {

        admMenu_tree_model_delete -> setText("Восстановить");

        for (int column = 0; column < 5; column ++)
            admMenu_tree_model -> itemFromIndex(admMenu_tree_model -> index(index.row(), column, check_parent_index(index)))
                                                                   -> setBackground(QBrush(QColor(255, 248, 220)));

        menu_update_list.append(QString("DELETE FROM admMenu WHERE ID = '%1'").arg(ID));
        menu_update_list.append(QString("DELETE FROM admRolesMenu WHERE admMenu_ID = '%1").arg(ID));

        admMenu_tree_model -> itemFromIndex(admMenu_tree_model -> index(index.row(), 5, check_parent_index(index))) -> setText("1");

    }
}

void management_menu::set_parent_ID()
{
    admMenu_tree_model -> setData(treeView -> currentIndex(), QString(sender() -> objectName()));

    menu_update_list.append(QString("UPDATE admMenu SET Parent = '%1' WHERE ID = '%2'")
                             .arg(sender() -> objectName())
                             .arg(admMenu_tree_model -> itemFromIndex(admMenu_tree_model -> index(treeView -> currentIndex().row(), 3, check_parent_index(treeView -> currentIndex()))) -> text()));
}

void management_menu::save_line_name_app_slot()
{
    QString ID = admMenu_tree_model -> data(admMenu_tree_model -> index(treeView -> currentIndex().row(), 3, check_parent_index(treeView -> currentIndex()))).toString();

    if (line_name_app -> text().isEmpty() == false && line_object_name_app -> text().isEmpty() == false)
    {
        admMenu_tree_model -> setData(treeView -> currentIndex(), QString(line_name_app -> text()));
        menu_update_list.append(QString("UPDATE admMenu SET Object = '%1' WHERE ID = '%2'")
                                 .arg(line_object_name_app -> text())
                                 .arg(ID));

        menu_update_list.append(QString("UPDATE admMenu SET Name = '%1' WHERE ID = '%2'")
                                 .arg(line_name_app -> text())
                                 .arg(ID));

        last_record_name = line_name_app -> text();
        last_record_object_name = line_object_name_app -> text();

    }
    else
    {
        QMessageBox::information(this, "Ошибка ввода", "Заполните все необходимые данные");
    }
}

void management_menu::menu_layout_create_clear(bool state)
{
    if (state == false)
    {
        label -> setVisible(false);
        menu_layout -> removeWidget(menu);
        menu -> clear();

        line_object_name_app -> setEnabled(true);
        line_object_name_app -> clear();
    }
    else
    {
        menu_layout -> addWidget(label);
        menu_layout -> addWidget(menu);
        label -> setVisible(true);
         menu -> show();
    }
}

void management_menu::line_name_app_change()
{
    QModelIndex index = treeView -> currentIndex();
    if (index.column() == 0)
    {
        QString object_name = admMenu_tree_model -> data(admMenu_tree_model -> index(index.row(), 1, check_parent_index(index))).toString();

        line_name_app -> setText(admMenu_tree_model -> itemFromIndex(index) -> text());
        line_object_name_app -> setText(object_name);
    }
}

QModelIndex management_menu::check_parent_index(QModelIndex index)
{
    QModelIndex index_null;
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(treeView -> model());

    if (model -> itemFromIndex(index) -> parent() == nullptr)
        return index_null;
    else
        return model -> itemFromIndex(index) -> parent() -> index();
}

void management_menu::aboutToHide_slot()
{
    menu_layout_create_clear(true);
}

void management_menu::managment_menu_display_settings()
{
    StorageDB = QSqlDatabase::addDatabase("QSQLITE");
    StorageDB.setDatabaseName("./StorageDB.db");
    StorageDB.open();

    admMenu_tree_model = create_tree_model();
    treeView -> setModel(admMenu_tree_model);

    treeView -> header() -> setStretchLastSection(false);
    treeView -> header() -> setDefaultAlignment(Qt::AlignCenter);
    treeView -> header() -> setSectionResizeMode(0, QHeaderView::Stretch);
    for (int column = 2; column < 5; column ++) treeView -> header() -> resizeSection(column, 70);
    treeView -> expandAll();

    treeView -> hideColumn(1);
    treeView -> hideColumn(5);

    connect(treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressed_item_admMenu_tree_model(QModelIndex)));
    connect(treeView, SIGNAL(activated(QModelIndex)), this, SLOT(pressed_item_admMenu_tree_model(QModelIndex)));
    connect(treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(line_name_app_change()));
    connect(treeView, SIGNAL(activated(QModelIndex)), this, SLOT(line_name_app_change()));

    line_name_app -> setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    line_name_app -> setMaximumHeight(30);
    line_name_app -> addAction(QIcon("./Images/Img-002.png"), QLineEdit::LeadingPosition);
    line_name_app -> setPlaceholderText("Название приложения / папки");

    line_object_name_app -> setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    line_object_name_app -> setMaximumHeight(30);
    line_object_name_app -> addAction(QIcon("./Images/Img-002.png"), QLineEdit::LeadingPosition);
    line_object_name_app -> setPlaceholderText("Объектное имя");

    save_line_name_app -> setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    save_line_name_app -> setMaximumSize(QSize(75,30));
    connect(save_line_name_app, SIGNAL(clicked()), this, SLOT(save_line_name_app_slot()));

    menu -> setMinimumWidth(230);
    menu -> setMaximumWidth(230);
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHide_slot()));

    label -> setMaximumSize(QSize(230,30));
    label -> setAlignment(Qt::AlignCenter);
    label -> setStyleSheet("border: 1px solid black;");

    insert_root_folder_admMenu -> setObjectName("root_group");
    insert_folder_admMenu -> setObjectName("group_");
    insert_application_admMenu -> setObjectName("form_");

    connect(insert_root_folder_admMenu, SIGNAL(clicked()), this, SLOT(insert_row_admMenu_slot()));
    connect(insert_folder_admMenu, SIGNAL(clicked()), this, SLOT(insert_row_admMenu_slot()));
    connect(insert_application_admMenu, SIGNAL(clicked()), this, SLOT(insert_row_admMenu_slot()));
    connect(admMenu_tree_model_update, SIGNAL(clicked()), this, SLOT(admMenu_tree_model_update_slot()));
    connect(admMenu_tree_model_delete, SIGNAL(clicked()), this, SLOT(admMenu_tree_model_delete_slot()));

    toolBar -> addWidget(insert_root_folder_admMenu);
    toolBar -> addWidget(insert_folder_admMenu);
    toolBar -> addWidget(insert_application_admMenu);
    toolBar -> addWidget(admMenu_tree_model_update);
    toolBar -> addWidget(admMenu_tree_model_delete);

    menuBar -> addAction(action_insert_root_folder);
    menuBar -> addAction(action_insert_folder);
    menuBar -> addAction(action_insert_application);
    menuBar -> addAction(action_update);
    menuBar -> addAction(action_delete);

    connect(action_insert_root_folder, SIGNAL(triggered()), SLOT(insert_row_admMenu_slot()));
    connect(action_insert_folder, SIGNAL(triggered()), SLOT(insert_row_admMenu_slot()));
    connect(action_insert_application, SIGNAL(triggered()), SLOT(insert_row_admMenu_slot()));
    connect(action_update, SIGNAL(triggered()), SLOT(admMenu_tree_model_update_slot()));
    connect(action_delete, SIGNAL(triggered()), SLOT(admMenu_tree_model_delete_slot()));

    line_name_app_layout -> addWidget(line_name_app);
    line_name_app_layout -> addWidget(line_object_name_app);
    line_name_app_layout -> addWidget(save_line_name_app);

    general_layout -> addWidget(toolBar);
    general_layout -> addWidget(treeView);
    general_layout -> addLayout(line_name_app_layout);

    central_layout -> addLayout(general_layout);
    central_layout -> addLayout(menu_layout);

    setLayout(central_layout);
}

void management_menu::showEvent(QShowEvent* event)
{
    emit sendToolBar(toolBar);
    //emit sendMenu(menuBar);
}

void management_menu::searchChildFolder(QModelIndex index)
{
    if (admMenu_tree_model -> itemFromIndex(index) -> hasChildren() == true)
    {
        for (int row = 0; row < admMenu_tree_model -> itemFromIndex(index) -> rowCount(); row ++)
        {
            childID.append(admMenu_tree_model -> data(admMenu_tree_model -> index(row, 3, index)).toInt());
            searchChildFolder(admMenu_tree_model -> index(row, 0, index));
        }
    }
}

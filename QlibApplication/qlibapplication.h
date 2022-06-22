#ifndef QLIBAPPLICATION_H
#define QLIBAPPLICATION_H

#include "qlibapplication_global.h"

#include "management_menu.h"
#include "management_roles.h"
#include "management_users.h"

#include "setting_line_one.h"
#include "setting_line_two.h"

#include "stage_testing_one.h"
#include "stage_testing_two.h"

#include <QWidget>
#include <QMap>

class QLIBAPPLICATIONSHARED_EXPORT QlibApplication : public QWidget {

    Q_OBJECT

public:
    QlibApplication(QWidget *parent = 0);
    ~QlibApplication();
    QWidget* getApp(QString object_name_app);
    bool check_connection(QString object_name_app);

};
#endif // QLIBAPPLICATION_H

#include "loncher.h"
#include "database.h"
#include "access.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    if (initialization_database() == true)
    {
        Access *authentication = new Access();
        if( authentication -> exec() == QDialog::Rejected)
            return 0;

        authentication -> deleteLater();

        Loncher *loncher = new Loncher();
        loncher -> setAttribute(authentication -> name,
                       authentication -> login -> text(),
                       authentication -> App_list);
        loncher -> show();
        return a.exec();

        return 0;
    }
    return 0;
}

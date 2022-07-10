#ifndef DATABASE_H
#define DATABASE_H

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

bool initialization_database();
void create_table_database(QSqlDatabase StorageDB, QString nameTable);
void create_senior_admin_record(QSqlDatabase StorageDB);
int searchID(QSqlDatabase StorageDB, QString nameTable, QString nameColumn, QString argument);
void error_output(QString text_error);

#endif // DATABASE_H

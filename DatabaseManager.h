#pragma once

#include "Task.h"
#include <QVector>
#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    explicit DatabaseManager(const QString& dbPath="tasks.db");
    ~DatabaseManager();

    bool open();
    void close();

    bool createTables();

    QVector<Task> getAllTasks();
    bool addTask(const Task& task);
    bool updateTask(const Task& task);
    bool deleteTask(int id);
    bool setDone(int id,bool done);

private:
    QSqlDatabase db;
    QString connectionName;
};


#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QUuid>
//UUID-universally unique identifier

DatabaseManager::DatabaseManager(const QString& dbPath){
    connectionName=QUuid::createUuid().toString();
    db=QSqlDatabase::addDatabase("QSQLITE",connectionName);
    db.setDatabaseName(dbPath);
}
DatabaseManager::~DatabaseManager()
{
    close();
    QSqlDatabase::removeDatabase(connectionName);
}

bool DatabaseManager::open()
{
    if(!db.open())
    {
        qWarning()<<"Failed to open database: "<<db.lastError().text();
        return false;
    }
    return createTables();
}

void DatabaseManager::close()
{
    if(db.isOpen())
        db.close();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(db);
    const QString sql=R"(
        CREATE TABLE IF NOT EXISTS tasks(
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            done INTEGER NOT NULL DEFAULT 0,
            priority INTEGER NOT NULL DEFAULT 2,
            due_date TEXT,
            category TEXT
    )
)";
    if(!query.exec(sql))
    {
        qWarning()<<"Failed to create table:"<<query.lastError().text();
        return false;
    }
    return true;
}

QVector<Task> DatabaseManager::getAllTasks()
{
    QVector<Task> tasks;
    QSqlQuery query(db);
    query.exec("SELECT id, title, done, priority, due_date, category FROM tasks ORDER BY id");

    while(query.next())
    {
        Task t;
        t.id=query.value(0).toInt();
        t.title=query.value(1).toString();
        t.done=query.value(2).toBool();
        t.priority=query.value(3).toInt();

        QString due=query.value(4).toString();
        if(!due.isEmpty())
            t.dueDate=QDate::fromString(due,"yyyy-MM-dd");

        t.category=query.value(5).toString();
        tasks.append(t);
    }
    return tasks;
}

bool DatabaseManager::addTask(const Task& task)
{
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO tasks(title, done, priority, due_date, category)
        VALUES(?, ?, ?, ?, ?)
    )");
    query.addBindValue(task.title);
    query.addBindValue(task.done);
    query.addBindValue(task.priority);
    query.addBindValue(task.dueDate.isValid() ? task.dueDate.toString("yyyy-MM-dd") : QVariant());
    query.addBindValue(task.category);

    if(!query.exec())
    {
        qWarning()<<"Add failed: "<<query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateTask(const Task& task)
{
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE tasks SET
            title = ?, done = ?, priority = ?, due_date = ?, category = ?
        WHERE id = ?
    )");
    query.addBindValue(task.title);
    query.addBindValue(task.done);
    query.addBindValue(task.priority);
    query.addBindValue(task.dueDate.isValid() ? task.dueDate.toString("yyyy-MM-dd") : QVariant());
    query.addBindValue(task.category);
    query.addBindValue(task.id);

    return query.exec();
}

bool DatabaseManager::deleteTask(int id)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM tasks WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

bool DatabaseManager::setDone(int id, bool done)
{
    QSqlQuery query(db);
    query.prepare("UPDATE tasks SET done = ? WHERE id = ?");
    query.addBindValue(done);
    query.addBindValue(id);
    return query.exec();
}

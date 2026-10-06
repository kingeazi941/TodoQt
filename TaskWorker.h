#pragma once

#include "Task.h"
#include "DatabaseManager.h"
#include <QVector>
#include <QObject>

class TaskWorker : public QObject
{
    Q_OBJECT
public:
    explicit TaskWorker(QObject* parent = nullptr);

public slots:
    void initialize();
    void loadTasks();
    void addTask(const Task& task);
    void updateTask(const Task& task);
    void deleteTask(int id);
    void setTaskDone(int id,bool done);

signals:
    void tasksLoaded(const QVector<Task>& tasks);
    void taskAdded(bool success);
    void taskUpdated(bool success);
    void taskDeleted(bool success);
    void errorOccurred(const QString& message);

private:
    DatabaseManager db;
};



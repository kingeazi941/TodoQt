#include "TaskWorker.h"
#include <QDebug>

TaskWorker::TaskWorker(QObject* parent):QObject(parent) {}

void TaskWorker::initialize()
{
    if(!db.open())
    {
        emit errorOccurred("Could not open database");
        return;
    }
    qDebug()<<"Database opened in worker thread";
}

void TaskWorker::loadTasks()
{
    auto tasks=db.getAllTasks();
    emit tasksLoaded(tasks);
}

void TaskWorker::addTask(const Task &task)
{
    bool ok=db.addTask(task);
    emit taskAdded(ok);
    if(ok) loadTasks();
}

void TaskWorker::updateTask(const Task &task)
{
    bool ok=db.updateTask(task);
    emit taskUpdated(ok);
    if(ok) loadTasks();
}

void TaskWorker::deleteTask(int id)
{
    bool ok=db.deleteTask(id);
    emit taskDeleted(ok);
    if(ok) loadTasks();
}

void TaskWorker::setTaskDone(int id, bool done)
{
    bool ok=db.setDone(id,done);
    emit taskUpdated(ok);
    if(ok) loadTasks();
}
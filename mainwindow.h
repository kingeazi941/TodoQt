#pragma once

#include "Task.h"
#include <QMainWindow>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QThread>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QLabel>
#include <QDate>

class TaskWorker;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddClicked();
    void onCompleteClicked();
    void onDeleteClicked();
    void onFilterChanged(int);
    void onTasksLoaded(const QVector<Task>& tasks);
    void onError(const QString& message);
    void onTaskAdded(bool success);
    void onTaskUpdated(bool success);
    void onTaskDeleted(bool success);
    void onEditTask(QListWidgetItem* item);

private:
    void setupUi();
    void setupWorker();
    void refreshList(const QVector<Task>& tasks);

    QListWidget* listWidget=nullptr;
    QLineEdit* titleEdit=nullptr;
    QComboBox* priorityBox=nullptr;
    QLineEdit* dueEdit=nullptr;
    QComboBox* filterBox=nullptr;

    QThread* workerThread=nullptr;
    TaskWorker* worker=nullptr;

    QVector<Task> currentTasks;

};


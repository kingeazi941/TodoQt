#include "mainwindow.h"
#include "TaskWorker.h"
#include "EditTaskDialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Todo App - Qt6 + SQLite + Multithreading");
    resize(700,550);

    setupUi();
    setupWorker();
}

MainWindow::~MainWindow()
{
    workerThread->quit();
    workerThread->wait();
}

void MainWindow::setupUi()
{
    QWidget* central=new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout=new QVBoxLayout(central);

    //Filter
    filterBox= new QComboBox;
    filterBox->addItems({"All","Pending","Completed","High Priority","Overdue"});
    connect(filterBox,QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,&MainWindow::onFilterChanged);
    mainLayout->addWidget(filterBox);

    //List
    listWidget=new QListWidget;
    mainLayout->addWidget(listWidget);

    //Input area
    QHBoxLayout* inputLayout=new QHBoxLayout;
    titleEdit=new QLineEdit;
    titleEdit->setPlaceholderText("Task title...");
    priorityBox=new QComboBox;
    priorityBox->addItems({"Low","Medium","High"});
    priorityBox->setCurrentIndex(1);
    dueEdit=new QLineEdit;
    dueEdit->setPlaceholderText("Due date (yyyy-MM-dd)");

    inputLayout->addWidget(titleEdit);
    inputLayout->addWidget(priorityBox);
    inputLayout->addWidget(dueEdit);
    mainLayout->addLayout(inputLayout);

    //Buttons
    QHBoxLayout* btnLayout=new QHBoxLayout;
    QPushButton* addBtn=new QPushButton("Add Task");
    QPushButton* completeBtn=new QPushButton("Mark Complete");
    QPushButton* deleteBtn=new QPushButton("Delete");

    connect(addBtn,&QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(completeBtn,&QPushButton::clicked, this, &MainWindow::onCompleteClicked);
    connect(deleteBtn,&QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(listWidget,&QListWidget::itemDoubleClicked,this,&MainWindow::onEditTask);

    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(completeBtn);
    btnLayout->addWidget(deleteBtn);
    mainLayout->addLayout(btnLayout);
}

void MainWindow::setupWorker()
{
    workerThread=new QThread(this);
    worker=new TaskWorker();
    worker->moveToThread(workerThread);

    //Start the thread
    connect(workerThread,&QThread::started,worker,&TaskWorker::initialize);
    connect(workerThread,&QThread::finished,worker,&QObject::deleteLater);

    // Results from worker(UI)
    connect(worker,&TaskWorker::tasksLoaded, this, &MainWindow::onTasksLoaded);
    connect(worker,&TaskWorker::errorOccurred, this, &MainWindow::onError);

    connect(worker, &TaskWorker::taskAdded, this, &MainWindow::onTaskAdded);
    connect(worker, &TaskWorker::taskUpdated, this, &MainWindow::onTaskUpdated);
    connect(worker, &TaskWorker::taskDeleted, this, &MainWindow::onTaskDeleted);

    workerThread->start();

    QMetaObject::invokeMethod(worker,"loadTasks",Qt::QueuedConnection);
}

void MainWindow::onAddClicked()
{
    QString title=titleEdit->text().trimmed();
    if(title.isEmpty())
    {
        QMessageBox::warning(this,"Error","Title cannot be empthy");
        return;
    }

    Task t;
    t.title=title;
    t.priority=priorityBox->currentIndex()+1;

    QString dueText=dueEdit->text().trimmed();
    if(!dueText.isEmpty())
    {
        t.dueDate=QDate::fromString(dueText,"yyyy-MM-dd");
        if(!t.dueDate.isValid())
        {
            QMessageBox::warning(this,"Error","Invalid date format. Use yyyy-MM-dd");
            return;
        }
    }

    QMetaObject::invokeMethod(worker,"addTask",Qt::QueuedConnection,Q_ARG(Task, t));
    titleEdit->clear();
    dueEdit->clear();
}

void MainWindow::onCompleteClicked()
{
    auto* item=listWidget->currentItem();
    if(!item) return;

    int id=item->data(Qt::UserRole).toInt();
    QMetaObject::invokeMethod(worker,"setTaskDone",Qt::QueuedConnection,Q_ARG(int,id),Q_ARG(bool,true));
}

void MainWindow::onDeleteClicked()
{
    auto* item=listWidget->currentItem();
    if(!item)
    {
        return;
    }
    int id=item->data(Qt::UserRole).toInt();
    QMetaObject::invokeMethod(worker,"deleteTask",Qt::QueuedConnection,Q_ARG(int,id));
}

void MainWindow::onFilterChanged(int)
{
    qDebug() << "Filter changed to:" << filterBox->currentText();
    qDebug() << "Number of tasks in memory:" << currentTasks.size();
    refreshList(currentTasks);
}

void MainWindow::onTasksLoaded(const QVector<Task> &tasks)
{
    currentTasks=tasks;
    refreshList(tasks);
}

void MainWindow::onError(const QString &message)
{
    QMessageBox::critical(this,"Database Error",message);
}

void MainWindow::refreshList(const QVector<Task> &tasks)
{
    listWidget->clear();
    QString filter=filterBox->currentText();

    qDebug() << "===== refreshList called =====";
    qDebug() << "Current filter:" << filter;
    qDebug() << "Total tasks:" << tasks.size();

    for(const Task& t : tasks)
    {
        qDebug() << "Task:" << t.title << "| done =" << t.done << "| priority =" << t.priority;

        if(filter =="Pending" && t.done)
        {
            continue;
        }
        if(filter =="Completed" && !t.done)
        {
            continue;
        }
        if(filter =="High Priority"&& t.priority != 3)
        {
            continue;
        }
        if(filter == "Overdue" && !t.isOverdue())
        {
            continue;
        }

        QString status=t.done ? "[✓]" : "[ ]";
        if(!t.done && t.isOverdue()) status="[!]";

        QString priorityStr;
        switch(t.priority)
        {
        case 1: priorityStr="Low";break;
        case 2: priorityStr="Medium";break;
        case 3: priorityStr="High";break;
        }

        QString text=QString("%1 %2 %3 %4")
                           .arg(status)
                           .arg(t.title)
                           .arg(priorityStr)
                           .arg(t.dueDate.isValid() ? t.dueDate.toString("yyyy-MM-dd") : "-");

        QListWidgetItem* item=new QListWidgetItem(text);
        item->setData(Qt::UserRole,t.id);

        if(t.isOverdue())
        {
            item->setForeground(Qt::red);
        }

        listWidget->addItem(item);
    }
}
void MainWindow::onTaskAdded(bool success)
{
    if (!success) {
        QMessageBox::warning(this, "Error", "Failed add task");
    }
}

void MainWindow::onTaskUpdated(bool success)
{
    if (!success) {
        QMessageBox::warning(this, "Error", "Failed to update task");
    }
}


void MainWindow::onTaskDeleted(bool success)
{
    if (!success) {
        QMessageBox::warning(this, "Error", "Failed to delete task");
    }
}

void MainWindow::onEditTask(QListWidgetItem *item)
{
    if(!item) return;
    int id =item->data(Qt::UserRole).toInt();

    Task taskToEdit;
    bool found=false;
    for(const Task& t: currentTasks)
    {
        if(t.id==id)
        {
            taskToEdit=t;
            found=true;
            break;
        }
    }
    if(!found) return;

    EditTaskDialog dialog(taskToEdit,this);
    if(dialog.exec()==QDialog::Accepted)
    {
        Task updated = dialog.getTask();

        if(updated.title.isEmpty())
        {
            QMessageBox::warning(this,"Error","Title cannot be empty");
            return;
        }
        if(!updated.dueDate.isValid() && !dialog.findChild<QLineEdit*>("dueEdit")->text().trimmed().isEmpty())
        {

        }

        QMetaObject::invokeMethod(worker,"updateTask",Qt::QueuedConnection,Q_ARG(Task,updated));
    }



}
#include "EditTaskDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDate>

EditTaskDialog::EditTaskDialog(const Task& task,QWidget* parent)
    : QDialog(parent), originalTask(task)
{
    setWindowTitle("Edit Task");
    setModal(true);
    resize(400,250);

    QFormLayout* form=new QFormLayout;

    titleEdit=new QLineEdit(task.title);
    form->addRow("Title:",titleEdit);

    priorityBox=new QComboBox;
    priorityBox->addItems({"Low","Medium","High"});
    priorityBox->setCurrentIndex(task.priority -1);
    form->addRow("Priority:",priorityBox);

    dueEdit=new QLineEdit;
    if(task.dueDate.isValid())
    {
        dueEdit->setText(task.dueDate.toString("yyyy-MM-dd"));
    }
    dueEdit->setPlaceholderText("yyyy-MM-dd (optional)");
    form->addRow("Due date:",dueEdit);

    categoryEdit=new QLineEdit(task.category);
    categoryEdit->setPlaceholderText("Optional");
    form->addRow("Category:",categoryEdit);

    doneCheck=new QCheckBox("Completed");
    doneCheck->setChecked(task.done);
    form->addRow("",doneCheck);

    QDialogButtonBox* buttons=new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttons,&QDialogButtonBox::accepted,this,&QDialog::accept);
    connect(buttons,&QDialogButtonBox::rejected,this,&QDialog::reject);

    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addWidget(buttons);

}

Task EditTaskDialog::getTask() const
{
    Task t=originalTask;
    t.title=titleEdit->text().trimmed();
    t.priority=priorityBox->currentIndex()+1;
    t.done=doneCheck->isChecked();
    t.category=categoryEdit->text().trimmed();

    QString dueText=dueEdit->text().trimmed();
    if(dueText.isEmpty())
    {
        t.dueDate=QDate();
    }else
    {
        t.dueDate=QDate::fromString(dueText,"yyyy-MM-dd");
    }
    return t;
}

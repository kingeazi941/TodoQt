#pragma once

#include "Task.h"
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>

class EditTaskDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditTaskDialog(const Task& task,QWidget* parent=nullptr);

    // Returns the edited task (only valid if user clicked OK)
    Task getTask() const;

private:
    QLineEdit* titleEdit;
    QComboBox* priorityBox;
    QLineEdit* dueEdit;
    QCheckBox* doneCheck;
    QLineEdit* categoryEdit;

    Task originalTask;
};


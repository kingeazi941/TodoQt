#pragma once
#include <QString>
#include <QDate>

struct Task
{
    int id=0;
    QString title;
    bool done=false;
    int priority=2;
    QDate dueDate;
    QString category;

    bool isOverdue() const{
        if(!dueDate.isValid() || done)
            return false;
        return dueDate < QDate::currentDate();
    }
};

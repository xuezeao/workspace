#ifndef THREADWORKING_H
#define THREADWORKING_H

#include <QObject>

class ThreadWorking : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorking(QObject *parent = 0);

signals:

public slots:
};

#endif // THREADWORKING_H
#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread>

class WorkThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkThread(QObject *parent = 0);

signals:

public slots:

protected:
    void run(void);

};

#endif // WORKTHREAD_H

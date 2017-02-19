#ifndef WTHREAD_H
#define WTHREAD_H

#include <QThread>
#include "httpgp.h"

class WThread : public QThread
{
    Q_OBJECT
public:
    explicit WThread(QObject *parent = 0);

signals:

public slots:
    void run();

private:
    HttpGP *httpgg;

};

#endif // WTHREAD_H

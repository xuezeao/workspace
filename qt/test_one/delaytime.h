#ifndef DELAYTIME_H
#define DELAYTIME_H

#include <QObject>

class DelayTime : public QObject
{
    Q_OBJECT
public:
    explicit DelayTime(QObject *parent = 0);
    void waitTaskInfo(int tim);
signals:

public slots:

};

#endif // DELAYTIME_H

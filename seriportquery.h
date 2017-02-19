#ifndef SERIPORTQUERY_H
#define SERIPORTQUERY_H

#include <QObject>

class SeriportQuery : public QObject
{
    Q_OBJECT
public:
    explicit SeriportQuery(QObject *parent = 0);

signals:

public slots:
    void requestSTM();
};

#endif // SERIPORTQUERY_H

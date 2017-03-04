#ifndef CATALOG_H
#define CATALOG_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QHash>

struct bowellVar {
public:
    int num;
    QString name;
    double a;
    double i;
    double e;
    double mag;
    double arg;
    double anomaly;
    double knot;
    double g;
    QDate tosc;
    int numOfObs;
    int perOfObs;
};

struct obserVar {
public:
    QString code;
    QString name;
    double utc;
    double longitude;
    double cosLatitude;
    double sinLatitude;
};

class Bowell : public QObject
{
    Q_OBJECT
public:
    Bowell(QString path);
    ~Bowell();
    bowellVar getVar(int num);
    bowellVar getVar(QString name);
    QString getName(int num);
    int getMaxNum();

signals:
    void releasedErr(QString err,int ErrCode);

private:
    Bowell() {}
    void read();
    QString sPath;
    QList<bowellVar> lvar;
    QHash<QString, bowellVar> hashName;
    QHash<int, bowellVar> hashNum;
    int maxNum;
};

class Obser : public QObject
{
    Q_OBJECT
public:
    Obser(QString path, QString code);
    ~Obser();
    obserVar getVar(double utc);

private:
    Obser() {}
    QString read();
    QString sCode;
    obserVar var;
    QString sPath;

signals:
    void releasedErr(QString err,int ErrCode);
};

class Dtime : public QObject
{
    Q_OBJECT
public:
    Dtime(QString path, double JD);
    ~Dtime();
    double getVar();

private:
    Dtime() {}
    bool read();
    double jd;
    double Julian_Date[1000];
    double eph_t[1000];
    int eph_count;
    double var;
    QString sPath;

};

#endif // CATALOG_H

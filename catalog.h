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

class Bowell
{
public:
    Bowell(QString path);
    ~Bowell();
    bowellVar getVar(int num);
    bowellVar getVar(QString name);
    QString getName(int num);
    int getMaxNum();

private:
    void read();
    QString sPath;
    QList<bowellVar> lvar;
    QHash<QString, bowellVar> hashName;
    QHash<int, bowellVar> hashNum;
    int maxNum;
};

class Obser
{
public:
    Obser(QString path, QString code);
    ~Obser();
    obserVar getVar(int utc);

private:
    QString read();
    QString sCode;
    obserVar var;
    QString sPath;
};

class Dtime
{
public:
    Dtime(QString path, double JD);
    ~Dtime();
    double getVar();

private:
    bool read();
    double jd;
    double Julian_Date[1000];
    double eph_t[1000];
    int eph_count;
    double var;
    QString sPath;

};

#endif // CATALOG_H

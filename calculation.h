#ifndef CALCULATION_H
#define CALCULATION_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QDir>
#include <QThread>
#include <QRunnable>
#include "catalog.h"
#include "dereader.h"
#include "convertcoor.h"
#include "time.h"

struct setVar {
public:
    int height;
    int elongation;
    int magnitude;
    double jdFrom;
    double jdTo;
    DEreader *de;
    int border;
    QVector<obserVar> vov;
    QVector<bowellVar> vbv;
    QVector<double> magSee;
    int force_var[16];
    int LL;
    int NOR;
    QString path;
    double gstep;
    double step;
};


class Calculation : public QObject
                  , public QRunnable
{
    Q_OBJECT
public:
    Calculation() {}
    ~Calculation() {}
    virtual void run() = 0;

signals:
    void call();
    void prg(int value);

protected:
    setVar sv;
    QVector<double> From;
    QVector<double> To;

    void h_min_sec(double c, int &hour, int &min, double &sec);
    QString formatStr(double grad, int f);
    QString formatNum(double num, int f);
    void makeXV();

    double *x0, *v0;
    double *dx, *dv;
    double *x, *v;
};

class Hunter : public Calculation
{
    Q_OBJECT
public:
    Hunter() {}
    Hunter(setVar &insv)
    {
        sv = insv;
        sv.step /= 1440.;
    }
    ~Hunter() {}
    void run();

private:
    void findObj(QFile &f, obserVar &ov, double stepPrg, double &value);

};

class Numerator : public Calculation
{
    Q_OBJECT
public:
    Numerator() {}
    Numerator(setVar &insv)
    {
        sv = insv;
        sv.step /= 1440.;
    }
    ~Numerator() {}
    void run();

private:
    void numObj();
};

class Scout : public Calculation
{
    Q_OBJECT
public:
    Scout() {}
    Scout(setVar &insv)
    {
        sv = insv;
        sv.step = 30 / 1440.;
    }
    ~Scout() {}
    void run();

private:
    void findObjs();
};

class Guard : public QObject
            , public QRunnable
{
    Q_OBJECT
public:
    Guard(setVar &gsv, obserVar &gov);
    void run();
    QVector<double> getJdFrom();
    QVector<double> getJdTo();

signals:
    void finished();
    void printedMes(QString mes);
    void prg(int value);
    void releasedErr(QString err,int ErrCode);


private:
    obserVar ov;
    QVector<double> From;
    QVector<double> To;
    setVar sv;

};

#endif // CALCULATION_H



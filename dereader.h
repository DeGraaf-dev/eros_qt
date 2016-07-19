#ifndef DEREADER_H
#define DEREADER_H

#include <math.h>
#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QDebug>

class DEreader
{

public:
    DEreader();
    DEreader(int fond, QString pathFond);
    ~DEreader();

    void GetPlanetPoz(double jdate, int index, bool geleo, double poz[]);

    struct DEconst {
    public:
        double AE;
        double speedLight;
        double G;
        double rSun;
        double compSun;
        double rEarth;
        double compEarth;
        double massPlanet[9];
        double massMoon;
        double massAst[3];
    };

    DEconst deConst;

private:
    struct DEheader {
    public:
        int sizeStr;
        int step;
        double tMin;
        double tMax;
        double EarthDivMoon;
        double nper[11];
        double pow[11];
        double raz[11];
    };
    DEheader deHeader;
    void cheb(bool vel, double a, double b, double t, int st, double tc[], double tcp[]);
    void coor(bool vel, int i, int n1, double tc[], double tcp[], double x[]);
    void read(double t);

    QVector<double> buf;
    QString sPathFond;
    double dJD;
};

#endif // DEREADER_H

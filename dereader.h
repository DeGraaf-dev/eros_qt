#ifndef DEREADER_H
#define DEREADER_H

#include <math.h>
#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QDebug>

class DEreader : public QObject
{    
    Q_OBJECT
public:
    DEreader(int numFond, QString pathFond);
    ~DEreader();

    bool GetPlanetPoz(double jdate, int index, bool geleo, double poz[]);

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
    DEreader();
    struct DEheader {
    public:
        int sizeStr;
        int step;
        int pow[11];
        int raz[11];
        int nper[11];
        double tMin;
        double tMax;
        double EarthDivMoon;
    };
    DEheader deHeader;
    void cheb(bool vel, double a, double b, double t, int st, double tc[], double tcp[]);
    void coor(bool vel, int i, int n1, double tc[], double tcp[], double x[]);
    bool read(double t);

    QVector<double> buf;
    double dJD;
    QFile *fond;

signals:
    void releasedErr(QString err);
};

#endif // DEREADER_H

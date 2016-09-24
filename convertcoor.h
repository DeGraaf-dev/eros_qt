#ifndef CONVERTCOOR_H
#define CONVERTCOOR_H

#include <QtHelp/QtHelp>
#include "math.h"
#include "catalog.h"
#include "time.h"
#include "dereader.h"

// глобальные переменные
struct VAR{
public:
    double W[13], U[13], C[78], D[78], R[78], XI[78], H[14];
    int NF, KD, KF, KE, NPQ, NSF, NES, NCL, NPER, NL, NS, NCOUNT;
    int NW[14], MC[12];
    double *BT, *BE, F1[516], FJ[528], *Y, *Z, *B, TDIF, DIR, PW, TP, SR, W1, TM, SM, SS;
};

class Coor {
public:
    Coor();
    ~Coor() {}
    Coor(double ix[], bool ecl);
    Coor(double ix[], double plc[], bool ecl);
    void getX(double cx[], bool ecl);
private:
    void equ2ecl(double c[], double equ[]);
    void ecl2equ(double c[], double ecl[]);
    double x[3];
};

class Spherical
{
public:
    Spherical();
    ~Spherical() {}
    Spherical(Coor &cx, bool ecl);
    double getRad();
    double getDegDelta();
    double getRadAlpha();
private:
    double rad;
    double delta;
    double alpha;
};

class Horizontal
{
public:
    Horizontal();
    ~Horizontal() {}
    Horizontal(obserVar &ov, double jdate, Coor &coor, bool ecl);
    Horizontal(obserVar &ov, double hourAlpha, double degDelta, double jdate);
    double getDegH();
    double getDegAz();
private:
    double degH;
    double degAz;
};

class ElemOrb
{
public:
    ElemOrb();
    ~ElemOrb() {}
    ElemOrb(bowellVar &bv);
    void getX(double cx[]);
    void getV(double cv[]);
private:
    double x[3];
    double v[3];
};

class Integrator
{
public:
    Integrator() {}
    Integrator(DEreader *de);
    ~Integrator() {}
    void rada27(double X[], double V[], double TI, double TF, int force_var[]
                , int NOR, int LL, DEreader *de);
    void ephemerida(double X[], double V[], double ephem, double JD0, DEreader *de, obserVar &ov, int NOR, int LL, int force_var[]);
    double magnitude_observ(double X[], double epher, DEreader *de, bowellVar &bv);
    double getDegDelta();
    double getHourAlpha();
private:
    void force(double X[], double V[], double TS, double *F, DEreader *de
               , int force_var[]);
    int radamaker(double X[], double V[], double TF, DEreader *de, int force_var[]);
    void abberation(double X[], double V[], double JD0, DEreader *de
                    , int force_var[], int NOR, int LL, double poz[]);
    VAR v;
    double alpha;
    double delta;
    double massObj[14];

    void force12(double X[], double massEarth, double coorEarth[], double rEarth2Aster, DEreader *de, double F[]);
    void force13(double X[], double TS, double rAster, DEreader *de, double F[]);
    void force14(double X[], double V[], double rAster, DEreader *de, double F[]);
    void force15(double X[], double massJupiter, double coorJupiter[], double rJupiter2Aster, double TS, DEreader *de, double F[]);
};

#endif // CONVERTCOOR_H
>>>>>>> origin/master

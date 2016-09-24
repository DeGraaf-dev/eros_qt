include "dereader.h"

DEreader::DEreader()
{

}

/*----------------------------------------------------------------------------*/
DEreader::DEreader(int fond, QString pathFond)
{
    sPathFond = pathFond;
    if (fond == 405) {
        deHeader.sizeStr = 1018;
        deHeader.step = 32;
        deHeader.tMin = 2305424.50;
        deHeader.tMax = 2525008.50;
        deHeader.EarthDivMoon = 0.813005600000000044E+02;

        deHeader.nper[0] = 3;
        deHeader.nper[1] = 171;
        deHeader.nper[2] = 231;
        deHeader.nper[3] = 309;
        deHeader.nper[4] = 342;
        deHeader.nper[5] = 366;
        deHeader.nper[6] = 387;
        deHeader.nper[7] = 405;
        deHeader.nper[8] = 423;
        deHeader.nper[9] = 441;
        deHeader.nper[10] = 753;

        deHeader.pow[0] = 14;
        deHeader.pow[1] = 10;
        deHeader.pow[2] = 13;
        deHeader.pow[3] = 11;
        deHeader.pow[4] = 8;
        deHeader.pow[5] = 7;
        deHeader.pow[6] = 6;
        deHeader.pow[7] = 6;
        deHeader.pow[8] = 6;
        deHeader.pow[9] = 13;
        deHeader.pow[10] = 11;

        deHeader.raz[0] = deHeader.step/4;
        deHeader.raz[1] = deHeader.step/2;
        deHeader.raz[2] = deHeader.step/2;
        deHeader.raz[3] = deHeader.step/1;
        deHeader.raz[4] = deHeader.step/1;
        deHeader.raz[5] = deHeader.step/1;
        deHeader.raz[6] = deHeader.step/1;
        deHeader.raz[7] = deHeader.step/1;
        deHeader.raz[8] = deHeader.step/1;
        deHeader.raz[9] = deHeader.step/8;
        deHeader.raz[10] = deHeader.step/2;

        deConst.AE = 149597870.691000015;
        deConst.speedLight = 299792.457999999984;
        deConst.G = 0.295912208285591095E-03;
        deConst.rSun = 696000.000000000000;
        deConst.compSun = 0.199999999999999991E-06;
        deConst.rEarth = 6378.13699999999972;
        deConst.compEarth = 0.108262599999999994E-02;

        deConst.massPlanet[0] = 0.491254745145081187E-10;
        deConst.massPlanet[1] = 0.724345248616270270E-09;
        deConst.massPlanet[2] = 0.899701134671249882E-09*(1 - 1 / (deHeader.EarthDivMoon + 1)); // Earth
        deConst.massPlanet[3] = 0.954953510577925806E-10;
        deConst.massPlanet[4] = 0.282534590952422643E-06;
        deConst.massPlanet[5] = 0.845971518568065874E-07;
        deConst.massPlanet[6] = 0.129202491678196939E-07;
        deConst.massPlanet[7] = 0.152435890078427628E-07;
        deConst.massPlanet[8] = 0.218869976542596968E-11;

        deConst.massMoon = 0.899701134671249882E-09 / (deHeader.EarthDivMoon + 1);

        deConst.massAst[0] = 0.139078737894227800E-12;
        deConst.massAst[1] = 0.295912208285591104E-13;
        deConst.massAst[2] = 0.384685870771268372E-13;
    }
    dJD = 0;
}

DEreader::~DEreader()
{
    buf.clear();
}

/*----------------------------------------------------------------------------*/
void DEreader::GetPlanetPoz(double jdate, int index, bool geleo, double poz[])
{
    if (jdate != dJD) {
        read(jdate);
        dJD = jdate;
    }
    double tc[16];
    double tcp[16];
    for (int i = 0; i < 16; i++) {
        tc[i] = 0;
        tcp[i] = 0;
    }

    double n1=0;
    double d = buf[0];
    double c = d - deHeader.raz[index];
    n1 = -3*deHeader.pow[index];
    do {
        c += deHeader.raz[index];
        d += deHeader.raz[index];
        n1 += 3*deHeader.pow[index];
    } while (jdate < c || jdate > d);
    cheb(false, c, d, jdate, deHeader.pow[index], tc, tcp);
    coor(false, index, n1, tc, tcp, poz);

    if (index == 2 || index == 9) {
        int idx = 0;
        if (index == 2) idx = 9;
        else idx = 2;
        n1 = 0;
        d = buf[0];
        c = d - deHeader.raz[idx];
        n1 = -3*deHeader.pow[idx];
        do {
            c += deHeader.raz[idx];
            d += deHeader.raz[idx];
            n1 += 3*deHeader.pow[idx];
        } while (jdate < c || jdate > d);
        cheb(false, c, d, jdate, deHeader.pow[idx], tc, tcp);
        double x[6];
        coor(false, idx, n1, tc, tcp, x);
        for (int k = 0; k < 6; k++)
            poz[k] = poz[k] - x[k] / (deHeader.EarthDivMoon + 1); // земля
        if (index == 9)
            for (int k = 0; k < 6; k++)
                poz[k] += x[k]; // луна
    }
    if (geleo) {
        n1 = 0;
        d = buf[0];
        c = d - deHeader.raz[10];
        n1 = -3*deHeader.pow[10];
        do {
            c += deHeader.raz[10];
            d += deHeader.raz[10];
            n1 += 3*deHeader.pow[10];
        } while (jdate < c || jdate > d);
        cheb(false, c, d, jdate, deHeader.pow[10], tc, tcp);
        double x[6];
        coor(false, 10, n1, tc, tcp, x);
        for (int k = 0; k < 6; k++)
            poz[k] = (poz[k] - x[k]) / deConst.AE;
    }
    else
        for (int k = 0; k < 6; k++)
            poz[k] /= deConst.AE;
}

void DEreader::cheb(bool vel, double a, double b, double t, int st, double tc[], double tcp[])
{
    double rat;
    double dlin = b - a;
    double tau = 2*(t - a) / dlin - 1;
    double tau2 = tau*2;
    tc[0] = 1;
    tc[1] = tau;
    for (int i = 2; i <= st; i++)
        tc[i] = tau2*tc[i - 1] - tc[i - 2];
    if (vel) {
        rat = 4 / dlin;
        tcp[0] = 0;
        tcp[1] = 2/dlin;
        for (int i = 2; i <= st; i++)
            tcp[i] = rat*tc[i - 1] + tau2*tcp[i - 1] - tcp[i - 2];
    }
}

/*----------------------------------------------------------------------------*/
void DEreader::coor(bool vel, int i, int n1, double tc[], double tcp[], double x[])
{
    int jj;
    double s;
    int ist = deHeader.pow[i];
    for (int k = 0; k < 3; k++) {
        s = 0;
        jj = deHeader.nper[i] + k*ist + n1 - 1;
        for (int j = 0; j < deHeader.pow[i]; j++)
            s += tc[j] * buf[jj + j];
        x[k] = s;
    }
    if (vel)
        for (int k = 0; k < 3; k++) {
            s = 0;
            jj = deHeader.nper[i] + k*ist + n1 - 1;
            s=0;
            for (int j = 0; j < deHeader.pow[i]; j++)
                s += tcp[j] * buf[jj + j];
            x[k + 3] = s;
        }
}

/*----------------------------------------------------------------------------*/
void DEreader::read(double t)
{
    int nrc = (dJD - deHeader.tMin) / deHeader.step;
    int nr = (t - deHeader.tMin) / deHeader.step;

    if (nrc != nr) {
        buf.clear();
        QFile fond(sPathFond);
        QDataStream stream(&fond);
        stream.setByteOrder(QDataStream::LittleEndian);
        if (fond.open(QIODevice::ReadOnly)) {
            fond.seek(nr * deHeader.sizeStr * sizeof(double));
            for (int i = 0; i < deHeader.sizeStr; i++) {
                double d;
                stream >> d;
                buf.append(d);
            }
            fond.close();
        }
    }
}
>>>>>>> origin/master

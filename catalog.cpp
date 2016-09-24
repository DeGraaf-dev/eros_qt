#include "catalog.h"

/*----------------------------------------------------------------------------*/
Bowell::Bowell(QString path) {
    sPath = path;
}

Bowell::~Bowell() {}

bowellVar Bowell::getVar(int num)
{
    bowellVar var;
    if (lvar.isEmpty())
        read();
    return hashNum.value(num, var);
}

bowellVar Bowell::getVar(QString name)
{
    bowellVar var;
    if (lvar.isEmpty())
        read();
    return hashName.value(name, var);
}

QString Bowell::getName(int num)
{
    bowellVar var;
    if (lvar.isEmpty())
        read();
    var = hashNum.value(num, var);
    return var.name;
}

int Bowell::getMaxNum()
{
    return maxNum;
}

void Bowell::read()
{
    QFile f(sPath);
    if (f.open(QIODevice::ReadOnly)) {
        QString sBowell = f.readAll();
        bowellVar var;
        maxNum = 0;
        f.close();
        for (int i = 0; i < sBowell.size(); i += 268) {
            if (i + 169 + 12 > sBowell.size())
                break;
            maxNum++;
            var.num = maxNum;
            var.name = sBowell.mid(i + 7, 19).trimmed();
            var.a = sBowell.mid(i + 169, 12).trimmed().toDouble();
            var.i = sBowell.mid(i + 148, 9).trimmed().toDouble();
            var.e = sBowell.mid(i + 158, 10).trimmed().toDouble();
            var.mag = sBowell.mid(i + 42, 5).trimmed().toDouble();
            var.arg = sBowell.mid(i + 126, 10).trimmed().toDouble();
            var.anomaly = sBowell.mid(i + 115, 10).trimmed().toDouble();
            var.knot = sBowell.mid(i + 137, 10).trimmed().toDouble();
            var.g = sBowell.mid(i + 49, 4).trimmed().toDouble();
            var.tosc.setDate(sBowell.mid(i + 106, 4).toInt(),
                             sBowell.mid(i + 110, 2).toInt(),
                             sBowell.mid(i + 112, 2).toInt());
            var.numOfObs = sBowell.mid(i + 101, 4).toInt();
            var.perOfObs = sBowell.mid(i + 95, 5).toInt();
            lvar.append(var);
            hashName.insert(var.name, lvar[var.num-1]);
            hashNum.insert(var.num, lvar[var.num-1]);
        }
    }
}

/*----------------------------------------------------------------------------*/
Obser::Obser(QString path, QString code) {
    sCode = code;
    sPath = path;
}

Obser::~Obser() {}

obserVar Obser::getVar(int utc)
{
    QString sObser = read();
    var.code = sObser.mid(0, 3);
    var.longitude = sObser.mid(4, 9).toDouble();
    var.cosLatitude = sObser.mid(14, 8).toDouble();
    var.sinLatitude = sObser.mid(23, 9).toDouble();
    var.name = sObser.mid(33, sObser.size() - 33 - 2);
    var.utc = utc;
    return var;
}

/*----------------------------------------------------------------------------*/
QString Obser::read()
{
    QFile f(sPath);
    if (f.open(QIODevice::ReadOnly)) {
        QString s;
        int idx;
        while (!f.atEnd()) {
            s = f.readLine();
            idx = s.indexOf(sCode, 0);
            if (idx != -1) {
                f.close();
                return s;
            }
        }
        return "-1";
    } else
        return "-1";
}

/*----------------------------------------------------------------------------*/
Dtime::Dtime(QString path, double JD) {
    sPath = path;
    jd = JD;
}

Dtime::~Dtime() {}

double Dtime::getVar()
{
    if (read()) {
        int i = 0;
        double J1, J2;
        double deph0;
        J2 = Julian_Date[eph_count];
        if (jd > J2)
            deph0 = eph_t[eph_count];
        else {
            J2 = Julian_Date[0];
            if (jd < J2)
                deph0 = eph_t[0];
            else {
                while (jd > J2) {
                    i++;
                    J2 = Julian_Date[i];
                }
                J2 = Julian_Date[i];
                J1 = Julian_Date[i - 1];
                deph0 = eph_t[i - 1] + (eph_t[i] - eph_t[i - 1]) / (J2 - J1)*(jd - J1);
            }
        }
        var = jd + deph0 / (60 * 60 * 24);
        return var;
    } else
        return -1;
}

bool Dtime::read()
{
    eph_count = 0;
    QString temp;
    int Year_, Month_, Day_;
    QFile sr_epher(sPath);
    if (sr_epher.open(QIODevice::ReadOnly)) {
        while (!sr_epher.atEnd()) {
            temp = sr_epher.readLine();
            Year_ = temp.mid(1, 4).toInt();
            Month_ = temp.mid(6, 2).toInt();
            Day_ = temp.mid(9, 2).toInt();

            eph_t[eph_count] = temp.mid(13, 7).toDouble();
            Julian_Date[eph_count] = QDate(Year_, Month_, Day_).toJulianDay();
            eph_count++;
        }
        eph_count--;
        sr_epher.close();
        return true;
    }
     else
        return false;
}
>>>>>>> origin/master

#include "calculation.h"

Numerator::Numerator(setVar &insv)
{
    sv = insv;
    sv.step /= 1440.;
}

void Numerator::run()
{
    sv.jdFrom = sv.jdFrom + .5 - sv.vov.first().utc / 24.;
    sv.jdTo = sv.jdTo + .5 - sv.vov.first().utc / 24.;
    Guard *c = new Guard(sv, sv.vov.first());
    c->run();
    From = c->getJdFrom();
    To = c->getJdTo();
    c->deleteLater();
    numObj();
    emit prg(0);
    emit call();
}

void Numerator::numObj()
{
    makeXV();
    int end = sv.force_var[11] == 1 ? 12 : 3;
    obserVar ov = sv.vov.first();
    double ephem;
    QString s;
    double mag;
    QString ephemPath = sv.path + "Libr" + QDir::separator() + "dtime.txt";
    Integrator *integrator = new Integrator(sv.de);
    double stepPrg = sv.force_var[11] == 1 ? From.size() * sv.vbv.size() - 3 : From.size() * sv.vbv.size();
    stepPrg = 100 / stepPrg;
    double value = 0;
    for (int i = sv.force_var[11] * 3; i < sv.vbv.count(); i++) {
        ElemOrb eo(sv.vbv[i]);
        eo.getX(x0);
        eo.getV(v0);
        Coor coor = Coor(x0, true);
        coor.getX(x0, false);
        coor = Coor(v0, true);
        coor.getX(v0, false);
        if (!sv.vbv[i].name.isEmpty()) {
            QFile f(sv.path + ov.code + QDir::separator() + sv.vbv[i].name + ".txt");
            if (f.exists())
                f.remove();
            for (int j = 0; j < From.count(); j++) {
                if (!f.open(QIODevice::WriteOnly))
                    break;
                memcpy(dx, x0, end*sizeof(double));
                memcpy(dv, v0, end*sizeof(double));
                f.write("yyyy.mm.dd hh:mm   alpha         delta        mag    h             As\n");
                if (fabs(From[j] - Time(sv.vbv[i].tosc).getJulianDay()) > 1e-13)
                    integrator->rada27(dx, dv, Time(sv.vbv[i].tosc).getJulianDay()
                                       , From[j], sv.force_var, sv.NOR, sv.LL, sv.de);
                for (double jj = From[j]; jj <= To[j]; jj += sv.step) {
                    ephem = Dtime(ephemPath, jj).getVar();
                    mag = integrator->magnitude_observ(dx, ephem, sv.de, sv.vbv[i]);
                    memcpy(x, dx, end*sizeof(double));
                    memcpy(v, dv, end*sizeof(double));
                    integrator->ephemerida(x, v, ephem, jj, sv.de, ov, sv.NOR, sv.LL
                                           , sv.force_var);
                    if (jj + sv.step < To[j])
                        integrator->rada27(dx, dv, jj, jj + sv.step, sv.force_var, sv.NOR
                                           , sv.LL, sv.de);
                    Horizontal hor(ov, integrator->getHourAlpha(), integrator->getDegDelta(), jj);
                    Time t(jj);
                    QString sal = formatStr(integrator->getHourAlpha(), 1);
                    QString sdel = formatStr(integrator->getDegDelta(), 1);
                    QString sh = formatStr(hor.getDegH(), 1);
                    QString saz = formatStr(hor.getDegAz(), 2);
                    QString stime = tr("%1.%2%3.%4%5 %6%7:%8%9")
                            .arg(t.getYear())
                            .arg(t.getMonth() < 10 ? "0":"").arg(t.getMonth())
                            .arg(t.getDay() < 10 ? "0":"").arg(t.getDay())
                            .arg(t.getHour() < 10 ? "0":"").arg(t.getHour())
                            .arg(t.getMin() < 10 ? "0":"").arg(t.getMin());
                    QString smag = tr("%1%2")
                            .arg(mag < 10 ? "0":"").arg(mag, 0, 'f', 1);
                    s = stime + "  " + sal + "  " + sdel + "  " + smag + "  "
                            + sh + "  " + saz + "\n";
                    f.write((char*)s.toUtf8().data());
                }
                if (int(value+stepPrg) - int(value) >= 1)
                    emit prg(int(value+stepPrg));
                value += stepPrg;
            }
            f.close();
        }
    }
    delete x0;
    delete dx;
    delete v0;
    delete dv;
    delete integrator;
}

void Calculation::h_min_sec(double c, int &hour, int &min, double &sec)
{
    hour = int(c);
    min = int((c - hour) * 60);
    sec = ((c - hour) * 60 - int((c - hour) * 60)) * 60;
}

QString Calculation::formatStr(double deg, int f)
{
    int h;
    int m;
    double s;
    h_min_sec(deg, h, m, s);
    QString sh = tr("%1%2")
            .arg(h >= 0 ? " ":"-")
            .arg(formatNum(abs(h), f));
    QString sm = tr("%1%2")
            .arg(abs(m) < 10 ? "0":"")
            .arg(abs(m));
    QString ss = tr("%1%2")
            .arg(abs(s) < 10 ? "0":"")
            .arg(fabs(s), 0, 'f', 2);
    return sh + " " + sm  + " " + ss;
}

QString Calculation::formatNum(double num, int f)
{
    QString s;
    for (int i = f; i > 0; i--)
        if (num < pow(10, i))
            s += "0";
    s += QString::number(num);
    return s;
}

void Calculation::makeXV()
{
    double tx[3];
    if (sv.force_var[11]) {
        x0 = new double[12];
        dx = new double[12];
        x = new double[12];
        v0 = new double[12];
        dv = new double[12];
        v = new double[12];
        for (int j = 0; j < 3; j++) {
            ElemOrb eo(sv.vbv[j]);
            eo.getX(tx);
            Coor coor(tx, true);
            coor.getX(tx, false);
            for (int i = 0; i < 3; i++)
                x0[3 + j * 3 + i] = tx[i];
            eo.getV(tx);
            coor = Coor(tx, true);
            coor.getX(tx, false);
            for (int i = 0; i < 3; i++)
                v0[3 + j * 3 + i] = tx[i];
        }
    } else {
        x0 = new double[3];
        x = new double[3];
        dx = new double[3];
        v0 = new double[3];
        dv = new double[3];
        v = new double[3];
    }
}

Hunter::Hunter(setVar &insv)
{
    sv = insv;
    sv.step /= 1440.;
}

void Hunter::run()
{
    if (!QDir().exists(sv.path + "hunter"))
        QDir().mkdir(sv.path + "hunter");
    obserVar ov;
    double stepPrg = 100. / (sv.vov.size() * sv.vbv.size() * (sv.jdTo - sv.jdFrom));
    double value = 0;
    foreach (ov, sv.vov) {
        QFile f(sv.path + "hunter" + QDir::separator() + ov.code + ".txt");
        if (f.open(QIODevice::WriteOnly)) {
            Time to(sv.jdTo);
            Time from(sv.jdFrom);
            QString wr = tr("Observatory = %1 %2"
                            "\nMagnitude Limit = %3"
                            "\nRise/Set Elevation = %4"
                            "\nInterval: %5.%6.%7 - %8.%9.%10\n")
                    .arg(ov.code).arg(ov.name)
                    .arg(sv.magnitude).arg(sv.height)
                    .arg(from.getYear()).arg(from.getMonth()).arg(from.getDay())
                    .arg(to.getYear()).arg(to.getMonth()).arg(to.getDay());
            f.write((char*)wr.toUtf8().data());
            sv.jdFrom = sv.jdFrom + .5 - ov.utc / 24.;
            sv.jdTo = sv.jdTo + .5 - ov.utc / 24.;
            Guard *c = new Guard(sv, ov);
            c->run();
            From = c->getJdFrom();
            To = c->getJdTo();
            c->deleteLater();
            findObj(f, ov, stepPrg, value);
        }
    }
    emit prg(0);
    emit call();
}

void Hunter::findObj(QFile &f, obserVar &ov, double stepPrg, double &value)
{
    makeXV();
    int end = sv.force_var[11] == 1 ? 12 : 3;
    double ephem;
    QString s;
    double mag;
    QString ephemPath = sv.path + QDir::separator() + "Libr" + QDir::separator() + "dtime.txt";
    bowellVar bv;
    Integrator *integrator = new Integrator(sv.de);
    f.write("      yyyy.mm.dd hh:mm   height       mag\n");
    for (int i = sv.force_var[11] * 3; i < sv.vbv.size(); i++) {
        bv = sv.vbv[i];
        f.write((char*)bv.name.toUtf8().data());
        f.write("\n");
        ElemOrb eo(bv);
        eo.getX(x0);
        eo.getV(v0);
        Coor coor = Coor(x0, true);
        coor.getX(x0, false);
        coor = Coor(v0, true);
        coor.getX(v0, false);
        for (int j = 0; j < From.count(); j++) {
            memcpy(dx, x0, end*sizeof(double));
            memcpy(dv, v0, end*sizeof(double));
            if (fabs(From[j] - Time(bv.tosc).getJulianDay()) > 1e-13)
                integrator->rada27(dx, dv, Time(bv.tosc).getJulianDay(), From[j]
                                   , sv.force_var, sv.NOR, sv.LL, sv.de);
            s.clear();
            for (double jj = From[j]; jj <= To[j]; jj += sv.step) {
                ephem = Dtime(ephemPath, jj).getVar();
                mag = integrator->magnitude_observ(dx, ephem, sv.de, bv);
                memcpy(x, dx, end*sizeof(double));
                memcpy(v, dv, end*sizeof(double));
                integrator->ephemerida(x, v, ephem, jj, sv.de, ov, sv.NOR, sv.LL
                                       , sv.force_var);
                if (jj + sv.step < To[j])
                    integrator->rada27(dx, dv, jj, jj + sv.step, sv.force_var, sv.NOR
                                       , sv.LL, sv.de);
                if (mag > sv.magnitude)
                    break;
                Horizontal hor(ov, integrator->getHourAlpha(), integrator->getDegDelta(), jj);
                if (hor.getDegH() >= sv.height && s.isEmpty() && jj + sv.step < To[j]) {
                    Time t(jj);
                    s = "From: ";
                    QString sh = formatStr(hor.getDegH(), 1);
                    QString stime = tr("%1.%2%3.%4%5 %6%7:%8%9")
                            .arg(t.getYear())
                            .arg(t.getMonth() < 10 ? "0":"").arg(t.getMonth())
                            .arg(t.getDay() < 10 ? "0":"").arg(t.getDay())
                            .arg(t.getHour() < 10 ? "0":"").arg(t.getHour())
                            .arg(t.getMin() < 10 ? "0":"").arg(t.getMin());
                    QString smag = tr("%1%2")
                            .arg(mag < 10 ? "0":"").arg(mag, 0, 'f', 1);
                    s += stime + "  " + sh + "  " + smag + "\n";
                }
                if (hor.getDegH() <= sv.height && !s.isEmpty()) {
                    Time t(jj);
                    s += "To:   ";
                    QString sh = formatStr(hor.getDegH(), 1);
                    QString stime = tr("%1.%2%3.%4%5 %6%7:%8%9")
                            .arg(t.getYear())
                            .arg(t.getMonth() < 10 ? "0":"").arg(t.getMonth())
                            .arg(t.getDay() < 10 ? "0":"").arg(t.getDay())
                            .arg(t.getHour() < 10 ? "0":"").arg(t.getHour())
                            .arg(t.getMin() < 10 ? "0":"").arg(t.getMin());
                    QString smag = tr("%1%2")
                            .arg(mag < 10 ? "0":"").arg(mag, 0, 'f', 1);
                    s += stime + "  " + sh + "  " + smag + "\n";
                    f.write((char*)s.toUtf8().data());
                    break;
                }
                if (jj + sv.step >= To[j] && hor.getDegH() >= sv.height && !s.isEmpty()) {
                    Time t(jj);
                    s += "To:   ";
                    QString sh = formatStr(hor.getDegH(), 1);
                    QString stime = tr("%1.%2%3.%4%5 %6%7:%8%9")
                            .arg(t.getYear())
                            .arg(t.getMonth() < 10 ? "0":"").arg(t.getMonth())
                            .arg(t.getDay() < 10 ? "0":"").arg(t.getDay())
                            .arg(t.getHour() < 10 ? "0":"").arg(t.getHour())
                            .arg(t.getMin() < 10 ? "0":"").arg(t.getMin());
                    QString smag = tr("%1%2")
                            .arg(mag < 10 ? "0":"").arg(mag, 0, 'f', 1);
                    s += stime + "  " + sh + "  " + smag + "\n";
                    f.write((char*)s.toUtf8().data());
                    break;
                }
            }
            if (int(value+stepPrg) - int(value) >= 1)
                emit prg(int(value+stepPrg));
            value += stepPrg;
        }
    }
    f.close();
    delete x0;
    delete dx;
    delete v0;
    delete dv;
    delete integrator;

}

Guard::Guard(setVar &gsv, obserVar &gov)
{
    sv = gsv;
    ov = gov;
    sv.gstep /= 1440.;
    sv.jdFrom = sv.jdFrom + .5 - ov.utc / 24.;
    sv.jdTo = sv.jdTo + .5 - ov.utc / 24.;
}

QVector<double> Guard::getJdFrom()
{
    return From;
}

QVector<double> Guard::getJdTo()
{
    return To;
}

void Guard::run()
{
    double poz[3];
    double h = 0;
    double hst = 0;
    if (!sv.de->GetPlanetPoz(sv.jdFrom, 2 /*Земля*/, true, poz))
            return;
    for (int j = 0; j < 3; j++)
        poz[j] *= -1;
    QString mes;
    Coor coor(poz, false);
    double value = 0;
    double stepPrg = 100 * sv.gstep / (sv.jdTo - sv.jdFrom);
    h = Horizontal(ov, sv.jdFrom, coor, false).getDegH();
    for (double i = sv.jdFrom + sv.gstep; i <= sv.jdTo; i += sv.gstep) {
        sv.de->GetPlanetPoz(i, 2 /*Земля*/, true, poz);
        for (int j = 0; j < 3; j++)
            poz[j] *= -1;
        coor = Coor(poz, false);
        hst = Horizontal(ov, i, coor, false).getDegH();
        if (h < sv.border && hst >= sv.border && From.count() > To.count()) {
            To << i;
            Time time(i);
            mes += "Восход: " + QString::number(time.getMonth()) + "."
                    + QString::number(time.getDay()) + " "
                    + QString::number(time.getHour())
                    + ":" + QString::number(time.getMin()) + "\n";
        } else if (hst <= sv.border && h > sv.border) {
            From << i;
            Time time(i);
            mes += "Заход: " + QString::number(time.getMonth()) + "."
                    + QString::number(time.getDay()) + " "
                    + QString::number(time.getHour())
                    + ":" + QString::number(time.getMin()) + " -> ";
        }
        h = hst;
        if (int(value+stepPrg) - int(value) >= 1)
            emit prg(int(value+stepPrg));
        value += stepPrg;
    }
    emit prg(0);
    emit printedMes(mes);
}

void Scout::findObjs()
{
    makeXV();
    int end = sv.force_var[11] == 1 ? 12 : 3;
    obserVar ov = sv.vov.first();
    double ephem;
    QString s;
    double mag;
    QString ephemPath = sv.path + "Libr" + QDir::separator() + "dtime.txt";
    Integrator *integrator = new Integrator(sv.de);
    double stepPrg = sv.force_var[11] == 1 ? From.size() * sv.vbv.size() - 3
                                           : From.size() * sv.vbv.size();
    stepPrg = 100 / stepPrg;
    double value = 0;
    for (int j = 0; j < From.count(); j++) {
        Time t(From[j]);
        QString date = tr("%1.%2.%3").arg(t.getYear()).arg(t.getMonth()).arg(t.getDay());
        QFile f(sv.path + ov.code + QDir::separator() + date + ".txt");
        if (f.exists())
            f.remove();
        if (f.open(QIODevice::WriteOnly)) {
            QString wr = tr("Observatory = %1 %2"
                            "\nMagnitude Limit = %3"
                            "\nRise/Set Elevation = %4"
                            "\nElangation = %5\nDate: %6")
                    .arg(ov.code).arg(ov.name)
                    .arg(sv.magnitude).arg(sv.height).arg(sv.elongation)
                    .arg(date);
            f.write((char*)wr.toUtf8().data());
            f.write("\nName                   hh:mm   alpha         delta        mag    h             As             num   per\n");
            for (int i = sv.force_var[11] * 3; i < sv.vbv.count(); i++) {
                double countHour = 0;
                ElemOrb eo(sv.vbv[i]);
                eo.getX(x0);
                eo.getV(v0);
                Coor coor = Coor(x0, true);
                coor.getX(x0, false);
                coor = Coor(v0, true);
                coor.getX(v0, false);
                memcpy(dx, x0, end*sizeof(double));
                memcpy(dv, v0, end*sizeof(double));
                if (fabs(From[j] - Time(sv.vbv[i].tosc).getJulianDay()) > 1e-13)
                    integrator->rada27(dx, dv, Time(sv.vbv[i].tosc).getJulianDay()
                                       , From[j], sv.force_var, sv.NOR, sv.LL, sv.de);
                for (double jj = From[j]; jj < To[j]; jj += sv.step) {
                    ephem = Dtime(ephemPath, jj).getVar();
                    mag = integrator->magnitude_observ(dx, ephem, sv.de, sv.vbv[i]);
                    if (mag <= sv.magnitude) {
                        if (!sv.magSee.isEmpty())
                            if (mag < sv.magSee.first() || mag > sv.magSee.last())
                                break;
                        memcpy(x, dx, end*sizeof(double));
                        memcpy(v, dv, end*sizeof(double));
                        integrator->ephemerida(x, v, ephem, jj, sv.de, ov, sv.NOR, sv.LL
                                               , sv.force_var);
                        if (jj + sv.step < To[j])
                            integrator->rada27(dx, dv, jj, jj + sv.step, sv.force_var, sv.NOR
                                               , sv.LL, sv.de);
                        Horizontal hor(ov, integrator->getHourAlpha(), integrator->getDegDelta(), jj);
                        if (hor.getDegH() >= sv.height) {
                            t = Time(jj);
                            QString sal = formatStr(integrator->getHourAlpha(), 1);
                            QString sdel = formatStr(integrator->getDegDelta(), 1);
                            QString sh = formatStr(hor.getDegH(), 1);
                            QString saz = formatStr(hor.getDegAz(), 2);
                            QString stime = tr("%1%2:%3%4")
                                    .arg(t.getHour() < 10 ? "0":"").arg(t.getHour())
                                    .arg(t.getMin() < 10 ? "0":"").arg(t.getMin());
                            QString smag = tr("%1%2")
                                    .arg(mag < 10 ? "0":"").arg(mag, 0, 'f', 1);
                            s = sv.vbv[i].name;
                            for (int k = sv.vbv[i].name.size(); k < 23; k++)
                                s.append(" ");
                            s += stime + "  " + sal + "  " + sdel + "  " + smag + "  "
                                    + sh + "  " + saz + "   " + formatNum(sv.vbv[i].numOfObs, 3)
                                    + "  " + formatNum(sv.vbv[i].perOfObs, 4);
                            for (double jjj = jj + sv.step * 2; jjj < To[j]; jjj += sv.step * 2) {
                                ephem = Dtime(ephemPath, jjj).getVar();
                                mag = integrator->magnitude_observ(dx, ephem, sv.de, sv.vbv[i]);
                                if (mag <= sv.magnitude) {
                                    if (!sv.magSee.isEmpty())
                                        if (mag < sv.magSee.first() || mag > sv.magSee.last())
                                            break;
                                    memcpy(x, dx, end*sizeof(double));
                                    memcpy(v, dv, end*sizeof(double));
                                    integrator->ephemerida(x, v, ephem, jjj, sv.de, ov, sv.NOR
                                                           , sv.LL, sv.force_var);
                                    if (jjj + sv.step*2 < To[j])
                                        integrator->rada27(dx, dv, jjj, jjj + sv.step*2, sv.force_var
                                                           , sv.NOR, sv.LL, sv.de);
                                    hor = Horizontal(ov, integrator->getHourAlpha(), integrator->getDegDelta(), jjj);
                                    if (hor.getDegH() < sv.height)
                                        break;
                                    countHour++;
                                }
                            }
                            if (countHour > 1) {
                                s += " " + QString::number(countHour) + "*60min" + "\n";
                                f.write((char*)s.toUtf8().data());
                                break;
                            }
                        }
                    } else
                        integrator->rada27(dx, dv, jj, jj + sv.step, sv.force_var, sv.NOR
                                           , sv.LL, sv.de);
                }
                if (int(value+stepPrg) - int(value) >= 1)
                    emit prg(int(value+stepPrg));
                value += stepPrg;
            }
            f.close();
        }
    }
    delete x0;
    delete dx;
    delete v0;
    delete dv;
    delete integrator;
}

Scout::Scout(setVar &insv)
{
    sv = insv;
    sv.step = 30 / 1440.;
}

void Scout::run()
{
    sv.jdFrom = sv.jdFrom + .5 - sv.vov.first().utc / 24.;
    sv.jdTo = sv.jdTo + .5 - sv.vov.first().utc / 24.;
    Guard *c = new Guard(sv, sv.vov.first());
    c->run();
    From = c->getJdFrom();
    To = c->getJdTo();
    c->deleteLater();
    findObjs();
    emit prg(0);
    emit call();
}

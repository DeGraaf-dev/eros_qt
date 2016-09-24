#include "time.h"

StarTime::StarTime()
{

}

StarTime::StarTime(double jd)
{
    double t0 = floor(jd + .5) - .5;
    double tt = (t0 - 2451545.) / 36525.;
    double t = 50.54841 + 41 * 60 + 6 * 60 * 60 + 8640184.812866*tt
            + .0931048*pow(tt, 2) - .0000062*pow(tt, 3);
    t += 1.0027379093*(jd - t0) * 86400;
    // 86400 - сутки в секундах
    while (t > 86400)
        t -= 86400;
    while (t < 0)
        t += 86400;
    starTime = t / 3600.;
}

StarTime::~StarTime()
{

}

double StarTime::getHourTime()
{
    return starTime;
}


Time::Time()
{

}

Time::Time(double JD)
{
    jd = JD;
    double ch, f4;
    long int a1, a5, b0, c0, d0, e3, z0;
    z0 = int(jd + .5);
    f4 = jd + .5 - z0;
    if (z0 < 2299161)
        a5 = z0;
    else {
        a1 = int((z0 - 1867216.25) / 36524.25);
        a5 = z0 + 1 + a1 - int(a1 / 4.);
    }
    b0 = a5 + 1524;
    c0 = int((b0 - 122.1) / 365.25);
    d0 = int(365.25*c0);
    e3 = int((b0 - d0) / 30.6001);
    ch = b0 - d0 - int(30.6001*e3) + f4;
    day = int(ch);
    if (e3 > 13.5)
        month = e3 - 13;
    else
        month = e3 - 1;
    year = c0 - 4715;
    if (month > 2)
        year--;
    hour = int((ch - day) * 24);
    min = int((((ch - day) * 24) - hour) * 60);
}

Time::Time(QDate d)
{
    double b = 0;
    month = d.month();
    year = d.year();
    day = d.day();
    if (month < 3) {
        month += 12;
        year--;
    }
    if ((year > 1582) || (year == 1582) && (month > 10) || (year == 1582) && (month == 10) && (day >= 15.))
        b = 2 - int(year / 100) + int(int(year / 100) / 4);
    jd = int(365.25*year) + int((30.6001)*(month + 1)) + day + b + 1720994.5;
}

Time::Time(QDate d, int h, int m, double s)
{
    double b = 0;
    month = d.month();
    year = d.year();
    day = d.day();
    if (month < 3) {
        month += 12;
        year--;
    }
    if ((year > 1582) || (year == 1582) && (month > 10) || (year == 1582) && (month == 10) && (day >= 15.))
        b = 2 - int(year / 100) + int(int(year / 100) / 4);
    jd = int(365.25*year) + int((30.6001)*(month + 1)) + day + b
            + 1720994.5 + h / 24. + m / 1440. + s / 86400.;
}

Time::~Time()
{

}

int Time::getYear()
{
    return year;
}

int Time::getMonth()
{
    return month;
}

int Time::getDay()
{
    return day;
}

int Time::getHour()
{
    return hour;
}

int Time::getMin()
{
    return min;
}

double Time::getSec()
{
    return sec;
}

double Time::getJulianDay()
{
    return jd;
}
>>>>>>> origin/master

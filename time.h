#ifndef TIME_H
#define TIME_H

#include <math.h>
#include <QDate>

class StarTime
{
public:
    StarTime();
    StarTime(double jd);
    ~StarTime();
    double getHourTime();
private:
    double starTime;
};

class Time
{
public:
    Time();
    Time(double JD);
    Time(QDate d);
    Time(QDate d, int h, int m, double s);
    ~Time();
    int getYear();
    int getMonth();
    int getDay();
    int getHour();
    int getMin();
    double getSec();
    double getJulianDay();
private:
    double jd;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;
};

#endif // TIME_H

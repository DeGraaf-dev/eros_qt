#ifndef EROSMAIN_H
#define EROSMAIN_H

#include <QMainWindow>

#include <QDebug>
#include <QDir>
#include <QList>
#include <QFile>
#include <QDate>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QThreadPool>

#include "catalog.h"
#include "dereader.h"
#include "calculation.h"
#include "time.h"

namespace Ui {
class ErosMain;
}

class ErosMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit ErosMain(QWidget *parent = 0);
    ~ErosMain();
    bool ErrToGuard;
    QString pathBowell;
    QString path405;
    QString pathObser;
    QString pathDTime;
    QString pathLoadFile;

private slots:
    void on_s_radioAll_clicked(bool checked);
    void on_s_radioNea_clicked(bool checked);
    void on_h_btnFind_clicked();
    void on_s_btnFind_clicked();
    void on_g_btnCalc_clicked();
    void on_n_btnCalc_clicked();
    void on_h_butObjAdd_clicked();
    void on_n_butObjAdd_clicked();
    void on_h_butObsAdd_clicked();
    void on_h_butObjFile_clicked();
    void on_h_butObsFile_clicked();
    void on_n_butObjFile_clicked();
    void on_h_butObjClear_clicked();
    void on_h_butObsClear_clicked();
    void on_n_butObjClear_clicked();
    void on_set_boxObs_currentIndexChanged(const QString &arg1);
    void on_set_boxOrder_currentTextChanged(const QString &arg1);
    void on_set_boxPrecision_currentIndexChanged(const QString &arg1);
    void s_printMes(QString mes);
    void s_nFinish();
    void s_hFinish();
    void s_sFinish();
    void s_gPrg(int value);
    void s_nPrg(int value);
    void s_hPrg(int value);
    void s_sPrg(int value);
    void on_g_clear_clicked();
    void on_set_checkMerc_clicked(bool checked);
    void on_set_checkVenus_clicked(bool checked);
    void on_set_checkEarth_clicked(bool checked);
    void on_set_checkMars_clicked(bool checked);
    void on_set_checkJupe_clicked(bool checked);
    void on_set_checkSaturn_clicked(bool checked);
    void on_set_checkUran_clicked(bool checked);
    void on_set_checkNeptun_clicked(bool checked);
    void on_set_checkMoon_clicked(bool checked);
    void on_set_checkPluto_clicked(bool checked);
    void on_set_checkCpv_clicked(bool checked);
    void on_set_checkPressSun_clicked(bool checked);
    void on_set_checkPressEarth_clicked(bool checked);
    void on_set_checkPressJupe_clicked(bool checked);
    void on_set_checkEffectSun_clicked(bool checked);
    void on_set_arrowUtc_valueChanged(int arg1);
    void on_h_arrowUtc_valueChanged(int arg1);

    void s_releasErr(QString err,int ErrCode);
    void on_n_lineObjNum_editingFinished();

    void on_h_lineObjNum_editingFinished();

    void on_n_lineObjName_editingFinished();

    void on_h_lineObjName_editingFinished();

signals:
    void releasedErr(QString err,int ErrCode);
private:

    Ui::ErosMain *ui;

    bool isUser;
    obserVar ov;
    Bowell *bowell;
    setVar sv;

    void setObserVar(QString s);
    void setGuard(QDate dFrom, QDate dTo);

    void setScout();

    QList<QByteArray> makeSl(QByteArray b);
    QString setUtc(int utc2);
};

#endif // EROSMAIN_H

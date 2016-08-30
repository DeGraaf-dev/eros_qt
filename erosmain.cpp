#include "erosmain.h"
#include "ui_erosmain.h"

ErosMain::ErosMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ErosMain)
{
    ui->setupUi(this);

    isUser = false;
    // load list obser
    sv.path = QCoreApplication::applicationDirPath() + QDir::separator();
    QFile fobs(sv.path + "Libr" + QDir::separator() + "obser.dat");
    if (fobs.open(QIODevice::ReadOnly)) {
        ui->set_boxObs->clear();
        ui->h_boxObs->clear();
        QString s;
        QString obs;
        s = fobs.readLine();
        while (s != "") {
            obs += s.mid(0, 4);
            obs += s.mid(32, s.size() - 32 - 2);
            ui->set_boxObs->addItem(obs);
            ui->h_boxObs->addItem(obs);
            obs.clear();
            s = fobs.readLine();
        }
        fobs.close();
    } else
        qDebug() << "Couldn't open obser.dat";
    /*----------------------------------------------------------------------------*/
    QFile loadFile(sv.path + "settings.json");
    if (loadFile.open(QIODevice::ReadOnly)) {
        QByteArray b = loadFile.readAll();
        QJsonDocument loadDoc = QJsonDocument::fromJson(b);
        QJsonObject json = loadDoc.object();
        /*----------------------------------------------------------------------------*/
        QJsonObject jforce = json["ModelForce"].toObject();
        if (jforce["merc"].toBool())   sv.force_var[0] = 1;
        else                           sv.force_var[0] = 0;
        ui->set_checkMerc->setChecked(jforce["merc"].toBool());
        if (jforce["venus"].toBool())  sv.force_var[1] = 1;
        else                           sv.force_var[1] = 0;
        ui->set_checkVenus->setChecked(jforce["venus"].toBool());
        if (jforce["earth"].toBool())  sv.force_var[2] = 1;
        else                           sv.force_var[2] = 0;
        ui->set_checkEarth->setChecked(jforce["earth"].toBool());
        if (jforce["mars"].toBool())   sv.force_var[3] = 1;
        else                           sv.force_var[3] = 0;
        ui->set_checkMars->setChecked(jforce["mars"].toBool());
        if (jforce["jupe"].toBool())   sv.force_var[4] = 1;
        else                           sv.force_var[4] = 0;
        ui->set_checkJupe->setChecked(jforce["jupe"].toBool());
        if (jforce["saturn"].toBool()) sv.force_var[5] = 1;
        else                           sv.force_var[5] = 0;
        ui->set_checkSaturn->setChecked(jforce["saturn"].toBool());
        if (jforce["uran"].toBool())   sv.force_var[6] = 1;
        else                           sv.force_var[6] = 0;
        ui->set_checkUran->setChecked(jforce["uran"].toBool());
        if (jforce["neptun"].toBool()) sv.force_var[7] = 1;
        else                           sv.force_var[7] = 0;
        ui->set_checkNeptun->setChecked(jforce["neptun"].toBool());
        if (jforce["cpv"].toBool())    sv.force_var[11] = 1;
        else                           sv.force_var[11] = 0;
        ui->set_checkCpv->setChecked(jforce["cpv"].toBool());
        if (jforce["pressEarth"].toBool()) sv.force_var[12] = 1;
        else                               sv.force_var[12] = 0;
        ui->set_checkPressEarth->setChecked(jforce["pressEarth"].toBool());
        if (jforce["pressJupe"].toBool())  sv.force_var[15] = 1;
        else                               sv.force_var[15] = 0;
        ui->set_checkPressJupe->setChecked(jforce["pressJupe"].toBool());
        if (jforce["pressSun"].toBool())   sv.force_var[13] = 1;
        else                               sv.force_var[13] = 0;
        ui->set_checkPressSun->setChecked(jforce["pressSun"].toBool());
        if (jforce["effectSun"].toBool())  sv.force_var[14] = 1;
        else                               sv.force_var[14] = 0;
        ui->set_checkEffectSun->setChecked(jforce["effectSun"].toBool());
        if (jforce["sun"].toBool())        sv.force_var[10] = 1;
        else                               sv.force_var[10] = 0;
        ui->set_checkSun->setChecked(jforce["sun"].toBool());
        if (jforce["moon"].toBool())       sv.force_var[9] = 1;
        else                               sv.force_var[9] = 0;
        ui->set_checkMoon->setChecked(jforce["moon"].toBool());
        if (jforce["pluto"].toBool())      sv.force_var[8] = 1;
        else                               sv.force_var[8] = 0;
        ui->set_checkPluto->setChecked(jforce["pluto"].toBool());
        /*----------------------------------------------------------------------------*/
        QJsonObject jobs = json["Observatory"].toObject();
        if (!jobs["obs"].toString().isEmpty()) {
            on_set_lineUtc_valueChanged(jobs["utc"].toDouble());
            ui->set_boxObs->setCurrentText(jobs["obs"].toString());
            setObserVar(jobs["obs"].toString());
        }
        /*----------------------------------------------------------------------------*/
        QJsonObject jint = json["Integrator"].toObject();
        sv.NOR = jint["order"].toInt();
        sv.LL = jint["precision"].toInt();
        ui->set_boxOrder->setCurrentText(QString::number(sv.NOR));
        on_set_boxOrder_currentTextChanged(QString::number(sv.NOR));
        ui->set_boxPrecision->setCurrentText(QString::number(sv.LL));
        /*----------------------------------------------------------------------------*/
        QJsonObject jguard = json["Guard"].toObject();
        if (jguard["h"].toString() == "0")
            ui->g_radioH0->setChecked(true);
        else if (jguard["h"].toString() == "-6")
            ui->g_radioH6->setChecked(true);
        else if (jguard["h"].toString() == "-12")
            ui->g_radioH12->setChecked(true);
        else if (jguard["h"].toString() == "-18")
            ui->g_radioH18->setChecked(true);
        QDate date;
        QJsonArray jadate = jguard["date"].toArray();
        QJsonObject jfrom = jadate[0].toObject();
        date.setDate(jfrom["year"].toInt(), jfrom["month"].toInt(), jfrom["day"].toInt());
        ui->g_dateFrom->setDate(date);
        ui->s_dateFrom->setDate(date);
        ui->h_dateFrom->setDate(date);
        ui->n_dateFrom->setDate(date);
        QJsonObject jto = jadate[1].toObject();
        date.setDate(jto["year"].toInt(), jto["month"].toInt(), jto["day"].toInt());
        ui->g_dateTo->setDate(date);
        ui->s_dateTo->setDate(date);
        ui->h_dateTo->setDate(date);
        ui->n_dateTo->setDate(date);
        /*----------------------------------------------------------------------------*/
        QJsonObject jscout = json["Scout"].toObject();
        ui->s_radioAll->setChecked(!jscout["nea"].toBool());
        ui->s_radioNea->setChecked(jscout["nea"].toBool());
        if (ui->s_radioNea->isChecked()) {
            ui->s_checkAmur->setChecked(jscout["nea"].toBool());
            ui->s_checkAten->setChecked(jscout["nea"].toBool());
            ui->s_checkApollo->setChecked(jscout["nea"].toBool());
            ui->s_checkAtira->setChecked(jscout["nea"].toBool());
            ui->s_checkAmur->setEnabled(true);
            ui->s_checkAten->setEnabled(true);
            ui->s_checkApollo->setEnabled(true);
            ui->s_checkAtira->setEnabled(true);
        }
        ui->s_lineEl->setText(jscout["el"].toString());
        ui->s_lineHeight->setText(jscout["height"].toString());
        ui->s_lineMag->setText(jscout["mag"].toString());
        /*----------------------------------------------------------------------------*/
        QJsonObject jnum = json["Numerator"].toObject();
        QJsonArray jaobj = jnum["objs"].toArray();
        for (int i = 0; i < jaobj.size(); i++)
            ui->n_listObj->addItem(jaobj[i].toString());
        ui->n_lineStep->setText(jnum["step"].toString());
        if (jnum["centre"] == "geo")
            ui->n_radioGeo->setChecked(true);
        else
            ui->n_radioGeo->setChecked(false);
        if (jnum["centre"] == "top")
            ui->n_radioTop->setChecked(true);
        else
            ui->n_radioTop->setChecked(false);
        /*----------------------------------------------------------------------------*/
        QJsonObject jhunter = json["Hunter"].toObject();
        jaobj = jhunter["obj"].toArray();
        for (int i = 0; i < jaobj.size(); i++)
            ui->h_listObj->addItem(jaobj[i].toString());
        jaobj = jhunter["obs"].toArray();
        for (int i = 0; i < jaobj.size(); i++)
            ui->h_listObs->addItem(jaobj[i].toString());
        ui->h_lineStep->setText(jhunter["step"].toString());
        ui->h_lineHeight->setText(jhunter["height"].toString());
        ui->h_lineMag->setText(jhunter["mag"].toString());

        loadFile.close();
    } else
        qDebug() << "Couldn't open load file";
    isUser = true;
    sv.de = new DEreader(405, sv.path + "Libr" + QDir::separator() + "405");
    bowell = new Bowell(sv.path + "Libr" + QDir::separator() + "astorb.dat");
    QString str = bowell->getName(1);
    str = "";
    ui->g_prgBar->setVisible(false);
    ui->h_prgBar->setVisible(false);
    ui->n_prgBar->setVisible(false);
    ui->s_prgBar->setVisible(false);
}

/*----------------------------------------------------------------------------*/
ErosMain::~ErosMain()
{
    QFile saveFile(sv.path + "settings.json");
    if (saveFile.open(QIODevice::WriteOnly)) {
        QJsonObject json;
        /*----------------------------------------------------------------------------*/
        QJsonObject jdateFrom;
        QDate dateFrom = ui->g_dateFrom->date();
        jdateFrom["year"] = dateFrom.year();
        jdateFrom["month"] = dateFrom.month();
        jdateFrom["day"] = dateFrom.day();
        QDate dateTo = ui->g_dateTo->date();
        QJsonObject jdateTo;
        jdateTo["year"] = dateTo.year();
        jdateTo["month"] = dateTo.month();
        jdateTo["day"] = dateTo.day();
        QJsonArray jadate;
        jadate.append(jdateFrom);
        jadate.append(jdateTo);
        QJsonObject jguard;
        jguard["date"] = jadate;
        if (ui->g_radioH0->isChecked())
            jguard["h"] = "0";
        else if (ui->g_radioH6->isChecked())
            jguard["h"] = "-6";
        else if (ui->g_radioH12->isChecked())
            jguard["h"] = "-12";
        else if (ui->g_radioH18->isChecked())
            jguard["h"] = "-18";

        json["Guard"] = jguard;
        /*----------------------------------------------------------------------------*/
        QJsonObject jscout;
        jscout["el"] = ui->s_lineEl->text();
        jscout["step"] = ui->h_lineStep->text();
        jscout["height"] = ui->s_lineHeight->text();
        jscout["mag"] = ui->s_lineMag->text();
        jscout["amur"] = ui->s_checkAmur->isChecked();
        jscout["apollo"] = ui->s_checkApollo->isChecked();
        jscout["aten"] = ui->s_checkAten->isChecked();
        jscout["atira"] = ui->s_checkAtira->isChecked();
        jscout["nea"] = ui->s_radioNea->isChecked();

        json["Scout"] = jscout;
        /*----------------------------------------------------------------------------*/
        QJsonObject jnum;
        QJsonArray jaobj;
        for (int i = 0; i < ui->n_listObj->count(); i++)
            jaobj.append(ui->n_listObj->item(i)->text());
        jnum["objs"] = jaobj;
        jnum["step"] = ui->n_lineStep->text();
        if (ui->n_radioGeo->isChecked()) jnum["centre"] = "geo";
        else                             jnum["centre"] = "top";

        json["Numerator"] = jnum;
        /*----------------------------------------------------------------------------*/
        QJsonObject jhunter;
        QJsonArray jaob;
        for (int i = 0; i < ui->h_listObj->count(); i++)
            jaob.append(ui->h_listObj->item(i)->text());
        jhunter["obj"] = jaob;
        QJsonArray jaobs;
        for (int i = 0; i < ui->h_listObs->count(); i++)
            jaobs.append(ui->h_listObs->item(i)->text());
        jhunter["obs"] = jaobs;
        jhunter["step"] = ui->h_lineStep->text();
        jhunter["height"] = ui->h_lineHeight->text();
        jhunter["mag"] = ui->h_lineMag->text();

        json["Hunter"] = jhunter;
        /*----------------------------------------------------------------------------*/
        QJsonObject jforce;
        jforce["merc"] = ui->set_checkMerc->isChecked();
        jforce["venus"] = ui->set_checkVenus->isChecked();
        jforce["earth"] = ui->set_checkEarth->isChecked();
        jforce["mars"] = ui->set_checkMars->isChecked();
        jforce["jupe"] = ui->set_checkJupe->isChecked();
        jforce["saturn"] = ui->set_checkSaturn->isChecked();
        jforce["uran"] = ui->set_checkUran->isChecked();
        jforce["neptun"] = ui->set_checkNeptun->isChecked();
        jforce["pluto"] = ui->set_checkPluto->isChecked();
        jforce["moon"] = ui->set_checkMoon->isChecked();
        jforce["sun"] = ui->set_checkSun->isChecked();
        jforce["cpv"] = ui->set_checkCpv->isChecked();
        jforce["pressEarth"] = ui->set_checkPressEarth->isChecked();
        jforce["pressSun"] = ui->set_checkPressSun->isChecked();
        jforce["effectSun"] = ui->set_checkEffectSun->isChecked();
        jforce["pressJupe"] = ui->set_checkPressJupe->isChecked();

        json["ModelForce"] = jforce;
        /*----------------------------------------------------------------------------*/
        QJsonObject jobs;
        if (!ui->set_boxObs->currentText().isEmpty())
            jobs["obs"] = ui->set_boxObs->currentText();
        jobs["utc"] = ui->set_lineUtc->value();

        json["Observatory"] = jobs;
        /*----------------------------------------------------------------------------*/
        QJsonObject jint;
        if (!ui->set_boxOrder->currentText().isEmpty())
            jint["order"] = ui->set_boxOrder->currentText().toInt();
        if (!ui->set_boxPrecision->currentText().isEmpty())
            jint["precision"] = ui->set_boxPrecision->currentText().toInt();

        json["Integrator"] = jint;
        /*----------------------------------------------------------------------------*/
        QJsonDocument saveDoc(json);
        saveFile.write(saveDoc.toJson());
        saveFile.close();
    } else
        qDebug() << "Couldn't open save file";

    delete ui;
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_set_boxOrder_currentTextChanged(const QString &arg1)
{
    sv.NOR = arg1.toInt();
    ui->set_boxPrecision->clear();
    if (arg1 == "11")
        for (int i = 4; i <= 8; i++)
            ui->set_boxPrecision->addItem(QString::number(i));
    else if (arg1 == "15")
        for (int i = 5; i <= 11; i++)
            ui->set_boxPrecision->addItem(QString::number(i));
    else if (arg1 == "19")
        for (int i = 6; i <= 14; i++)
            ui->set_boxPrecision->addItem(QString::number(i));
    else if (arg1 == "23")
        for (int i = 8; i <= 17; i++)
            ui->set_boxPrecision->addItem(QString::number(i));
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_s_radioAll_clicked(bool checked)
{
    ui->s_checkAmur->setChecked(!checked);
    ui->s_checkAten->setChecked(!checked);
    ui->s_checkApollo->setChecked(!checked);
    ui->s_checkAtira->setChecked(!checked);
    ui->s_checkAmur->setEnabled(!checked);
    ui->s_checkAten->setEnabled(!checked);
    ui->s_checkApollo->setEnabled(!checked);
    ui->s_checkAtira->setEnabled(!checked);
}

void ErosMain::on_s_radioNea_clicked(bool checked)
{
    ui->s_checkAmur->setChecked(checked);
    ui->s_checkAten->setChecked(checked);
    ui->s_checkApollo->setChecked(checked);
    ui->s_checkAtira->setChecked(checked);
    ui->s_checkAmur->setEnabled(checked);
    ui->s_checkAten->setEnabled(checked);
    ui->s_checkApollo->setEnabled(checked);
    ui->s_checkAtira->setEnabled(checked);
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_g_btnCalc_clicked()
{
    ui->g_prgBar->setVisible(true);
    setGuard(ui->g_dateFrom->date(), ui->g_dateTo->date());
    Guard *gCalc = new Guard(sv, ov);
    QThreadPool::globalInstance()->start(gCalc);
    connect(gCalc, SIGNAL(finished()),          this, SLOT(deleteLater()));
    connect(gCalc, SIGNAL(printedMes(QString)), this, SLOT(s_printMes(QString)));
    connect(gCalc, SIGNAL(prg(int)), this, SLOT(s_gPrg(int)));
}
/*----------------------------------------------------------------------------*/
void ErosMain::setGuard(QDate dFrom, QDate dTo)
{
    if (ui->g_radioH0->isChecked())       sv.border = 0;
    else if (ui->g_radioH6->isChecked())  sv.border = -6;
    else if (ui->g_radioH12->isChecked()) sv.border = -12;
    else sv.border = -18;
    sv.jdFrom = Time(dFrom).getJulianDay();
    sv.jdTo = Time(dTo).getJulianDay();
    sv.gstep = ui->g_lineStep->text().toDouble();
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_n_btnCalc_clicked()
{
    if (!ui->n_listObj->count())
        return;
    ui->n_prgBar->setVisible(true);
    ui->n_btnCalc->setEnabled(false);
    sv.vbv.clear();
    if (sv.force_var[11])
        for (int i = 1; i <= 3; i++)
            sv.vbv << bowell->getVar(i);
    for (int i = 0; i < ui->n_listObj->count(); i++)
        sv.vbv << bowell->getVar(ui->n_listObj->item(i)->text());
    sv.vov.clear();
    sv.vov << ov;
    setGuard(ui->n_dateFrom->date(), ui->n_dateTo->date());
    sv.step = ui->n_lineStep->text().toDouble();

    Calculation *nCalc = new Numerator(sv);
    QThreadPool::globalInstance()->start(nCalc);
    connect(nCalc, SIGNAL(call()), this, SLOT(s_nFinish()));
    connect(nCalc, SIGNAL(prg(int)), this, SLOT(s_nPrg(int)));
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_h_btnFind_clicked()
{
    if (!ui->h_listObj->count())
        return;
    if (!ui->h_listObs->count())
        return;
    ui->h_prgBar->setVisible(true);
    ui->h_btnFind->setEnabled(false);
    sv.vbv.clear();
    if (sv.force_var[11])
        for (int i = 1; i <= 3; i++)
            sv.vbv << bowell->getVar(i);
    for (int i = 0; i < ui->h_listObj->count(); i++)
        sv.vbv << bowell->getVar(ui->h_listObj->item(i)->text());
    QString opath = sv.path + "Libr" + QDir::separator() + "obser.dat";
    sv.vov.clear();
    for (int i = 0; i < ui->h_listObs->count(); i++) {
        QString s = ui->h_listObs->item(i)->text();
        int idx = s.indexOf(" ");
        sv.vov << Obser(opath, s.mid(idx + 1, 3)).getVar(s.mid(0, idx).toInt());
    }
    setGuard(ui->h_dateFrom->date(), ui->h_dateTo->date());
    sv.magnitude = ui->h_lineMag->text().toInt();
    sv.height = ui->h_lineHeight->text().toInt();
    sv.step = ui->h_lineStep->text().toDouble();

    Calculation *hCalc = new Hunter(sv);
    QThreadPool::globalInstance()->start(hCalc);
    connect(hCalc, SIGNAL(call()), this, SLOT(s_hFinish()));
    connect(hCalc, SIGNAL(prg(int)), this, SLOT(s_hPrg(int)));
}

void ErosMain::setScout()
{
    sv.magSee.clear();
    if (!ui->s_lineMinSeeMag->text().isEmpty()) {
        sv.magSee.append(ui->s_lineMinSeeMag->text().toDouble());
        sv.magSee.append(ui->s_lineMaxSeeMag->text().toDouble());
    }
    int max = bowell->getMaxNum();
    for (int i = 0; i < max; i++) {
        sv.vbv << bowell->getVar(i+1);
        int del = 0;
        if (!ui->s_lineMaxA->text().isEmpty())
            if (sv.vbv.last().a > ui->s_lineMaxA->text().toInt()
                    || sv.vbv.last().a < ui->s_lineMinA->text().toInt())
                del++;
        if (!ui->s_lineMaxE->text().isEmpty())
            if (sv.vbv.last().e > ui->s_lineMaxE->text().toInt()
                    || sv.vbv.last().e < ui->s_lineMinE->text().toInt())
                del++;
        if (!ui->s_lineMaxI->text().isEmpty())
            if (sv.vbv.last().i > ui->s_lineMaxI->text().toInt()
                    || sv.vbv.last().i < ui->s_lineMinI->text().toInt())
                del++;
        if (!ui->s_lineMaxW->text().isEmpty())
            if (sv.vbv.last().arg > ui->s_lineMaxW->text().toInt()
                    || sv.vbv.last().arg < ui->s_lineMinW->text().toInt())
                del++;
        if (!ui->s_lineMaxM->text().isEmpty())
            if (sv.vbv.last().anomaly > ui->s_lineMaxM->text().toInt()
                    || sv.vbv.last().anomaly < ui->s_lineMinM->text().toInt())
                del++;
        if (!ui->s_lineMaxMag->text().isEmpty())
            if (sv.vbv.last().mag > ui->s_lineMaxMag->text().toInt()
                    || sv.vbv.last().mag < ui->s_lineMinMag->text().toInt())
                del++;
        if (!ui->s_lineMaxKnot->text().isEmpty())
            if (sv.vbv.last().knot > ui->s_lineMaxKnot->text().toInt()
                    || sv.vbv.last().knot < ui->s_lineMinKnot->text().toInt())
                del++;
        if (!ui->s_lineMaxObsNum->text().isEmpty())
            if (sv.vbv.last().numOfObs > ui->s_lineMaxObsNum->text().toInt()
                    || sv.vbv.last().numOfObs < ui->s_lineMinObsNum->text().toInt())
                del++;
        if (!ui->s_lineMaxObsPeriod->text().isEmpty())
            if (sv.vbv.last().perOfObs > ui->s_lineMaxObsPeriod->text().toInt()
                    || sv.vbv.last().perOfObs < ui->s_lineMinObsPeriod->text().toInt())
                del++;
        double per = (1 - sv.vbv.last().e) * sv.vbv.last().a;
        double aph = (1 + sv.vbv.last().e) * sv.vbv.last().a;
        if (!ui->s_lineMaxAph->text().isEmpty())
            if (aph > ui->s_lineMaxAph->text().toInt()
                    || aph < ui->s_lineMinAph->text().toInt())
                del++;
        if (!ui->s_lineMaxPer->text().isEmpty())
            if (per > ui->s_lineMaxPer->text().toInt()
                    || per < ui->s_lineMinPer->text().toInt())
                del++;
        int nea = 0;
        if (ui->s_checkAmur->isChecked())
            if (per <= 1.3 && per >= 1.01)
                nea++;
        if (ui->s_checkApollo->isChecked())
            if (per <= 1.017 && per >= 1)
                nea++;
        if (ui->s_checkAten->isChecked())
            if (per <= 1 && per >= .983)
                nea++;
        if (ui->s_checkAtira->isChecked())
            if (per <= .983)
                nea++;
        if (ui->s_radioNea->isChecked())
            if (per > 1.3)
                del++;
        if (!nea || del)
            sv.vbv.removeLast();
    }
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_s_btnFind_clicked()
{
    ui->s_prgBar->setVisible(true);
    ui->s_btnFind->setEnabled(false);
    sv.vbv.clear();
    if (sv.force_var[11])
        for (int i = 1; i <= 3; i++)
            sv.vbv << bowell->getVar(i);
    setScout();
    setGuard(ui->s_dateFrom->date(), ui->s_dateTo->date());
    sv.magnitude = ui->s_lineMag->text().toInt();
    sv.height = ui->s_lineHeight->text().toInt();
    sv.elongation = ui->s_lineEl->text().toInt();
    sv.step = 30;
    sv.vov.clear();
    sv.vov << ov;
    Calculation *sCalc = new Scout(sv);
    QThreadPool::globalInstance()->start(sCalc);
    connect(sCalc, SIGNAL(call()), this, SLOT(s_sFinish()));
    connect(sCalc, SIGNAL(prg(int)), this, SLOT(s_sPrg(int)));
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_h_butObjAdd_clicked()
{
    if (ui->h_lineObjName->text().isEmpty())
        ui->h_listObj->addItem(bowell->getName(ui->h_lineObjNum->text().toInt()));
    else if (!ui->h_lineObjNum->text().isEmpty())
        ui->h_listObj->addItem(ui->h_lineObjName->text());
}
/*----------------------------------------------------------------------------*/
void ErosMain::on_h_butObsAdd_clicked()
{
    if (!ui->h_lineUtc->text().isEmpty())
        ui->h_listObs->addItem(ui->h_lineUtc->text() + " " + ui->h_boxObs->currentText());
}
/*----------------------------------------------------------------------------*/
void ErosMain::on_n_butObjAdd_clicked()
{
    if (!ui->n_lineObjNum->text().isEmpty()) {
        ui->n_listObj->addItem(bowell->getName(ui->n_lineObjNum->text().toInt()));
		return;
    }
	if (!ui->n_lineObjName->text().isEmpty())
        ui->n_listObj->addItem(ui->n_lineObjName->text());
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_h_butObjFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(NULL, tr("Выберите файл с именами астероидов"),
                                                    "./", tr("Текстовый документ (*.txt)"));
    if (path.isEmpty())
        return;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return;
    QList<QByteArray> sl = makeSl(f.readAll());
    for (int i = 0; i < sl.size(); i++)
        ui->h_listObj->addItem(sl[i]);
    f.close();
}

QList<QByteArray> ErosMain::makeSl(QByteArray b)
{
    QList<QByteArray> sl = b.split('\n');
    for (int i = 0; i < sl.size(); i++) {
        if (sl[i].at(sl[i].size() - 1) == '\r')
            sl[i].remove(sl[i].size() - 1, 1);
        sl[i] = sl[i].trimmed();
    }
    return sl;
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_h_butObsFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(NULL, tr("Выберите файл с обсерваториями"),
                                                    "./", tr("Текстовый документ (*.txt)"));
    if (path.isEmpty())
        return;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return;
    QList<QByteArray> sl = makeSl(f.readAll());
    for (int i = 0; i < sl.size(); i++)
        ui->h_listObs->addItem(sl[i]);
    f.close();
}
/*----------------------------------------------------------------------------*/
void ErosMain::on_n_butObjFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(NULL, tr("Выберите файл с именами астероидов"),
                                                    "./", tr("Текстовый документ (*.txt)"));
    if (path.isEmpty())
        return;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return;
    QList<QByteArray> sl = makeSl(f.readAll());
    for (int i = 0; i < sl.size(); i++)
        ui->n_listObj->addItem(sl[i]);
    f.close();
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_h_butObjClear_clicked()
{
    ui->h_listObj->clear();
}
/*----------------------------------------------------------------------------*/
void ErosMain::on_h_butObsClear_clicked()
{
    ui->h_listObs->clear();
}
/*----------------------------------------------------------------------------*/
void ErosMain::on_n_butObjClear_clicked()
{
    ui->n_listObj->clear();
}

/*----------------------------------------------------------------------------*/
void ErosMain::on_set_boxObs_currentIndexChanged(const QString &arg1)
{
    if (isUser)
        setObserVar(arg1);
}

/*----------------------------------------------------------------------------*/
void ErosMain::setObserVar(QString s)
{
    QString path = sv.path + "Libr" + QDir::separator() + "obser.dat";
    ov = Obser(path, s.mid(0, 3)).getVar(ui->set_lineUtc->value());
    if (!QDir().exists(QCoreApplication::applicationDirPath() + QDir::separator() + s.mid(0, 3)))
        QDir().mkdir(QCoreApplication::applicationDirPath() + QDir::separator() + s.mid(0, 3));
}

/*----------------------------------------------------------------------------*/

void ErosMain::on_set_boxPrecision_currentIndexChanged(const QString &arg1)
{
    if (isUser)
        sv.LL = arg1.toInt();
}

void ErosMain::s_printMes(QString mes)
{
    ui->g_textMes->insertPlainText(mes);
    ui->g_btnCalc->setEnabled(true);
    ui->g_prgBar->setVisible(false);
}

void ErosMain::s_nFinish()
{
    ui->n_btnCalc->setEnabled(true);
    ui->n_prgBar->setVisible(false);
}

void ErosMain::s_hFinish()
{
    ui->h_btnFind->setEnabled(true);
    ui->h_prgBar->setVisible(false);
}

void ErosMain::s_sFinish()
{
    ui->s_btnFind->setEnabled(true);
    ui->s_prgBar->setVisible(false);
}

void ErosMain::s_gPrg(int value)
{
    ui->g_prgBar->setValue(value);
}

void ErosMain::s_nPrg(int value)
{
    ui->n_prgBar->setValue(value);
}

void ErosMain::s_hPrg(int value)
{
    ui->h_prgBar->setValue(value);
}

void ErosMain::s_sPrg(int value)
{
    ui->s_prgBar->setValue(value);
}

void ErosMain::on_g_clear_clicked()
{
    ui->g_textMes->clear();
}

void ErosMain::on_set_checkMerc_clicked(bool checked)
{
    if (checked)
        sv.force_var[0] = 1;
    else
        sv.force_var[0] = 0;
}

void ErosMain::on_set_checkVenus_clicked(bool checked)
{
    if (checked)
        sv.force_var[1] = 1;
    else
        sv.force_var[1] = 0;
}

void ErosMain::on_set_checkEarth_clicked(bool checked)
{
    if (checked)
        sv.force_var[2] = 1;
    else
        sv.force_var[2] = 0;
}

void ErosMain::on_set_checkMars_clicked(bool checked)
{
    if (checked)
        sv.force_var[3] = 1;
    else
        sv.force_var[3] = 0;
}

void ErosMain::on_set_checkJupe_clicked(bool checked)
{
    if (checked)
        sv.force_var[4] = 1;
    else
        sv.force_var[4] = 0;
}

void ErosMain::on_set_checkSaturn_clicked(bool checked)
{
    if (checked)
        sv.force_var[5] = 1;
    else
        sv.force_var[5] = 0;
}

void ErosMain::on_set_checkUran_clicked(bool checked)
{
    if (checked)
        sv.force_var[6] = 1;
    else
        sv.force_var[6] = 0;
}

void ErosMain::on_set_checkNeptun_clicked(bool checked)
{
    if (checked)
        sv.force_var[7] = 1;
    else
        sv.force_var[7] = 0;
}

void ErosMain::on_set_checkSun_clicked(bool checked)
{
    if (checked)
        sv.force_var[14] = 1;
    else
        sv.force_var[14] = 0;
}

void ErosMain::on_set_checkMoon_clicked(bool checked)
{
    if (checked)
        sv.force_var[9] = 1;
    else
        sv.force_var[9] = 0;
}

void ErosMain::on_set_checkPluto_clicked(bool checked)
{
    if (checked)
        sv.force_var[8] = 1;
    else
        sv.force_var[8] = 0;
}

void ErosMain::on_set_checkCpv_clicked(bool checked)
{
    if (checked)
        sv.force_var[11] = 1;
    else
        sv.force_var[11] = 0;
}

void ErosMain::on_set_checkPressSun_clicked(bool checked)
{
    if (checked)
        sv.force_var[13] = 1;
    else
        sv.force_var[13] = 0;
}

void ErosMain::on_set_checkPressEarth_clicked(bool checked)
{
    if (checked)
        sv.force_var[12] = 1;
    else
        sv.force_var[12] = 0;
}

void ErosMain::on_set_checkPressJupe_clicked(bool checked)
{
    if (checked)
        sv.force_var[15] = 1;
    else
        sv.force_var[15] = 0;
}

void ErosMain::on_set_checkEffectSun_clicked(bool checked)
{
    if (checked)
        sv.force_var[14] = 1;
    else
        sv.force_var[14] = 0;
}

void ErosMain::on_set_lineUtc_valueChanged(double arg1)
{
    ov.utc = arg1;
    ui->set_lineUtc->prefix().clear();
    if (arg1 <= 0)
        ui->set_lineUtc->setPrefix("UTC  ");
    else if (arg1 > 0)
        ui->set_lineUtc->setPrefix("UTC +");

    ui->set_lineUtc->suffix().clear();
    if (arg1-int(arg1))
        ui->set_lineUtc->setSuffix(":30");
    else
        ui->set_lineUtc->setSuffix(":00");
}

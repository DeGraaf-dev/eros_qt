#include "convertcoor.h"

Spherical::Spherical(Coor &cx, bool ecl)
{
    double x[3];
    cx.getX(x, ecl);
    rad = 0;
    for (int i = 0; i < 3; i++)
        rad += pow(x[i],2);
    rad = sqrt(rad);
    delta = asin(x[2] / rad);
    alpha = atan(x[1] / x[0]);
    if (sin(alpha)*x[1] < 0)
        alpha += M_PI;
}

double Spherical::getRad()
{
    return rad;
}

double Spherical::getDegDelta()
{
    return delta;
}

double Spherical::getRadAlpha()
{
    return alpha;
}

void Coor::equ2ecl(double c[], double equ[])
{
    double eps = (23 * 60 * 60 + 26 * 60 + 21.448)*M_PI / 648000;
    c[0] = equ[0];
    c[1] = equ[1] * cos(eps) + equ[2] * sin(eps);
    c[2] = equ[2] * cos(eps) - equ[1] * sin(eps);
}

void Coor::ecl2equ(double c[], double ecl[])
{
    double eps = (23 * 60 * 60 + 26 * 60 + 21.448)*M_PI / 648000;
    c[0] = ecl[0];
    c[1] = ecl[1] * cos(eps) - ecl[2] * sin(eps);
    c[2] = ecl[1] * sin(eps) + ecl[2] * cos(eps);
}

Coor::Coor(double ix[], bool ecl)
{
    if (ecl)
        memcpy(x, ix, 3*sizeof(double));
    else
        equ2ecl(x, ix);
}

Coor::Coor(double ix[], double plc[], bool ecl)
{
    double ox[3];
    memcpy(ox, ix, 3*sizeof(double));
    for (int i = 0; i < 3; i++)
        ox[i] -= plc[i];
    if (ecl)
        memcpy(x, ox, 3*sizeof(double));
    else
        equ2ecl(x, ox);
}

void Coor::getX(double cx[], bool ecl)
{
    if (ecl)
        memcpy(cx, x, 3*sizeof(double));
    else
        ecl2equ(cx, x);
}

Horizontal::Horizontal(obserVar &ov, double jdate, Coor &coor, bool ecl)
{
    double t = StarTime(jdate).getHourTime();
    t = t * 15 + ov.longitude;
    t = t * M_PI / 180;
    t -= Spherical(coor, ecl).getRadAlpha();
    double rad_fi = atan(ov.sinLatitude / ov.cosLatitude);
    if (rad_fi > M_PI / 2)
        rad_fi -= M_PI;
    else if (rad_fi < -M_PI / 2)
        rad_fi += M_PI;
    double del = Spherical(coor, ecl).getDegDelta();
    double rad_z = acos(sin(rad_fi)*sin(del) + cos(rad_fi)*cos(del)*cos(t));
    double arc_a = atan(cos(del)*sin(t) / (-cos(rad_fi)*sin(del) + sin(rad_fi)*cos(del)*cos(t)));
    double cos_a = -cos(rad_fi)*sin(del) + sin(rad_fi)*cos(del)*cos(t);
    if (cos_a < 0)
        arc_a += M_PI;
    if (rad_z > M_PI)
        rad_z = 2 * M_PI - rad_z;
    else if (rad_z < 0)
        rad_z = fabs(rad_z);
    degH = 90 - rad_z * 180 / M_PI;
    degAz = arc_a * 180 / M_PI;
    if (degAz < 0)
        degAz += 360;
    if (degAz > 360)
        degAz -= 360;
}

Horizontal::Horizontal(obserVar &ov, double hourAlpha, double degDelta, double jdate)
{
    double t = StarTime(jdate).getHourTime() - hourAlpha;
    t = t * 15 + ov.longitude;
    t = t * M_PI / 180;
    double rad_fi = atan(ov.sinLatitude / ov.cosLatitude);
    if (rad_fi > M_PI / 2)
        rad_fi -= M_PI;
    else if (rad_fi < -M_PI / 2)
        rad_fi += M_PI;
    double del = degDelta * M_PI / 180.;
    double rad_z = acos(sin(rad_fi)*sin(del) + cos(rad_fi)*cos(del)*cos(t));
    double arc_a = atan(cos(del)*sin(t) / (-cos(rad_fi)*sin(del) + sin(rad_fi)*cos(del)*cos(t)));
    double cos_a = -cos(rad_fi)*sin(del) + sin(rad_fi)*cos(del)*cos(t);
    if (cos_a < 0)
        arc_a += M_PI;
    if (rad_z > M_PI)
        rad_z = 2 * M_PI - rad_z;
    else if (rad_z < 0)
        rad_z = fabs(rad_z);
    degH = 90 - rad_z * 180 / M_PI;
    degAz = arc_a * 180 / M_PI;
    if (degAz < 0)
        degAz += 360;
    if (degAz > 360)
        degAz -= 360;
}

double Horizontal::getDegH()
{
    return degH;
}

double Horizontal::getDegAz()
{
    return degAz;
}

ElemOrb::ElemOrb(bowellVar &bv)
{
    double anomaly = bv.anomaly * M_PI / 180.;
    double e1 = 0;
    double e = anomaly;
    while (fabs(e - e1) > 1e-14) {
        e1 = e;
        e = anomaly + bv.e*sin(e1);
    }
    double sv = sqrt(1 - pow(bv.e, 2))*sin(e) / (1 - bv.e*cos(e));
    double cv = (cos(e) - bv.e) / (1 - bv.e*cos(e));
    double arg = bv.arg / 180.* M_PI;
    double su = sv*cos(arg) + cv*sin(arg);
    double cu = cv*cos(arg) - sv*sin(arg);
    double r = bv.a*(1 - bv.e*cos(e));
    double knot = bv.knot / 180.* M_PI;
    double i = bv.i / 180.* M_PI;
    x[0] = r*(cu*cos(knot) - su*sin(knot)*cos(i));
    x[1] = r*(cu*sin(knot) + su*cos(knot)*cos(i));
    x[2] = r*su*sin(i);
    double p = bv.a*(1 - pow(bv.e, 2));
    double mu = 0.0002959122082855911025;
    double VR = sqrt(mu / p)*bv.e*sv;
    double VN = sqrt(mu / p)*(1 + bv.e*cv);
    v[0] = (x[0] / r)*VR + (-su*cos(knot) - cu*sin(knot)*cos(i))*VN;
    v[1] = (x[1] / r)*VR + (-su*sin(knot) + cu*cos(knot)*cos(i))*VN;
    v[2] = (x[2] / r)*VR + cu*sin(i)*VN;
}

void ElemOrb::getX(double cx[])
{
    memcpy(cx, x, 3*sizeof(double));
}

void ElemOrb::getV(double cv[])
{
    memcpy(cv, v, 3*sizeof(double));
}

// правые части
Integrator::Integrator(DEreader* de)
{
    for (int i = 0; i < 9; i++)
        massObj[i] = de->deConst.massPlanet[i];
    massObj[9] = de->deConst.massMoon;
    massObj[10] = 1; // sun
    for (int i = 0; i < 3; i++)
        massObj[i+11] =de->deConst.massAst[i];
}

void Integrator::force(double X[], double V[], double TS, double *F, DEreader *de
                       , int force_var[])
{
    QVector<double> rAst;
    for (int i = 0; i < 1 + force_var[11] * 3; i++) {
        rAst.append(0);
        for (int j = 0; j < 3; j++)
            rAst[i] += pow(X[i * 3 + j], 2);
        rAst[i] = sqrt(rAst[i]);
    }
    double coorPlanet[10][3];
    for (int j = 0; j < 10; j++)
        if (force_var[j])
            de->GetPlanetPoz(TS, j, true, coorPlanet[j]);
        else
            for (int i = 0; i < 3; i++)
                coorPlanet[j][i] = 0;
    double rPlanet[10];
    for (int i = 0; i < 10; i++) {
        rPlanet[i] = 0;
        for (int j = 0; j < 3; j++)
            rPlanet[i] += pow(coorPlanet[i][j], 2);
        rPlanet[i] = sqrt(rPlanet[i]);
    }
    double distAster[4][4];
    if (rAst.size() != 1)
        for (int j = 0; j < rAst.size(); j++)
            for (int i = 0; i < rAst.size(); i++) {
                distAster[j][i] = 0;
                for (int k = 0; k < 3; k++)
                    distAster[j][i] += pow((X[j * 3 + k] - X[i * 3 + k]), 2);
                distAster[j][i] = sqrt(distAster[j][i]);
            }
    double rPlanet2Aster[4][10];
    for (int j = 0; j < rAst.size(); j++)
        for (int i = 0; i < 10; i++) {
            rPlanet2Aster[j][i] = 0;
            for (int k = 0; k < 3; k++)
                rPlanet2Aster[j][i] += pow((coorPlanet[i][k] - X[3 * j + k]), 2);
            rPlanet2Aster[j][i] = sqrt(rPlanet2Aster[j][i]);
        }
    for (int k = 0; k < rAst.size(); k++) {
        for (int i = 0; i < 3; i++) {
            F[k * 3 + i] = -de->deConst.G*X[i + k * 3] / pow(rAst[k], 3);
            for (int j = 0; j < 10; j++) {
                if (force_var[j]) {
                    F[k * 3 + i] += (coorPlanet[j][i] - X[k * 3 + i])
                            * massObj[j] / pow(rPlanet2Aster[k][j], 3);
                    F[k * 3 + i] -= coorPlanet[j][i] * massObj[j] / pow(rPlanet[j], 3);
                }
            }
            if (force_var[11])
                for (int j = 1; j < rAst.size(); j++)
                    if (j != k)	{
                        F[k * 3 + i] += (X[j * 3 + i] - X[k * 3 + i])
                                * massObj[10 + j] / pow(distAster[k][j], 3);
                        F[k * 3 + i] -= (X[j * 3 + i] * massObj[10 + j] / pow(rAst[j], 3));
                    }
        }
    }
    if (force_var[12])
        force12(X, massObj[2], coorPlanet[2], rPlanet2Aster[0][2], de, F);
    if (force_var[13])
        force13(X, TS, rAst[0], de, F);
    if (force_var[14])
        force14(X, V, rAst[0], de, F);
    if (force_var[15])
        force15(X, massObj[4], coorPlanet[4], rPlanet2Aster[0][4], TS, de, F);
}

int Integrator::radamaker(double X[], double V[], double TF, DEreader *de, int force_var[])
{
    int j, k, l, m, n, j2, la, jdm;
    int NI = 2;
    double XL = 1.0e-9;
    int N_CLASS = 2;
    int NV = 3 + 9 * force_var[11];
    double *be_g_ptr, *be_ptr, *be_i_ptr, *b_g_ptr, *b_ptr, *b_i_ptr, *bt_g_ptr, *bt_ptr, *w_ptr, *u_ptr, *r_ptr, *c_ptr;
    double s, q, res, hsum, temp, val, bdouble, t, tval, t2, eps = 1.0e-10L;

    while (1){
        for (k = 0, be_g_ptr = v.BE + NV*(v.KE - 1), b_g_ptr = v.B + NV*(v.KE - 1); k < NV; k++, be_g_ptr++, b_g_ptr++){
            *be_g_ptr = *b_g_ptr / v.W[v.KE - 1];
            for (j = 0, be_ptr = v.BE + k, b_ptr = v.B + k, w_ptr = v.W; j < v.KD; j++, be_ptr += NV, b_ptr += NV, w_ptr++)	{
                *be_ptr = *b_ptr / (*w_ptr);
                for (l = j + 1, b_i_ptr = b_ptr + NV; l < v.KE; l++, b_i_ptr += NV)	{
                    n = v.NW[l] + j; *be_ptr += *b_i_ptr * v.D[n];
                }
            }
        }

        t = v.TP; tval = fabs(t); t2 = pow(t, (double)N_CLASS);

        for (m = 0; m < v.NL; m++){
            for (j = 1, j2 = 1; j < v.KF; j++){
                la = v.NW[j - 1] - 1;
                jdm = j - 1;
                s = v.H[j];
                q = pow(s, (double)(N_CLASS - 1));
                if (v.NPQ){
                    for (k = 0, b_g_ptr = v.B + (v.KE - 1)*NV; k < NV; k++, b_g_ptr++){
                        res = *b_g_ptr;
                        for (l = 0, b_ptr = b_g_ptr - NV; l < v.KD; l++, b_ptr -= NV){
                            res = *b_ptr + s*res;
                        }
                        v.Y[k] = X[k] + q*(t*V[k] + t2*s*(v.F1[k] * v.W1 + s*res));
                    }
                }
                else{
                    for (k = 0, b_g_ptr = v.B + (v.KE - 1)*NV; k < NV; k++, b_g_ptr++){
                        res = *b_g_ptr;
                        temp = res * v.U[v.KE - 1];
                        for (l = 0, b_ptr = b_g_ptr - NV, u_ptr = v.U + v.KE - 2; l < v.KD; l++, b_ptr -= NV, u_ptr--){
                            res = *b_ptr + s*res;
                            temp = *b_ptr * (*u_ptr) + s*temp;
                        }
                        v.Y[k] = X[k] + q*(t*V[k] + t2*s*(v.F1[k] * v.W1 + s*res));
                        v.Z[k] = V[k] + s*t*(v.F1[k] + s*temp);
                    }
                }
                force(v.Y, v.Z, v.TM + s*t, v.FJ, de, force_var/*, NOR, LL*/); v.NF++;
                if (j2){
                    j2 = 0;
                    for (k = 0, be_ptr = v.BE, b_ptr = v.B; k < NV; k++, be_ptr++, b_ptr++){
                        temp = *be_ptr;
                        res = (v.FJ[k] - v.F1[k]) / s;
                        *be_ptr = res;
                        *b_ptr += (res - temp) * v.W[0];
                    }
                }
                else{
                    for (k = 0, be_ptr = v.BE + (j - 1)*NV, b_ptr = v.B + (j - 1)*NV; k < NV; k++, be_ptr++, b_ptr++){
                        temp = *be_ptr;
                        res = (v.FJ[k] - v.F1[k]) / s;
                        for (l = 0, r_ptr = v.R + la + 1, be_i_ptr = v.BE + k; l < jdm; l++, r_ptr++, be_i_ptr += NV){
                            res = (res - *be_i_ptr)*(*r_ptr);
                        }
                        *be_ptr = res; temp = res - temp; *b_ptr += temp * v.W[j - 1];
                        for (l = 0, c_ptr = v.C + la + 1, b_i_ptr = v.B + k; l < jdm; l++, c_ptr++, b_i_ptr += NV){
                            *b_i_ptr += *c_ptr * temp;
                        }
                    }
                }
            }

            if (m < NI - 1) continue;

            hsum = 0.0L;
            val = pow(tval, (double)(-v.KE));
            for (k = 0, b_ptr = v.B + (v.KE - 1)*NV; k < NV; k++, b_ptr++){
                bdouble = *b_ptr;
                hsum += bdouble*bdouble;
            }
            hsum = val*sqrt(hsum);
            if (v.NSF) continue; if (fabs(hsum - v.SM) <= 0.01L * hsum) break;
            v.SM = hsum;
        }
        if (v.NSF == 0){
            if (hsum != 0.0L) v.TP = pow(v.SS / hsum, v.PW) * v.DIR;

            if (v.NES)
                v.TP = XL;
            else{
                if (v.TP / t <= 1.0L){
                    v.TP *= 0.8L; v.NCOUNT++;
                    if (v.NCOUNT > 10) return 0; else return 1;
                }
            }
            v.NSF = 1;
        }

        for (k = 0, b_g_ptr = v.B + (v.KE - 1)*NV; k < NV; k++, b_g_ptr++){
            res = *b_g_ptr;
            for (l = 0, b_ptr = v.B + k; l < v.KD; l++, b_ptr += NV){
                res += *b_ptr;
            }
            X[k] += V[k] * t + t2*(v.F1[k] * v.W1 + res);
            if (v.NCL) continue;
            res = *b_g_ptr * v.U[v.KE - 1];
            for (l = 0, b_ptr = v.B + k; l<v.KD; l++, b_ptr += NV){
                res += *b_ptr * v.U[l];
            }
            V[k] += t*(v.F1[k] + res);
        }

        v.TM += t;
        v.NS++;
        if (v.NPER) return 0;

        force(X, V, v.TM, v.F1, de, force_var/*, NOR, LL*/); v.NF++;
        if (v.NES)
            v.TP = XL;
        else{
            if (hsum != 0.0L) v.TP = pow(v.SS / hsum, v.PW) * v.DIR;
            if (v.TP / t > v.SR) v.TP = v.SR*t;
        }
        if (v.DIR*(v.TM + v.TP) >= v.DIR*TF - eps){
            v.TP = TF - v.TM; v.NPER = 1;
        }
        q = v.TP / t;
        for (k = 0, b_g_ptr = v.B, bt_g_ptr = v.BT; k < NV; k++, b_g_ptr++, bt_g_ptr++){
            res = 1.0L;
            for (j = 0, b_ptr = b_g_ptr, bt_ptr = bt_g_ptr; j < v.KE; j++, b_ptr += NV, bt_ptr += NV){
                if (v.NS>1) *bt_ptr = *b_ptr - *bt_ptr;
                if (j < v.KE - 1){
                    m = v.MC[j] - 1;
                    for (l = j + 1, b_i_ptr = b_ptr + NV; l < v.KE; l++, b_i_ptr += NV, m++){
                        *b_ptr += v.XI[m] * (*b_i_ptr);
                    }
                }
                res *= q;
                temp = res * (*b_ptr);
                *b_ptr = temp + (*bt_ptr);
                *bt_ptr = temp;
            }
        }
        v.NL = NI;
    }
}

// интегратор
void Integrator::rada27(double X[], double V[], double TI, double TF, int force_var[]
                        , int NOR, int LL, DEreader *de)
{
    double HH[48] = {
        0.212340538239152E+00, 0.590533135559265E+00, 0.911412040487296E+00,
        0.0985350857988264E+00, 0.3045357266463639E+00, 0.5620251897526139E+00,
        0.8019865821263918E+00, 0.9601901429485313E+00, 0.0562625605369221E+00,
        0.1802406917368924E+00, 0.3526247171131696E+00, 0.5471536263305554E+00,
        0.7342101772154105E+00, 0.8853209468390957E+00, 0.9775206135612875E+00,
        0.0362578128832095E+00, 0.1180789787899987E+00, 0.2371769848149604E+00,
        0.3818827653047059E+00, 0.5380295989189891E+00, 0.6903324200723622E+00,
        0.8238833438370047E+00, 0.9256126102908040E+00, 0.9855875903511235E+00,
        0.0252736203975203E+00, 0.0830416134474051E+00, 0.1691751003771814E+00,
        0.2777967151090320E+00, 0.4015027202328608E+00, 0.5318623869104160E+00,
        0.6599918420853348E+00, 0.7771593929561621E+00, 0.8753807748555569E+00,
        0.9479645488728194E+00, 0.9899817195383196E+00, 0.0186103650109879E+00,
        0.0614755408992690E+00, 0.1263051786933106E+00, 0.2098429717265625E+00,
        0.3078989982803983E+00, 0.4155560359786595E+00, 0.5274156139958823E+00,
        0.6378686027177611E+00, 0.7413764592942375E+00, 0.8327489886084423E+00,
        0.9074047753009974E+00, 0.9616018612603216E+00, 0.9926353489739107E+00 };

    int mc[12] = { 1, 13, 24, 34, 43, 51, 58, 64, 69, 73, 76, 78 };
    int nw[14] = { 0, 0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78 };
    int NXI[78] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 3, 6, 10, 15, 21, 28, 36, 45,
                    55, 66, 78, 4, 10, 20, 35, 56, 84, 120, 165, 220, 286, 5, 15, 35, 70,
                    126, 210, 330, 495, 715, 6, 21, 56, 126, 252, 462, 792, 1287, 7, 28, 84,
                    210, 462, 924, 1716, 8, 36, 120, 330, 792, 1716, 9, 45, 165, 495, 1287,
                    10, 55, 220, 715, 11, 66, 286, 12, 78, 13 };

    int kd2, la, lc, j, l, k, m, n, jd, jdm, lb, ld, le;

    int NI = 2, NV = 3 + 9 * force_var[11], NCLASS = 2;
    double XL = 1.0e-9;

    for (j = 0; j < 14; j++) { v.NW[j] = nw[j]; v.H[j] = 0.0L; }
    for (j = 0; j < 13; j++) v.U[j] = v.W[j] = 0.0L;
    for (j = 0; j < 12; j++) v.MC[j] = mc[j];
    for (j = 0; j < 78; j++) v.XI[j] = v.C[j] = v.D[j] = v.R[j] = 0.0L;
/*    delete v.F1;
    v.F1 = new double[NV]; v.FJ = v.F1 + NV; */v.Y = v.FJ + NV; v.Z = v.Y + NV;
    v.B = v.Z + NV; v.BE = v.B + 13 * NV; v.BT = v.BE + 13 * NV;

    v.KD = (NOR - 3) / 2; kd2 = v.KD / 2; v.KE = v.KD + 1;
    v.KF = v.KD + 2; v.PW = 1.0L / ((double)(v.KD + 3));
    v.NCL = v.NPQ = v.NES = 0;
    if (NCLASS == 1) v.NCL = 1; if (NCLASS < 2)  v.NPQ = 1;

    if (NV == 1) v.SR = 1.2L; else v.SR = 1.5L;
    if (LL < 0)	  v.NES = 1;
    for (n = 1, la = kd2*kd2 - 1; n < v.KF; n++, la++){
        v.H[n] = HH[la];
        v.W[n - 1] = 1.0L / ((double)((n + 1)*(1 + (n + 1)*(NCLASS - 1))));
        v.U[n - 1] = n + 2;
    }

    v.W1 = 1.0L / ((double)NCLASS);

    for (j = 0; j < v.KD; j++){
        m = v.MC[j] - 1;
        for (l = j + 1; l < v.KE; l++, m++){
            v.XI[m] = NXI[m] * v.W[j] / v.W[l];
        }
    }

    v.C[0] = -v.H[1] * v.W[0]; v.D[0] = v.H[1] / v.W[1]; v.R[0] = 1.0L / (v.H[2] - v.H[1]);

    for (k = 2, la = 0, lc = 0; k < v.KE; k++){
        lb = la; la = lc + 1; lc = v.NW[k + 1] - 1;
        jd = lc - la - 1;
        v.C[la] = -v.H[k] * v.C[lb];
        v.C[lc] = (v.C[la - 1] / v.W[jd] - v.H[k]) * v.W[jd + 1];
        v.D[la] = v.H[1] * v.D[lb] * v.W[k - 1] / v.W[k];
        v.D[lc] = (v.D[la - 1] * v.W[k - 1] + v.H[k]) / v.W[k];
        v.R[la] = 1.0L / (v.H[k + 1] - v.H[1]);
        v.R[lc] = 1.0L / (v.H[k + 1] - v.H[k]);
        if (k != 2){
            for (l = 3; l <= k; l++){
                ld = la + l - 2;
                le = lb + l - 3;
                jdm = ld - la - 1;
                v.C[ld] = v.W[jdm + 1] * v.C[le] / v.W[jdm] - v.H[k] * v.C[le + 1];
                v.D[ld] = (v.D[le] + v.H[l - 1] * v.D[le + 1]) * v.W[k - 1] / v.W[k];
                v.R[ld] = 1.0L / (v.H[k + 1] - v.H[l - 1]);
            }
        }
    }

    double *v_ptr, *b_ptr, *b_g_ptr, *bt_ptr, *bt_g_ptr, *be_ptr, *be_g_ptr;

    v.NSF = v.NPER = 0; v.TDIF = TF - TI; v.DIR = v.TDIF / fabs(v.TDIF);
    v.SS = pow(10.0, -LL); v.NL = NI + 30;

    if (v.NES) v.TP = XL = fabs(XL)*v.DIR;
    else v.TP = (NOR / 11.0L) * pow(0.5L, 0.4L* (double)LL) * v.DIR / 2.0L;
    if (v.TP / v.TDIF > 0.5L) v.TP = 0.5L*v.TDIF;

    for (k = 0, v_ptr = V, bt_g_ptr = v.BT, b_g_ptr = v.B, be_g_ptr = v.BE; k < NV; k++, v_ptr++, bt_g_ptr++, b_g_ptr++, be_g_ptr++){
        if (v.NCL) *v_ptr = 0.0L;
        for (l = 0, bt_ptr = bt_g_ptr, b_ptr = b_g_ptr, be_ptr = be_g_ptr; l < v.KE; l++, b_ptr += NV, bt_ptr += NV, be_ptr += NV){
            *b_ptr = *bt_ptr = 0.0L;
        }
    }

    v.NF = 0; v.NCOUNT = 0;

    do
    {
        v.NS = 0; v.TM = TI; v.SM = 10000.0L;
        force(X, V, v.TM, v.F1, de, force_var/*, NOR, LL*/); v.NF++;
    } while (radamaker(X, V, TF, de, force_var/*, NOR, LL*/));
}

// производит учет абберации
void Integrator::abberation(double X[], double V[], double JD0, DEreader *de
                            , int force_var[], int NOR, int LL, double poz[])
{
    double radius_vremia = 0, radius_vremia_ = 0, time_abb;
    int count = 0;
    double cc = de->deConst.speedLight / de->deConst.AE * 86400;
    int nv = 3 + force_var[11]*9;
    de->GetPlanetPoz(JD0, 2, true, poz);
    for (int i = 0; i < 3; i++)
        radius_vremia += pow(X[i] - poz[i], 2);
    radius_vremia = sqrt(radius_vremia) / cc;
    double *xpred, *vpred;
    xpred = new double[nv];
    vpred = new double[nv];
    memcpy(xpred, X, nv*sizeof(double));
    memcpy(vpred, V, nv*sizeof(double));
    while (fabs(radius_vremia - radius_vremia_) > 1e-16 && count < 40){
        time_abb = JD0 - radius_vremia;
        memcpy(X, xpred, nv*sizeof(double));
        memcpy(V, vpred, nv*sizeof(double));
        rada27(X, V, JD0, time_abb, force_var, NOR, LL, de);
        radius_vremia_ = radius_vremia;
        for (int i = 0; i < 3; i++)
            radius_vremia += pow(X[i] - poz[i], 2);
        radius_vremia = sqrt(radius_vremia) / cc;
        count++;
    }
    delete xpred;
    delete vpred;
}

void Integrator::force12(double X[], double massEarth, double coorEarth[], double rEarth2Aster
                         , DEreader *de, double F[])
{
    double planet_c[3];
    Coor(X, coorEarth, false).getX(planet_c, false);
    double sin_phi = planet_c[2] / rEarth2Aster;
    double Garmonica_aux;
    double p2 = (3 * sin_phi*sin_phi - 1) / 2;
    double secEarthGarm = massEarth * de->deConst.compEarth * pow(de->deConst.rEarth, 2) / pow(de->deConst.AE, 2);
    Garmonica_aux = (3 * p2 + 3 * pow(sin_phi,2)) * secEarthGarm / pow(rEarth2Aster, 5);
    for (int i = 0; i < 3; i++)
        F[i] += planet_c[i] * Garmonica_aux;
    F[2] -= secEarthGarm * 3 * sin_phi / pow(rEarth2Aster, 4);
}

void Integrator::force13(double X[], double TS, double rAster, DEreader *de, double F[])
{
    double iSun = 7.25*M_PI / 180;
    double Sun_oblateness = de->deConst.G*de->deConst.compSun*pow(de->deConst.rSun / de->deConst.AE, 2);
    double omega_Sun = (73.667 + (TS - 2396758.5) / 365.2425*0.01396) * M_PI / 180;
    double cc[3] = {
        sin(iSun)*sin(omega_Sun),
        -sin(iSun)*cos(omega_Sun),
        cos(iSun),
    };
    double Garmonica_aux2[3];
    Coor(X, false).getX(Garmonica_aux2, true);
    double z = 0, s;
    for (int i = 0; i < 3; i++)
        z += cc[i] * Garmonica_aux2[i];
    s = 3 * z - pow(rAster, 2);
    double pe[3];
    for (int i = 0; i < 3; i++)
        pe[i] = Sun_oblateness / pow(rAster, 3) * ((3 * cc[i] * z - Garmonica_aux2[i])
                                                   *pow(rAster, 2) - 2 * Garmonica_aux2[i] * s) / pow(rAster, 3);
    Coor(pe, true).getX(Garmonica_aux2, false);
    for (int i = 0; i < 3; i++)
        F[i] += Garmonica_aux2[i];
}

void Integrator::force14(double X[], double V[], double rAster, DEreader *de, double F[])
{
    double relative_aux = de->deConst.G / pow(299792.458 / de->deConst.AE * 86400, 2);
    double rel_vel = 0;
    double rel_vel2 = 0;
    for (int i = 0; i < 3; i++) {
        rel_vel += X[i] * V[i];
        rel_vel2 += pow(V[i], 2);
    }
    for (int i = 0; i < 3; i++)
        F[i] += relative_aux*((4 * de->deConst.G / rAster - rel_vel2)*X[i] + 4 * rel_vel*V[i])
                / pow(rAster, 3);
}

void Integrator::force15(double X[], double massJupiter, double coorJupiter[], double rJupiter2Aster
                         , double TS, DEreader *de, double F[])
{
    double Jupiter_Garmonica = 1.4736E-2;
    double Radius_Jupiter = 71492;
    double secJupGarm = massJupiter * Jupiter_Garmonica *pow(Radius_Jupiter, 2) / pow(de->deConst.AE, 2);
    double T = (TS - 2451545) / 36525;
    double aux_X1 = cos(0.00015707963267948965*T);
    double aux_Y1 = cos(0.00005235987755982989*T);
    double aux_X2 = sin(0.00015707963267948965*T);
    double aux_Y2 = sin(0.00005235987755982989*T);
    double aux_A[3] = {
        aux_X2*(-0.43041922177685454*aux_Y1 - 0.9019874904580493*aux_Y2)
      - aux_X1*(0.014654512120466917*aux_Y1 + 0.030710028601556798*aux_Y2),
        aux_X1*(-0.43041922177685454*aux_Y1 - 0.9019874904580493*aux_Y2)
      + aux_X2*(0.014654512120466917*aux_Y2 + 0.030710028601556798*aux_Y2),
                 0.90251013224202530*aux_Y1 - 0.4306686211003560*aux_Y2
    };
    double aux_Z = 0;
    double Jup_X[3];
    Coor(X, coorJupiter, false).getX(Jup_X, false);
    for (int i = 0; i < 3; i++)
        aux_Z += aux_A[i] * Jup_X[i];
    double Jup_to_Ast = pow(rJupiter2Aster, 2);
    double useless = 3 * aux_Z - Jup_to_Ast;
    for (int i = 0; i < 3; i++) {
        F[i] += secJupGarm* ((3 * aux_A[i] * aux_Z - Jup_X[i])*Jup_to_Ast - 2
                             * Jup_X[i] * useless) / pow(Jup_to_Ast, 3);
    }
}

// расчет эфемерид
void Integrator::ephemerida(double X[], double V[], double ephem, double JD0, DEreader *de
                            , obserVar &ov, int NOR, int LL, int force_var[])
{
    double poz[3];
    abberation(X, V, ephem, de, force_var, NOR, LL, poz);
    Coor c(X, poz, false);
    double al = Spherical(c, false).getRadAlpha();
    double del = Spherical(c, false).getDegDelta();
    double ST = StarTime(JD0).getHourTime();
    ST = ST * 15 + ov.longitude;
    ST = ST * M_PI / 180;
    double spi = (de->deConst.rEarth / de->deConst.AE) / Spherical(c, false).getRad();
    alpha = al - atan(ov.cosLatitude * spi*sin(ST - al)
                      / (cos(del) - ov.cosLatitude * spi*cos(ST - al)));
    double gamma = atan((ov.sinLatitude / ov.cosLatitude)
                        *cos((al - alpha) / 2) * 1 / cos((ST - (al + alpha) / 2)));
    delta = del + atan(ov.sinLatitude * spi / sin(gamma)*sin(del - gamma)
                       / (1 - ov.sinLatitude * spi / sin(gamma)*cos(del - gamma)));
    // перевод в градусы
    alpha = alpha / M_PI * 180;
    delta = delta / M_PI * 180;
    // перевод в часы
    alpha = alpha / 15.;
    if (alpha < 0)
        alpha += 24;
    if (alpha > 24)
        alpha -= 24;
}

double Integrator::getDegDelta()
{
    return delta;
}

double Integrator::getHourAlpha()
{
    return alpha;
}

// вычисление видимой астероидной величины
double Integrator::magnitude_observ(double X[], double epher , DEreader *de, bowellVar &bv)
{
    double coorEarth[3], planet_centric[3];
    de->GetPlanetPoz(epher, 2, true, coorEarth);
    Coor(X, coorEarth, false).getX(planet_centric, false);
    double distance_to_Earth = 0;
    for (int i = 0; i < 3; i++)
        distance_to_Earth += pow(planet_centric[i], 2);
    distance_to_Earth = sqrt(distance_to_Earth);
    double distance_to_Sun = 0;
    for (int i = 0; i < 3; i++)
        distance_to_Sun += pow(X[i], 2);
    distance_to_Sun = sqrt(distance_to_Sun);
    double distance_Earth_to_Sun = 0;
    for (int i = 0; i < 3; i++)
        distance_Earth_to_Sun += pow(coorEarth[i], 2);
    distance_Earth_to_Sun = sqrt(distance_Earth_to_Sun);
    double var_p = (distance_Earth_to_Sun + distance_to_Earth + distance_to_Sun) / 2;
    double var_SS = sqrt(var_p*(var_p - distance_to_Sun)*(var_p - distance_Earth_to_Sun)
                  *(var_p - distance_to_Earth));
    double var_tb = var_SS / var_p / (var_p - distance_Earth_to_Sun);
    double var_f[2];
    double varmas_a[2] = { 3.33, 1.87 };
    double varmas_b[2] = { 0.63, 1.22 };
    for (int i = 0; i < 2; i++)
        var_f[i] = exp(-varmas_a[i] * pow(var_tb, varmas_b[i]));
    double var_a1, var_b1;
    var_a1 = distance_to_Sun*distance_to_Earth;
    var_b1 = (1 - bv.g)*var_f[0] + bv.g*var_f[1];
    double magnitude_observ = 0;
    if (var_a1 != 0 && var_b1 != 0)
        magnitude_observ = bv.mag + 5.*log10(var_a1) - 2.5*log10(var_b1);
    return magnitude_observ;
}

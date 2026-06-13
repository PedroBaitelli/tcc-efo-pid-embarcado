#define _USE_MATH_DEFINES   /* M_PI no MSVC */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <math.h>
#include "pid_autotuning.h"

/* =====================================================================
 * Zeros e Polos do FOI de -60° discretizado (Ts = 1 ms)
 * ===================================================================== */
#define FOI_N_POLES 12

static const double foi_zeros[FOI_N_POLES] = {
   -33.488077564107542,
     0.387310889565132,
     0.078179305469233,
     0.001847154982911,
     0.000000003344458,
     0.751059346146684,
     0.920331637563663,
     0.976324797249240,
     0.993113104503224,
     0.998008890246395,
     0.999425349631836,
     0.999834235100050
};

static const double foi_poles[FOI_N_POLES] = {
     0.999927637615723,
     0.999749115346111,
     0.999130359667629,
     0.996987870192997,
     0.989594589508138,
     0.964381403876207,
     0.881830021070990,
     0.646590400517423,
     0.220486414421931,
     0.005287651880456,
     0.000000012755965,
     0.000000000000000
};

static const double FOI_K = 4.786300923226396e-05;

/* =====================================================================
 * Calcula a magnitude do FOI discretizado
 * ===================================================================== */
static double calcularMagnitudeFOI(double omega, double Ts) {
    double zr = cos(omega * Ts);   // Real
    double zi = sin(omega * Ts);   // Imaginário

    double num_re = 1.0, num_im = 0.0;  
    double den_re = 1.0, den_im = 0.0;  

    for (int i = 0; i < FOI_N_POLES; i++) {
        // Numerador
        double zz_r = zr - foi_zeros[i];
        double zz_i = zi;
        double tmp_r = num_re * zz_r - num_im * zz_i;
        double tmp_i = num_re * zz_i + num_im * zz_r;
        num_re = tmp_r;
        num_im = tmp_i;

        // Denominador
        double zp_r = zr - foi_poles[i];
        double zp_i = zi;
        tmp_r = den_re * zp_r - den_im * zp_i;
        tmp_i = den_re * zp_i + den_im * zp_r;
        den_re = tmp_r;
        den_im = tmp_i;
    }

    double mag_num = sqrt(num_re * num_re + num_im * num_im);
    double mag_den = sqrt(den_re * den_re + den_im * den_im);

    if (mag_den < 1e-30) return 0.0;  

    return FOI_K * mag_num / mag_den;
}

/* =====================================================================
 * Calcula ganhos PID 
 * ===================================================================== */
void calcularGanhosPID(double A, double T120, double d, double Ts,
                       double *Kp, double *Ti, double *Td) {
    double omega120 = 2.0 * M_PI / T120;
    double F_mag = calcularMagnitudeFOI(omega120, Ts);
    double M120 = (M_PI * A) / (4.0 * d * F_mag);

    *Kp = 0.97 / M120;
    *Ti = 0.90 * T120;
    *Td = 0.028 * T120;
}
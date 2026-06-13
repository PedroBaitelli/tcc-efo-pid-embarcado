#include <Arduino.h>
#include <string.h>



// G1(s)
// static const float b0 =  0.0136499825f;
// static const float b1 =  0.0f;
// static const float a1 =  0.9863500175f;
// static const float a2 =  0.0f;

// G2(s) discretizado ZOH, Ts = 1000 µs (1 ms)
// G2(s) = 189.72 / (s² + 41.32s + 189.72)
static const float b0 = 0.0000935654f;
static const float b1 = 0.0000922855f;
static const float a1 =  1.9593361829f;
static const float a2 =  -0.9595220338f;

// P1 — G2 com ganho DC = 2
// G(s) = 379.44 / (s² + 41.32s + 189.72)
// Discretizado ZOH, Ts = 1 ms
// static const float b0 =  0.0001871307f;
// static const float b1 =  0.0001845710f;
// static const float a1 =  1.9593361829f;
// static const float a2 = -0.9595220338f;

// P2 — Planta rápida
// G(s) = 500 / (s² + 50s + 500)
// Discretizado ZOH, Ts = 1 ms
// static const float b0 =  0.0002458747f;
// static const float b1 =  0.0002418107f;
// static const float a1 =  1.9507417391f;
// static const float a2 = -0.9512294245f;

// P3 — Planta lenta subamortecida
// G(s) = 10 / (s² + 6s + 10)
// Discretizado ZOH, Ts = 1 ms
// static const float b0 =  0.0000049900f;
// static const float b1 =  0.0000049800f;
// static const float a1 =  1.9940079940f;
// static const float a2 = -0.9940179641f;

// Estado interno da planta
static float y_km1 = 0.0f;
static float y_km2 = 0.0f;
static float u_km1 = 0.0f;
static float u_km2 = 0.0f;

// // P4 — Aircraft pitch 
// // G(s) = (1.15s + 0.18) / (s³ + 0.74s² + 0.92s)
// // 3ª ordem com integrador — Discretizado ZOH, Ts = 1 ms
// static const double b0 =  0.0000005749f;
// static const double b1 = -0.0000000000f;
// static const double b2 = -0.0000005747f;
// static const double a1 =  2.9992593541f;
// static const double a2 = -2.9985196278f;
// static const double a3 =  0.9992602737f;

// // Estado interno da planta — 3ª ordem
// static double y_km1 = 0.0f;
// static double y_km2 = 0.0f;
// static double y_km3 = 0.0f;
// static double u_km1 = 0.0f;
// static double u_km2 = 0.0f;
// static double u_km3 = 0.0f;

// ============================================================
// PROTOCOLO
// ============================================================
#define BAUDRATE  921600
#define PKT_SIZE  8

#define HDR_RX1  0xAA
#define HDR_RX2  0x55
#define HDR_TX1  0xBB
#define HDR_TX2  0x66

uint8_t pacote_tx[PKT_SIZE];

static uint32_t pkt_ok  = 0;
static uint32_t pkt_err = 0;

// ============================================================
float calcularPlanta(float u_k) {
    float y_k = a1 * y_km1 + a2 * y_km2
              + b0 * u_km1 + b1 * u_km2;

    y_km2 = y_km1;
    y_km1 = y_k;
    u_km2 = u_km1;
    u_km1 = u_k;

    return y_k;
}

// // For the P4 case
// float calcularPlanta(float u_k) {
//     double y_k = a1 * y_km1 + a2 * y_km2 + a3 * y_km3
//               + b0 * u_km1 + b1 * u_km2 + b2 * u_km3;

//     // // Saturação — impede divergência do integrador
//     // if (y_k > 5.0f) y_k = 5.0f;
//     // if (y_k < -5.0f) y_k = -5.0f;

//     y_km3 = y_km2;
//     y_km2 = y_km1;
//     y_km1 = y_k;
//     u_km3 = u_km2;
//     u_km2 = u_km1;
//     u_km1 = (double)u_k;

//     return (float)y_k;
// }

// ============================================================
void montarPacoteTX(float y) {
    pacote_tx[0] = HDR_TX1;
    pacote_tx[1] = HDR_TX2;
    memcpy(&pacote_tx[2], &y, 4);
    pacote_tx[6] = 0x00;
    pacote_tx[7] = pacote_tx[2] ^ pacote_tx[3]
                 ^ pacote_tx[4] ^ pacote_tx[5];
}

// ============================================================
typedef enum {
    RX_WAIT_HDR1 = 0,
    RX_WAIT_HDR2,
    RX_DATA
} RxState_t;

static RxState_t rxState = RX_WAIT_HDR1;
static uint8_t rxBuf[6];
static uint8_t rxCount = 0;

bool parserFeedByte(uint8_t b, float *u_out) {
    switch (rxState) {
        case RX_WAIT_HDR1:
            if (b == HDR_RX1) {
                rxState = RX_WAIT_HDR2;
            }
            break;

        case RX_WAIT_HDR2:
            if (b == HDR_RX2) {
                rxState = RX_DATA;
                rxCount = 0;
            } else if (b == HDR_RX1) {
                rxState = RX_WAIT_HDR2;
            } else {
                rxState = RX_WAIT_HDR1;
            }
            break;

        case RX_DATA:
            rxBuf[rxCount++] = b;
            if (rxCount == 6) {
                rxState = RX_WAIT_HDR1;
                uint8_t chk = rxBuf[0] ^ rxBuf[1]
                            ^ rxBuf[2] ^ rxBuf[3];
                if (chk == rxBuf[5]) {
                    memcpy(u_out, rxBuf, 4);
                    return true;
                }
                pkt_err++;
            }
            break;
    }
    return false;
}

// ============================================================
void setup() {
    Serial.begin(115200);                          // Debug USB -> PC
    Serial2.begin(BAUDRATE, SERIAL_8N1, 16, 17);  // Protocolo -> STM32

    Serial.println("=== ESP32 Simulador de Planta HiL ===");
    Serial.printf("Baudrate: %d\n", BAUDRATE);
    Serial.printf("Coeficientes G2(z) — Ts = 1 ms:\n");
    Serial.printf("  b0 = %.10f\n", b0);
    Serial.printf("  b1 = %.10f\n", b1);
    Serial.printf("  a1 = %.10f\n", a1);
    Serial.printf("  a2 = %.10f\n", a2);
    Serial.println("Aguardando pacotes do STM32...");
}

// ============================================================
void loop() {
    while (Serial2.available() > 0) {
        uint8_t b = Serial2.read();
        float u_k;

        if (parserFeedByte(b, &u_k)) {
            float y_k = calcularPlanta(u_k);

            montarPacoteTX(y_k);
            Serial2.write(pacote_tx, PKT_SIZE);
            pkt_ok++;
        }
    }

    static uint32_t last_debug = 0;
    if (millis() - last_debug > 5000) {
        Serial.printf("[ESP32] pkts_ok=%lu  pkts_err=%lu  y_km1=%.6f\n",
                      (unsigned long)pkt_ok, (unsigned long)pkt_err,
                      y_km1);
        last_debug = millis();
    }
}
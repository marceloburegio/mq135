#include "mgos.h"
#include "mgos_adc.h"

#include <math.h>

struct mgos_mq135 {
  int pin;
  float rload; // The load resistance on the board
  float rzero; // Calibration resistance at atmospheric CO2 level
};

/// Parameters for calculating ppm of CO2 from sensor resistance
#define MGOS_MQ135_PARA 116.6020682
#define MGOS_MQ135_PARB 2.769034857

/// Parameters to model temperature and humidity dependence
#define MGOS_MQ135_CORA 0.00035
#define MGOS_MQ135_CORB 0.02718
#define MGOS_MQ135_CORC 1.39538
#define MGOS_MQ135_CORD 0.0018
#define MGOS_MQ135_CORE -0.003333333
#define MGOS_MQ135_CORF -0.001923077
#define MGOS_MQ135_CORG 1.130128205

/// Atmospheric CO2 level for calibration purposes
#define MGOS_MQ135_ATMOCO2 403 // 403 = C02 in the forests / 410-425 = CO2 in a city with high car pollution

// Private functions follow
// Private functions end

// Public functions follow
struct mgos_mq135 *mgos_mq135_create(int pin) {
  struct mgos_mq135 *mq;
  
  if (!mgos_adc_enable(pin)) {
    return NULL;
  }
  
  mq = calloc(1, sizeof(struct mgos_mq135));
  if (!mq) {
    return NULL;
  }
  memset(mq, 0, sizeof(struct mgos_mq135));
  
  mq->pin = pin;
  mq->rload = 22.0;
  mq->rzero = 76.63;
  
  LOG(LL_DEBUG, ("Sensor MQ135 was successfully created using pin %d", pin));
  
  return mq;
}

/* Get the correction factor to correct for temperature and humidity */
float mgos_mq135_get_correction_factor(float t, float h) {
  // Linearization of the temperature dependency curve under and above 20 degree C
  // below 20degC: fact = a * t * t - b * t - (h - 33) * d
  // above 20degC: fact = a * t + b * h + c
  // this assumes a linear dependency on humidity
  if (t < 20) {
    return MGOS_MQ135_CORA * t * t - MGOS_MQ135_CORB * t + MGOS_MQ135_CORC - (h - 33.) * MGOS_MQ135_CORD;
  }
  else {
    return MGOS_MQ135_CORE * t + MGOS_MQ135_CORF * h + MGOS_MQ135_CORG;
  }
}

/* Get the resistance of the sensor, ie. the measurement value */
float mgos_mq135_get_resistance(struct mgos_mq135 *mq) {
  int val = mgos_adc_read(mq->pin);
  return ((1023. / (float)val) - 1.) * mq->rload; // Before was: ((1023. / (float)val) * 5. - 1.) * mq->rload;
}

/* Get the resistance of the sensor, ie. the measurement value corrected for temperature/humidity */
float mgos_mq135_get_corrected_resistance(struct mgos_mq135 *mq, float t, float h) {
  return mgos_mq135_get_resistance(mq) / mgos_mq135_get_correction_factor(t, h);
}

/* Get the ppm of CO2 sensed (assuming only CO2 in the air) */
float mgos_mq135_get_ppm(struct mgos_mq135 *mq) {
  return MGOS_MQ135_PARA * pow((mgos_mq135_get_resistance(mq) / mq->rzero), -MGOS_MQ135_PARB);
}

/* Get the ppm of CO2 sensed (assuming only CO2 in the air), corrected for temperature/humidity */
float mgos_mq135_get_corrected_ppm(struct mgos_mq135 *mq, float t, float h) {
  return MGOS_MQ135_PARA * pow((mgos_mq135_get_corrected_resistance(mq, t, h) / mq->rzero), -MGOS_MQ135_PARB);
}

/* Get the resistance RZero of the sensor for calibration purposes */
float mgos_mq135_get_rzero(struct mgos_mq135 *mq) {
  return mgos_mq135_get_resistance(mq) * pow((MGOS_MQ135_ATMOCO2 / MGOS_MQ135_PARA), (1. / MGOS_MQ135_PARB));
}

/* Get the corrected resistance RZero of the sensor for calibration purposes */
float mgos_mq135_get_corrected_rzero(struct mgos_mq135 *mq, float t, float h) {
  return mgos_mq135_get_corrected_resistance(mq, t, h) * pow((MGOS_MQ135_ATMOCO2 / MGOS_MQ135_PARA), (1. / MGOS_MQ135_PARB));
}

/* Initialization function for MGOS -- currently a noop */
bool mgos_mq135_init(void) {
  return true;
}

// Public functions end
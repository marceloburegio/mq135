#ifndef CS_MOS_LIBS_MQ135_INCLUDE_MGOS_MQ135_H_
#define CS_MOS_LIBS_MQ135_INCLUDE_MGOS_MQ135_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mgos_mq135;

/* Initialise MQ135 sensor. Return an opaque MQ135 handle, or `NULL` on error */
struct mgos_mq135 *mgos_mq135_create(int pin);

/* Close MQ135 handle */
void mgos_mq135_close(struct mgos_mq135 *mq);

/* Get the correction factor to correct for temperature and humidity */
float mgos_mq135_get_correction_factor(float t, float h);

/* Get the resistance of the sensor, ie. the measurement value */
float mgos_mq135_get_resistance(struct mgos_mq135 *mq);

/* Get the resistance of the sensor, ie. the measurement value corrected for temperature/humidity */
float mgos_mq135_get_corrected_resistance(struct mgos_mq135 *mq, float t, float h);

/* Get the ppm of CO2 sensed (assuming only CO2 in the air) */
float mgos_mq135_get_ppm(struct mgos_mq135 *mq);

/* Get the ppm of CO2 sensed (assuming only CO2 in the air), corrected for temperature/humidity */
float mgos_mq135_get_corrected_ppm(struct mgos_mq135 *mq, float t, float h);

/* Get the resistance RZero of the sensor for calibration purposes */
float mgos_mq135_get_rzero(struct mgos_mq135 *mq);

/* Get the corrected resistance RZero of the sensor for calibration purposes */
float mgos_mq135_get_corrected_rzero(struct mgos_mq135 *mq, float t, float h);

/* Initialization function for MGOS -- currently a noop */
bool mgos_mq135_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CS_MOS_LIBS_MQ135_INCLUDE_MGOS_MQ135_H_ */

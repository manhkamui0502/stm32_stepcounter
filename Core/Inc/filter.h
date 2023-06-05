#include <stdint.h>
#include "main.h"

#ifndef KalmanFilter_h
#define KalmanFilter_h

typedef struct
{
  float err_measure;
  float err_estimate;
  float q;
  float current_estimate;
  float last_estimate;
  float kalman_gain;
} KalmanFilter;

void KalmanFilter_init(KalmanFilter *filter, float mea_e, float est_e, float q);
float KalmanFilter_updateEstimate(KalmanFilter *filter, float mea);

#endif


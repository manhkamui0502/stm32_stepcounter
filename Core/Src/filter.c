#include "filter.h"
#include <math.h>

void KalmanFilter_init(KalmanFilter *filter, float mea_e, float est_e, float q)
{
  filter->err_measure = mea_e;
  filter->err_estimate = est_e;
  filter->q = q;
}

float KalmanFilter_updateEstimate(KalmanFilter *filter, float mea)
{
  filter->kalman_gain = filter->err_estimate / (filter->err_estimate + filter->err_measure);
  filter->current_estimate = filter->last_estimate + filter->kalman_gain * (mea - filter->last_estimate);
  filter->err_estimate = (1.0 - filter->kalman_gain) * filter->err_estimate + fabs(filter->last_estimate - filter->current_estimate) * filter->q;
  filter->last_estimate = filter->current_estimate;

  return filter->current_estimate;
}

#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double kp, double ki, double kd, double tau){
	PID::kp = kp;
	PID::ki = ki;
	PID::kd = kd;
	PID::tau = tau;
	
	pre_cte = 0.0;
	p_error = i_error = d_error = 0.0;
}

void PID::UpdateError(double cte) {
	p_error = cte;
	d_error = cte - pre_cte;
	i_error =0.9*i_error + cte;
	pre_cte = cte;

}

double PID::steer_val(double speed) {
	double steer_value;
	ki = 0.0 + tau*speed; 
	steer_value = kp*p_error + ki*i_error + kd*d_error;
	return -steer_value;
}


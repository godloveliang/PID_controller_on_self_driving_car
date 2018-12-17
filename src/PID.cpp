#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd){
	PID::Kp = Kp;
	PID::Ki = Ki;
	PID::Kd = Kd;
	
	p_error = i_error = d_error = 0.0;
}

void PID::UpdateError(double cte, double pre_cte) {
	p_error = cte;
	d_error = cte - pre_cte;
	i_error += cte;

}

double PID::TotalError() {
	return 0;
}


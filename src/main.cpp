#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  // PID pid;
  // TODO: Initialize the pid variable.
 
  // try 1, can keep on the lane but wave sharply.
  //pid.Init(0.15, 0.005, 3.0);

  //try 2, 
  //pid.Init(0.12, 0.005, 3.0);
 
  //try 3
  // pid.Init(0.13, 0.0003, 3.0);

  //double twiddle_p[3] = {0.13, 0.0003, 3.0};
  double twiddle_p[3] = {0.12, 0.00029, 3.5};
  double twiddle_dp[3] = {0.005, 0.00001, 0.1}; 

  int n=0;
  double total_err = 0.0;
  double best_err = 100000;
  //k_par used to traversing the index of twiddle_p/dp;
  //k_circle used to show how many circles the car has drived,nearly 1000 steps amount to 1 circle.
  int k_par = 0;
  int k_circle = 0;
  int n_cout = 0;
  double total_i_err = 0.0;

  h.onMessage([&twiddle_p, &twiddle_dp, &n, &total_err, &best_err, &k_par, &k_circle, &n_cout, &total_i_err]
		  (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */

	  double pre_cte;
	  if(n==0){
	    pre_cte = cte;
	  }


	  //take out the first 100 steps when change twiddle_p.
	  if(n>100){	  
	    total_err += cte*cte;
	  }
	  total_i_err += cte;


	  if (k_circle == 0){
	    twiddle_p[k_par] += twiddle_dp[k_par];
	    k_circle = 1;
	  }


	  if(n == 1000 && k_circle == 1){
	    if(total_err < best_err){
	      best_err = total_err;
	      twiddle_dp[k_par] *= 1.1;
	      k_circle = 0;
	      k_par += 1;
	      k_par = k_par%3;
	    }
	    else{
	      twiddle_p[k_par] -= 2*twiddle_dp[k_par];
	      k_circle = 2;
	    }
	    n = 1;
	    total_err = 0.0;
	    total_i_err = 0.0;
	  }


	  if(n == 1000 && k_circle == 2){
	    if(total_err < best_err){
	      best_err = total_err;
	      twiddle_dp[k_par] *= 1.1;
	    }
	    else{
	      twiddle_p[k_par] += twiddle_dp[k_par];
	      twiddle_dp[k_par] *= 0.9;
	    }
	    k_circle =0;
	    n = 1;
	    total_err = 0.0;
	    total_i_err = 0.0;
	    k_par += 1;
	    k_par = k_par%3;
	  }
	  

	  PID pid;
	  pid.Init(twiddle_p[0],twiddle_p[1], twiddle_p[2]);
	  pid.UpdateError(cte, pre_cte);
	  steer_value = -pid.Kp*pid.p_error -pid.Ki*total_i_err -pid.Kd*pid.d_error;
	  
	  if(steer_value > 0.8){
	    steer_value = 0.8;
	  }

	  if(steer_value < -0.8){
	    steer_value = -0.8;
	  }

	  pre_cte = cte;
	  n += 1;

          // DEBUG
          // std::cout << n << "-> CTE: " << cte << " Steering Value: " << steer_value << std::endl;;
	  if(n ==1000){
		  std::cout<< n_cout <<"--twiddle_p="<<twiddle_p[0]<<"  "<<twiddle_p[1]<<"  "<<twiddle_p[2]<<
		"  "<<"twiddle_dp="<<twiddle_dp[0]<<"  "<<twiddle_dp[1]<<"  "<<twiddle_dp[2]<< std::endl;
		  n_cout +=1;
	  }


          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.35;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
         // std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}

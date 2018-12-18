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

  PID pid;
  // TODO: Initialize the pid variable.
  // pid.Init(0.12, 0.00029, 3.5); 
  double kp = 0.12;
  double ki = 0.0;
  double kd = 3.5;
  double tau = 0.001;

  pid.Init(kp, ki, kd, tau);

  h.onMessage([&pid] (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
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

	  static double pre_steer = 0.0;
	  static double pre_angle = 0.0;
	  static bool brake = false;
	  static int num_brake_steps = 0;
	  static double throttle = 0.3;
	  const double max_speed = 80;

	  if(abs(angle - pre_angle)>1.3 && num_brake_steps == 0){
	    brake = true;
	    num_brake_steps = 3;
	  }

	  if(brake){
	    if(speed > max_speed) throttle = 0.3;
	    else{
	      throttle -= 0.1;
	      if(throttle<0.0) throttle = 0.0;
	    }
	    num_brake_steps -= 1;
	    if(num_brake_steps ==0) brake=false;
	  }

	  else{
	    if(speed>max_speed){
	      throttle -= 0.1;
	      if(throttle < 0.0) throttle = 0.0;
	    }
	    else{
	      throttle += 0.1;
	      if(throttle > 0.8) throttle = 0.8;
	    }
	  }

	  pid.UpdateError(cte);
	  steer_value = pid.steer_val(speed);
	  
	  if(steer_value > 0.2){
	    steer_value = 0.2 + (steer_value - 0.2)*0.5;
	    if(steer_value > 0.3) steer_value = 0.3 + (steer_value - 0.3)*0.5;
	    if(steer_value > 0.4) steer_value = 0.4 + (steer_value - 0.4)*0.5;
	  }

	  if(steer_value < -0.2){
	    steer_value = -0.2+ (steer_value + 0.2)*0.5;
	    if(steer_value < -0.3) steer_value = -0.3 + (steer_value + 0.3)*0.5;
	    if(steer_value < -0.4) steer_value = -0.4 + (steer_value + 0.5)*0.5;
	  }

	  steer_value = 0.1*pre_steer + 0.9*steer_value;

          // DEBUG
	  static int n =0;
          std::cout << n << "-> CTE: " << cte << " Steering Value: " << steer_value <<"  throttle"<<throttle<< std::endl;;
	  n += 1;
 	  pre_angle = angle;
	  pre_steer = steer_value;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle;
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

//
// Created by jun on 7/16/17.
//
#include <iostream>
#include <fstream>
#include <uWS/uWS.h>
#include <chrono>
#include <thread>
#include <vector>
#include "Eigen-3.3/Eigen/Core"
#include "Eigen-3.3/Eigen/QR"
#include "json.hpp"

#include "utilities.h"
#include "ego.h"
#include "map.h"


int main() {

  uWS::Hub h;

  Map highway_map;

  Ego my_car(highway_map);

  h.onMessage([&my_car]
               (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,
                uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    //auto sdata = string(data).substr(0, length);
    //cout << sdata << endl;
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {

      std::string s = hasData(data);

      if (s != "") {
        auto j = nlohmann::json::parse(s);

        std::string event = j[0].get<std::string>();
        
        if (event == "telemetry") {
          // j[1] is the data JSON object
          
        	// Main car's localization data (without noise).
          double car_x = j[1]["x"];  // in m
          double car_y = j[1]["y"];  // in m
          double car_speed = j[1]["speed"];  // in MPH
          double car_yaw = j[1]["yaw"];  // in degree
          double yaw_in_rad = deg2rad(car_yaw);
//          double car_vx = car_speed*std::cos(yaw_in_rad);
//          double car_vy = car_speed*std::sin(yaw_in_rad);
          double car_s = j[1]["s"];  // in m
          double car_d = j[1]["d"];  // in m
          car_speed *= 4.0/9;  // MPH to m/s

//          std::cout << "Cartesian: " << car_x << ", " << car_y << std::endl;
//          std::cout << "Frenet: " << car_s << ", " << car_d << std::endl;
//
//          std::pair<double, double> car_frenet = highway_map.frenetToCartesian(car_s, car_d);
//          std::cout << "Cartesian from Frenet: " << car_frenet.first << ", " << car_frenet.second << std::endl;
//
//          std::pair<double, double> car_cartesian = highway_map.cartesianToFrenet(car_x, car_y);
//          std::cout << "Frenet from Cartesian: " << car_cartesian.first << ", " << car_cartesian.second << std::endl;

          std::vector<double> localization = {car_x, car_y, car_speed, car_yaw, car_s, car_d};
          // Previous path data passed to the planner.
          auto previous_path_x = j[1]["previous_path_x"];
          auto previous_path_y = j[1]["previous_path_y"];

          // End s and d values of the previous path.
          double end_path_s = j[1]["end_path_s"];
          double end_path_d = j[1]["end_path_d"];

          // All other car's data on the same side of the road
          // in the format [[ID, x (m), y (m), vx (m/s), vy (m/s), s (m), d (m)]].
          auto sensor_fusion_readout = j[1]["sensor_fusion"];
          std::vector<std::vector<double>> sensor_fusion;
          for ( auto &it : sensor_fusion_readout ) {
            double vx = it[3];
            double vy = it[4];
            it[3] = std::sqrt(vx*vx + vy*vy);
            it[4] = std::atan2(vy, vx);
            std::vector<double> value (std::next(it.begin(), 1), it.end());
            sensor_fusion.push_back(value);
          }

          nlohmann::json msgJson;

          my_car.update(localization, sensor_fusion);

          // Transfer the trajectory in Frenet coordinate system output
          // by "my_car" and pass it to the simulator.

          vehicle_traj trajectory_frenet = my_car.getPath();
          vehicle_traj trajectory_cartesian = my_car.getMap()->trajFrenetToCartesian(trajectory_frenet);

          msgJson["next_x"] = trajectory_cartesian.first;
          msgJson["next_y"] = trajectory_cartesian.second;

          auto msg = "42[\"control\"," + msgJson.dump() + "]";

          //this_thread::sleep_for(chrono::milliseconds(1000));
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }

      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the
  // program doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data,
                     size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1) {
      res->end(s.data(), s.length());
    } else {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code,
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }

  h.run();
}

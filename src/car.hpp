/*
 * Note!!!
 *
 * The simulator does not simulate the real physical movement of a car. Instead,
 * every 20 ms the car moves to the next point on the list perfectly. The car's
 * new rotation becomes the line between the previous waypoint and the car's new
 * location.
 *
 * The simulator runs a cycle every 20 ms, but your C++ path planning program
 * will provide a new path at least one 20 ms cycle behind. The simulator will
 * simply keep progressing down its last given path while it waits for a new
 * generated path. As a result, by the time a new path reaches the simulator,
 * the vehicle has already passed the first few waypoints on that path. The
 * simulator has built-in tools to deal with this timing difference. The
 * simulator actually expects the received path to be a little out of date
 * compared to where the car is, and the simulator will consider which point
 * on the received path is closest to the car and adjust appropriately.
 *
 */
#ifndef PATH_PLANNING_CAR_H
#define PATH_PLANNING_CAR_H

#include <iostream>
#include <vector>
#include <list>
#include <map>

#include "map.hpp"
#include "utilities.hpp"

class Car;


class PathOptimizer {

  PathOptimizer();

public:
  using trajectory = std::pair<std::vector<double>, std::vector<double>>;
  using polynomial_coeff = std::vector<double>;

  ~PathOptimizer();

  /**
   * Validate a JMT path.
   */
  static bool validatePath(polynomial_coeff coeff_s, polynomial_coeff coeff_d,
                           double delta_t, double time_step,
                           double speed_limit, double acceleration_limit, double jerk_limit);

  /**
   * Compute the trajectory in the Frenet coordinate system.
   */
  static PathOptimizer::trajectory
  computeJmtPath(polynomial_coeff coeff_s, polynomial_coeff coeff_d, double delta_t, double time_step);

  /**
   * Get the optimized path when starting up.
   */
  static PathOptimizer::trajectory startUp(Car* car);

  /**
   * Get the optimized path when keeping lane.
   */
  static trajectory keepLane(Car* car);

  /**
   * Get the optimized path when changing to the left lane.
   */
  trajectory changeLaneLeft(Car* car);

  /**
   * Get the optimized path when changing to the right lane.
   */
  trajectory changeLaneRight(Car* car);
};


class CarState;

class Car {

public:
  using trajectory = std::pair<std::vector<double>, std::vector<double>>;
  using dynamics = std::pair<std::vector<double>, std::vector<double>>;

private:
  friend class PathOptimizer;

  bool is_initialized_;

  double time_step_; // timestep in second

  // parameters in Cartisian coordinate system
  double px_; // in m
  double py_; // in m
  double vx_; // in m/s
  double vy_; // in m/s
  double ax_; // in m/s^2
  double ay_; // in m/s^2

  // parameters in Frenet-Serret coordinate system
  double ps_; // in m
  double pd_; // in m
  double vs_; // in m/s
  double vd_; // in m/s
  double as_; // in m/s^2
  double ad_; // in m/s^2

  Map map_;

  std::vector<trajectory> opt_paths_;

  CarState* state_; // vehicle state

  // key: lane ID, value: vehicle dynamics
  std::map<uint16_t, dynamics> closest_front_cars_;
  std::map<uint16_t, dynamics> closest_rear_cars_;

  uint8_t target_lane_id_; // target lane ID

  std::vector<double> path_s_;
  std::vector<double> path_d_;

  // ignore cars which are too far away (not detectable in real life)
  double max_tracking_distance = 100; // in m

  double max_speed_= mph2mps(50); // maximum speed (m/s)
  double max_acceleration_ = 10; // maximum acceleration (m/s^2)
  double max_jerk_ = 10; // maximum jerk (m/s^3)

  /**
   * Update the unprocessed waypoints.
   */
  void updateUnprocessedPath();

  /**
   * Truncate a path (remove tail) to a given length
   *
   * @param n_keep: number of points to keep
   */
  void truncatePath(unsigned int n_keep);

  /**
   * Print out the vehicle's information.
   */
  void info() const;

public:
  explicit Car(const Map& map, double time_step=0.02);

  ~Car();

  /**
   * Update all parameters and states.
   */
  void update(const std::vector<double>& localization,
              const std::vector<std::vector<double>>& sensor_fusion);

  /**
   * Update parameters based on the localization data.
   *
   * @param localization: x, y, speed, yaw, s, d
   */
  void updateParameters(const std::vector<double>& localization);

  /**
   * Update dynamics of the closest vehicles in each lane.
   */
  void updateClosestVehicles(const std::vector<std::vector<double>>& sensor_fusion);

  /**
   * Estimate the dynamics of car at the path end.
   * @return: ((ps, vs, as), (pd, vd, ad))
   */
  dynamics estimateFinalDynamics() const;

  /**
   * Extend the current path.
   */
  void extendPath(std::vector<double> path_s, std::vector<double> path_d);

  /**
   * Update path when starting up.
   */
  void startUp();

  /**
   * Update path when keeping lane.
   */
  void keepLane();

  /**
   * Return the corresponding Cartisian path of the current Frenet path.
   */
  trajectory getPathXY() const;

  std::map<uint16_t, dynamics> getClosestFrontVehicles() const;
  std::map<uint16_t, dynamics> getClosestRearVehicles() const;

  uint16_t getCurrentLaneId() const;
  double getCurrentLaneCenter() const;

  uint16_t getTargetLaneId() const;

  double getCurrentSpeed() const;
  double getMaxSpeed() const;
};


#endif //PATH_PLANNING_CAR_H

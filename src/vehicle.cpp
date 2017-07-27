//
// Created by jun on 7/16/17.
//
#include <iostream>
#include <cmath>

#include "vehicle.h"
#include "map.h"


double kPI = std::atan(1)*4;

/*
 * Vehicle class
 */

Vehicle::Vehicle() {
  is_initialized_ = false;
}

Vehicle::~Vehicle() {}

void Vehicle::updateParameters(const std::vector<double>& localization,
                               const Map& map) {
  px_ = localization[0];
  py_ = localization[1];
  vx_ = localization[2];
  vy_ = localization[3];
  ps_ = localization[4];
  pd_ = localization[5];

  lane_id_ = map.compute_lane_id(pd_);

  is_initialized_ = true;
}

void Vehicle::printout() const {
  std::cout << "Lane ID = " << lane_id_ << ", "
            << "px = " << px_ << ", " << "py = " << py_ << ", "
            << "vx = " << vx_ << ", " << "vy = " << vy_ << ", "
            << "ps = " << ps_ << ", " << "pd = " << pd_ << ", " << std::endl;
}

int Vehicle::getLaneID() const { return lane_id_; }

double Vehicle::getPx() const { return px_; }

double Vehicle::getPy() const { return py_; }

double Vehicle::getVx() const { return vx_; }

double Vehicle::getVy() const { return vy_; }

double Vehicle::getPs() const { return ps_; }

double Vehicle::getPd() const { return pd_; }
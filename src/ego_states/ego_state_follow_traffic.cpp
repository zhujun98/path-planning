//
// Created by jun on 7/27/17.
//
#include <iostream>
#include <vector>

#include "../ego.h"
#include "../map.h"
#include "ego_state_follow_traffic.h"
#include "../utilities.h"
#include "../ego_transition_states/ego_transition_state.h"


EgoStateFollowTraffic::EgoStateFollowTraffic() {
  transition_states_.push_back(EgoTransitionStateFactory::createState(FT_TO_CS));
  transition_states_.push_back(EgoTransitionStateFactory::createState(FT_TO_CLR));
  transition_states_.push_back(EgoTransitionStateFactory::createState(FT_TO_CLL));
}

EgoStateFollowTraffic::~EgoStateFollowTraffic() {}

void EgoStateFollowTraffic::onEnter(Ego& ego) {
  std::cout << "Enter state: *** FOLLOW TRAFFIC ***" << std::endl;
}

void EgoStateFollowTraffic::onUpdate(Ego &ego) {
  planPath(ego);
}

void EgoStateFollowTraffic::onExit(Ego& ego) {
  std::cout << "Exit state: *** FOLLOW TRAFFIC ***" << std::endl;
}

void EgoStateFollowTraffic::planPath(Ego& ego) {

  auto state0 = getState0(ego);
  double ps0 = state0.first[0];
  double vs0 = state0.first[1];

  double ps1, vs1, as1;
  double pd1, vd1, ad1;

  // get the distance and the speed of the front car
  std::vector<double> front_vehicle = ego.getClosestVehicle(ego.getLaneID(), 1);
  double prediction_time = 2.0;

  vs1 = ego.getTargetSpeed();
  ps1 = ps0 + 0.5*(vs0 + vs1)*prediction_time;
  if ( !front_vehicle.empty() ) {
    double ps_front = front_vehicle[0];
    double vs_front = front_vehicle[1];

    double ps1_tmp = ps_front + vs_front*prediction_time - ego.getMinSafeDistance();
    double vs1_tmp = 2*(ps1_tmp - ps0) - vs0;
    if ( vs1_tmp < ego.getTargetSpeed() ) {
      vs1 = vs1_tmp;
      ps1 = ps1_tmp;
    }
  }

  pd1 = (ego.getLaneID() - 0.5) * ego.getMap()->getLaneWidth();
  vd1 = 0;
  as1 = 0;
  ad1 = 0;

  std::vector<double> state1_s = {ps1, vs1, as1};
  std::vector<double> state1_d = {pd1, vd1, ad1};
  vehicle_state state1 = std::make_pair(state1_s, state1_d);

  PathPlanner planner(ego.getMaxSpeed(), ego.getMaxAcceleration(), ego.getMaxJerk());

  vehicle_trajectory new_path = planner.plan(state0, state1, prediction_time);

  ego.extendPath(new_path);
}

//
// Created by jun on 7/24/17.
//

#ifndef PATH_PLANNING_EGO_STATE_CHANGE_LANE_H
#define PATH_PLANNING_EGO_STATE_CHANGE_LANE_H

#include "ego_state.h"

class Ego;
class Map;


class EgoStateChangeLane : public EgoState {
private:

  int target_lane_id_;

  void planPath(Ego& ego);

  bool checkCollision(const Ego& ego);

public:
  //
  // constructor
  //
  // @param target_lane_id: target lane ID
  //
  EgoStateChangeLane(int target_lane_id);

  //
  // destructor
  //
  ~EgoStateChangeLane();

  void onEnter(Ego& ego);

  EgoState* onUpdate(Ego& ego);

  void onExit(Ego& ego);
};


#endif //PATH_PLANNING_EGO_STATE_CHANGE_LANE_H

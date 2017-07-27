//
// Created by jun on 7/24/17.
//

#ifndef PATH_PLANNING_EGO_STATE_CONSTANT_SPEED_H
#define PATH_PLANNING_EGO_STATE_CONSTANT_SPEED_H

#include <vector>

#include "ego_state.h"

class Ego;
class Map;


class EgoStateConstantSpeed : public EgoState {
private:

  double target_speed_;

  void planPath(Ego& ego);

  bool checkCollision(const Ego& ego);

public:
  //
  // constructor
  //
  EgoStateConstantSpeed();
  EgoStateConstantSpeed(double speed);

  //
  // destructor
  //
  ~EgoStateConstantSpeed();

  void onEnter(Ego& ego);

  EgoState* onUpdate(Ego& ego);

  void onExit(Ego& ego);
};


#endif //PATH_PLANNING_EGO_STATE_CONSTANT_SPEED_H

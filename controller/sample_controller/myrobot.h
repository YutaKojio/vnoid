﻿#pragma once

#include "robot.h"
#include "iksolver.h"
#include "footstep.h"
#include "footstep_planner.h"
#include "stepping_controller.h"
#include "stabilizer.h"

namespace cnoid{
namespace vnoid{

class MyRobot : public Robot{
public:
    double    standby_period;      ///< period of initial standby mode
	double    standby_com_height;  ///< com height in standby mode

    int       plan_cycle;
    bool      use_joystick;
    double    max_stride;
    double    max_turn;
    Joystick  joystick;

	Timer          timer;
    Param          param;
    Centroid       centroid;
    Base           base;
    vector<Hand>   hand;
    vector<Foot>   foot;
    vector<Joint>  joint;
    Footstep       footstep;

    FootstepPlanner     footstep_planner;
    SteppingController  stepping_controller;
    Stabilizer          stabilizer;
    IkSolver            ik_solver;

public:
	virtual void  Init   (SimpleControllerIO* io);
	virtual void  Control();
	
	MyRobot();

};

}
}
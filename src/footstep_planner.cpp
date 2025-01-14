﻿#include "footstep.h"
#include "footstep_planner.h"
#include "robot.h"
#include "rollpitchyaw.h"

namespace cnoid{
namespace vnoid{

const double eps = 1.0e-10;

///////////////////////////////////////////////////////////////////////////////////////////////////

FootstepPlanner::FootstepPlanner(){
}

void FootstepPlanner::Plan(const Param& param, Footstep& footstep){
    
    // we assume that foot placement, support foot flag, and dcm of step[0] are specified from outside

    // determine foot placement and support foot flag of remaining steps
    int nstep = footstep.steps.size();
	for(int i = 0; i < nstep-1; i++){
	    Step& st0 = footstep.steps[i+0];
	    Step& st1 = footstep.steps[i+1];

	    int sup =  st0.side;
	    int swg = !st0.side;

	    double  dtheta = st0.turn;
	    double  l      = st0.stride;
        double  d      = st0.sway;
	    double  w      = (sup == 0 ? 1.0 : -1.0) * st0.spacing;
	    double  dz     = st0.climb;
	    Vector3 dprel;

	    if(std::abs(dtheta) < eps){
		    dprel = Vector3(l, w + d, dz);
	    }
	    else{
		    double r = l/dtheta;
		    dprel = Vector3(
			    (r - w/2.0 - d)*sin(dtheta),
			    (r + w/2.0) - (r - w/2.0 - d)*cos(dtheta),
			    dz);
	    }

	    st1.side = !st0.side;
	    st1.foot_pos[sup] = st0.foot_pos[sup];
	    st1.foot_ori[sup] = st0.foot_ori[sup];
	    st1.foot_pos[swg] = st0.foot_pos[sup] + Eigen::AngleAxisd(st0.foot_ori[sup], Vector3::UnitZ())*dprel;
	    st1.foot_ori[swg] = st0.foot_ori[sup] + dtheta;
	}

    // generate reference dcm and zmp 
		
	// set final step's state
	int i = nstep-1;
	footstep.steps[i].zmp = (footstep.steps[i].foot_pos[0] + footstep.steps[i].foot_pos[1])/2.0;
	footstep.steps[i].dcm = (footstep.steps[i].foot_pos[0] + footstep.steps[i].foot_pos[1])/2.0;
	i--;

	// calc N-1 to 0 step's state
	for( ; i >= 0; i--) {
		int sup =  footstep.steps[i].side;
		int swg = !footstep.steps[i].side;
		
		double a = exp(-footstep.steps[i].duration/param.T);
		// for initial step, the dcm is specified from outside. determine zmp accordingly
		if(i == 0){
			footstep.steps[i].zmp = (footstep.steps[i].dcm - a*footstep.steps[i+1].dcm)/(1.0 - a);
		}
		// for other steps, set zmp to support foot, and determine dcm
		else{
			footstep.steps[i].zmp = footstep.steps[i].foot_pos[sup];
			footstep.steps[i].dcm = footstep.steps[i].zmp + a*(footstep.steps[i+1].dcm - footstep.steps[i].zmp);
		}
	}
}

}
}

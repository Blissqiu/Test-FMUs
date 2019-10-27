#include "config.h"
#include "model.h"
#include <math.h>
#include <stdlib.h>

/*

c_1: triggered, periodic
c_2: triggered, aperiodic
c_3: inferred, periodic
c_4: inferred, aperiodic

c_1 - - - - -
c_2 --  --  --
c_3 +   +   +
c_4 ++      ++
t   0123456789

c_1 = t % 2 == 0
c_2 = t % 4 == 0 || (t - 1) % 4 == 0

c_3 = t % 4 == 0
c_4 = t % 8 == 0 || (t - 1) % 8 == 0

*/


void setStartValues(ModelInstance *comp) {
    M(reference)  = 32767;
    M(position)   = 32767;
	M(Upi)        = 0;
	M(c1)         = 0;
	M(c2)         = 0;
	M(c1Ticks)    = 0;
	M(c2Ticks)    = 0;
	M(totalTicks) = 0;
}

void calculateValues(ModelInstance *comp) {
    // TODO
}

Status getUInt16(ModelInstance* comp, ValueReference vr, uint16_t *value, size_t *index) {
    
    switch (vr) {
        case vr_reference:
            *value = M(reference);
            break;
        case vr_position:
            *value = M(position);
            break;
        case vr_upi:
            *value = M(Upi);
            break;
        default:
            return Error;
    }
    
    return OK;
}

Status getFloat64(ModelInstance* comp, ValueReference vr, double *value, size_t *index) {
    
    calculateValues(comp);
        
    switch (vr) {
//        case vr_m:
//            value[(*index)++] = M(m);
//            return OK;
//        case vr_n:
//            value[(*index)++] = M(n);
//            return OK;
//        case vr_u:
//            value[(*index)++] = M(u)[0];
//            value[(*index)++] = M(u)[1];
//            value[(*index)++] = M(u)[2];
//            return OK;
//        case vr_A:
//            value[(*index)++] = M(A)[0][0];
//            value[(*index)++] = M(A)[0][1];
//            value[(*index)++] = M(A)[0][2];
//            value[(*index)++] = M(A)[1][0];
//            value[(*index)++] = M(A)[1][1];
//            value[(*index)++] = M(A)[1][2];
//            return OK;
//        case vr_y:
//            value[(*index)++] = M(y)[0];
//            value[(*index)++] = M(y)[1];
//            return OK;
        default:
            return Error;
    }
    
}

Status setFloat64(ModelInstance* comp, ValueReference vr, const double *value, size_t *index) {
    switch (vr) {
//        case vr_u:
//            M(u)[0] = value[(*index)++];
//            M(u)[1] = value[(*index)++];
//            M(u)[2] = value[(*index)++];
//            calculateValues(comp);
//            return OK;
        default:
            return Error;
    }
}

Status setUInt16(ModelInstance* comp, ValueReference vr, const uint16_t *value, size_t *index) {
    
    switch (vr) {
        case vr_reference:
            M(reference) = value[(*index)++];
            return OK;
        case vr_position:
            M(position) = value[(*index)++];
            return OK;
        case vr_upi:
            M(Upi) = value[(*index)++];
            return OK;
        default:
            return Error;
    }
    
    return OK;
}

void eventUpdate(ModelInstance *comp) {

	// update event time
	double c1_      = 2 * (comp->time / 2 + 1);
	double c2_even_ = 4 * (comp->time / 4 + 1);
	double c2_odd_  = 4 * (comp->time / 4 + 1) + 1;

	double nextEventTime = fmin(c1_, c2_even_);
	nextEventTime = fmin(nextEventTime, c2_odd_);

	// TODO: lockPreemption()

	// set triggered clocks
	M(c1) = fmod(comp->time, 2) == 0;
	M(c2) = fmod(comp->time, 4) == 0 || fmod(comp->time - 1, 4) == 0;

	// update the counters
	if (M(c1)) {
		M(c1Ticks)++;
		M(totalTicks)++;
	}

	if (M(c2)) {
		M(c2Ticks)++;
		M(totalTicks)++;
	}

	// TODO: unlockPreemption()

	comp->valuesOfContinuousStatesChanged   = false;
	comp->nominalsOfContinuousStatesChanged = false;
	comp->terminateSimulation               = false;
	comp->nextEventTime                     = nextEventTime;
	comp->nextEventTimeDefined              = true;
	comp->clocksTicked                      = M(c1) || M(c2);
}

Status activateClock(ModelInstance* comp, ValueReference vr) {
    return OK;
}

Status getClock(ModelInstance* comp, ValueReference vr, int* value) {

	switch (vr) {
	case vr_c1:
		*value = M(c1);
		return OK;
	case vr_c2:
		*value = M(c2);
		return OK;
	case vr_c3:
		*value = M(c3);
		return OK;
	case vr_c4:
		*value = M(c4);
		return OK;
	default:
		return Error;
	}

}

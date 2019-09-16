#include "config.h"
#include "model.h"


void setStartValues(ModelInstance *comp) {
    M(reference) = 32767;
    M(position)  = 32767;
    M(Upi)       = 0;
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
    comp->valuesOfContinuousStatesChanged   = false;
    comp->nominalsOfContinuousStatesChanged = false;
    comp->terminateSimulation               = false;
    comp->nextEventTimeDefined              = false;
}

Status activateClock(ModelInstance* comp, ValueReference vr) {
    return OK;
}

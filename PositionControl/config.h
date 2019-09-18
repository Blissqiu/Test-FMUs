#ifndef config_h
#define config_h

#include "stdint.h"

// define class name and unique id
#define MODEL_IDENTIFIER PositionControl
#define MODEL_GUID "{8c4e810f-3df3-4a00-8276-176fa3c9f000}"

// define model size
#define NUMBER_OF_STATES 0
#define NUMBER_OF_EVENT_INDICATORS 0

#define GET_FLOAT64
#define GET_UINT16
#define SET_FLOAT64
#define SET_UINT16
#define EVENT_UPDATE
#define ACTIVATE_CLOCK

#define FIXED_SOLVER_STEP 1

typedef enum {
    vr_reference,
    vr_position,
    vr_upi,
    vr_timing_event,
    vr_data_received_event
} ValueReference;

typedef struct {

    uint16_t reference;
    uint16_t position;
    double linearPosition;
    uint16_t Upi;

} ModelData;

#endif /* config_h */

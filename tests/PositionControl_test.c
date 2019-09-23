#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include "fmi3Functions.h"
#include "util.h"
#include "config.h"


#define CHECK_STATUS(S) status = S; if (status != fmi3OK) goto out;

#define N_CLOCKS  2
#define N_INPUTS  2
#define N_OUTPUTS 1

//////////////////////////
// Define callback

// Global variables
static fmi3IntermediateUpdateInfo updateInfo;
static const fmi3ValueReference vr_clocks[N_CLOCKS] = { vr_timing_event, vr_data_received_event };
static fmi3Boolean clocks_active[N_CLOCKS] = { fmi3False };
static int clockTick = 0;
static fmi3Float64 time = 0;
static const fmi3ValueReference vrInputs[N_INPUTS] = { vr_reference, vr_position };
static fmi3UInt16 inputs[N_INPUTS] = { 0 };
static const fmi3ValueReference vrOutputs[N_OUTPUTS] = { vr_upi };
static fmi3UInt16 outputs[N_OUTPUTS] = { 0 };

// Callback
fmi3Status cb_intermediateUpdate(fmi3InstanceEnvironment instanceEnvironment, fmi3IntermediateUpdateInfo* intermediateUpdateInfo) {
    // save intermediateUpdateInfo for later
    updateInfo = *intermediateUpdateInfo;
    return fmi3OK;
}

static const fmi3UInt16* calculateInputs() {
    inputs[0] = UINT16_MAX * sin(4 * M_PI * (time + 10e-3)); // 2 Hz sine with 10 ms phase shift
    inputs[1] = UINT16_MAX * sin(4 * M_PI * time);           // 2 Hz sine
    return inputs;
}

static const fmi3Boolean* calculateClocks() {
    clocks_active[0] = clockTick % 2 == 0; // tick every 2nd step
    clocks_active[1] = clockTick % 5 == 0; // tick evary 5th step
    return clocks_active;
}

static bool anyClockActive() {
    return clocks_active[0] || clocks_active[1];
}

int main(int argc, char* argv[]) {
    
    fmi3Status status = fmi3OK;
    
    const fmi3CallbackFunctions callbacks = {
        .instanceEnvironment = NULL,
        .logMessage          = cb_logMessage,
        .allocateMemory      = cb_allocateMemory,
        .freeMemory          = cb_freeMemory,
        .intermediateUpdate  = cb_intermediateUpdate,
        .lockPreemption      = NULL,
        .unlockPreemption    = NULL
    };
    
    printf("Running Clocked Co-Simulation example... ");

    //////////////////////////
    // Initialization sub-phase

    // Set callback functions,
    fmi3EventInfo s_eventInfo;

    //set Co-Simulation mode
    const fmi3CoSimulationConfiguration csConfig = {
        .intermediateVariableGetRequired         = fmi3False,
        .intermediateInternalVariableGetRequired = fmi3False,
        .intermediateVariableSetRequired         = fmi3False,
        .coSimulationMode                        = fmi3ModeHybridCoSimulation
    };

    // Instantiate slave
    const fmi3Instance s = fmi3Instantiate("instance", fmi3CoSimulation, MODEL_GUID, "", &callbacks, fmi3False, fmi3True, &csConfig);

    if (s == NULL) {
        status = fmi3Error;
        goto out;
    }

    // Start and stop time
    const fmi3Float64 stopTime  = 1000e-3; // 1000 ms
    // Communication constant step size
    const fmi3Float64 stepSize = 500e-6; // 500 us

    // set all variable start values

    // Initialize slave
    CHECK_STATUS(fmi3SetupExperiment(s, fmi3False, 0.0, time, fmi3True, stopTime))
    CHECK_STATUS(fmi3EnterInitializationMode(s))
    // Set the input values at time = startTime
    CHECK_STATUS(fmi3SetUInt16(s, vrInputs, N_INPUTS, calculateInputs(), N_INPUTS))
    CHECK_STATUS(fmi3ExitInitializationMode(s))

    //////////////////////////
    // Simulation sub-phase
    fmi3Float64 step = stepSize; // Starting non-zero step size
    
    bool eventMode = true;

    while (time < stopTime) {
        
        calculateClocks();

        if (anyClockActive()) {
            
            /* set possible active inferred clocks to true or to false*/
            
            if(!eventMode) {
                CHECK_STATUS(fmi3EnterEventMode(s))
                eventMode = true;
            };
            
            CHECK_STATUS(fmi3SetClock(s, vr_clocks, N_CLOCKS, clocks_active, NULL));
            
            // fmi3SetInterval(s, ...); /* Only needed if interval changes */
        };

        if (eventMode) {
            CHECK_STATUS(fmi3NewDiscreteStates(s, &s_eventInfo))
        } else {
            // Continuous mode (default mode)
            fmi3Float64 tend = time + step;
            fmi3Float64 t = tend * 2;
            fmi3Boolean earlyReturn = fmi3False;
            
            CHECK_STATUS(fmi3DoStep(s, time, step, fmi3False, &earlyReturn))

            if (earlyReturn) {
                t = updateInfo.intermediateUpdateTime;
                /* rollback FMUs to earliest event time */
                CHECK_STATUS(fmi3EnterEventMode(s))
                eventMode = true;
                time = t;
            } else{
                time = tend;
            }
        }

        if (updateInfo.clocksTicked) {
            // fmi3GetClock(s, ...);
            // fmi3GetInterval(s, /*Intervals*/, ...);
        };
        
        if (eventMode && !s_eventInfo.newDiscreteStatesNeeded && !anyClockActive()) {
            CHECK_STATUS(fmi3EnterContinuousTimeMode(s))
            eventMode = false;
            // step = min(/*Intervals*/, s_eventInfo.nextEventTime, ...);
        };

        // Get outputs
        CHECK_STATUS(fmi3GetUInt16(s, vrOutputs, 1, outputs, 1))

        // Set inputs
        CHECK_STATUS(fmi3SetUInt16(s, vrInputs, N_INPUTS, calculateInputs(), N_INPUTS))
    };

    fmi3Status terminateStatus;
out:
    
    if (s && status != fmi3Error && status != fmi3Fatal) {
        terminateStatus = fmi3Terminate(s);
    }
    
    if (s && status != fmi3Fatal && terminateStatus != fmi3Fatal) {
        fmi3FreeInstance(s);
    }
    
    return status == fmi3OK ? EXIT_SUCCESS : EXIT_FAILURE;
}

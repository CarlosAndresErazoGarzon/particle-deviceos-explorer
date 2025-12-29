#include "ema_filter.h"

void ema_init(EmaFilter* filter, float alpha) {
    filter->alpha = alpha;
    filter->previousOutput = 0.0f;
    filter->initialized = 0;
}

float ema_apply(EmaFilter* filter, float input) {
    if (filter->initialized == 0) {
        filter->previousOutput = input;
        filter->initialized = 1;
        return input;
    }

    // Out = (Alpha * In) + ((1 - Alpha) * PrevOut)
    float output = (filter->alpha * input) + ((1.0f - filter->alpha) * filter->previousOutput);
    
    filter->previousOutput = output;
    
    return output;
}
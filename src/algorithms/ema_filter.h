/*
 * File: src/algorithms/ema_filter.h
 */

#ifndef EMA_FILTER_H
#define EMA_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float alpha;          
    float previousOutput; 
    int initialized;      
} EmaFilter;

void ema_init(EmaFilter* filter, float alpha);
float ema_apply(EmaFilter* filter, float input);

#ifdef __cplusplus
}
#endif

#endif // EMA_FILTER_H
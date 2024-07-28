#include "LowpassFilter.h"

LowpassFilter::LowpassFilter(float alpha) : alpha(alpha), filtered_value(0.0) {}

float LowpassFilter::filter(float new_value) {
    filtered_value = alpha * new_value + (1 - alpha) * filtered_value;
    return filtered_value;
}


#ifndef LowpassFilter_h
#define LowpassFilter_h

class LowpassFilter {
    public:
        LowpassFilter(float alpha);
        float filter(float new_value);
    private:
        float alpha;
        float filtered_value;
};

#endif



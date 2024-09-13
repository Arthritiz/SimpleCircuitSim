#ifndef Circuit_h
#define Circuit_h

class Circuit {
public:
    virtual ~Circuit() = default;
    virtual void prepare(double) = 0;
    virtual double process(double) = 0;
};

#endif /* Circuit_h */

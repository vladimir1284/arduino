#define DEPTH 50

class MeanFilter
{
public:
    MeanFilter();
    float getValue();
    void insertValue(float val);

private:
    float data[DEPTH];
    float cumul;
    int   cursor;
};
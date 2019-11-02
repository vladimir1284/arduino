#define DEPTH 50

class MeanFilter
{
public:
    MeanFilter();
    int getValue();
    void insertValue(int val);

protected:
    int data[DEPTH];
    int cumul,
        cursor;
};
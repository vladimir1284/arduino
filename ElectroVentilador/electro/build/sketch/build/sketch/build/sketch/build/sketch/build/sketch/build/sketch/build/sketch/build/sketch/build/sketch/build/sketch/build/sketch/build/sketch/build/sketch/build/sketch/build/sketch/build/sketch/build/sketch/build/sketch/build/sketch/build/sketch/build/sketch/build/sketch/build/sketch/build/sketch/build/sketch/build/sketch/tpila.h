#define DEPTH 50

class TPila {

public:
    TPila();
    void push(int val);
    int pop(),
        get(int n);
    bool empty();


private:
    int data[DEPTH];
    int tope;


};
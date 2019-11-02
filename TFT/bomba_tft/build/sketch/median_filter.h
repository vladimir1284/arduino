#define SIZE 10

class MedianFilter
{

public:
  MedianFilter();

  void insertValue(int newVal);

  int getValue();

protected:
  int bufferRing[SIZE]; /**< Array of the ellemnts in their FIFO order */
  int bufferPtrs[SIZE]; /**< Array of pointers to the next element in ascending order */
  int head,             /**< Position to insert the next element */
      first;            /**< Index of the first pointer */

  void removeOldestValue(int oldestIndex),
      insertNewValue(int newVal);
};

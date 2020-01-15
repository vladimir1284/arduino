#include "median_filter.h"

MedianFilter::MedianFilter(){
  int i;
  for(i = 0; i < SIZE; i++){
    bufferRing[i] = 0;
    bufferPtrs[i] = i+1;
  }
  first = 0;
  head  = 0;}

//----------------------------------------------------------------------------------
int MedianFilter::getValue(){
	int j, i = first;
	for(j = 0; j < SIZE/2; j++){
		i = bufferPtrs[i];
	}
  return bufferRing[i];
}

void MedianFilter::insertValue(int newVal){

  // Delete oldest value
  removeOldestValue(head);

  // Insert in the ring and move the ptr for the next element
  insertNewValue(newVal);
}

//----------------------------------------------------------------------------------
void MedianFilter::removeOldestValue(int oldestIndex){
  // Evaluate the first value
  if (oldestIndex == first){
    first = bufferPtrs[first];
  } else {    
    int i = first;
    while(bufferPtrs[i] < SIZE){
      if (oldestIndex == bufferPtrs[i]){ // Evaluate the next element
        bufferPtrs[i] = bufferPtrs[oldestIndex]; // Eliminate the next element
        break; // Stop search
      }
      i = bufferPtrs[i];
    }
  }
}

//----------------------------------------------------------------------------------
void MedianFilter::insertNewValue(int newValue){
  int i = first;
  bool stored = false;

  // Evaluate the first value
  if (newValue < bufferRing[first]){
    bufferPtrs[head] = first; // New element point to the current first
    first = head; // New element become first
    stored = true;
  }

  // Iterate over the linked sorted list
  while (!stored){
    if (bufferPtrs[i] == SIZE) { // Current element is the last one
      bufferPtrs[i] = head; // Make the new element the last one
      bufferPtrs[head] = SIZE;
      stored = true;      
    } 
    if (!stored){
      if (newValue < bufferRing[bufferPtrs[i]]){// Evaluate the next element
        stored = true;
        bufferPtrs[head] = bufferPtrs[i]; // New element point to the next element
        bufferPtrs[i] = head; // Current element points to the new elemnt
      }
    }
    i = bufferPtrs[i];
  }
  
  // Store in the ring
  bufferRing[head] = newValue;
  head = (head+1)%SIZE;
}

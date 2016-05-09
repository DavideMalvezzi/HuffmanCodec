
#include "BitUtils.h"

word setBit(word b, int bit){
  b = b | (1 << bit);
  return b;
}

word clearBit(word b, int bit){
  b = b & (~(1 << bit));
  return b;
}

int readBit(word b, int bit){
  return b & (1 << bit);
}

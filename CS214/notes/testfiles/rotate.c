#include <stdlib.h>
#include <stdio.h>

unsigned int rotate_right(unsigned int x, unsigned int shift) {
  return (x >> shift) | (x << (32 - shift));
}

unsigned int rotate_left(unsigned int x, unsigned int shift) {
  // 1001101011 << 3 shift left by shift
  // 1101011000

  // 1001101011 >> 29 shift right by 32-shift
  // 0000000100

  //  1101011000
  //| 0000000100
  //  1101011100 
  return (x << shift) | (x >> (32 - shift));
}


int main() {
  
  return EXIT_SUCCESS;

}

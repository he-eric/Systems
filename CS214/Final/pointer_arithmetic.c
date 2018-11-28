#include <stdio.h>
#define ARSZ 10

int
main(){
  float fa[ARSZ], *fp1, *fp2;

  fp1 = fp2 = fa; /* address of first element */
  while(fp2 != &fa[ARSZ]){
    printf("Difference: %ld\n", (long)(fp2-fp1));
    printf("fp2: %p fp1: %p\n", fp2, fp1);
    fp2++;
  }

  void *vp = fa;
  printf("vp: %p\n", vp);

  return 0;
}

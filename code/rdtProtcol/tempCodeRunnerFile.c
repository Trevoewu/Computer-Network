#include<stdio.h>
int main(){
  int num = 1;
  while(1){
     num = (~(num));
    printf("num: %d\n",num);
  }
}
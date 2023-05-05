#include<stdio.h>
int findMin(int arr[], int len) {
  int min = arr[0];
  for (int i = 0; i < len; i++) {
    if (arr[i] < min) {
      min = arr[i];
    }
  }
  return min;
}
int main() {
  int arr[4] = {1, -1, 3, 4};
  printf("min:%d", findMin(arr, 4));
}
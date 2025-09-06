#include <stdio.h>

int Fibonacci(int n){
  if(n<=1) return 1;
  return Fibonacci(n-2)+Fibonacci(n-1);
}

int main(int argc,char** argv){
  int f = Fibonacci(8);
  printf("Fibonacci: %d",f);
  return 0;
}
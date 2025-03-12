#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void printUsage() {}

int main(int argc, char *argv[]) {

  printf("%d\n", argc);
  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }

  int retc;
  // while ((retc = getopt(argc, argv, "a:b")) != -1) {
  //   if (retc == 'a') {
  //     printf("a, %c, %s\n", retc, optarg);
  //   }
  //   if (retc == 'b') {
  //     printf("b, %c, %s\n", retc, optarg);
  //   }
  // }

  struct option long_options[5] = {{"add", required_argument, 0, 0},
                                   {"minus", required_argument, 0, 0}};

  return 0;
}

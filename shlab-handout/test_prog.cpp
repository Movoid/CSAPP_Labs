/*================================*\

  test_prog -
  this is a simple signal program,
  only for testing the tsh.

\*================================*/

#include <bits/stdc++.h>
using namespace std;

void handle_SIGINT(int sig) {
  write(STDOUT_FILENO, "111\n", 4);
}

int main() {

  signal(SIGINT, handle_SIGINT);
  sleep(100); // NOLINT

  sigset_t empty;
  sigemptyset(&empty);
  while (true) {
    sigsuspend(&empty); // NOLINT
  }
}
#include <bits/stdc++.h>

using namespace std;

char cookie[]{"59b997fa"};

int main() {

  for (int i = 0; i < 9; i++) {
    cout << hex << static_cast<unsigned>(cookie[i]) << ' ';
  }
  cout << endl;
}
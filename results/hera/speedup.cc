
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

int
main(int argc, char *argv[])
{
  int k, iterations = 10;
  double d, speedup;
  vector<double> v, m;
  if (argc > 1) {
    iterations = atoi(argv[1]);
  }
  for (int j = 0; j < 5; ++j) {
    v.clear();
    cin >> k;
    for (int i = 0; i < iterations; ++i) {
      cin >> d;
      v.push_back(d);
    }
    m.push_back(accumulate(v.begin(), v.end(), 0.0)/v.size());
  }
  for (int i = 0; i < 5; ++i) {
    speedup = m[0]/m[i];
    k       = (1 << i);
    cout << k << " " << speedup << endl;
    cerr << k << " " << speedup/k << endl; /* efficiency */
  }
  return 0;
}


#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

int
main()
{
  double d;
  vector<double> v, m;
  for (int j = 0; j < 5; ++j) {
    v.clear();
    for (int i = 0; i < 10; ++i) {
      cin >> d;
      v.push_back(d);
    }
    m.push_back(accumulate(v.begin(), v.end(), 0.0)/v.size());
  }
  
  for (int i = 0; i < 5; ++i) {
    cout << (1 << i) << " " << m[0]/m[i] << endl;
  }
  
  return 0;
}

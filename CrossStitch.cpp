// C++11
#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <string.h>
#include <cassert>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <string>

using namespace std;
typedef long long ll;

const ll CYCLE_PER_SEC = 2400000000;
double TIME_LIMIT = 10.0;

unsigned long long xor128(){
  static unsigned long long rx=123456789, ry=362436069, rz=521288629, rw=88675123;
  unsigned long long rt = (rx ^ (rx<<11));
  rx=ry; ry=rz; rz=rw;
  return (rw=(rw^(rw>>19))^(rt^(rt>>8)));
}

unsigned long long int getCycle() {
  unsigned int low, high;
  __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
  return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
}

double getTime(unsigned long long int begin_cycle) {
  return (double)(getCycle() - begin_cycle) / CYCLE_PER_SEC;
}

// ↘ ↙ ↖ ↗
const int DY[4] = {1, 1, -1, -1};
const int DX[4] = {1, -1, -1, 1};

struct Point {
  int y;
  int x;
};

struct Pinhole {
  Point start_p;
  Point end_p;
};

ll startCycle;
int S;

class CrossStitch {
  public:
    void init (vector<string> pattern) {
      S = pattern.size();
    }

    vector<string> embroider(vector<string> pattern) {
      vector<string> ret;
      init(pattern);
      // for each color, for each cell (r, c) do two stitches (r+1, c)-(r, c+1)-(r+1, c+1)-(r, c)
      for (char col = 'a'; col <= 'z'; ++col) {
        bool first = true;
        for (int r = 0; r < S; ++r)
          for (int c = 0; c < S; ++c)
            if (pattern[r][c] == col) {
              if (first) {
                ret.push_back(string(1, col));
                first = false;
              }
              ret.push_back(to_string(r+1) + " " + to_string(c));
              ret.push_back(to_string(r) + " " + to_string(c+1));
              ret.push_back(to_string(r+1) + " " + to_string(c+1));
              ret.push_back(to_string(r) + " " + to_string(c));
            }
      }
      return ret;
    }

    void createFirstPath() {
      int bestLength = 0;
      int length = 0;
    }

    void needlework() {
      startCycle = getCycle();

      double currentTime = getTime(startCycle);

      while (currentTime < TIME_LIMIT) {

        currentTime = getTime(startCycle);
      }
    }

    int calcThreadLength (vector<Pinhole> &npoints) {
      int length = 0;
      int size = npoints.size();

      for (int i = 1; i < size; i++) {
        Pinhole ph1 = npoints[i-1];
        Pinhole ph2 = npoints[i];

        int dy = ph2.start_p.y - ph1.end_p.y;
        int dx = ph2.start_p.x - ph1.end_p.x;
        length += sqrt(dy*dy + dx*dx);
      }

      return length;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
  for (int i = 0; i < v.size(); ++i) { cin >> v[i]; }
}
int main() {
  int S;
  cin >> S;
  TIME_LIMIT = 2.0;
  vector<string> pattern(S);
  getVector(pattern);
  CrossStitch cs;
  vector<string> ret = cs.embroider(pattern);
  cout << ret.size() << endl;
  for (int i = 0; i < (int)ret.size(); ++i) { cout << ret[i] << endl; }
  cout.flush();
}

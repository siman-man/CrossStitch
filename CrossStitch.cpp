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

  Point(int y = -1, int x = -1) {
    this->y = y;
    this->x = x;
  }

  string to_s() {
    return to_string(y) + " " + to_string(x);
  }
};

struct PinHole {
  char color;
  Point start_p;
  Point end_p;

  PinHole(char color = -1, Point sp = Point(), Point ep = Point()) {
    this->color = color;
    this->start_p = sp;
    this->end_p = ep;
  }

  void swap() {
    Point temp = end_p;
    end_p = start_p;
    start_p = temp;
  }
};

ll startCycle;
int S;
int N;
int C;
vector<string> g_pattern;
unordered_map<char, int> g_colorSize;

class CrossStitch {
  public:
    void init (vector<string> pattern) {
      S = pattern.size();
      C = 0;
      N = 0;
      g_pattern = pattern;

      for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
          char color = pattern[y][x];
          if (color == '.') continue;

          N++;
          g_colorSize[color]++;
          C = max(C, color - 'a' + 1);
        }
      }

      fprintf(stderr,"S = %d, C = %d\n", S, C);
    }

    vector<string> embroider(vector<string> pattern) {
      vector<string> ret;

      init(pattern);

      vector<PinHole> path = createFirstPath();
      vector<string> answer = path2answer(path);

      fprintf(stderr,"CurrentScore = %f\n", calcScore(path));

      return answer;
    }

    double calcScore(vector<PinHole> points) {
      int psize = points.size();
      double L = N * 2 * sqrt(2);
      double W = calcThreadLength(points);

      fprintf(stderr,"W = %f, L = %f\n", W, L);

      return max(0.0, pow((5 - W / L) / 5, 3));
    }

    vector<string> path2answer(vector<PinHole> points) {
      int psize = points.size();
      vector<string> answer;
      unordered_map<char, bool> checkList;

      for (int i = 0; i < psize; i++) {
        PinHole ph = points[i];
        if (!checkList[ph.color]) {
          checkList[ph.color] = true;
          answer.push_back(string(1, ph.color));
        }
        answer.push_back(ph.start_p.to_s());
        answer.push_back(ph.end_p.to_s());
      }

      return answer;
    }

    vector<PinHole> createFirstPath() {
      vector<PinHole> path;

      for (char color = 'a'; color <= 'z'; color++) {
        for (int y = 0; y < S; y++) {
          for (int x = 0; x < S; x++) {
            if (g_pattern[y][x] == color) {
              Point sp1(y, x);
              Point ep1(y+1, x+1);
              path.push_back(createPinHole(color, sp1, ep1));

              Point sp2(y+1, x);
              Point ep2(y, x+1);
              path.push_back(createPinHole(color, sp2, ep2));
            }
          }
        }
      }

      return path;
    }

    PinHole createPinHole(char color, Point sp, Point ep) {
      return PinHole(color, sp, ep);
    }

    void needlework() {
      startCycle = getCycle();
      int bestLength = 0;
      int length = 0;

      double currentTime = getTime(startCycle);
      ll tryCount = 0;

      while (currentTime < TIME_LIMIT) {

        currentTime = getTime(startCycle);
        tryCount++;
      }

      fprintf(stderr, "tryCount = %lld\n", tryCount);
    }

    double calcThreadLength (vector<PinHole> &npoints) {
      double length = 0.0;
      int size = npoints.size();

      for (int i = 0; i < size-1; i++) {
        PinHole ph1 = npoints[i];
        PinHole ph2 = npoints[i+1];

        if (ph1.color != ph2.color) continue;

        int dy = ph1.end_p.y - ph2.start_p.y;
        int dx = ph1.end_p.x - ph2.start_p.x;
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
  for (int i = 0; i < (int)ret.size(); ++i) {
    cout << ret[i] << endl;
  }
  cout.flush();
}

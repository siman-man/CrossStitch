// C++11
#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <string.h>
#include <cassert>
#include <map>
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

  void swapHole() {
    Point temp = end_p;
    end_p = start_p;
    start_p = temp;
  }

  double afterLength(PinHole ph) {
    int dy = end_p.y - ph.start_p.y;
    int dx = end_p.x - ph.start_p.x;
    return sqrt(dy*dy + dx*dx);
  }

  double beforeLength(PinHole ph) {
    int dy = ph.end_p.y - start_p.y;
    int dx = ph.end_p.x - start_p.x;
    return sqrt(dy*dy + dx*dx);
  }
};

ll startCycle;
int S;
int N;
int C;
vector<string> g_pattern;
map<char, int> g_colorSize;
map<char, vector<PinHole> > g_paths;

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

      fprintf(stderr,"S = %d, C = %d, N = %d\n", S, C, N);
    }

    vector<string> embroider(vector<string> pattern) {
      vector<string> ret;

      init(pattern);

      createFirstPath();

      for (int i = 0; i < C; i++) {
        char color = 'a' + i;
        g_paths[color] = needlework(g_paths[color], TIME_LIMIT/C);
      }

      vector<PinHole> path = createPath();
      vector<string> answer = createAnswer();

      fprintf(stderr,"CurrentScore = %f\n", calcScore(path));

      return answer;
    }

    double calcScore(vector<PinHole> points) {
      double L = N * 2 * sqrt(2);
      double W = calcThreadLength(points);

      fprintf(stderr,"W = %f, L = %f\n", W, L);

      return max(0.0, pow((5 - W / L) / 5, 3));
    }

    vector<PinHole> createPath() {
      vector<PinHole> paths;

      map<char, vector<PinHole> >::iterator it = g_paths.begin();

      while (it != g_paths.end()) {
        vector<PinHole> path = (*it).second;
        int psize = path.size();

        for (int i = 0; i < psize; i++) {
          paths.push_back(path[i]);
        }

        it++;
      }

      return paths;
    }

    vector<string> createAnswer() {
      vector<string> answer;
      map<char, vector<PinHole> >::iterator it = g_paths.begin();

      while (it != g_paths.end()) {
        char color = (*it).first;
        vector<PinHole> path = (*it).second;
        int psize = path.size();

        for (int i = 0; i < psize; i++) {
          if (i == 0) {
            answer.push_back(string(1, color));
          }
          PinHole ph = path[i];
          answer.push_back(ph.start_p.to_s());
          answer.push_back(ph.end_p.to_s());
        }

        it++;
      }

      return answer;
    }

    void cleanPath(vector<PinHole> &path) {
      int psize = path.size();

      for (int i = 0; i < psize-1; i++) {
        PinHole *ph1 = &path[i];
        PinHole *ph2 = &path[i+1];

        if (ph1->end_p.y == ph2->start_p.y && ph1->end_p.x == ph2->start_p.x) {
          ph2->swapHole();
        }
      }
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
              g_paths[color].push_back(createPinHole(color, sp1, ep1));

              Point sp2(y+1, x);
              Point ep2(y, x+1);
              path.push_back(createPinHole(color, sp2, ep2));
              g_paths[color].push_back(createPinHole(color, sp2, ep2));
            }
          }
        }
        cleanPath(g_paths[color]);
      }

      return path;
    }

    PinHole createPinHole(char color, Point sp, Point ep) {
      return PinHole(color, sp, ep);
    }

    vector<PinHole> needlework(vector<PinHole> path, double LIMIT = 2.0) {
      startCycle = getCycle();
      int minLength = calcThreadLength(path);
      int goodLength = minLength;
      int length = 0;
      int psize = path.size();
      vector<PinHole> bestPath = path;
      ll sc = getCycle();

      double currentTime = getTime(sc);
      ll tryCount = 0;
      int R = 1000000;
      double k = 0.5;

      while (currentTime < LIMIT) {
        double remainTime = LIMIT - currentTime;
        int i = xor128()%psize;
        int j = xor128()%psize;

        double diffLength = swapPinHole(i, j, path);
        length = goodLength + diffLength;

        if (minLength > length) {
          bestPath = path;
          minLength = length;
        }
        if (goodLength > length || (xor128()%R < R*exp(-diffLength/(k*remainTime)))) {
          goodLength = length;
        } else {
          swapPinHole(i, j, path);
        }

        currentTime = getTime(sc);
        tryCount++;

        if (tryCount % 1000000 == 0 && diffLength > 0) {
          fprintf(stderr,"diff = %f, rate = %f\n", diffLength, exp(-diffLength/(k*remainTime)));
        }
      }

      fprintf(stderr, "tryCount = %lld\n", tryCount);
      return bestPath;
    }

    double swapPinHole(int i, int j, vector<PinHole> &path) {
      PinHole ph1 = path[i];
      PinHole ph2 = path[j];
      double beforeLength = calcLength(i, path) + calcLength(j, path);
      path[i] = ph2;
      path[j] = ph1;
      double afterLength = calcLength(i, path) + calcLength(j, path);

      return afterLength - beforeLength;
    }

    double calcLength(int i, vector<PinHole> &path) {
      double length = 0.0;
      int psize = path.size();

      if (i > 0) {
        PinHole ph1 = path[i];
        PinHole ph2 = path[i-1];
        length += ph1.beforeLength(ph2);
      }

      if (i < psize-1) {
        PinHole ph1 = path[i];
        PinHole ph2 = path[i+1];
        length += ph1.afterLength(ph2);
      }

      return length;
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

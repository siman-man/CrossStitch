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

  double dist(Point p) {
    int dy = y - p.y;
    int dx = x - p.x;
    return sqrt(dy*dy + dx*dx);
  }

  bool operator ==(Point p) {
    return y == p.y && x == p.x;
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
};

ll startCycle;
int S;
int N;
int C;
vector<string> g_pattern;
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
          C = max(C, color - 'a' + 1);
        }
      }

      fprintf(stderr,"S = %d, C = %d, N = %d\n", S, C, N);
    }

    vector<string> embroider(vector<string> pattern) {
      vector<string> ret;

      init(pattern);

      createNNPath();

      for (int i = 0; i < C; i++) {
        char color = 'a' + i;
        //g_paths[color] = simpleNeedlework(g_paths[color], TIME_LIMIT/C);
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
        if (path[i].end_p == path[i+1].start_p) {
          path[i+1].swapHole();
        }
      }
    }

    void createNNPath() {
      map<char, vector<Point> > colorCoords;

      for (char color = 'a'; color <= 'z'; color++) {
        for (int y = 0; y < S; y++) {
          for (int x = 0; x < S; x++) {
            if (g_pattern[y][x] != color) continue;
            colorCoords[color].push_back(Point(y,x));
          }
        }
      }

      map<char, vector<Point> >::iterator it = colorCoords.begin();
      while (it != colorCoords.end()) {
        char color = (*it).first;
        vector<Point> coords = (*it).second;
        Point bp = coords[0];
        coords.erase(coords.begin());
        int csize = coords.size();

        for (int i = 0; i < csize; i++) {
          double minDist = 99999.0;
          int minId = 0;
          int bsize = coords.size();

          Point sp1(bp.y, bp.x);
          Point ep1(bp.y+1, bp.x+1);
          g_paths[color].push_back(createPinHole(color, sp1, ep1));

          Point sp2(bp.y+1, bp.x);
          Point ep2(bp.y, bp.x+1);
          g_paths[color].push_back(createPinHole(color, sp2, ep2));

          for (int j = 0; j < bsize; j++) {
            double dist = bp.dist(coords[j]);
            if (minDist > dist) {
              minDist = dist;
              minId = j;
            }
          }

          bp = coords[minId];
          coords.erase(coords.begin() + minId);
        }

        Point sp1(bp.y, bp.x);
        Point ep1(bp.y+1, bp.x+1);
        g_paths[color].push_back(createPinHole(color, sp1, ep1));

        Point sp2(bp.y+1, bp.x);
        Point ep2(bp.y, bp.x+1);
        g_paths[color].push_back(createPinHole(color, sp2, ep2));

        cleanPath(g_paths[color]);

        it++;
      }
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
      double k = 0.4;
      double diffLength = 0.0;

      while (currentTime < LIMIT) {
        double remainTime = LIMIT - currentTime;
        int ope = xor128()%3;
        int i = xor128()%psize;
        int j = xor128()%psize;

        if (i == j) continue;

        switch(ope) {
          case 0:
            swapPinHoleNonDiff(i, j, path);
            if (isCorrectHole(i, path) && isCorrectHole(j, path)) {
              diffLength = swapPinHole(i, j, path);
            } else {
              swapPinHoleNonDiff(i, j, path);
              continue;
            }
            break;
          case 1:
            diffLength = switchHole(i, path);
            if (!isCorrectHole(i, path)) {
              path[i].swapHole();
              continue;
            }
            break;
          case 2:
            insertPinHole(i, j, path);
            if (isCorrectHole(i, path) && isCorrectHole(j, path)) {
              diffLength = calcThreadLength(path) - goodLength;
            } else {
              insertPinHole(j, i, path);
              continue;
            }
            break;
          case 3:
            resolveConflict(i, j, path);
            if (isCorrectPath(path)) {
              diffLength = calcThreadLength(path) - goodLength;
            } else {
              resolveConflict(i, j, path);
              continue;
            }
            break;
        }

        length = goodLength + diffLength;

        if (minLength > length) {
          bestPath = path;
          minLength = length;
        }

        if (goodLength > length || (xor128()%R < R*exp(-diffLength/(k*remainTime)))) {
          goodLength = length;
        } else {
          switch(ope) {
            case 0:
              swapPinHoleNonDiff(i, j, path);
              break;
            case 1:
              path[i].swapHole();
              break;
            case 2:
              insertPinHole(j, i, path);
              break;
            case 3:
              resolveConflict(i, j, path);
              break;
          }
        }

        currentTime = getTime(sc);
        tryCount++;

        if (tryCount % 1000000 == 0 && diffLength > 0) {
          //fprintf(stderr,"diff = %f, rate = %f\n", diffLength, exp(-diffLength/(k*remainTime)));
        }
      }

      //cleanPath(bestPath);

      //fprintf(stderr, "tryCount = %lld\n", tryCount);
      return bestPath;
    }

    vector<PinHole> simpleNeedlework(vector<PinHole> path, double LIMIT = 2.0) {
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
      double k = 0.4;
      double diffLength = 0.0;

      while (currentTime < LIMIT) {
        double remainTime = LIMIT - currentTime;
        int ope = xor128()%4;
        int i = xor128()%psize;
        int j = xor128()%psize;

        if (i == j) continue;

        switch(ope) {
          case 0:
            diffLength = swapPinHole(i, j, path);
            break;
          case 1:
            diffLength = switchHole(i, path);
            break;
          case 2:
            insertPinHole(i, j, path);
            diffLength = calcThreadLength(path) - goodLength;
            break;
          case 3:
            resolveConflict(i, j, path);
            diffLength = calcThreadLength(path) - goodLength;
            break;
        }

        length = goodLength + diffLength;

        if (minLength > length) {
          bestPath = path;
          minLength = length;
        }

        if (goodLength > length || (xor128()%R < R*exp(-diffLength/(k*remainTime)))) {
          goodLength = length;
        } else {
          switch(ope) {
            case 0:
              swapPinHoleNonDiff(i, j, path);
              break;
            case 1:
              path[i].swapHole();
              break;
            case 2:
              insertPinHole(j, i, path);
              break;
            case 3:
              resolveConflict(i, j, path);
              break;
          }
        }

        currentTime = getTime(sc);
        tryCount++;
      }

      cleanPath(bestPath);

      //fprintf(stderr, "tryCount = %lld\n", tryCount);
      return bestPath;
    }

    bool isCorrectPath(vector<PinHole> &path) {
      int psize = path.size();

      for (int i = 0; i < psize; i++) {
        if (!isCorrectHole(i, path)) return false;
      }

      return true;
    }

    bool isCorrectHole(int i, vector<PinHole> &path) {
      if (i > 0) {
        if (path[i].start_p == path[i-1].end_p) return false;
      }

      if (i < path.size()-1) {
        if (path[i].end_p == path[i+1].start_p) return false;
      }

      return true;
    }

    void insertPinHole(int i, int j, vector<PinHole> &path) {
      PinHole ph = path[i];

      path.erase(path.begin()+i);
      path.insert(path.begin()+j, ph);
    }

    void resolveConflict(int i, int j, vector<PinHole> &path) {
      if (i > j) {
        int t = i;
        i = j;
        j = t;
      }

      while (i < j) {
        PinHole temp = path[i];
        path[i] = path[j];
        path[j] = temp;
        i++;
        j--;
      }
    }

    double switchHole(int i, vector<PinHole> &path) {
      double beforeLength = calcLength(i, path);
      path[i].swapHole();
      double afterLength = calcLength(i, path);
      return afterLength - beforeLength;
    }

    double swapPinHole(int i, int j, vector<PinHole> &path) {
      PinHole temp = path[i];
      double beforeLength = calcLength(i, path) + calcLength(j, path);

      path[i] = path[j];
      path[j] = temp;

      double afterLength = calcLength(i, path) + calcLength(j, path);

      return afterLength - beforeLength;
    }

    void swapPinHoleNonDiff(int i, int j, vector<PinHole> &path) {
      PinHole temp = path[i];
      path[i] = path[j];
      path[j] = temp;
    }

    double calcLength(int i, vector<PinHole> &path) {
      double length = 0.0;
      PinHole ph2 = path[i];

      if (i > 0) {
        PinHole ph1 = path[i-1];
        length += ph2.start_p.dist(ph1.end_p);
      }

      if (i < path.size()-1) {
        PinHole ph3 = path[i+1];
        length += ph2.end_p.dist(ph3.start_p);
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
        length += ph1.end_p.dist(ph2.start_p);
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
  TIME_LIMIT = 1.0;
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

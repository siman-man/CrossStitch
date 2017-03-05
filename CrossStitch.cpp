// C++11
#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <string.h>
#include <cassert>
#include <float.h>
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
  int z;

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
double DIST_TABLE[10000][10000];
vector<string> g_pattern;
map<char, vector<Point> > g_colorCoords;
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
          g_colorCoords[color].push_back(Point(y,x));
          C = max(C, color - 'a' + 1);
        }
      }

      for (int z = 0; z < S*S-1; z++) {
        int y1 = z/S;
        int x1 = z%S;
        Point p1(y1, x1);

        for (int z2 = z+1; z2 < S*S; z2++) {
          int y2 = z2/S;
          int x2 = z2%S;
          Point p2(y2, x2);
          double dist = p1.dist(p2);

          DIST_TABLE[z][z2] = dist;
          DIST_TABLE[z2][z] = dist;
        }
      }

      fprintf(stderr,"S = %d, C = %d, N = %d\n", S, C, N);
    }

    vector<string> embroider(vector<string> pattern) {
      vector<string> ret;
      ll sc = getCycle();

      init(pattern);

      map<char, vector<Point> >::iterator it = g_colorCoords.begin();
      while (it != g_colorCoords.end()) {
        char color = (*it).first;
        g_paths[color] = createFIPath(color);
        //g_paths[color] = createNNPath(color, g_colorCoords[color]);
        it++;
      }

      TIME_LIMIT -= getTime(sc);
      fprintf(stderr,"TIME_LIMIT = %f\n", TIME_LIMIT);

      for (int i = 0; i < C; i++) {
        char color = 'a' + i;
        g_paths[color] = needlework(g_paths[color], TIME_LIMIT / C);
      }

      vector<PinHole> path = createPath();
      vector<string> answer = createAnswer(path);

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

    vector<string> createAnswer(vector<PinHole> &path) {
      vector<string> answer;
      int psize = path.size();
      map<char, bool> checkList;

      for (int i = 0; i < psize; i++) {
        PinHole ph = path[i];
        if (!checkList[ph.color]) {
          checkList[ph.color] = true;
          answer.push_back(string(1, ph.color));
        }
        answer.push_back(ph.start_p.to_s());
        answer.push_back(ph.end_p.to_s());
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

    vector<PinHole> createNNPath(char color, vector<Point> coords) {
      vector<PinHole> path;
      Point bp = coords[0];
      coords.erase(coords.begin());
      int csize = coords.size();

      for (int i = 0; i < csize; i++) {
        double minDist = 99999.0;
        int minId = 0;
        int bsize = coords.size();

        path.push_back(createPinHole(color, Point(bp.y, bp.x), Point(bp.y+1, bp.x+1)));
        path.push_back(createPinHole(color, Point(bp.y+1, bp.x), Point(bp.y, bp.x+1)));

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

      path.push_back(createPinHole(color, Point(bp.y, bp.x), Point(bp.y+1, bp.x+1)));
      path.push_back(createPinHole(color, Point(bp.y+1, bp.x), Point(bp.y, bp.x+1)));

      cleanPath(path);

      return path;
    }

    vector<PinHole> createFIPath(char color) {
      vector<PinHole> path;
      vector<Point> ppath;
      vector<Point> coords = g_colorCoords[color];
      Point p1 = coords[0];

      ppath.push_back(p1);
      coords.erase(coords.begin());

      int csize = coords.size();

      for (int i = 0; i < csize; i++) {
        int size = csize-i;
        int rsize = (i+1);
        double md = -1.0;
        int index = 1;

        for (int k = 0; k < size; k++) {
          Point *p = &coords[k];
          int z1 = p->y * S + p->x;
          double mmd = DBL_MAX;

          for (int j = 0; j < min(20, rsize); j++) {
            int jj = xor128()%rsize;
            int z2 = coords[jj].y * S + coords[jj].x;
            mmd = min(mmd, DIST_TABLE[z1][z2]);
            //mmd = min(mmd, p.dist(coords[j]));
          }

          if (md < mmd) {
            md = mmd;
            index = k;
          }
        }

        Point ep = coords[index];
        int z1 = ep.y * S + ep.x;
        coords.erase(coords.begin()+index);

        double minDist = DBL_MAX;
        int minIndex  = -1;

        for (int j = 0; j < rsize; j++) {
          int aid = (j+1)%rsize;
          int z2 = ppath[j].y * S + ppath[j].x;
          int z3 = ppath[aid].y * S + ppath[aid].x;

          double d1 = DIST_TABLE[z1][z2];
          //double d1 = ep.dist(ppath[j]);
          double d2 = DIST_TABLE[z1][z3];
          //double d2 = ep.dist(ppath[aid]);
          double d3 = DIST_TABLE[z2][z3];
          //double d3 = ppath[j].dist(ppath[aid]);
          double dist = d1 + d2 - d3;

          if (minDist > dist) {
            minDist = dist;
            minIndex = aid;
          }
        }

        ppath.insert(ppath.begin() + minIndex, ep);
      }

      for (int i = 0; i < ppath.size(); i++) {
        Point p = ppath[i];
        path.push_back(createPinHole(color, Point(p.y, p.x), Point(p.y+1, p.x+1)));
        path.push_back(createPinHole(color, Point(p.y+1, p.x), Point(p.y, p.x+1)));
      }

      cleanPath(path);

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
      double k = 0.4;
      double diffLength = 0.0;
      int i, j;

      while (currentTime < LIMIT) {
        double remainTime = LIMIT - currentTime;
        int ope = xor128()%2;

        do {
          i = xor128()%psize;
          j = xor128()%psize;
        } while (i == j);

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
            diffLength = insertPinHole(i, j, path);
            if (isCorrectHole(i, path) && isCorrectHole(j, path)) {
              //diffLength = calcThreadLength(path) - goodLength;
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

      fprintf(stderr, "tryCount = %lld\n", tryCount);
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

    double insertPinHole(int i, int j, vector<PinHole> &path) {
      PinHole ph = path[i];
      double beforeLength = calcLength(i, path) + calcLength(j, path);

      path.erase(path.begin()+i);
      path.insert(path.begin()+j, ph);

      double afterLength = calcLength(i, path) + calcLength(j, path);
      return afterLength - beforeLength;
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

## Problem Statement

You are given a pattern which has to be embroidered by means of cross-stitch.

あなたにはクロススティッチ（刺繍で模様がかかれた布）が与えられます。

In this technique, pixels of the pattern are embroidered with pairs of diagonal stitches that cross each other to form an X on the front side of the canvas.

この技法では、パターンのピクセルが、対角線のステッチの対を用いて刺繍されて、互いに交差してキャンバスの前面にXを形成する。

The shape of the stitches on the back side of the canvas doesn't matter. The pixels of each color are embroidered using a single thread

キャンバスの裏側のステッチの形状は関係ありません。 各色のピクセルは、一本の糸を使用して刺繍されます

(without cutting it between stitches). Individual stitches can be done in any order as long as all necessary stitches for the color are done eventually.

（ステッチ間で切断することはありません）。 それぞれのステッチは、色に必要なすべてのステッチが最終的に行われる限り、任意の順序で行うことができます。

Your goal is to embroider the given patten correctly using minimal possible length of thread.

あなたの目標は、使用する糸の長さをなるべく短く与えられたパターンを正しく刺繍することです。


## Implementation

Your code must implement one method embroider(vector<string> pattern). The pattern is a square image of S x S pixels.

あなたは embroider(vector<string> pattern) を実装する必要があります。 patternはS * Sのピクセルで与えられます。

pattern[i][j] describes the pixel of the pattern in row i and column j: '.' denotes an empty space (doesn't need to be embroidered),

pattern[i][j] は i行目のj列のピクセルを表しています。 '.' は空白のスペースを表します。 (刺繍には必要ありません)

characters 'a' through 'z' denote pixels of various colors. If the pattern uses C different colors, they will be denoted with the first C letters of the alphabet.

文字のa-zはそれぞれのピクセルの色を表しています。 パターンがCの異なる色を使用する場合は、アルファベットの最初のC文字で示されます。

The return from the method will describe the points (corners of pixels) in which the needle will pierce the canvas for each thread color. Each element of the return is

このメソッドからの復帰では、針が各糸色ごとにキャンバスを突き刺す点（ピクセルの角）が記述されます。

- either a single character which marks the beginning of a section describing the next color (the first section starts with "a", the second with "b" etc.),
  次の色を記述するセクションの先頭を示す1文字（最初のセクションは "a"で始まり、2番目のセクションは "b"など）か、

- or a string formatted as "ROW COL" which gives the row and column coordinates of the point at which the needle pierces the canvas
  ("0 0" and "S+1 S+1" correspond to the top left and bottom right corners of the pattern, respectively).  

  The returned stitches must satisfy the following constraints (see visualizer code for a detailed implementation):

  返されるステッチは、次の制約を満たさなければなりません（詳細な実装については、ビジュアライザーのコードを参照してください）。

- For each stitch, its starting point must differ from its ending point (i.e. the needle can't pierce the canvas in the same point two times in row). Several different stitches can share endpoints.
  各ステッチについて、その開始点は終点とは異なる必要があります（つまり、針が同じポイントで2回連続してキャンバスを穿孔することはできません）。 いくつかの異なるステッチがエンドポイントを共有することができます。

- Each stitch on the front side must be a diagonal of exactly one pixel.
  正面の各ステッチは、ちょうど1ピクセルの対角線でなければなりません。

- Each stitch on the front side of the canvas can be done only once (i.e. there should never be two stitches over the same diagonal on the front side).
  It is allowed to have repeating stitches on the back side of the canvas.
  キャンバスの表側の各ステッチは、1回だけ行うことができる（すなわち、表側の同じ対角線上に2つのステッチが存在してはならない）。 キャンバスの裏側に繰り返し縫いを入れることが許されています。

- In the end each colored pixel in the pattern must have both its diagonals embroidered on the front side in correct color. There should be no extra stitches on the front side.
  最後に、パターン内の各色付きピクセルは、その正面に両方の対角線が正しい色で刺繍されていなければなりません。 前側に余分な縫い目はないはずです。

## スコア

For each test case we will calculate your raw score. If your solution produced an invalid return (not all pixels were embroidered, some invalid stitches were done etc.),

各テストケースであなたの素点の計算を行います。もし有効ではない回答を行った場合は0点となります。

raw score for this test case will be 0. Otherwise, raw score will be calculated as follows. The total "useful" length of thread (i.e. thread visible on the front side of the canvas)

L doesn't depend on your return and equals (number of embroidered pixels) * 2 * sqrt(2). The total "wasted" length of thread (i.e. thread used on the back side of the canvas)

W is calculated based on your return. The raw score is calculated as max(0, ((5 - W / L) / 5)^3).


## 注意点

-	The time limit is 10 seconds per test case (this includes only the time spent in your code). The memory limit is 1024 megabytes.

  時間制限は10秒で、メモリの制限は1GBです。

-	There is no explicit code size limit. The implicit source code size limit is around 1 MB (it is not advisable to submit codes of size close to that or larger). Once your code is compiled, the binary size should not exceed 1 MB.
-	The compilation time limit is 30 seconds. You can find information about compilers that we use and compilation options here.
-	There are 10 example test cases and 100 full submission (provisional) test cases.
-	The match is rated.


## 問題サイズ

Sは [10 - 100]の間
Cは [2 - 20]の間

## 考察

### 初日

クロススティッチと呼ばれる刺繍に関する問題。与えられた刺繍のパターンを縫うために必要な糸の長さをなるべく最小限にしましょうという問題。

クロススティッチは表面と裏面の２つがあり、表面の糸の長さは固定されているので、裏面の糸の長さでの勝負になる。

第一感としては「巡回セールスマンの亜種だな」色の種類 * 巡回セールスマンを解く問題。

各色は1回で縫いきらないとダメ。

巡回セールスマンの2回訪れるバージョンか

ノードを1R,1L,2R,2Lと表現すれば巡回セールスマンの問題に落とし込めそう

1R -> 1L -> 2R -> 2L という風に表現出来るから。
1R -> 2R -> 1L -> 2L も可能。

あとはこれの糸の長さを計算する部分の実装を行う。

クロスさせるパタンは右方向だと 「左下 -> 右上」「左上 -> 右下」の2つ
クロスさせるパタンは左方向だと 「右上 -> 左下」「右下 -> 左上」の2つ

LRにもそれぞれ2pa

LD, RDの２つにわける

焼きなましで経路を調整させるところまでは見えた。焼きなまし問題を応用させればいけるっぽさ

* 任意の2つの場所を入れ替える
* クロスされている部分の解消
* LRのそれぞれのやり方を別のものにする

遷移以外に自明に短くなる手法を考察する

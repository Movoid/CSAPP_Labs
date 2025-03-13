/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <stdio.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
// N 行 M 列，转置到 M 行 N 列
// 32*32, 64*64, 61*67
// Cache sindex_bitcnt=5 line_per_set=1 block_bitcnt=5
// 32 Sets, 1 Line per Set, 32 bytes = 8 ints per Block.
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {

  if (M == 32 && N == 32) {
    // Case 1
    // (1 << 10) / 32 / 4 = 8 cols
    for (int b_row = 0; b_row < N / 8; b_row++) {
      for (int b_col = 0; b_col < M / 8; b_col++) {
        int b_base_i = 8 * b_row;
        int b_base_j = 8 * b_col;
        // 对角线 block, 高概率 Cache Thrashing.
        // 32*32 == 1<<10 正好越过了 Set Index,
        // 导致 A B 反复抢占 Cache Line.
        if (b_row == b_col) {
          int a, b, c, d, e, f, g, h;
          for (int k = 0; k < 8; k++) {
            a = A[b_base_i + k][b_base_j + 0];
            b = A[b_base_i + k][b_base_j + 1];
            c = A[b_base_i + k][b_base_j + 2];
            d = A[b_base_i + k][b_base_j + 3];
            e = A[b_base_i + k][b_base_j + 4];
            f = A[b_base_i + k][b_base_j + 5];
            g = A[b_base_i + k][b_base_j + 6];
            h = A[b_base_i + k][b_base_j + 7];
            B[b_base_j + 0][b_base_i + k] = a;
            B[b_base_j + 1][b_base_i + k] = b;
            B[b_base_j + 2][b_base_i + k] = c;
            B[b_base_j + 3][b_base_i + k] = d;
            B[b_base_j + 4][b_base_i + k] = e;
            B[b_base_j + 5][b_base_i + k] = f;
            B[b_base_j + 6][b_base_i + k] = g;
            B[b_base_j + 7][b_base_i + k] = h;
          }
        } else {
          for (int off_i = 0; off_i < 8; off_i++) {
            for (int off_j = 0; off_j < 8; off_j++) {
              B[b_base_j + off_j][b_base_i + off_i] = A[b_base_i + off_i][b_base_j + off_j];
            }
          }
        }
      }
    }
  } else if (M == 64 && N == 64) {
    // [Refered to](https://zhuanlan.zhihu.com/p/387662272)
    // (1 << 10) / 64 / 4 = 4 cols
    int a, b, c, d, e, f, g, h;
    int borrow_i, borrow_j;
    for (int b_i = 0; b_i < 64; b_i += 8) {
      // 先处理对角线, 因为需要借用 B 中 block.
      borrow_j = b_i;
      borrow_i = (b_i == 0 ? 8 : 0);
      // move A block low 4 lines to borrowed B block high 4 lines.
      for (int i = 0; i < 4; i++) {
        a = A[b_i + 4 + i][b_i + 0];
        b = A[b_i + 4 + i][b_i + 1];
        c = A[b_i + 4 + i][b_i + 2];
        d = A[b_i + 4 + i][b_i + 3];
        e = A[b_i + 4 + i][b_i + 4];
        f = A[b_i + 4 + i][b_i + 5];
        g = A[b_i + 4 + i][b_i + 6];
        h = A[b_i + 4 + i][b_i + 7];
        B[borrow_i + i][borrow_j + 0] = a;
        B[borrow_i + i][borrow_j + 1] = b;
        B[borrow_i + i][borrow_j + 2] = c;
        B[borrow_i + i][borrow_j + 3] = d;
        B[borrow_i + i][borrow_j + 4] = e;
        B[borrow_i + i][borrow_j + 5] = f;
        B[borrow_i + i][borrow_j + 6] = g;
        B[borrow_i + i][borrow_j + 7] = h;
      }
      // printMatrix(A);
      // printMatrix(B);
      // transpose borrowed B block high left and high right block.
      for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
          // high left based on B[borrow_i][borrow_j]
          a = B[borrow_i + i][borrow_j + j];
          B[borrow_i + i][borrow_j + j] = B[borrow_i + j][borrow_j + i];
          B[borrow_i + j][borrow_j + i] = a;

          // high right based on B[borrow_i][borrow_j + 4]
          a = B[borrow_i + i][borrow_j + 4 + j];
          B[borrow_i + i][borrow_j + 4 + j] = B[borrow_i + j][borrow_j + 4 + i];
          B[borrow_i + j][borrow_j + 4 + i] = a;
        }
      }
      // printMatrix(A);
      // printMatrix(B);
      // move A block high 4 lines to B output block high 4 lines.
      for (int i = 0; i < 4; i++) {
        a = A[b_i + i][b_i + 0];
        b = A[b_i + i][b_i + 1];
        c = A[b_i + i][b_i + 2];
        d = A[b_i + i][b_i + 3];
        e = A[b_i + i][b_i + 4];
        f = A[b_i + i][b_i + 5];
        g = A[b_i + i][b_i + 6];
        h = A[b_i + i][b_i + 7];
        B[b_i + i][b_i + 0] = a;
        B[b_i + i][b_i + 1] = b;
        B[b_i + i][b_i + 2] = c;
        B[b_i + i][b_i + 3] = d;
        B[b_i + i][b_i + 4] = e;
        B[b_i + i][b_i + 5] = f;
        B[b_i + i][b_i + 6] = g;
        B[b_i + i][b_i + 7] = h;
      }
      // printMatrix(A);
      // printMatrix(B);
      // transpose output B block high left and high right blocks.
      for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
          // high left based on B[b_i][b_i]
          a = B[b_i + i][b_i + j];
          B[b_i + i][b_i + j] = B[b_i + j][b_i + i];
          B[b_i + j][b_i + i] = a;

          // high left based on B[b_i][b_i + 4]
          a = B[b_i + i][b_i + 4 + j];
          B[b_i + i][b_i + 4 + j] = B[b_i + j][b_i + 4 + i];
          B[b_i + j][b_i + 4 + i] = a;
        }
      }
      // printMatrix(A);
      // printMatrix(B);
      // swap output B high right block
      // with borrowed B high left block.
      for (int i = 0; i < 4; i++) {
        // prefetch output B high right block.
        a = B[b_i + i][b_i + 4];
        b = B[b_i + i][b_i + 5];
        c = B[b_i + i][b_i + 6];
        d = B[b_i + i][b_i + 7];
        // prefetch borrowed B high left block.
        e = B[borrow_i + i][borrow_j + 0];
        f = B[borrow_i + i][borrow_j + 1];
        g = B[borrow_i + i][borrow_j + 2];
        h = B[borrow_i + i][borrow_j + 3];
        // write.
        B[borrow_i + i][borrow_j + 0] = a;
        B[borrow_i + i][borrow_j + 1] = b;
        B[borrow_i + i][borrow_j + 2] = c;
        B[borrow_i + i][borrow_j + 3] = d;
        B[b_i + i][b_i + 4] = e;
        B[b_i + i][b_i + 5] = f;
        B[b_i + i][b_i + 6] = g;
        B[b_i + i][b_i + 7] = h;
      }
      // printMatrix(A);
      // printMatrix(B);
      // copy borrowed B block high 4 lines to output B block low 4 lines.
      for (int i = 0; i < 4; i++) {
        // borrowed B block based on B[borrow_i][borrow_j]
        // output B block based on B[b_i + 4][b_i]
        a = B[borrow_i + i][borrow_j + 0];
        b = B[borrow_i + i][borrow_j + 1];
        c = B[borrow_i + i][borrow_j + 2];
        d = B[borrow_i + i][borrow_j + 3];
        e = B[borrow_i + i][borrow_j + 4];
        f = B[borrow_i + i][borrow_j + 5];
        g = B[borrow_i + i][borrow_j + 6];
        h = B[borrow_i + i][borrow_j + 7];
        B[b_i + 4 + i][b_i + 0] = a;
        B[b_i + 4 + i][b_i + 1] = b;
        B[b_i + 4 + i][b_i + 2] = c;
        B[b_i + 4 + i][b_i + 3] = d;
        B[b_i + 4 + i][b_i + 4] = e;
        B[b_i + 4 + i][b_i + 5] = f;
        B[b_i + 4 + i][b_i + 6] = g;
        B[b_i + 4 + i][b_i + 7] = h;
      }
      // printMatrix(A);
      // printMatrix(B);
      // process non-diagonal block
      for (int b_j = 0; b_j < 64; b_j += 8) {
        // diagonal block in this row is already done, skip.
        if (b_i == b_j) continue;
        // A input block based on A[b_i][b_j].
        // B output block based on B[b_j][b_i]

        // transpose A block high 4 lines
        for (int i = 0; i < 4; i++) {
          a = A[b_i + i][b_j + 0];
          b = A[b_i + i][b_j + 1];
          c = A[b_i + i][b_j + 2];
          d = A[b_i + i][b_j + 3];
          e = A[b_i + i][b_j + 4];
          f = A[b_i + i][b_j + 5];
          g = A[b_i + i][b_j + 6];
          h = A[b_i + i][b_j + 7];
          B[b_j + 0][b_i + i] = a;
          B[b_j + 1][b_i + i] = b;
          B[b_j + 2][b_i + i] = c;
          B[b_j + 3][b_i + i] = d;
          B[b_j + 0][b_i + 4 + i] = e;
          B[b_j + 1][b_i + 4 + i] = f;
          B[b_j + 2][b_i + 4 + i] = g;
          B[b_j + 3][b_i + 4 + i] = h;
        }
        // printMatrix(A);
        // printMatrix(B);
        // transpose A block low left block.
        for (int i = 0; i < 4; i++) {
          a = A[b_i + 4][b_j + i];
          b = A[b_i + 5][b_j + i];
          c = A[b_i + 6][b_j + i];
          d = A[b_i + 7][b_j + i];
          e = B[b_j + i][b_i + 4];
          f = B[b_j + i][b_i + 5];
          g = B[b_j + i][b_i + 6];
          h = B[b_j + i][b_i + 7];
          B[b_j + i][b_i + 4] = a;
          B[b_j + i][b_i + 5] = b;
          B[b_j + i][b_i + 6] = c;
          B[b_j + i][b_i + 7] = d;
          B[b_j + 4 + i][b_i + 0] = e;
          B[b_j + 4 + i][b_i + 1] = f;
          B[b_j + 4 + i][b_i + 2] = g;
          B[b_j + 4 + i][b_i + 3] = h;
        }
        // printMatrix(A);
        // printMatrix(B);
        // transpose A block low right block.
        for (int i = 0; i < 4; i++) {
          a = A[b_i + 4 + i][b_j + 4];
          b = A[b_i + 4 + i][b_j + 5];
          c = A[b_i + 4 + i][b_j + 6];
          d = A[b_i + 4 + i][b_j + 7];
          B[b_j + 4][b_i + 4 + i] = a;
          B[b_j + 5][b_i + 4 + i] = b;
          B[b_j + 6][b_i + 4 + i] = c;
          B[b_j + 7][b_i + 4 + i] = d;
        }
      }
    }
  }
}
/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, tmp;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
  /* Register your solution function */
  registerTransFunction(transpose_submit, transpose_submit_desc);

  /* Register any additional transpose functions */
  registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
  int i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; ++j) {
      if (A[i][j] != B[j][i]) {
        return 0;
      }
    }
  }
  return 1;
}

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

const int INF = 0x3f3f; // 使用 short 能表达的最大范围，节省空间
short dp[1001][1001];
short next_dp[1001][1001];

int denoms[] = {100, 50, 20, 10, 5, 1};
int suffix_sum[7];

void solve() {
    int ab, bc, ca;
    if (!(cin >> ab >> bc >> ca)) return;

    int n[3][6];
    int start_sum[3] = {0, 0, 0};
    int total_cnt[6] = {0};

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 6; ++j) {
            cin >> n[i][j];
            start_sum[i] += n[i][j] * denoms[j];
            total_cnt[j] += n[i][j];
        }
    }

    // 目标金额
    int target[3];
    target[0] = start_sum[0] - ab + ca;
    target[1] = start_sum[1] + ab - bc;
    target[2] = start_sum[2] + bc - ca;

    if (target[0] < 0 || target[1] < 0 || target[2] < 0) {
        cout << "impossible" << endl;
        return;
    }

    // 预计算后缀和：剩余更小面值能提供的最大金额
    suffix_sum[6] = 0;
    for (int i = 5; i >= 0; --i) {
        suffix_sum[i] = suffix_sum[i + 1] + total_cnt[i] * denoms[i];
    }

    // 初始化 DP
    memset(dp, 0x3f, sizeof(dp));
    dp[0][0] = 0;

    // 按面值从大到小处理
    for (int d = 0; d < 6; ++d) {
        int val = denoms[d];
        int N = total_cnt[d];
        memset(next_dp, 0x3f, sizeof(next_dp));

        int next_max_diff = suffix_sum[d + 1];

        // 遍历当前可达状态
        for (int va = 0; va <= target[0]; ++va) {
            // 剪枝：如果当前 va 加上后面所有的钱都够不到 target，跳过
            if (va + suffix_sum[d] < target[0]) continue; 
            
            for (int vb = 0; vb <= target[1]; ++vb) {
                if (dp[va][vb] >= INF) continue;
                if (vb + suffix_sum[d] < target[1]) continue;

                // 枚举当前面值的分配：A拿i张，B拿j张
                for (int i = 0; i <= N; ++i) {
                    int nva = va + i * val;
                    if (nva > target[0] || target[0] - nva > next_max_diff) continue;

                    for (int j = 0; j <= N - i; ++j) {
                        int nvb = vb + j * val;
                        if (nvb > target[1] || target[1] - nvb > next_max_diff) continue;

                        int k = N - i - j; // C 拿剩下的
                        int cost = (abs(i - n[0][d]) + abs(j - n[1][d]) + abs(k - n[2][d])) / 2;
                        
                        if (dp[va][vb] + cost < next_dp[nva][nvb]) {
                            next_dp[nva][nvb] = dp[va][vb] + cost;
                        }
                    }
                }
            }
        }
        memcpy(dp, next_dp, sizeof(dp));
    }

    if (dp[target[0]][target[1]] >= INF) {
        cout << "impossible" << endl;
    } else {
        cout << (int)dp[target[0]][target[1]] << endl;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    int T;
    if (!(cin >> T)) return 0;
    while (T--) solve();
    return 0;
}
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <set>
#include <sstream>
#define ll long long
#define MODD 1000000009
#define N 7500000
using namespace std;

ll pow_n_m[2*N+1];
ll pow_n[2*N+1];
ll factinv[2*N+1];
ll fact[2*N+1];

ll binomial(ll x, ll y) {
  if (x < y) {
    return 0;
  }
  return fact[x] * factinv[y] % MODD * factinv[x - y] % MODD;
}

ll ff(int p, int L) {
  ll answer = 0;
  for (int j = 0; j <= p / (L+1); ++j) {
    ll curr_term = pow_n_m[j] * binomial(p - j * L, j) % MODD * pow_n[p - j * (L+1)] % MODD;
    if (j & 1)
      answer = (answer + curr_term)%MODD;
    else
      answer = (answer - curr_term)%MODD;
  }
  answer = (answer + MODD)%MODD;
  return answer;
}

ll fe(int L) {
  return (N * ff(N - L - 1, L) % MODD - N * ff(N - 1 , L) % MODD + MODD) % MODD;
}

ll modpow(ll x,int y) {
  ll answer = 1;
  while(y)
  {
    if(y&1)
      answer=(answer*x)%MODD;
    x=(x*x)%MODD;
    y/=2;
  }
  return answer;
}

int main() {
  pow_n_m[0] = pow_n[0] = 1;
  for (int i = 1; i <= 2*N; ++i) {
    pow_n_m[i] = (pow_n_m[i-1] * (N - 1)) % MODD;
    pow_n[i] = (pow_n[i-1] * N) % MODD;
  }

  fact[0] = factinv[0] = 1;
  for (int i = 1; i <= 2*N; ++i) {
    fact[i] = (fact[i - 1] * i) % MODD;
    factinv[i] = modpow(fact[i], MODD - 2);
  }
  printf("Done with precomputation.\n");
  ll answer = 0;
  for (int L = 1; L <= N; ++L) {
    answer = (answer + L * (fe(L) - fe(L - 1))%MODD)%MODD;
  }
  answer = (answer + MODD)%MODD;
  printf("%lld\n",answer);

}


#include <vector>
#include <unordered_map>
#include <algorithm>

class Solution {
public:
    struct DSU {
        std::vector<int> p, r;
        DSU(int n): p(n), r(n,0) { for (int i=0;i<n;++i) p[i]=i; }
        int find(int a){ return p[a]==a?a:p[a]=find(p[a]); }
        void unite(int a,int b){ a=find(a); b=find(b); if(a==b) return; if(r[a]<r[b]) p[a]=b; else if(r[b]<r[a]) p[b]=a; else {p[b]=a; r[a]++;} }
    };

    int maxActivated(std::vector<std::vector<int>>& points) {
        int n = (int)points.size();
        if(n==0) return 1;
        DSU dsu(n);
        std::unordered_map<int,int> mx, my;
        for(int i=0;i<n;++i){
            int x = points[i][0];
            int y = points[i][1];
            auto itx = mx.find(x);
            if(itx!=mx.end()) dsu.unite(i, itx->second);
            else mx[x]=i;
            auto ity = my.find(y);
            if(ity!=my.end()) dsu.unite(i, ity->second);
            else my[y]=i;
        }
        std::unordered_map<int,int> compSize;
        for(int i=0;i<n;++i) compSize[dsu.find(i)]++;
        std::vector<int> sizes;
        for(auto &kv: compSize) sizes.push_back(kv.second);
        std::sort(sizes.begin(), sizes.end(), std::greater<int>());
        int best = 1; // at least we can add the new point alone
        if(!sizes.empty()) best = std::max(best, sizes[0] + 1);
        if(sizes.size()>=2) best = std::max(best, sizes[0] + sizes[1] + 1);
        return best;
    }
};

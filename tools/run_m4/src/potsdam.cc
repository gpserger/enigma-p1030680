// potsdam.cc - decrypt target under the three known Potsdam daily keys with all
// 26^4 message keys x all 676 middle/right ring settings; rank by n-gram score.
#include "core.h"
#include <omp.h>
#include <map>
struct Hit { float s; MKey k; };
int main(int argc, char** argv){
  const char* ctpath = argv[1]; std::string tri = argv[2], quad = argv[3];
  uint8_t ct[MAXLEN]; int n = read_text(ctpath, ct);
  NGram T3; T3.load(tri,3,'l'); NGram Q4; Q4.load(quad,4,'c');
  struct Day { const char* name; int refl, greek, wL,wM,wR; const char* rings; const char* plugs; };
  Day days[3] = {{"30Apr45",1,8,4,5,7,"AAEL","AE BF CM DQ HU JN LX PR SZ VW"},
                 {"01May45",0,9,3,2,7,"VCCH","CH EJ NV OU TY LG SZ PK DI QB"},
                 {"02May45",1,9,4,1,7,"AAFB","CP DG EJ FI KT LZ MS NO QU RW"}};
  for (auto& d: days) {
    int S[26]; parse_plugs(d.plugs,S);
    std::vector<std::vector<Hit>> topT(omp_get_max_threads()), topQ(omp_get_max_threads());
    const int K=40;
    #pragma omp parallel for schedule(dynamic) collapse(2)
    for (int rM=0; rM<26; rM++) for (int rR=0; rR<26; rR++) {
      auto& tt = topT[omp_get_thread_num()]; auto& tq = topQ[omp_get_thread_num()];
      MKey k{d.refl,d.greek,d.wL,d.wM,d.wR,0,0,0,0,0,rM,rR};
      uint8_t pt[MAXLEN]; Traj tr;
      for (int pM=0;pM<26;pM++) for (int pR=0;pR<26;pR++) for (int pL=0;pL<26;pL++) {
        k.pM=pM;k.pR=pR;k.pL=pL; make_traj(k,n,tr);
        for (int pG=0;pG<26;pG++) {
          for (int i=0;i<n;i++){
            int shR=tr.R[i]-rR, shM=tr.M[i]-rM, shL=tr.L[i], shG=pG;
            int x=S[ct[i]];
            x=rot_f(d.wR,shR,x); x=rot_f(d.wM,shM,x); x=rot_f(d.wL,shL,x); x=rot_f(d.greek,shG,x);
            x=TB.refl[d.refl][x];
            x=rot_b(d.greek,shG,x); x=rot_b(d.wL,shL,x); x=rot_b(d.wM,shM,x); x=rot_b(d.wR,shR,x);
            pt[i]=S[x];
          }
          k.pG=pG;
          float s3=T3.score(pt,n), s4=Q4.score(pt,n);
          auto push=[&](std::vector<Hit>& v, float s){ if((int)v.size()<K){v.push_back({s,k}); std::push_heap(v.begin(),v.end(),[](const Hit&a,const Hit&b){return a.s>b.s;});}
            else if(s>v.front().s){ std::pop_heap(v.begin(),v.end(),[](const Hit&a,const Hit&b){return a.s>b.s;}); v.back()={s,k}; std::push_heap(v.begin(),v.end(),[](const Hit&a,const Hit&b){return a.s>b.s;});}};
          push(tt,s3); push(tq,s4);
        }
      }
    }
    for (int which=0; which<2; which++) {
      std::vector<Hit> all; for (auto& v: (which?topQ:topT)) all.insert(all.end(),v.begin(),v.end());
      std::sort(all.begin(),all.end(),[](const Hit&a,const Hit&b){return a.s>b.s;});
      printf("=== %s %s %s %s%s%s rings(given)=%s plugs=%s | top by %s\n", d.name, REFL_NAME[d.refl], ROTOR_NAME[d.greek], ROTOR_NAME[d.wL],ROTOR_NAME[d.wM],ROTOR_NAME[d.wR], d.rings,d.plugs, which?"quadgram(wehrmacht)":"trigram(Krah naval)");
      for (int j=0;j<20 && j<(int)all.size();j++){ auto& h=all[j]; static uint8_t T[MAXLEN][26]; make_path(h.k,n,T); uint8_t o[MAXLEN]; decrypt(T,S,ct,n,o);
        printf("%9.4f q=%7.4f t=%8.1f  ringsMR=%c%c  window(G L M R)=%c%c%c%c  %s\n", h.s, Q4.score(o,n), T3.score(o,n), 'A'+h.k.rM,'A'+h.k.rR,'A'+h.k.pG,'A'+h.k.pL,'A'+h.k.pM,'A'+h.k.pR, to_str(o,n).c_str()); }
    }
    fflush(stdout);
  }
}

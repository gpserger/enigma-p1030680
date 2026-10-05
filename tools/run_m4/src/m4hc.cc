// m4hc.cc - ciphertext-only Enigma M3/M4 solver: exhaustive rotor keyspace (with exact
// naval double-notch / middle-ring deduplication), per-key plugboard hill climb,
// two-stage (cheap climb on all keys -> I-Stecker partial exhaustion on survivors).
//
// Usage:
//  m4hc probe --ct F --tri F --quad F --key "REFL GREEK WL WM WR WIN RINGS" --plugs "AB CD.."
//  m4hc job   --ct F --tri F --quad F --refl R --greek G --wheels LMR --out DIR [opts]
#include "core.h"
#include "hc.h"
#include <map>
#include <set>
#include <chrono>
#include <string>
#include <queue>
#include <omp.h>
#include <random>

int g_climb_order=3;
static double now(){ return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

struct Opts {
  std::string ct, tri, quad, out, key, plugs, truth, ringmode="all";
  int refl=-1, greek=-1, wL=-1,wM=-1,wR=-1;
  int topA=2000;      // survivors from stage A -> stage B
  int topN=50;        // candidates written
  int maxplugs=10;
  int restartsA=0;    // extra random-seed climbs in stage A (0 = only empty board)
  const char* iletters="ENRXSI";
  long limit=-1;      // limit number of keys (for timing)
  int threads=1;
};

static int parse_wheel(char c){ return c-'1'; }

// ---------- key enumeration ----------
struct KeyGen {
  int refl, greek, wL, wM, wR, len;
  std::vector<std::pair<int,int>> rightMid; // (pR window, wM middle window rep)
  void init(int len_) {
    len = len_;
    int nPhR = (wR >= 5) ? 13 : 26;
    for (int pR = 0; pR < nPhR; pR++) {
      std::set<std::vector<uint8_t>> seen;
      for (int wm = 0; wm < 26; wm++) {
        MKey k{refl,greek,wL,wM,wR,0,0,wm,pR,0,wm,0}; Traj t; make_traj(k,len,t);
        std::vector<uint8_t> sig(2*len);
        for (int i=0;i<len;i++){ sig[2*i]=md(t.M[i]-wm); sig[2*i+1]=t.L[i]; }
        if (seen.insert(sig).second) rightMid.push_back({pR,wm});
      }
    }
  }
  long count() const { return (long)rightMid.size()*26*26*26*(greek>=0?26:1); }
};

// Build T for all (oL,g) quickly: given fixed right/middle, fill inner forward tables.
struct Inner {
  uint8_t A[MAXLEN][26], Ai[MAXLEN][26]; int Lsh[MAXLEN];
  void build(const MKey& k, int len) {
    Traj t; make_traj(k,len,t);
    for (int i=0;i<len;i++){
      int shR=t.R[i]-k.rR, shM=t.M[i]-k.rM;
      for (int c=0;c<26;c++){ int x=rot_f(k.wR,shR,c); x=rot_f(k.wM,shM,x); A[i][c]=x; Ai[i][x]=c; }
      Lsh[i]=t.L[i]-k.pL; // number of left steps so far (relative)
    }
  }
  void path(const MKey& k, int len, uint8_t T[][26]) const {
    for (int i=0;i<len;i++){
      int shL = k.pL + Lsh[i] - k.rL; int shG = k.pG;
      uint8_t U[26];
      for (int c=0;c<26;c++){
        int x=rot_f(k.wL,shL,c); if(k.greek>=0) x=rot_f(k.greek,shG,x); x=TB.refl[k.refl][x];
        if(k.greek>=0) x=rot_b(k.greek,shG,x); x=rot_b(k.wL,shL,x); U[c]=x; }
      for (int c=0;c<26;c++) T[i][c]=Ai[i][U[A[i][c]]];
    }
  }
};

struct Cand { float s; MKey k; int S[26]; };
struct CandCmp { bool operator()(const Cand&a,const Cand&b)const{return a.s>b.s;} }; // min-heap

static void heap_push(std::vector<Cand>& h, size_t K, const Cand& c){
  if (h.size()<K){ h.push_back(c); std::push_heap(h.begin(),h.end(),CandCmp()); }
  else if (c.s > h.front().s){ std::pop_heap(h.begin(),h.end(),CandCmp()); h.back()=c; std::push_heap(h.begin(),h.end(),CandCmp()); }
}

static std::string key_str(const MKey& k){
  char b[128];
  if (k.greek>=0) snprintf(b,sizeof b,"%s %s %s-%s-%s rings A%c%c%c win %c%c%c%c", REFL_NAME[k.refl], ROTOR_NAME[k.greek], ROTOR_NAME[k.wL],ROTOR_NAME[k.wM],ROTOR_NAME[k.wR],
     'A'+k.rL,'A'+k.rM,'A'+k.rR,'A'+k.pG,'A'+k.pL,'A'+k.pM,'A'+k.pR);
  else snprintf(b,sizeof b,"%s %s-%s-%s rings %c%c%c win %c%c%c", REFL_NAME[k.refl], ROTOR_NAME[k.wL],ROTOR_NAME[k.wM],ROTOR_NAME[k.wR],
     'A'+k.rL,'A'+k.rM,'A'+k.rR,'A'+k.pL,'A'+k.pM,'A'+k.pR);
  return b;
}

// order letters by ciphertext frequency (desc)
static void ct_order(const uint8_t* ct,int n,int* order){ int f[26]={0}; for(int i=0;i<n;i++)f[ct[i]]++;
  for(int i=0;i<26;i++)order[i]=i; std::stable_sort(order,order+26,[&](int a,int b){return f[a]>f[b];}); }

// I-Stecker seed list: none + every pair touching one of the letters
static std::vector<std::pair<int,int>> iseeds(const char* letters){
  std::vector<std::pair<int,int>> v; v.push_back({-1,-1});
  bool in[26]={0}; for(const char*p=letters;*p;p++) in[*p-'A']=true;
  for(int a=0;a<26;a++) for(int b=a+1;b<26;b++) if(in[a]||in[b]) v.push_back({a,b});
  for(int a=0;a<26;a++) if(in[a]) v.push_back({a,a});   // letter fixed self-steckered
  return v;
}

// Full I-Stecker climb on one key; returns best score, board in Sbest
static float istecker(const Scorer& sc, const std::vector<std::pair<int,int>>& seeds, const int* order, int maxplugs, int* Sbest, long& evals, int* nsucc=nullptr, const int* Strue=nullptr){
  float best=-1e30f;
  for (auto& sd: seeds){
    int S[26]; bool fx[26]={0}; for(int i=0;i<26;i++)S[i]=i;
    if (sd.first>=0){ S[sd.first]=sd.second; S[sd.second]=sd.first; fx[sd.first]=fx[sd.second]=true; }
    float s=climb(sc,S,fx,order,maxplugs,evals);
    if (nsucc && Strue && memcmp(S,Strue,sizeof S)==0) { (*nsucc)++; if (getenv("M4HC_VERBOSE")) fprintf(stderr,"  seed %c%c succeeded\n", sd.first<0?'-':'A'+sd.first, sd.first<0?'-':'A'+sd.second); }
    if (s>best){best=s; memcpy(Sbest,S,sizeof S);}
  }
  return best;
}

int main(int argc, char** argv){
  if (argc<2){ fprintf(stderr,"usage: m4hc probe|job ...\n"); return 1; }
  std::string mode=argv[1]; Opts o;
  for (int i=2;i<argc;i++){ std::string a=argv[i]; auto nx=[&](){ return std::string(argv[++i]); };
    if(a=="--ct")o.ct=nx(); else if(a=="--tri")o.tri=nx(); else if(a=="--quad")o.quad=nx(); else if(a=="--out")o.out=nx();
    else if(a=="--key")o.key=nx(); else if(a=="--plugs")o.plugs=nx(); else if(a=="--truth")o.truth=nx();
    else if(a=="--refl")o.refl=atoi(nx().c_str()); else if(a=="--greek")o.greek=atoi(nx().c_str());
    else if(a=="--wheels"){ std::string w=nx(); o.wL=parse_wheel(w[0]); o.wM=parse_wheel(w[1]); o.wR=parse_wheel(w[2]); }
    else if(a=="--topA")o.topA=atoi(nx().c_str()); else if(a=="--topN")o.topN=atoi(nx().c_str());
    else if(a=="--threads")o.threads=atoi(nx().c_str()); else if(a=="--maxplugs")o.maxplugs=atoi(nx().c_str()); else if(a=="--limit")o.limit=atol(nx().c_str());
    else if(a=="--iletters"){ static std::string s; s=nx(); o.iletters=s.c_str(); }
    else { fprintf(stderr,"bad arg %s\n",a.c_str()); return 1; } }
  uint8_t ct[MAXLEN]; int n=read_text(o.ct.c_str(),ct);
  NGram T3;
  // climb model: --tri FILE (Krah-format trigram, default) or env M4HC_CLIMB="mix:DIR/prefix:wm,wb,wt,wq" or "quad:FILE" or "tric:FILE"
  const char* cm=getenv("M4HC_CLIMB");
  if (cm && !strncmp(cm,"mix:",4)) {
    char pre[512]; float w[4]; sscanf(cm+4,"%[^:]:%f,%f,%f,%f",pre,&w[0],&w[1],&w[2],&w[3]);
    NGram m1,m2,m3,m4; std::string P=pre;
    m1.load(P+"_monograms.txt",1,'c'); m2.load(P+"_bigrams.txt",2,'c'); m3.load(P+"_trigrams.txt",3,'c'); m4.load(P+"_quadgrams.txt",4,'c');
    T3.n=4; T3.t.assign(26*26*26*26,0);
    for (int i=0;i<26*26*26*26;i++){ int d=i%26, cd=i%676, bcd=i%17576; T3.t[i]=w[0]*m1.t[d]+w[1]*m2.t[cd]+w[2]*m3.t[bcd]+w[3]*m4.t[i]; }
    g_climb_order=4;
  } else if (cm && !strncmp(cm,"quad:",5)) { T3.load(cm+5,4,'c'); g_climb_order=4; }
  else if (cm && !strncmp(cm,"tric:",5)) { T3.load(cm+5,3,'c'); g_climb_order=3; }
  else if(!T3.load(o.tri,3,'l')){fprintf(stderr,"tri load fail\n");return 1;}
  NGram Q4; if(!Q4.load(o.quad,4,'c')){fprintf(stderr,"quad load fail\n");return 1;}
  int order[26]; ct_order(ct,n,order);
  auto seeds=iseeds(o.iletters);

  if (mode=="probe"){
    // key: refl greek wL wM wR pG pL pM pR rL rM rR  (numbers, greek -1 for M3)
    MKey k; sscanf(o.key.c_str(),"%d %d %d %d %d %d %d %d %d %d %d %d",&k.refl,&k.greek,&k.wL,&k.wM,&k.wR,&k.pG,&k.pL,&k.pM,&k.pR,&k.rL,&k.rM,&k.rR);
    static uint8_t T[MAXLEN][26]; make_path(k,n,T);
    int St[26]; parse_plugs(o.plugs.c_str(),St);
    Scorer sc{T3.t.data(),n,ct,T};
    uint8_t p[MAXLEN]; decrypt(T,St,ct,n,p);
    printf("truth: climbscore=%.0f quad=%.4f %s\n", sc.eval(St), Q4.score(p,n), to_str(p,n).c_str());
    long ev=0; double t0=now();
    int S[26]; for(int i=0;i<26;i++)S[i]=i; bool fx[26]={0};
    float s=climb(sc,S,fx,order,o.maxplugs,ev); decrypt(T,S,ct,n,p);
    printf("empty-board climb: %.0f quad=%.4f plugs=%s %s  (%s)\n", s, Q4.score(p,n), plug_str(S).c_str(), to_str(p,n).c_str(), memcmp(S,St,sizeof S)==0?"TRUE BOARD":"wrong");
    int Sb[26]; int ns=0; t0=now(); ev=0;
    float sb=istecker(sc,seeds,order,o.maxplugs,Sb,ev,&ns,St); double dt=now()-t0; decrypt(T,Sb,ct,n,p);
    printf("I-Stecker(%zu seeds): %.0f quad=%.4f plugs=%s %s (%s) seeds-succeeded=%d time=%.4fs evals=%ld (%.1f Mevals/s)\n",
      seeds.size(), sb, Q4.score(p,n), plug_str(Sb).c_str(), to_str(p,n).c_str(), memcmp(Sb,St,sizeof Sb)==0?"TRUE BOARD":"wrong", ns, dt, ev, ev/dt/1e6);
    return 0;
  }


  if (mode=="bench"){
    // args reused: --plugs = plaintext file, --key = "trials nulls rngseed", --iletters list separated by ','
    std::vector<std::string> P; { FILE* f=fopen(o.plugs.c_str(),"r"); char b[8192]; while(fgets(b,sizeof b,f)){ std::string x; for(char*c=b;*c;c++) if(*c>='A'&&*c<='Z') x+=*c; if(x.size()>=72) P.push_back(x);} fclose(f); }
    int trials=100, nulls=200; unsigned rs=1; sscanf(o.key.c_str(),"%d %d %u",&trials,&nulls,&rs);
    std::vector<std::string> sets; { std::string x=o.iletters; size_t p=0; while(true){ size_t q=x.find(',',p); sets.push_back(x.substr(p,q==std::string::npos?std::string::npos:q-p)); if(q==std::string::npos)break; p=q+1; } }
    const int L=72; omp_set_num_threads(o.threads);
    auto rndkey=[&](std::mt19937& g){ MKey k; k.refl=g()%2; k.greek=8+g()%2; int w[8]={0,1,2,3,4,5,6,7}; std::shuffle(w,w+8,g); k.wL=w[0];k.wM=w[1];k.wR=w[2];
      k.pG=g()%26;k.pL=g()%26;k.pM=g()%26;k.pR=g()%26;k.rL=0;k.rM=g()%26;k.rR=g()%26; return k; };
    for (auto& set: sets){
      auto sd=iseeds(set.c_str());
      int succ=0; std::vector<float> tq, nq, tt, nt; double ttime=0; long tev=0;
      #pragma omp parallel for schedule(dynamic,1) reduction(+:succ,ttime,tev)
      for (int t=0;t<trials;t++){
        std::mt19937 g(rs*1000003u+t);
        const std::string& pl=P[g()%P.size()]; int off=g()%(pl.size()-L+1);
        uint8_t pt[MAXLEN]; for(int i=0;i<L;i++) pt[i]=pl[off+i]-'A';
        MKey k=rndkey(g); int St[26]; for(int i=0;i<26;i++)St[i]=i; { int l[26]; for(int i=0;i<26;i++)l[i]=i; std::shuffle(l,l+26,g); for(int j=0;j<10;j++){St[l[2*j]]=l[2*j+1];St[l[2*j+1]]=l[2*j];} }
        static thread_local uint8_t T[MAXLEN][26]; make_path(k,L,T); uint8_t c[MAXLEN]; decrypt(T,St,pt,L,c);
        int ord[26]; ct_order(c,L,ord);
        Scorer sc{T3.t.data(),L,c,T}; int Sb[26]; long ev=0; double t0=now();
        istecker(sc,sd,ord,o.maxplugs,Sb,ev); ttime+=now()-t0; tev+=ev;
        uint8_t d[MAXLEN]; decrypt(T,Sb,c,L,d); int ok=0; for(int i=0;i<L;i++) ok+=(d[i]==pt[i]);
        bool s_ = ok>=L-3; if(s_) succ++;
        float q=Q4.score(d,L), tr=T3.score(d,L);
        #pragma omp critical
        { if(s_){ tq.push_back(q); tt.push_back(tr);} }
      }
      #pragma omp parallel for schedule(dynamic,1)
      for (int t=0;t<nulls;t++){
        std::mt19937 g(rs*7777777u+t+99);
        const std::string& pl=P[g()%P.size()]; int off=g()%(pl.size()-L+1);
        uint8_t pt[MAXLEN]; for(int i=0;i<L;i++) pt[i]=pl[off+i]-'A';
        MKey k=rndkey(g); int St[26]; for(int i=0;i<26;i++)St[i]=i; { int l[26]; for(int i=0;i<26;i++)l[i]=i; std::shuffle(l,l+26,g); for(int j=0;j<10;j++){St[l[2*j]]=l[2*j+1];St[l[2*j+1]]=l[2*j];} }
        static thread_local uint8_t T[MAXLEN][26]; make_path(k,L,T); uint8_t c[MAXLEN]; decrypt(T,St,pt,L,c);
        MKey w=rndkey(g); make_path(w,L,T);   // wrong key
        int ord[26]; ct_order(c,L,ord);
        Scorer sc{T3.t.data(),L,c,T}; int Sb[26]; long ev=0; istecker(sc,sd,ord,o.maxplugs,Sb,ev);
        uint8_t d[MAXLEN]; decrypt(T,Sb,c,L,d); float q=Q4.score(d,L), tr=T3.score(d,L);
        #pragma omp critical
        { nq.push_back(q); nt.push_back(tr); }
      }
      auto qs=[](std::vector<float> v,double p){ if(v.empty())return (double)NAN; std::sort(v.begin(),v.end()); return (double)v[std::min(v.size()-1,(size_t)(p*v.size()))]; };
      printf("seedset=%-8s seeds=%3zu success=%d/%d (%.1f%%) ms/key=%.2f Mev/s=%.2f | truth quad p10/p50=%.3f/%.3f tri p10/p50=%.0f/%.0f | null quad p50/p99/max=%.3f/%.3f/%.3f tri p50/p99/max=%.0f/%.0f/%.0f\n",
        set.c_str(), sd.size(), succ, trials, 100.0*succ/trials, 1000*ttime/trials, tev/ttime/1e6,
        qs(tq,.1),qs(tq,.5),qs(tt,.1),qs(tt,.5), qs(nq,.5),qs(nq,.99),qs(nq,.999999), qs(nt,.5),qs(nt,.99),qs(nt,.999999));
      fflush(stdout);
      if (!o.out.empty()){ FILE* f=fopen((o.out+"_"+set+".null").c_str(),"w"); for(size_t i=0;i<nq.size();i++) fprintf(f,"%.4f\t%.1f\n",nq[i],nt[i]); fclose(f);
        f=fopen((o.out+"_"+set+".truth").c_str(),"w"); for(size_t i=0;i<tq.size();i++) fprintf(f,"%.4f\t%.1f\n",tq[i],tt[i]); fclose(f); }
    }
    return 0;
  }

  if (mode=="job"){
    KeyGen kg{o.refl,o.greek,o.wL,o.wM,o.wR}; kg.init(n);
    long total=kg.count();
    MKey truth{}; bool hasTruth=false; float truthA=0; long beatTruth=0;
    if(!o.truth.empty()){ sscanf(o.truth.c_str(),"%d %d %d %d %d %d %d %d %d %d %d %d",&truth.refl,&truth.greek,&truth.wL,&truth.wM,&truth.wR,&truth.pG,&truth.pL,&truth.pM,&truth.pR,&truth.rL,&truth.rM,&truth.rR); hasTruth=true; }
    static uint8_t T[MAXLEN][26], Tt[MAXLEN][26];
    // truth path signature for identifying the equivalent enumerated key
    if (hasTruth) make_path(truth,n,Tt);
    std::vector<Cand> heapA; long keys=0, ev=0; double t0=now();
    std::vector<float> allA;
    int NT=o.threads; omp_set_num_threads(NT);
    std::vector<std::vector<Cand>> heaps(NT); std::vector<std::vector<float>> allT(NT);
    std::vector<long> evT(NT,0), keysT(NT,0);
    long nOuter=(long)kg.rightMid.size()*26*26;
    long outerLimit = o.limit>0 ? std::max(1L, o.limit/(26L*(o.greek>=0?26:1))) : nOuter;
    #pragma omp parallel for schedule(dynamic,1)
    for (long jo=0; jo<std::min(nOuter,outerLimit); jo++){
      int tid=omp_get_thread_num();
      static thread_local uint8_t T[MAXLEN][26];
      auto& rm=kg.rightMid[jo/676]; int oM=(jo/26)%26, oR=jo%26;
      int pR=rm.first, wm=rm.second;
      MKey k{o.refl,o.greek,o.wL,o.wM,o.wR,0,0,wm,pR,0,md(wm-oM),md(pR-oR)};
      Inner in; in.build(k,n);
      int ng = o.greek>=0?26:1;
      for (int oL=0;oL<26;oL++) for (int g=0; g<ng; g++){
        k.pL=oL; k.pG=g; in.path(k,n,T);
        Scorer sc{T3.t.data(),n,ct,T};
        int S[26]; for(int i=0;i<26;i++)S[i]=i; bool fx[26]={0};
        float s=climb(sc,S,fx,order,o.maxplugs,evT[tid]);
        Cand c; c.s=s; c.k=k; memcpy(c.S,S,sizeof S); heap_push(heaps[tid],o.topA,c);
        if (hasTruth){ allT[tid].push_back(s); if (memcmp(T,Tt,sizeof(uint8_t)*26*n)==0){ truthA=s; fprintf(stderr,"[truth-equivalent key enumerated: %s stageA=%.0f]\n", key_str(k).c_str(), s);} }
        keysT[tid]++;
      }
    }
    for(int t=0;t<NT;t++){ keys+=keysT[t]; ev+=evT[t]; for(auto&c:heaps[t]) heap_push(heapA,o.topA,c); allA.insert(allA.end(),allT[t].begin(),allT[t].end()); }
    double tA=now()-t0;
    if (hasTruth){ for(float x: allA) if (x>truthA) beatTruth++; }
    fprintf(stderr,"stageA: %ld keys (of %ld) in %.1fs = %.0f keys/s, %.1f Mevals/s%s\n", keys,total,tA,keys/tA,ev/tA/1e6,
      hasTruth? (" truth stageA rank="+std::to_string(beatTruth+1)).c_str():"");
    // stage B
    std::sort(heapA.begin(),heapA.end(),[](const Cand&a,const Cand&b){return a.s>b.s;});
    std::vector<Cand> fin; double t1=now(); long evB=0;
    fin.resize(heapA.size());
    #pragma omp parallel for schedule(dynamic,1) reduction(+:evB)
    for (long j=0;j<(long)heapA.size();j++){
      static thread_local uint8_t T[MAXLEN][26];
      auto& c=heapA[j]; make_path(c.k,n,T); Scorer sc{T3.t.data(),n,ct,T};
      Cand d=c; d.s=istecker(sc,seeds,order,o.maxplugs,d.S,evB);
      fin[j]=d;
    }
    double tB=now()-t1;
    fprintf(stderr,"stageB: %zu keys x %zu seeds in %.1fs\n", heapA.size(), seeds.size(), tB);
    // final ranking: output both scores
    struct Row{ float tri,quad; Cand c; std::string p; };
    std::vector<Row> rows;
    for (auto& c: fin){ make_path(c.k,n,T); uint8_t p[MAXLEN]; decrypt(T,c.S,ct,n,p); rows.push_back({c.s/(n-2),Q4.score(p,n),c,to_str(p,n)}); }
    std::sort(rows.begin(),rows.end(),[](const Row&a,const Row&b){return a.tri>b.tri;});
    FILE* f = o.out.empty()? stdout : fopen(o.out.c_str(),"w");
    fprintf(f,"# job refl=%s greek=%s wheels=%s-%s-%s keys=%ld stageA=%.1fs stageB=%.1fs\n", REFL_NAME[o.refl], o.greek>=0?ROTOR_NAME[o.greek]:"-", ROTOR_NAME[o.wL],ROTOR_NAME[o.wM],ROTOR_NAME[o.wR], keys,tA,tB);
    for (int i=0;i<(int)rows.size() && i<o.topN;i++){ auto& r=rows[i];
      fprintf(f,"%8.1f\t%7.4f\t%s\t%s\t%s\n", r.tri, r.quad, key_str(r.c.k).c_str(), plug_str(r.c.S).c_str(), r.p.c_str()); }
    if (f!=stdout) fclose(f);
    return 0;
  }
  return 1;
}

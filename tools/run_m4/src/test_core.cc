#include "core.h"
static int L(char c){return c-'A';}
static void run(const char* name, MKey k, const char* plugs, const char* ct, const char* expect_prefix){
  uint8_t c[MAXLEN]; int n=0; for(const char*p=ct;*p;p++) c[n++]=*p-'A';
  static uint8_t T[MAXLEN][26]; make_path(k,n,T); int S[26]; parse_plugs(plugs,S); uint8_t o[MAXLEN]; decrypt(T,S,c,n,o);
  std::string s=to_str(o,n); bool ok = s.compare(0,strlen(expect_prefix),expect_prefix)==0;
  printf("%s %s\n  %s\n", ok?"PASS":"FAIL", name, s.c_str());
}
int main(){
  // CFYZR: B 5-3-1 rings LWB start BER
  run("CFYZR", MKey{2,-1,4,2,0, 0,L('B'),L('E'),L('R'), L('L'),L('W'),L('B')}, "BT CH DR EW FU GK JO LV MS PZ",
      "NFOSOIFKXNEMBCXCWMSCMORVYWSVHFBZJHNEMQFWZQOLUIZBFFBSNKSQXSHRDAMFRSESGJJD","ANROEMEINSBERTAXQUARTIER");
  // U-264: B-thin Beta II IV I rings AAAV start VJNA (greek ring A)
  run("U264", MKey{0,8,1,3,0, L('V'),L('J'),L('N'),L('A'), 0,0,L('V')}, "AT BL DF GJ HM NW OP QY RZ VX",
      "NCZWVUSXPNYMINHZXMQXSFWXWLKJAHSHNMCOCCAKUQPMKCSMHKSEINJUSBLKIOSXCKUBHMLLXCSJUSRRDVKOHULXWCCBGVLIYXEOAHXRHKKFVDREWEZLXOBAFGYUJQUKGRTVUKAMEURBVEKSUHHVOYHABCJWMAKLFKLMYFVNRIZRVVRTKOFDANJMOLBGFFLEOPRGTFLVRHOWOPBEKVWMUQFMPWPARMFHAGKXIIBG","VONVONJLOOKS");
  // IPZAYK: 1 May Potsdam B-thin Gamma IV III VIII rings VCCH (greek ring V) msg key PUYY -> offsets
  MKey k{0,9,3,2,7, md(L('P')-L('V')), L('U'),L('Y'),L('Y'), L('C'),L('C'),L('H')};
  run("IPZAYK", k, "CH EJ NV OU TY LG SZ PK DI QB","JCRSAJTGSJEYEXYKKZZSHVUOCTRFRCRPFVYPLKPPLGRHVVBBTBRSXSWXGGTYTVKQNGSCHVGF","IPZAYK");
}

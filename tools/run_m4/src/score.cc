#include "core.h"
int main(int argc,char**argv){ NGram T3; T3.load(argv[1],3,'l'); NGram Q4; Q4.load(argv[2],4,'c');
 char buf[4096]; while(fgets(buf,sizeof buf,stdin)){ uint8_t p[MAXLEN]; int n=0; for(char*c=buf;*c;c++){int x=*c; if(x>='A'&&x<='Z'&&n<MAXLEN)p[n++]=x-'A';}
 if(!n)continue; printf("tri=%8.1f quad=%7.4f n=%d %s", T3.score(p,n), Q4.score(p,n), n, buf);} }

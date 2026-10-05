import re,glob,os
base='/opt/enigma-p1030680/archives'
trans=set(os.path.basename(f)[:-4] for f in glob.glob(base+'/transcripts578/DEFE*.txt'))
for d in sorted(glob.glob(base+'/ocr/DEFE-3-578_0*'))+[base+'/ocr/DEFE-3-744']:
    vol=os.path.basename(d)
    for f in sorted(glob.glob(d+'/*.txt')):
        n=int(os.path.basename(f)[:-4]); name=f'{vol}_p{n:03d}'
        t=open(f,errors='ignore').read().upper()
        if re.search(r'/\s*1\s*/\s*5\s*/\s*4',t) or re.search(r'/1/5\b',t):
            hdr=re.search(r'(KC/?S.{0,60})',t.replace('\n',' '))
            print(name,'T' if name in trans else '-', hdr.group(1)[:70] if hdr else t.replace('\n',' ')[40:120])

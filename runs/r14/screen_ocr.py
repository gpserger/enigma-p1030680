import re,glob,os
base='/opt/enigma-p1030680/archives'
trans=set()
for f in glob.glob(base+'/transcripts578/DEFE*.txt'):
    b=os.path.basename(f)[:-4]  # DEFE-3-578_01_p001
    trans.add(b)
kw=re.compile(r'TRAINING|25TH|26TH|4TH U|4THU|31ST|5TH U|5THU|TF ?\d|T\.F|WUENSCHE|WUNSCHE|4701|4702|4703|STRANDER|FRONT ?LINE|WARNEMUEND|TRAVEMUEND|NEUSTADT|ACCEPTANCE|RECOVERY|4860|MOSEL|KREFELD|HERTHA',re.I)
out=[]
for d in sorted(glob.glob(base+'/ocr/DEFE-3-578_0*'))+[base+'/ocr/DEFE-3-744']:
    vol=os.path.basename(d)
    for f in sorted(glob.glob(d+'/*.txt')):
        n=int(os.path.basename(f)[:-4])
        t=open(f,errors='ignore').read()
        T=t.upper().replace('O','0') # crude for digits
        toos=re.findall(r'T[0O]{2}\s*:?\s*([0-9]{4})',T)
        tois=re.findall(r'T[0O][1I]\s*:?\s*([0-9?]{4})/([0-9]{1,2})/([0-9]{1,2})',T)
        may1=any(dd=='1' and mm=='5' for _,dd,mm in tois) or '/1/5/45' in T
        name=f'{vol}_p{n:03d}'
        k=sorted(set(m.group(0).upper() for m in kw.finditer(t)))
        inwin=[x for x in toos if 1150<=int(x)<=1635]
        if (may1 and (inwin or not toos)) or (k and ('/1/5' in T or '/2/5' in T)):
            first=' '.join(t.split('\n')[1:9])[:200].replace('\n',' ')
            out.append((name, name in trans, toos[:4], tois[:3], k, first))
for o in out: print(o[0],'T' if o[1] else '-',o[2],o[3],o[4],'|',o[5])

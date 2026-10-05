import re
cr=[l.strip() for l in open('E.k0cand.txt')]
st={int(m.group(1)):int(m.group(2)) for m in re.finditer(r'crib (\d+) .*?block-stops=(\d+)',open('out/E_probe.log').read())}
k=[c for i,c in enumerate(cr) if st.get(i,10**9)<=15000]
open('E.run.txt','w').write(''.join(c+'\n' for c in k))
open('E.probe.tsv','w').write(''.join(f'{c}\t{st.get(i)}\n' for i,c in enumerate(cr)))
print(len(k),'of',len(cr))

#!/usr/bin/env python3
"""Crib helper: collision (self-encipherment) test for P1030680.
usage: cc.py CRIB [OFF|all]   -> prints collisions per offset ('?' ignored)"""
import sys
CT=open('/opt/enigma-p1030680/ciphertext.txt').read().strip()
def coll(crib,off):
    return [off+i for i,c in enumerate(crib) if c!='?' and off+i<len(CT) and CT[off+i]==c]
if __name__=='__main__':
    crib=sys.argv[1].upper(); a=sys.argv[2] if len(sys.argv)>2 else 'all'
    offs=range(0,len(CT)-len(crib)+1) if a=='all' else [int(x) for x in a.split(',')]
    ok=[]
    for o in offs:
        c=coll(crib,o)
        if a!='all' or True: print(f'off {o:2d} len {len(crib)} end {o+len(crib)} collisions {c}')
        if not c: ok.append(o)
    print('clean offsets:',ok)

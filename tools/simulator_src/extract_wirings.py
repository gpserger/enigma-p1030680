#!/usr/bin/env python3
"""Extracts rotor/reflector/ETW wiring tables and model presets from the
Universal Enigma simulator's JS source (Daniel Palloks, v2.6.2 EN).

Usage:
    iconv -f ISO-8859-1 -t UTF-8 enigma-u_v262_en.html > enigma-u_v262_en.utf8.html
    python3 extract_wirings.py enigma-u_v262_en.utf8.html
"""
import re
import sys


def main(path):
    with open(path, encoding='utf-8') as f:
        src = f.read()

    print("=== ROTORS (walze[]) ===")
    walze_re = re.compile(r"walze\[(\d+)\]\s*=\s*\[(.*?)\];", re.DOTALL)
    for m in walze_re.finditer(src):
        parts = re.findall(r"'((?:[^'\\]|\\.)*)'", m.group(2))
        print(m.group(1), parts)

    print("\n=== REFLECTORS (ukw[]) ===")
    ukw_re = re.compile(r"ukw\[(\d+)\]\s*=\s*\[(.*?)\];")
    for m in ukw_re.finditer(src):
        parts = re.findall(r"'((?:[^'\\]|\\.)*)'", m.group(2))
        print(m.group(1), parts)

    print("\n=== M4 special constants (thin reflectors, Greek wheels) ===")
    for name in ['ukbduenn', 'ukcduenn', 'walzebeta', 'walzegamma']:
        mm = re.search(name + r"\s*=\s*'([a-z]+)'", src)
        print(name, mm.group(1) if mm else None)

    print("\n=== ETW wirings ===")
    for name in ['etq', 'ett']:
        mm = re.search(r"var " + name + r"\s*=\s*'([a-z]+)'", src)
        print(name, mm.group(1) if mm else None)

    print("\n=== commercial wiring blocks (D/K/Swiss-K/A28/G-machines) ===")
    for name in ['com1', 'com2', 'com3']:
        mm = re.search(name + r"\s*=\s*'([a-z]+)'", src)
        print(name, mm.group(1) if mm else None)

    print("\n=== UKW-D presets (upairs[]) ===")
    up_re = re.compile(r"upairs\[(\d+)\]\s*=\s*\[(.*?)\];")
    for m in up_re.finditer(src):
        parts = re.findall(r"'((?:[^'\\]|\\.)*)'", m.group(2))
        print(m.group(1), parts)

    print("\n=== udlabels ===")
    mm = re.search(r"udlabels\s*=\s*'([^']+)'", src)
    print(mm.group(1) if mm else None)

    print("\n=== models m[] (presets) ===")
    mmod_re = re.compile(r"m\[(\d+)\]\s*=\s*\{(.*?)\};", re.DOTALL)
    for m in mmod_re.finditer(src):
        body = re.sub(r'\s+', ' ', m.group(2)).strip()
        print(m.group(1), body)


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else 'enigma-u_v262_en.utf8.html')

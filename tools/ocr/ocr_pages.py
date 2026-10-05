#!/usr/bin/env python3
"""Render PDF pages to /tmp and OCR them with rapidocr. Output: archives/ocr/<pdf>/<page>.txt
Usage: ocr_pages.py PDF [first last]"""
import sys, os, subprocess, tempfile, glob
from rapidocr_onnxruntime import RapidOCR
pdf = sys.argv[1]; first = int(sys.argv[2]) if len(sys.argv) > 2 else 1
n = int(subprocess.check_output(['pdfinfo', pdf]).decode().split('Pages:')[1].split()[0])
last = int(sys.argv[3]) if len(sys.argv) > 3 else n
base = os.path.splitext(os.path.basename(pdf))[0]
out = os.path.join('archives/ocr', base); os.makedirs(out, exist_ok=True)
ocr = RapidOCR()
for p in range(first, last + 1):
    txt = os.path.join(out, f'{p:04d}.txt')
    if os.path.exists(txt): continue
    with tempfile.TemporaryDirectory(dir='/tmp') as td:
        subprocess.run(['pdftoppm', '-f', str(p), '-l', str(p), '-r', '200', '-gray', '-png', pdf, f'{td}/pg'], check=True)
        img = glob.glob(f'{td}/pg*.png')[0]
        res, _ = ocr(img)
    lines = []
    if res:
        # sort by y then x; group into lines by y proximity
        boxes = sorted(res, key=lambda r: (r[0][0][1], r[0][0][0]))
        cur, cy = [], None
        for box, text, conf in boxes:
            y = box[0][1]
            if cy is None or abs(y - cy) < 18:
                cur.append((box[0][0], text)); cy = y if cy is None else cy
            else:
                lines.append(' '.join(t for _, t in sorted(cur))); cur, cy = [(box[0][0], text)], y
        if cur: lines.append(' '.join(t for _, t in sorted(cur)))
    open(txt, 'w').write('\n'.join(lines) + '\n')
    print(f'{base} p{p} {len(lines)} lines', flush=True)

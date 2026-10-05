#!/usr/bin/env python3
"""Standalone verification of py-enigma against the real, historical M4
Kriegsmarine test vector -- the first message broken by Stefan Krah's M4
Project, on 20 Feb 2006. It's a contact/attack report from U-264 (Kptlt.
Hartwig Looks), forced to submerge under depth-charge attack, with a short
weather addendum at the end ("VONVONJLOOKS" = "von, von [Kptlt.] Looks",
the sender's self-identification). See
https://www.bytereef.org/m4-project-first-break.html and
https://www.bytereef.org/m4_project.html for the full history and the
German/English text of the decoded message; the exact date the message was
originally transmitted (as opposed to broken, in 2006) is not confirmed
here. Settings and ciphertext are as published by Dirk Rijmenants /
bytereef.org and are also bundled as a unit test inside py-enigma itself
(enigma/tests/test_enigma.py::KriegsmarineTestCase).

Run with: /opt/enigma-p1030680/.venv/bin/python3 verify_py_enigma_m4.py
"""
from enigma.machine import EnigmaMachine

stecker = '1/20 2/12 4/6 7/10 8/13 14/23 15/16 17/25 18/26 22/24'

machine = EnigmaMachine.from_key_sheet(
    rotors='Beta II IV I',
    ring_settings='A A A V',
    reflector='B-Thin',
    plugboard_settings=stecker)

ciphertext = (
    'FCLC QRKN NCZW VUSX PNYM INHZ XMQX SFWX WLKJ AHSH NMCO CCAK UQPM KCSM'
    'HKSE INJU SBLK IOSX CKUB HMLL XCSJ USRR DVKO HULX WCCB GVLI YXEO AHXR'
    'HKKF VDRE WEZL XOBA FGYU JQUK GRTV UKAM EURB VEKS UHHV OYHA BCJW MAKL'
    'FKLM YFVN RIZR VVRT KOFD ANJM OLBG FFLE OPRG TFLV RHOW OPBE KVWM UQFM'
    'PWPA RMFH AGKX IIBG FCLC QRKM VA'
).replace(' ', '')

# The first and last 2 four-letter groups are message indicators, not
# ciphertext, and the trailing 'VA' is a leftover partial group -- strip them,
# exactly as the original 2006 break did.
ciphertext = ciphertext[8:-10]

expected = (
    'VONV ONJL OOKS JHFF TTTE'
    'INSE INSD REIZ WOYY QNNS'
    'NEUN INHA LTXX BEIA NGRI'
    'FFUN TERW ASSE RGED RUEC'
    'KTYW ABOS XLET ZTER GEGN'
    'ERST ANDN ULAC HTDR EINU'
    'LUHR MARQ UANT ONJO TANE'
    'UNAC HTSE YHSD REIY ZWOZ'
    'WONU LGRA DYAC HTSM YSTO'
    'SSEN ACHX EKNS VIER MBFA'
    'ELLT YNNN NNNO OOVI ERYS'
    'ICHT EINS NULL'
).replace(' ', '')

machine.set_display('VJNA')
plaintext = machine.process_text(ciphertext)

print('Machine  : M4, rotors Beta II IV I, rings AAAV, reflector B-Thin')
print('Plugboard:', stecker)
print('Start pos: VJNA')
print('Ciphertext (post-indicator-strip):')
print(' ', ciphertext)
print('Decrypted plaintext:')
print(' ', plaintext)
print('Expected plaintext:')
print(' ', expected)
print('End position (should be VJWY):', machine.get_display())
print()
print('MATCH:', plaintext == expected)

assert plaintext == expected, 'py-enigma FAILED to reproduce the known M4 plaintext!'
assert machine.get_display() == 'VJWY'
print('\npy-enigma M4 verification: PASS')

from enigma.machine import EnigmaMachine
ct="NFOSOIFKXNEMBCXCWMSCMORVYWSVHFBZJHNEMQFWZQOLUIZBFFBSNKSQXSHRDAMFRSESGJJD"
print(len(ct))
m=EnigmaMachine.from_key_sheet(rotors='V III I',reflector='B',ring_settings='L W B',plugboard_settings='BT CH DR EW FU GK JO LV MS PZ')
m.set_display('BER')
print(m.process_text(ct))

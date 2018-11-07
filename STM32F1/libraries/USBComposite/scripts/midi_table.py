from math import *

def freq(m):
	return 27.5 * 2**((m-21)/12.)

print("static constexpr uint32 midiNoteFrequency_10ths[128] = {", end='')
for i in range(128):
	if not i % 16:
		print("\n ",end='')
	print("%d, " % int(floor(freq(i)*10 + 0.5)), end='')
print("};")
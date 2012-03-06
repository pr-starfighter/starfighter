<CsoundSynthesizer>
<CsInstruments>
sr = 48000
ksmps = 32
nchnls = 1
0dbfs = 1

; Item sound:
; Four notes mixed together and some reverberation added.
; The notes are sine waves with a very short attack and decay applied,
; necessary to avoid clicking sounds.

gisine ftgen 1, 0, 16384, 10, 1
ga1 init 0

instr 1
	a1 poscil 0.4, p4, gisine
	a2 linseg 0, 0.001, 1, p3 - 0.002, 1, 0.001, 0
	ga1 = ga1 + a1 * a2
endin

instr 99
	a1 reverb ga1, 0.7
	out ga1 + a1 * 0.15
	ga1 = 0
endin
</CsInstruments>
<CsScore>
i 1 0.00 0.15 300
i 1 0.08 0.15 450
i 1 0.16 0.15 375
i 1 0.24 0.15 600
i 99 0 0.65
e
</CsScore>
</CsoundSynthesizer>


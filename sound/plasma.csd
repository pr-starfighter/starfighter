<CsoundSynthesizer>
<CsInstruments>
sr = 48000
ksmps = 32
nchnls = 1
0dbfs = 1

; Plasma shot:
; Basically a sine wave sweep from 2 kHz to 0.1 kHz in 0.1 s,
; but with white noise with an amplitude of 500 Hz applied to the frequency.

instr 1
	gisine ftgen 1, 0, 16384, 10, 1
	k1 line 2500, p3, 100
	k2 linrand 500
	a1 poscil 0.2, k1 + k2, gisine
	out a1
endin
</CsInstruments>
<CsScore>
i 1 0 0.1
e
</CsScore>
</CsoundSynthesizer>


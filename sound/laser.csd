<CsoundSynthesizer>
<CsInstruments>
sr = 48000
ksmps = 32
nchnls = 1
0dbfs = 1

; Laser shot:
; Basically a sine wave sweep from 5 kHz to 0.1 kHz in 0.1 s, decaying further to 0 Hz.
; White noise with an amplitude of 250 to 500 Hz is applied to the frequency.

instr 1
	gisine ftgen 1, 0, 16384, 10, 1
	k1 expon 5000, p3 * 0.2, 100
	k2 linrand 500
	k3 line 0.5, p3, 1
	a1 poscil 0.3, k1 + k2 * k3, gisine
	out a1
endin
</CsInstruments>
<CsScore>
i 1 0 0.5
e
</CsScore>
</CsoundSynthesizer>


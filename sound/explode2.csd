<CsoundSynthesizer>
<CsInstruments>
sr = 48000
ksmps = 32
nchnls = 1
0dbfs = 1

; Explosions
; Noise filtered by fofilter.
; After p8 s attack time, linear decay to 0.
; The filter frequency varies linearly between p4 and p5.
; The fofilter impulse rise and decay times are given by p6 and p7.
; p6 low: smoother sound, p6 high: more rumbling sound
; p7 low: low deep sound, p7 high: tending towards white noise
; p8: attack time (s)
; p9: maximum amplitude

instr 1
	pset 0, 0, 1, 50, 30, 0.01, 0.01, 0.01, 1, 1
	k1 linseg 0, p8, p9, p3 - p8, 0
	k2 line p4, p3, p5
	k3 line 1, p3, 0
	a1 noise k1, 0.1
	a1 fofilter a1, k2, p6, p7
	a1 limit a1, -1, 1
	out a1 * p10 * k3
endin

; Metalic sound

instr 2
	k1 line 1, p3, 0
	a1 barmodel 1, 1, p4, 0.001, 0.23, 5, p5, p6, p7
	out a1 * k1
endin
</CsInstruments>
<CsScore>
i 2 0.0 0.1 40 0.1 5000 0.5
i 1 0 0.7 200 100 0.01 0.0006 0.05 1.0 0.15
e1
</CsScore>
</CsoundSynthesizer>

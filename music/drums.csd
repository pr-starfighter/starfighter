<CsoundSynthesizer>
<CsInstruments>
sr = 48000
ksmps = 32
nchnls = 2
0dbfs = 1

; Use Fluidsynth to render the score using soundfonts

gifluid fluidEngine; start fluidsynth engine
ifont fluidLoad "/usr/share/sounds/sf2/FluidR3_GM.sf2", gifluid, 1
fluidProgramSelect gifluid, 1, ifont, 0, 38 ;Synth bass 1
fluidProgramSelect gifluid, 2, ifont, 0, 2 ;Electric piano
fluidProgramSelect gifluid, 3, ifont, 8, 50 ;Synth strings
fluidProgramSelect gifluid, 10, ifont, 128, 25 ;TR-808 drums

; Second engine so we can vary its volume

gifluid2 fluidEngine; start fluidsynth engine
ifont2 fluidLoad "/usr/share/sounds/sf2/FluidR3_GM.sf2", gifluid2, 1
fluidProgramSelect gifluid2, 2, ifont2, 0, 2 ;Electric piano

gkmix init 0


instr 1, 3, 10
	ikey	notnum
	ivel	ampmidi 127
		fluidNote gifluid, p1, ikey, ivel
endin

instr 2
	ikey	notnum
	ivel	ampmidi 127
		fluidNote gifluid2, p1, ikey, ivel
endin

instr 98
	gkmix line p4, p3, p5
endin

instr 99
	iamp init 3
	al, ar fluidOut gifluid
	al2, ar2 fluidOut gifluid2
	alo = al + al2 * gkmix
	aro = ar + ar2 * gkmix
	outs alo * iamp, aro * iamp
endin
</CsInstruments>
<CsScore>
; Tempo
t 0 150

; Final output
i 99 0 3600

; Crescendo's for the piano chords
i 98 36 32 0 1
i 98 196 32 0 1

e 1
</CsScore>
</CsoundSynthesizer>

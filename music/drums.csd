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
fluidProgramSelect gifluid, 2, ifont, 0, 29 ;Distortion guitar
fluidProgramSelect gifluid, 10, ifont, 128, 25 ;TR-808 drums

instr 1, 2, 10
	ikey	notnum
	ivel	ampmidi 127
		fluidNote gifluid, p1, ikey, ivel
endin

instr 99
	al, ar fluidOut gifluid
	al, ar reverbsc al, ar, 0.6, 10000
	outs al, ar
endin
</CsInstruments>
<CsScore>
i 99 0 3600
e 1
</CsScore>
</CsoundSynthesizer>

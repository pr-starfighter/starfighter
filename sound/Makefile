SFX = plasma explode explode2 explode3 explode4
OGG = $(SFX:%=%.ogg)
WAV = $(SFX:%=%.wav)

all: $(OGG)

%.ogg: %.wav
	oggenc $<

%.wav: %.csd
	csound -g -o $@ $<

clean:
	rm -f $(OGG) $(WAV)

.PHONY: all clean

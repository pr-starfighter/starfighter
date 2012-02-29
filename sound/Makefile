SFX = plasma
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

SONGS = space warm-up number2
OGG = $(SONGS:%=%.ogg)
WAV = $(SONGS:%=%.wav)
MID = $(SONGS:%=%.mid)
PDF = $(SONGS:%=%.pdf)

all: $(OGG)

%.ogg: %.wav
	oggenc $<

%.mid: %.abc
	abc2midi $< -o $@

%.wav: %.mid %.csd
	csound -d -o $@ -T -F $< `basename $< .mid`.csd

%.pdf: %.abc
	abcm2ps -B 4 -O - $< | ps2pdf - $@

pdf: $(PDF)

clean:
	rm -f $(OGG) $(WAV) $(MID) $(PDF)

.PHONY: all clean

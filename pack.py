#!/usr/bin/env python
# pack.py - pack files into a Parallel Realities Starfighter .pak file
# Copyright (C) 2006 Josh Triplett <josh@freedesktop.org
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License only.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

import struct, sys

def pack(pak, files):
    pak.write("PACK")
    for f in files:
        data = f.read()
        pak.write(f.name + "\x00"*(56-len(f.name)))
        pak.write(struct.pack("<I", len(data)))
        pak.write(data)

def main(args):
    try:
        pak = args[1]
        return pack(file(pak, "wb"), [file(filename) for filename in args[2:]])
    except IndexError:
        print "Usage: pack filename.pak files..."
    except (IOError, OSError), e:
        print e
    return 1

__all__ = ["pack"]

if __name__ == "__main__": sys.exit(main(sys.argv))

#!/usr/bin/env python
# unpack.py - unpack files from a Parallel Realities Starfighter .pak file
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

import os, struct, sys

class PakException(Exception): pass

class BadMagic(PakException):
    def __init__(self, pak):
        PakException.__init__(self,
            '%s: not a pack file (does not start with "PACK")' % pak.name)

class ShortRead(PakException):
    def __init__(self, pak):
        PakException.__init__(self, '%s: file corrupt (short read)' % pak.name)

class PathTraversal(PakException):
    def __init__(self, pak, filename):
        PakException.__init__(self,
            '%s: attempted directory traversal with filename "%s"'
            % (pak.name, filename))

def full_read(pak, size):
    data = pak.read(size)
    if len(data) != size: raise ShortRead(pak)
    return data

def unpack(pak):
    pwd = os.getcwd()
    if full_read(pak, 4) != "PACK": raise BadMagic(pak)
    while True:
        filename = pak.read(56)
        if filename == "": break # End of file
        elif len(filename) < 56: raise ShortRead(pak)
        filename = filename.split("\x00")[0]
        (size,) = struct.unpack("<I", full_read(pak, 4))
        data = full_read(pak, size)
        dirname = os.path.abspath(os.path.dirname(filename))
        if(not(dirname.startswith(pwd))):
            raise PathTraversal(pak, filename)
        try: os.makedirs(dirname)
        except OSError, e:
            if e.errno != 17: raise # Ignore "file exists"
        out = file(filename, "w+")
        out.write(data)
        out.close()

def main(args):
    try:
        pak = args[1]
        return unpack(file(pak, "rb"))
    except IndexError:
        print "Usage: unpack filename.pak"
    except (IOError, OSError, PakException), e:
        print e
    return 1

__all__ = ["unpack", "PakException", "BadMagic", "ShortRead", "PathTraversal"]

if __name__ == "__main__": sys.exit(main(sys.argv))

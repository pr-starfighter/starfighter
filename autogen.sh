#!/bin/sh

echo "Generating configure script..."
autoreconf -vif
echo "Done."
echo "Building locales..."
cd locale
./build.py
cd ..

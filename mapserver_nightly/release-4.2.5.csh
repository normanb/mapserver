#!/bin/csh

setenv CVSROOT /data2/cvsroot
unalias rm

set VERSION = "4.2.5"

cvs -Q export -r rel-4-2-5 mapserver

mv mapserver mapserver-$VERSION
cd mapserver-$VERSION

rm -rf gdft

flex -Pmsyy -i -omaplexer.c maplexer.l
bison -p msyy -d -omapparser.c mapparser.y

cd mapscript/perl
swig -perl5 -shadow -outdir . -o mapscript_wrap.c ../mapscript.i

# =====================================================================
# Create Python module wrappers

cd ../python

# ---------------------------------------------------------------------
# old school wrapper for Python 2.1 or older
swig -python -shadow -outdir . -o mapscript_wrap.c ../mapscript.i 

# ---------------------------------------------------------------------
# modern wrapper for Python 2.2+
#
# the Python setup.py script will try to use modern_mapscript_wrap.c
# if it exists.

# test to see if swig supports modern 
set num = `swig -python -help |& grep -c '\-modern'`

# if so, run swig again
if ($num =~ "1") mkdir modern; swig -python -modern -outdir modern -o modern/mapscript_wrap.c ../mapscript.i

# =====================================================================
# End Python wrappers


cd ../java
mkdir -p edu/umn/gis/mapscript
swig -java -package edu.umn.gis.mapscript -outdir edu/umn/gis/mapscript -o mapscript_wrap.c ../mapscript.i

# cd ../tcl
 
cd ../../..

tar cf mapserver-$VERSION.tar mapserver-$VERSION
gzip *.tar

cp mapserver-$VERSION.tar.gz /data2/web-docs/ms-cvs/docs/dist
chmod 755 /data2/web-docs/ms-cvs/docs/dist/mapserver-$VERSION.tar.gz

rm mapserver-$VERSION.tar.gz

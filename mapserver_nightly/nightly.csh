#!/bin/csh

setenv CVSROOT /data2/cvsroot
unalias rm

cvs -Q checkout mapserver

mv mapserver mapserver_dev
cd mapserver_dev

#cp /export/home/WWW/MAPS/docs/dist/gd-1.2.tar.gz .
#gunzip gd-1.2.tar.gz
#tar xf gd-1.2.tar
#rm gd-1.2.tar

rm -rf gdft

flex -Pmsyy -i -omaplexer.c maplexer.l
bison -p msyy -d -omapparser.c mapparser.y

cd mapscript/perl
swig -perl5 -shadow -outdir . -o mapscript_wrap.c ../mapscript.i

cd ../python
swig -python -shadow -outdir . -o mapscript_wrap.c ../mapscript.i 

cd ../java

cd ../tcl
 
cd ../../..

tar cf nightly.tar mapserver_dev
gzip nightly.tar

cp nightly.tar.gz /data2/web-docs/ms-cvs/docs/dist

rm *.gz
rm -rf mapserver_dev




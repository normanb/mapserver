#!/usr/bin/perl

use DBI;
use mapscript;

$file = 'polygon';

%types = ( 'point' => 1,
	   'arc' => 3,
	   'polygon' => 5,
	   'multipoint' => 8
	 );

system('rm *.shp *.shx *.dbf'); # remove old files

# create the shapefile
$shapefile = new shapefileObj($file, $types{polygon}) or die "Unable to create shapefile '$file'. ", $mapscript::ms_error->{message};

# create the xbase file
$dbh = DBI->connect("DBI:XBase:./");
$sth = $dbh->prepare("create table $file ( id int, comment char(100) )");
$sth->execute() or die $sth->errstr();

# now create a few shapes
$point = new pointObj();
$line = new lineObj();

$point->{x} = 50;
$point->{y} = 50;
$line->add($point);
$point->{x} = 100;
$point->{y} = 100;
$line->add($point);
$point->{x} = 0;
$point->{y} = 100;
$line->add($point);
$point->{x} = 50;
$point->{y} = 50;
$line->add($point);

$shape = new shapeObj();
$shape->add($line);

$shapefile->add($shape);

$shapefile = '';

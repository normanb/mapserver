#!/usr/local/bin/perl

use CGI qw(:standard :html escape);
use Net::Finger;
use mapscript;

$quake_user = 'quake';
$quake_server = 'gldfs.cr.usgs.gov';
$quake_port = 79;
@quakes = '';
$quake_data = '';
$quake_timestamp = '';

sub get_quakes {
    $quake_data = finger('Username' => $quake_user, 'Host' => $quake_server, 'Port' => $quake_port); # use array reference

    my $i = 0;
    my $is_quake = 0;
    foreach (@$quake_data) {
	chop;

	$quake_timestamp = $_ if /^Updated as of/; 
	
	if($is_quake) {
	    $quakes[$i]{lat} = substr($_, 18, 7);
	    $quakes[$i]{lon} = substr($_, 26, 7);
	    $quakes[$i]{magnitude} = substr($_, 40, 3);
	    
	    if($quakes[$i]{lon} =~ /E$/) {
		$quakes[$i]{lon} =~ s/E//;
	    } else {
		$quakes[$i]{lon} =~ s/W//;
		$quakes[$i]{lon} *= -1;
	    }
	    
	    if($quakes[$i]{lat} =~ /N$/) {
		$quakes[$i]{lat} =~ s/N//;
	    } else {
		$quakes[$i]{lat} =~ s/S//;
		$quakes[$i]{lat} *= -1;
	    }

	    $i++;
	}

	$is_quake = 1 if /^yy/;
    }

    return;
}

$image_path = '/usr/local/www/docs_arachnid/tmp/';
$image_virtual_path = '/tmp/';
$image_id = $$ . time() . ".gif";
$map = '';

$zoom_direction = 0; # defaults
$zoom_size = 2;

sub set_extent() {
    my $zoom, @imgext;
    my $x, $y;
    my $cellsizex, $cellsizey;  

    if($cgi->param('imgext')) { # if an interactive interface then calculate a new extent
	@imgext = split(' ', $cgi->param('imgext'));
	$x = $cgi->param('img.x');
	$y = $cgi->param('img.y');
	$zoom_size = $cgi->param('zoomsize');
	$zoom_direction = $cgi->param('zoomdir');

	if($zoom_direction == 0) { # pan
	    $zoom = 1;
	} else { # in or out
	    $zoom = $zoom_size*$zoom_direction;
	    $zoom = 1.0/abs($zoom) if $zoom < 0;
	}

	$cx = ($imgext[2]-$imgext[0])/($map->{width}-1); # calculate cellsize in x and y
	$cy = ($imgext[3]-$imgext[1])/($map->{height}-1);

	$x = $imgext[0] + $cx*$x; # change x,y from image to map coordinates, offset from UL corner of previous image
	$y = $imgext[3] - $cy*$y;

	$map->{extent}->{minx} = $x - .5*(($imgext[2] - $imgext[0])/$zoom); # calculate new extent
	$map->{extent}->{miny} = $y - .5*(($imgext[3] - $imgext[1])/$zoom);
	$map->{extent}->{maxx} = $x + .5*(($imgext[2] - $imgext[0])/$zoom);
	$map->{extent}->{maxy} = $y + .5*(($imgext[3] - $imgext[1])/$zoom);
   }
}

sub render_quakes {
    die $mapscript::ms_error->{message} unless $map = new mapObj("/export/stlime/mapserv/ms_test/perl/quakes/quakes.map");
    
    &set_extent();

    my $img = $map->prepareImage();

    my $point = new pointObj();
    
    my $layer = $map->getLayerByName('relief');
    $layer->draw($map, $img); # draw basemap

    $layer = $map->getLayerByName('quakes');
    my $class = $layer->getClass(0);
    my $i = 1; # quake counter
    foreach my $q (@quakes) {
	$class->{size} = int($q->{magnitude}*2);
	$class->{maxsize} = $class->{minsize} = $class->{size}; # don't want to scale

	$point->{x} = $q->{lon};
	$point->{y} = $q->{lat};
	$point->draw($map, $layer, $img, undef, "$i");
	$i++;
    }

    #$layer = $map->getLayerByName('timestamp'); # add timestamp
    #$layer->{status} = 1; # turn the layer on
    #$class = $layer->getClass(0);
    #$class->setText($quake_timestamp);
    #$layer->draw($map, $img);
    
    $map->drawLabelCache($img);
    mapscript::msSaveImage($img, $image_path . $image_id, 1,1);

    return;
}

# here is the main program
$cgi = new CGI;

&get_quakes();
&render_quakes();

# now the html

print header();
print start_html(-title=>'MapServer Test Suite - Earthquake Mapper', -bgcolor=>"#ffffff");

print "<!-- " .$mapscript::MS_VERSION. " -->";
print "<!-- " .$map->{scale}. " -->";

print "<center><table width=\"600\"><tr><td>";

print "<font size=\"+2\" face=\"arial,helvetica\"><b>MapScript - Earthquake Mapper</b></font><p>";

print "This is a simple of example of using MapScript. Near real-time earthquake data is retrieved ";
print "from the USGS \"finger\" servers. The resulting information is parsed using perl and each ";
print "quake is plotted on top of a basemap. The size of the marker indicates quake magnitude. ";
print "Many other possible enhancements.  If the interface seems a bit slow it's because the ";
print "USGS servers can be difficult to reach- the perils of remote data. Here's the ";
print "<a href=\"/ms_test/perl/quakes/mapquakes.pl\">source</a> code.<p>";

print "<form name=\"mapquakes\" action=\"/cgi-bin/mapquakes.pl\" method=\"get\">";
print "<center>";
print "<input border=\"0\" type=\"image\" name=\"img\" src=\"". $image_virtual_path . $image_id ."\"><br>";
print "<input type=\"hidden\" name=\"imgext\" value=\"" . join(' ', $map->{extent}->{minx},$map->{extent}->{miny},$map->{extent}->{maxx},$map->{extent}->{maxy}) ."\">";

if($zoom_direction == 0) {
    print "<input type=\"radio\" name=\"zoomdir\" value=\"1\"> zoom in &nbsp;&nbsp;";
    print "<input type=\"radio\" name=\"zoomdir\" value=\"0\" checked> pan &nbsp;&nbsp;";
    print "<input type=\"radio\" name=\"zoomdir\" value=\"-1\"> zoom out &nbsp;&nbsp;&nbsp;&nbsp;";
} else {
    if($zoom_direction == -1) {
	print "<input type=\"radio\" name=\"zoomdir\" value=\"1\"> zoom in &nbsp;&nbsp;";
	print "<input type=\"radio\" name=\"zoomdir\" value=\"0\"> pan &nbsp;&nbsp;";
	print "<input type=\"radio\" name=\"zoomdir\" value=\"-1\" checked> zoom out &nbsp;&nbsp;&nbsp;&nbsp;";
    } else {
	print "<input type=\"radio\" name=\"zoomdir\" value=\"1\" checked> zoom in &nbsp;&nbsp;";
	print "<input type=\"radio\" name=\"zoomdir\" value=\"0\"> pan &nbsp;&nbsp;";
	print "<input type=\"radio\" name=\"zoomdir\" value=\"-1\"> zoom out &nbsp;&nbsp;&nbsp;&nbsp;";
    }
}

print "zoom size <input type=\"text\" size=\"2\" name=\"zoomsize\" value=\"$zoom_size\">";

print "</center>";
print "</form>";

print "<p><font face=\"arial,helvetica\"><b>Here's some of the output from the USGS:</b></font><p>";
print "<p><pre>";

$show = 0;
$number = 0;
foreach (@$quake_data) {
    $show = 1 if /Updated/;    
   
    printf "<b>%2d</b>: ", $number if $show and $number;
    print "    ". $_ if $show and !$number;
    print $_ if $show and $number;
    $number++ if $show and $number;

    $number = 1 if /^yy/;
}

print "</pre>";

print "</td></tr></table>";

print end_html();
exit(0);

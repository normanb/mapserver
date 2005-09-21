.. $Id$

   ===========================================================================
   Copyright (c) 2005 Jeff McKenna, DM Solutions Group Inc.
   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:
 
   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.
 
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
   ===========================================================================
..

*****************************************************************************
 MapServer Filter Encoding (FE) HOWTO - Version 4.6
*****************************************************************************

:Author: Jeff McKenna
:Contact: jmckenna@dmsolutions.ca
:Author: Yewondwossen Assefa
:Contact: assefa@dmsolutions.ca
:Revision: $Revision$
:Last Updated: $Date$

..  The next heading encountered becomes our H2
..

.. sectnum::

.. contents:: Table of Contents
    :depth: 2
    :backlinks: top


Introduction
============

This document describes the procedures for taking advantage of the Filter 
Encoding (FE) support in WFS GetFeature requests, which was added to 
MapServer in version 4.2.  

This document assumes that you are already familiar with the following 
aspects of MapServer:

- MapServer application development and setting up .map files.

- Familiarity with the WFS specification would be an asset. Links to the 
  MapServer WFS documents are included in the next section. 

Links to SLD-related Information
--------------------------------

- `Filter Encoding Implementation Specification`_.
- `MapServer WFS Client HOWTO`_.
- `MapServer WFS Server HOWTO`_.
- `Open GIS Consortium (OGC) home page`_.

.. _`Filter Encoding Implementation Specification`: http://www.opengeospatial.org/docs/02-059.pdf
.. _`MapServer WFS Client HOWTO`: http://ms.gis.umn.edu/docs/howto/wfs_client
.. _`MapServer WFS Server HOWTO`: http://ms.gis.umn.edu/docs/howto/wfs_server
.. _`Open GIS Consortium (OGC) home page`: http://www.opengeospatial.org

Currently Supported Features
============================

The following table lists the currently supported features for FE.

**Table 1. Currently Supported Features**

+----------------------+-------------------------------------+
| Feature Set          | Feature                             |
+======================+=====================================+
| Spatial Capabilities |                                     |
+----------------------+-------------------------------------+
|                      | BBox                                |
|                      +-------------------------------------+   
|                      | Intersects                          |
|                      +-------------------------------------+     
|                      | DWithin                             |
+----------------------+-------------------------------------+
| Scalar Capabilities  |                                     |
+----------------------+-------------------------------------+
|                      |                                     |
+----------------------+-------------------------------------+
| Logical Operators    |                                     |
+----------------------+-------------------------------------+
|                      | And                                 |
|                      +-------------------------------------+   
|                      | Or                                  |
|                      +-------------------------------------+     
|                      | Not                                 |
+----------------------+-------------------------------------+
| Comparison Operators |                                     |
+----------------------+-------------------------------------+
|                      | PropertyIsNotEqualTo (<>)           |
|                      +-------------------------------------+   
|                      | PropertyIsLessThan (<)              |
|                      +-------------------------------------+     
|                      | PropertyIsGreaterThan (>)           |
|                      +-------------------------------------+   
|                      | PropertyIsLessThanOrEqualTo (<=)    |
|                      +-------------------------------------+     
|                      | PropertyIsGreaterThanOrEqualTo (>=) |
|                      +-------------------------------------+     
|                      | PropertyIsLike                      |
|                      +-------------------------------------+     
|                      | PropertyIsBetween (range)           |
+----------------------+-------------------------------------+

Get and Post Requests
=====================

MapServer already has the capability to receive and parse Get requests and 
URL-encoded Post requests. The ability for MapServer to be able to receive 
Post requests with XML-encoded information sent in the body of the request 
has been added. Also, the ability to generate XML-encoded Post requests for 
WFS layers has been added.

Both Get and Post request are now supported for all WFS requests:

- GetCapabilities
 
- GetFeatures

- DescribeFeatureType 

Supporting these WFS requests in Post was implemented to keep consistency 
between all supported WFS requests.

When sending requests, the default request method used is Post. To change this 
behavior, we have introduced a layer level meta data, wfs_request_method, 
which can be set to "GET". 

Use of Filter Encoding in MapServer
===================================

This section describes how to use FE on both the server and client sides. 

Server Side
-----------

To be able to use Filter Encoding, you need to create a valid WFS server using 
MapServer. Please refer to the `WFS Server HOWTO`_ for specifics. 

.. _`WFS Server HOWTO`: http://ms.gis.umn.edu/docs/howto/wfs_server

There is nothing special that should be added to a WFS server for Filter 
Encoding, but you should note that, when requesting the capabilities of 
your WFS server, the document returned should contain the supported filters. 
Here is part of a Capabilities document as well as the Post support:

::

    <?xml version='1.0' encoding="ISO-8859-1" ?>
    <WFS_Capabilities 
       version="1.0.0" 
       updateSequence="0" 
       xmlns="http://www.opengis.net/wfs" 
       xmlns:ogc="http://www.opengis.net/ogc" 
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://www.opengis.net/wfs http://ogc.dmsolutions.ca/wfs/1.0.0/WFS-capabilities.xsd">

    <!-- MapServer version 4.1 OUTPUT=GIF OUTPUT=PNG OUTPUT=JPEG OUTPUT=WBMP 
    OUTPUT=PDF OUTPUT=SWF SUPPORTS=PROJ SUPPORTS=FREETYPE SUPPORTS=WMS_SERVER 
    SUPPORTS=WMS_CLIENT SUPPORTS=WFS_SERVER SUPPORTS=WFS_CLIENT INPUT=POSTGIS 
    INPUT=OGR INPUT=GDAL INPUT=SHAPEFILE -->

    <Service>
      <Name>MapServer WFS</Name>
      <Title>GMap WMS Demo Server</Title>
      <OnlineResource>http://localhost/cgi-bin/mapserv.exe?map=
        c:/msapps/wfs_filter/htdocs/ns_wfsserver.map&</OnlineResource>
    </Service>

    <Capability>
      <Request>
        <GetCapabilities>
          <DCPType>
            <HTTP>
              <Get onlineResource="http://localhost/cgi-bin/mapserv.exe?map=
                c:/msapps/wfs_filter/htdocs/ns_wfsserver.map&" />
            </HTTP>
          </DCPType>
          <DCPType>
            <HTTP>
              <Post onlineResource="http://localhost/cgi-bin/mapserv.exe?map=
                c:/msapps/wfs_filter/htdocs/ns_wfsserver.map&" />
            </HTTP>
          </DCPType>
        </GetCapabilities>
        ...
      </Request>
    </Capability>
    ...
    <Filter_Capabilities>
      <Spatial_Capabilities>
        <Spatial_Operators>
                 <Intersect/>
                 <DWithin/>
               <BBOX/>
        </Spatial_Operators>
      </Spatial_Capabilities>
      <Scalar_Capabilities>
        <Logical_Operators />
        <Comparison_Operators>
          <Simple_Comparisons />
          <Like />
          <Between />
        </Comparison_Operators>
      </Scalar_Capabilities>
    </Filter_Capabilities>

    </WFS_Capabilities>
    
Client Side
-----------

To be able to generate a Filter to a WFS server, a layer level metadata called 
*wfs_filter* has been added, which should contain the filter to be sent to the 
server. Following is an example of a valid WFS client layer with a filter:

::

    LAYER
      NAME popplace
      METADATA
        "wfs_version"   "1.0.0"       
        "wfs_typename"  "popplace"
        "wfs_filter"    "<PropertyIsGreaterThan><PropertyName>POP_RANGE</PropertyName>
                                         <Literal>4</Literal></PropertyIsGreaterThan>"        
      END
      TYPE POINT
      STATUS ON
      PROJECTION
        "init=epsg:42304"
      END
      CONNECTIONTYPE WFS
      CONNECTION "http://www2.dmsolutions.ca:8090/cgi-bin/mswfs_filter?"
      LabelItem "Name"  
      CLASSITEM "Capital" 
      CLASS
        SYMBOL 2
        SIZE 8
        NAME "Cities"
        LABEL
          COLOR  255 0 0
          FONT fritqat-italic
          TYPE truetype
          SIZE 8
          POSITION AUTO         
          OUTLINECOLOR 255 255 255  
          FORCE TRUE
        END 
        COLOR 0 0 0
      END
    END # Layer

*Note:*

- The filter given as a value of the wfs_filter metadata should not contain 
  <Filter> start and end tags.

- The CONNECTION points to a valid WFS server supporting filters

- The returned shapes will be drawn using the class defined in the layer.

Limitations
===========

- A limited set of spatial operators are supported. 


Tests
=====

Here are some test URLs for the different Filters supported:

- `PropertyIsEqualTo`_
- `PropertyIsNotEqualTo`_
- `PropertyIsLessThan`_
- `PropertyIsGreaterThan`_
- `PropertyIsLessThanOrEqualTo`_
- `PropertyIsGreaterThanOrEqualTo`_
- `PropertyIsBetween`_
- `PropertyIsLike`_
- `Logical operator OR`_
- `Logical operator AND`_ 
- `Logical operator NOT`_
- `Spatial operator BBOX`_
- `Spatial operator Dwithin`_
- `Spatial operator Intersects`_

.. _`PropertyIsEqualTo`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><PropertyIsEqualTo><PropertyName>NAME</PropertyName><Literal>Digby</Literal></PropertyIsEqualTo></Filter>
.. _`PropertyIsNotEqualTo`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><PropertyIsNotEqualTo><PropertyName>NAME</PropertyName><Literal>Digby</Literal></PropertyIsNotEqualTo></Filter>
.. _`PropertyIsLessThan`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><PropertyIsLessThan><PropertyName>POP_RANGE</PropertyName><Literal>2</Literal></PropertyIsLessThan></Filter>
.. _`PropertyIsGreaterThan`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><PropertyIsGreaterThan><PropertyName>POP_RANGE</PropertyName><Literal>5</Literal></PropertyIsGreaterThan></Filter>
.. _`PropertyIsLessThanOrEqualTo`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><PropertyIsLessThanOrEqualTo><PropertyName>POP_RANGE</PropertyName><Literal>2</Literal></PropertyIsLessThanOrEqualTo></Filter>
.. _`PropertyIsGreaterThanOrEqualTo`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><PropertyIsGreaterThanOrEqualTo><PropertyName>POP_RANGE</PropertyName><Literal>5</Literal></PropertyIsGreaterThanOrEqualTo></Filter>
.. _`PropertyIsBetween`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><PropertyIsBetween><PropertyName>POP_RANGE</PropertyName><LowerBoundary>5</LowerBoundary><UpperBoundary>6</UpperBoundary></PropertyIsBetween></Filter>
.. _`PropertyIsLike`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><PropertyIsLike wildcard='*' singleChar='.' escape='!'><PropertyName>NAME</PropertyName><Literal>Syd*</Literal></PropertyIsLike></Filter>
.. _`Logical operator OR`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><OR><PropertyIsEqualTo><PropertyName>NAME</PropertyName><Literal>Sydney</Literal></PropertyIsEqualTo><PropertyIsEqualTo><PropertyName>NAME</PropertyName><Literal>Digby</Literal></PropertyIsEqualTo></OR></Filter>
.. _`Logical operator AND`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><AND><PropertyIsLike wildcard='*' singleChar='.' escape='!'><PropertyName>NAME</PropertyName><Literal>Syd*</Literal></PropertyIsLike><PropertyIsEqualTo><PropertyName>POP_RANGE</PropertyName><Literal>4</Literal></PropertyIsEqualTo></AND></Filter>
.. _`Logical operator NOT`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><AND><NOT><PropertyIsEqualTo> <PropertyName>POP_RANGE</PropertyName><Literal>2</Literal></PropertyIsEqualTo></NOT><NOT><PropertyIsEqualTo><PropertyName>POP_RANGE</PropertyName><Literal>3</Literal></PropertyIsEqualTo></NOT></AND></Filter> 
.. _`Spatial operator BBOX`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><BBOX><PropertyName>Name>NAME</PropertyName><Box srsName='EPSG:42304'><coordinates>2518000,150000 2918000,450000</coordinates></Box></BBOX></Filter>
.. _`Spatial operator Dwithin`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><DWithin><PropertyName>Geometry</PropertyName><gml:Point><gml:coordinates>2587598.000000,404010.531250</gml:coordinates></gml:Point><Distance units='m'>1000</Distance></DWithin></Filter>
.. _`Spatial operator Intersects`: http://www2.dmsolutions.ca/cgi-bin/mswfs_filter?&VERSION=1.0.0&SERVICE=WFS&REQUEST=GetFeature&TYPENAME=popplace&Filter=<Filter><Intersect><PropertyName>Geometry</PropertyName><gml:Polygon><gml:outerBoundaryIs><gml:LinearRing><gml:coordinates>2488789,289552 2588789,289552 2588789,389552 2488789,389552 2488789,289552</gml:coordinates></gml:LinearRing></gml:outerBoundaryIs></gml:Polygon></Intersect></Filter>
   
About This Document
===================

Copyright Information
---------------------

Copyright (c) 2005, Yewondwossen Assefa, Jeff McKenna.
                
This documentation is covered by the same Open Source license as the MapServer 
software itself.  See MapServer's `License and Credits`__ page for the complete 
text.
            
__ http://mapserver.gis.umn.edu/license.html   

Disclaimer
----------

No liability for the contents of this document can be accepted.
Use the concepts, examples and other content at your own risk.
As this is a new edition of this document, there may be errors
and inaccuracies that may be damaging to your system.
Although this is highly unlikely, the author(s) do not take any 
responsibility for that:  proceed with caution.


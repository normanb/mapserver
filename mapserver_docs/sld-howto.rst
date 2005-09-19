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
 MapServer Styled Layer Descriptor (SLD) HOWTO - Version 4.6
*****************************************************************************

:Author: Yewondwossen Assefa
:Contact: assefa@dmsolutions.ca
:Author: Jeff McKenna
:Contact: jmckenna@dmsolutions.ca
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

This document describes the procedures for taking advantage of the Styled 
Layer Descriptor (SLD) support in WMS GetMap requests for MapServer 4.6.
SLD support has been developed for the server side support (ability to 
read an SLD and apply it with a GetMap request) and for the client side 
(includes sending SLD requests to server and generate SLD files on the fly 
from MapServer map file).

This document assumes that you are already familiar with the following 
aspects of MapServer:

- MapServer application development and setting up *.map* files.
- Familiarity with the WMS specification would be an asset. Links to the 
  MapServer WMS documents are included in the next section.

Links to SLD-related Information
--------------------------------

- `Styled Layer Descriptor Implementation Specification`_.
- `MapServer WMS Client HOWTO`_.
- `MapServer WMS Server HOWTO`_.
- `Open GIS Consortium (OGC) home page`_.

.. _`Styled Layer Descriptor Implementation Specification`: http://www.opengeospatial.org/docs/02-070.pdf
.. _`MapServer WMS Client HOWTO`: http://ms.gis.umn.edu/docs/howto/wms_client
.. _`MapServer WMS Server HOWTO`: http://ms.gis.umn.edu/docs/howto/wms_server
.. _`Open GIS Consortium (OGC) home page`: http://www.opengeospatial.org

Server Side Support
===================

General Information
-------------------

There are two ways a WMS request can pass an SLD document with a GetMap 
request to MapServer:

- SLD parameter pointing to remote SLD (SLD=http://URL_TO_SLD).
- SLD_BODY parameter to send the SLD definition in the URL.

These two methods are both available through MapServer. An example of a request 
would be:

http://www2.dmsolutions.ca/cgi-bin/mswms_world?SERVICE=WMS&amp;VeRsIoN=1.1.1&amp;Request=GetMap&amp;LAYERS=WorldGen_Outline&amp;SLD=http://www2.dmsolutions.ca/msapps/world_testdata/tests/sld_tests/sld_line_simple.xml

The SLD in the above request follows:

::

        <StyledLayerDescriptor version="1.0.0">
          <NamedLayer>
            <Name>WorldGen_Outline</Name>
            <UserStyle>
              <Title>xxx</Title>
              <FeatureTypeStyle>
                <Rule>
                <LineSymbolizer>
                 <Geometry>
                   <PropertyName>center-line</PropertyName>
                 </Geometry>
                 <Stroke>
                   <CssParameter name="stroke">#0000ff</CssParameter>
                 </Stroke>
                </LineSymbolizer>
                </Rule>
              </FeatureTypeStyle>
            </UserStyle>
          </NamedLayer>
        </StyledLayerDescriptor> 
        
When MapServer gets a valid SLD through a request, it parses this SLD to extract 
all the styles attached to the NamedLayers, and then it applies these styles to 
the map before it is returned to the client. When applying the SLD, MapServer 
compares the names used in the map files with the names of the NamedLayers in 
the SLD document.

Note : All the examples given in this document are live uses of valid SLDs and 
a MapServer installation with SLD support.

Additional WMS features related to SLDs have also been developed:

*Table1. Additional WMS Features*

=====================  ========= =============================
Features               Supported Notes
=====================  ========= =============================
Method GET : SLD URL   Yes
Method GET : SLD_BODY  Yes       Additional item
Describer Layer        Yes
GetLegendGraphic       Yes
GetStyles              Yes       Uses MapScript to get the SLD
=====================  ========= =============================

About This Document
===================

Copyright Information
---------------------

Copyright (c) 2005, Jean-François Doyon, Jeff McKenna.
                
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


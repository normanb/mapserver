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


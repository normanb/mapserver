# $Id$
#
# Project:  MapServer
# Purpose:  Comprehensive xUnit style Python mapscript test suite
# Author:   Sean Gillies, sgillies@frii.com
#
# ===========================================================================
# Copyright (c) 2004, Sean Gillies
# 
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
# ===========================================================================
#
# Execute this module as a script from mapserver/mapscript/python
#
#     python tests/runtests.py -v
#
# ===========================================================================

import unittest

# Import test cases
from cases.hashtest import HashTableTestCase
from cases.owstest import OWSRequestTestCase 
from cases.clonetest import MapCloningTestCase
from cases.maptest import MapConstructorTestCase, MapLayersTestCase
from cases.layertest import LayerConstructorTestCase
from cases.recttest import RectObjTestCase

# Create a test suite
suite = unittest.TestSuite()

# Add tests to the suite
suite.addTest(HashTableTestCase)
suite.addTest(MapCloningTestCase)
suite.addTest(OWSRequestTestCase)
suite.addTest(MapConstructorTestCase)
suite.addTest(MapLayersTestCase)
suite.addTest(LayerConstructorTestCase)
suite.addTest(RectObjTestCase)

# If module is run as a script, execute every test case in the suite
if __name__ == '__main__':
    unittest.main()


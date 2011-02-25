/******************************************************************************
 * $Id$
 *
 * Project:  MapServer
 * Purpose:  OpenGIS Web Coverage Server (WCS) Declarations.
 * Author:   Steve Lime, Frank Warmerdam and the MapServer Team
 *
 ******************************************************************************
 * Copyright (c) 1996-2005 Regents of the University of Minnesota.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies of this Software or works derived from this Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef MAPWCS_H
#define MAPWCS_H

#include "mapserver.h"
#include "mapowscommon.h"
#include <limits.h>
#include <time.h>
#include <float.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

/*
 * Definitions
 */

#define MS_WCS_GML_COVERAGETYPE_RECTIFIED_GRID_COVERAGE "RectifiedGridCoverage"


/*
** Structure to hold metadata taken from the image or image tile index
*/
typedef struct {
  const char *srs;
  char srs_urn[500];
  rectObj extent, llextent;
  double geotransform[6];
  int xsize, ysize;
  double xresolution, yresolution;
  int bandcount; 
  int imagemode;
  const char *bandinterpretation[10];
} coverageMetadataObj;

typedef struct {
  char *version;		/* 1.0.0 for now */
  char *updatesequence;		/* string, int or timestampe */
  char *request;		/* GetCapabilities|DescribeCoverage|GetCoverage */
  char *service;		/* MUST be WCS */
  char *section;		/* of capabilities document: /WCS_Capabilities/Service|/WCS_Capabilities/Capability|/WCS_Capabilities/ContentMetadata */
  char **coverages;		/* NULL terminated list of coverages (in the case of a GetCoverage there will only be 1) */
  char *crs;	        /* request coordinate reference system */
  char *response_crs;	/* response coordinate reference system */
  rectObj bbox;		    /* subset bounding box (3D), although we'll only use 2D */
  char *time;
  long width, height, depth;	/* image dimensions */
  double originx, originy;      /* WCS 1.1 GridOrigin */
  double resx, resy, resz;      /* resolution */
  char *interpolation;          /* interpolationMethod */
  char *format;
  char *exceptions;		/* exception MIME type, (default application=vnd.ogc.se_xml) */
} wcsParamsObj;

/* -------------------------------------------------------------------- */
/*      Prototypes from mapwcs.c used in mapwcs11.c.                    */
/*                                                                      */
/*      Note, all prototypes are deliberately not exported from DLL     */
/*      since they are for internal use within the core.                */
/* -------------------------------------------------------------------- */
void msWCSFreeParams(wcsParamsObj *params);
int msWCSException(mapObj *map, const char *code, const char *locator, 
                   const char *version);
int msWCSIsLayerSupported(layerObj *layer);
int msWCSGetCoverageMetadata( layerObj *layer, coverageMetadataObj *cm );
void msWCSSetDefaultBandsRangeSetInfo( wcsParamsObj *params,
                                       coverageMetadataObj *cm,
                                       layerObj *lp );
const char *msWCSGetRequestParameter(cgiRequestObj *request, char *name);

/* -------------------------------------------------------------------- */
/*      Some WCS 1.1 specific functions from mapwcs11.c                 */
/* -------------------------------------------------------------------- */
int msWCSGetCapabilities11(mapObj *map, wcsParamsObj *params, 
                           cgiRequestObj *req);
int msWCSDescribeCoverage11(mapObj *map, wcsParamsObj *params );
int msWCSReturnCoverage11( wcsParamsObj *params, mapObj *map, imageObj *image);
int msWCSGetCoverageBands11( mapObj *map, cgiRequestObj *request, 
                             wcsParamsObj *params, layerObj *lp,
                             char **p_bandlist );
int msWCSException11(mapObj *map, const char *locator, 
                     const char *exceptionCode, const char *version);


/* -------------------------------------------------------------------- */
/*      Some WCS 2.0 specific functions and structs from mapwcs20.c     */
/* -------------------------------------------------------------------- */

enum
{
    MS_WCS20_TRIM = 0,
    MS_WCS20_SLICE = 1
};

enum
{
    MS_WCS20_ERROR_VALUE = -1,
    MS_WCS20_SCALAR_VALUE = 0,
    MS_WCS20_TIME_VALUE = 1,
    MS_WCS20_UNDEFINED_VALUE = 2
};

#define MS_WCS20_UNBOUNDED DBL_MAX
#define MS_WCS20_UNBOUNDED_TIME 0xFFFFFFFF

typedef struct
{
    union
    {
        double scalar;
        time_t time;
    };
    int unbounded; /* 0 if bounded, 1 if unbounded */
} timeScalarUnion;

typedef struct
{
    char *axis;         /* the name of the subsetted axis */
    int operation;      /* Either TRIM or SLICE */
    char *crs;          /* optional CRS to use */
    int timeOrScalar;   /* 0 if scalar value, 1 if time value */
    timeScalarUnion min; /* Minimum and Maximum of the subsetted axis;*/
    timeScalarUnion max;
} wcs20SubsetObj;
typedef wcs20SubsetObj * wcs20SubsetObjPtr;

typedef struct
{
    char *name;         /* name of the axis */
    int size;           /* pixelsize of the axis */
    double resolution;  /* resolution of the axis */
    char *resolutionUOM; /* resolution units of measure */
    wcs20SubsetObjPtr subset;
} wcs20AxisObj;
typedef wcs20AxisObj * wcs20AxisObjPtr;

typedef struct
{
    char *version;      /* 2.0.0 */
    char *request;      /* GetCapabilities|DescribeCoverage|GetCoverage */
    char *service;      /* MUST be WCS */
    char **accept_versions; /* NULL terminated list of Accepted versions */
    char **sections;    /* NULL terminated list of GetCapabilities sections */
    char *updatesequence; /* GetCapabilities updatesequence */
    char **ids;         /* NULL terminated list of coverages (in the case of a GetCoverage there will only be 1) */
    long width, height; /* image dimensions */
    double resolutionX; /* image resolution in X axis */
    double resolutionY; /* image resolution in Y axis */
    char *resolutionUnits; /* Units of Measure for resolution */
    char *format;       /* requested output format */
    int multipart;      /* flag for multipart GML+image */
    char *interpolation; /* requested interpolation method */
    char *outputcrs;    /* requested CRS for output */
    char *subsetcrs;    /* determined CRS of the subsets */
    rectObj bbox;       /* determined Bounding Box */
    int numaxes;        /* number of axes */
    wcs20AxisObjPtr *axes; /* list of axes, NULL if none*/
    char **invalid_get_parameters; /* NULL terminated list of invalid GET parameters */
} wcs20ParamsObj;
typedef wcs20ParamsObj * wcs20ParamsObjPtr;

typedef struct
{
    union
    {
        struct
        {
            char *name;
            char *interpretation;
            char *uom;
            char *definition;
            char *description;
        };
        char *values[5];
    };
} wcs20rasterbandMetadataObj;
typedef wcs20rasterbandMetadataObj * wcs20rasterbandMetadataObjPtr;

typedef struct
{
    char *native_format;    /* mime type of the native format */
    const char *srs;
    char srs_uri[200];
    rectObj extent;
    double geotransform[6];
    double xresolution;
    double yresolution;
    int xsize;
    int ysize;
    int imagemode;
    size_t numnilvalues;
    char **nilvalues;
    char **nilvalues_reasons;
    size_t numbands;
    wcs20rasterbandMetadataObjPtr bands;
} wcs20coverageMetadataObj;
typedef wcs20coverageMetadataObj * wcs20coverageMetadataObjPtr;

#define MS_WCS_20_PROFILE_CORE      "http://www.opengis.net/spec/WCS/2.0/conf/core"
#define MS_WCS_20_PROFILE_KVP       "http://www.opengis.net/spec/WCS_protocol-binding_get-kvp/1.0"
#define MS_WCS_20_PROFILE_POST      "http://www.opengis.net/spec/WCS_protocol-binding_post-xml/1.0"
#define MS_WCS_20_PROFILE_GEOTIFF   "http://www.opengis.net/spec/WCS_coverage-encoding_geotiff/1.0/"
#define MS_WCS_20_PROFILE_GML_GEOTIFF "http://www.placeholder.com/GML_and_GeoTIFF"
#define MS_WCS_20_PROFILE_EPSG      "http://www.placeholder.com/EPSG"
#define MS_WCS_20_PROFILE_IMAGECRS  "http://www.placeholder.com/IMAGECRS"
#define MS_WCS_20_PROFILE_SCALING   "http://www.placeholder.com/SCALING"
#define MS_WCS_20_PROFILE_INTERPOLATION "http://www.placeholder.com/INTERPOLATION"

int msWCSDispatch20(mapObj *map, cgiRequestObj *request);

int msWCSException20(mapObj *map, const char *locator,
                     const char *exceptionCode, const char *version);

/* Makro to continue the iteration over an xml structure    */
/* when the current node has the type 'text' or 'comment'   */
#define XML_LOOP_IGNORE_COMMENT_OR_TEXT(node)                   \
    if(xmlNodeIsText(node) || node->type == XML_COMMENT_NODE)   \
    {                                                           \
        continue;                                               \
    }

/* Makro to set an XML error that an unknown node type      */
/* occurred.                                                */
#define XML_UNKNOWN_NODE_ERROR(node,function)                   \
    msSetError(MS_WCSERR, "Unknown XML element '%s'.",          \
            function, (char *)node->name);                      \
    return MS_FAILURE;

#define MS_WCS_20_CAPABILITIES_INCLUDE_SECTION(params,section)  \
    (params->sections == NULL                                   \
    || CSLFindString(params->sections, "All") != -1             \
    || CSLFindString(params->sections, section) != -1)

#endif /* nef MAPWCS_H */

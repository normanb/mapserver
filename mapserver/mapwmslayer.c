/**********************************************************************
 * $Id$
 *
 * Project:  MapServer
 * Purpose:  Implementation of WMS CONNECTIONTYPE - client to WMS servers
 * Author:   Daniel Morissette, DM Solutions Group (morissette@dmsolutions.ca)
 *
 **********************************************************************
 * Copyright (c) 2001, Daniel Morissette, DM Solutions Group Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 **********************************************************************
 * $Log$
 * Revision 1.11  2001/11/01 02:46:29  dan
 * Added msWMSGetFeatureInfoURL()
 *
 * Revision 1.10  2001/10/29 16:45:47  dan
 * Uncommented the unlink() call to delete the downloaded map image
 *
 * Revision 1.9  2001/10/11 22:29:20  dan
 * Took out terminate_handler 2.  Tested, working fine on Linux.
 *
 * Revision 1.8  2001/10/09 22:02:00  assefa
 * Use the non-preemptive mode : tested on windows.
 *
 * Revision 1.7  2001/08/22 15:56:58  dan
 * Added wms_latlonboundingbox metadata to check if layer overlaps.
 *
 * Revision 1.6  2001/08/22 04:48:34  dan
 * Create wld file for the WMS map slide so that it can be reprojected by GDAL
 *
 * Revision 1.5  2001/08/21 23:12:26  dan
 * Test layer status at beginning of msDrawWMSLayer()
 *
 * Revision 1.4  2001/08/21 19:11:53  dan
 * Improved error handling, verify HTTP status, etc.
 *
 * Revision 1.3  2001/08/17 20:08:07  dan
 * A few more fixes... we're able to load layers from cubeserv in GMap... cool!
 *
 * Revision 1.2  2001/08/17 17:54:32  dan
 * Handle SRS and BBOX properly.  Still have to reproject image received 
 * from server.
 *
 * Revision 1.1  2001/08/14 21:26:54  dan
 * Initial revision - only loads and draws WMS CONNECTION URL for now.
 *
 **********************************************************************/

#include "map.h"
#include "maperror.h"

#include <time.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <process.h>
#endif

#ifdef USE_WMS_LYR

#define WMS_V_1_0_0  100
#define WMS_V_1_0_7  107
#define WMS_V_1_1_0  110


/*
** Note: This code uses the w3c-libwww library to access remote WMS servers 
** via the HTTP protocol.  
** See http://www.w3.org/Library/ for the lib's source code and install docs.
** See http://www.w3.org/Library/Examples/LoadToFile.c for sample code on which
** big portions of msWMSGetImage was based.
*/

#include <WWWLib.h>
#include <WWWHTTP.h>
#include <WWWInit.h>

static int tracer (const char * fmt, va_list pArgs)
{
/* Do not write to stderr on windoze as this makes Apache hang. */
#ifndef _WIN32
    return (vfprintf(stderr, fmt, pArgs));
#endif
}


#ifdef PREEMPTIVE_MODE

/* Preemptive version: no event loop ... does noe work on Windows.
 * we should take that version out eventually
 */

/*
**  We get called here from the event loop when we are done
**  loading.
*/
static int terminate_handler (HTRequest * request, HTResponse * response,
                              void * param, int status)
{

    if (status != 200)
    {
        static char errmsg[100];
        msDebug("HTTP GET request returned status %d\n", status);

        sprintf(errmsg, "HTTP GET request failed with status %d", status);
        msSetError(MS_WMSCONNERR, errmsg, "msGetImage()");
    }

    if (param) 
        *((int*)param) = status;

    return 0;
}


/**********************************************************************
 *                          msWMSGetImage()
 *
 * Fetch a map slide via HTTP request and save to specified temp file.
 *
 * Based on sample code from http://www.w3.org/Library/Examples/LoadToFile.c
 **********************************************************************/
int msWMSGetImage(const char *geturl, const char *outputfile)
{
    HTRequest * request;
    int nHTTPStatus = 0;

    int nStatus = MS_SUCCESS;

    if (geturl == NULL || outputfile == NULL)
    {
        msSetError(MS_WMSCONNERR, "URL or output file parameter missing.", 
                   "msGetImage()");
        return(MS_FAILURE);
    }

    request = HTRequest_new();
    HTProfile_newPreemptiveClient("MapServer WMS Client", "1.0");

    /* Add our own after filter to return HTTP status */
    HTNet_addAfter(terminate_handler, NULL, &nHTTPStatus, 
                   HT_ALL, HT_FILTER_LAST);

    HTPrint_setCallback(tracer);
    HTTrace_setCallback(tracer);

    if (HTLoadToFile(geturl, request, outputfile) != YES)
    {
        msDebug("HTLoadFile() != YES\n");
        msSetError(MS_WMSCONNERR, "HTLoadToFile Error.", "msWMSGetImage()");
        nStatus = MS_FAILURE;
    }

    if (nHTTPStatus != 200)
    {
        // msSetError() already called in terminate_handler
        nStatus = MS_FAILURE;
    }


#ifdef __TODO__
    HTResponse *response;
    const char * mimetype;  // Should be HTFormat

    response = HTRequest_response(request);
    mimetype = HTResponse_format(response);
    msDebug("Content-type: %s\n",  mimetype?mimetype:"(null)");
    if (mimetype == NULL)
    {
        msSetError(MS_WMSCONNERR, "Remote server error or invalid response.", 
                   "msWMSGetImage()");
        nStatus = MS_FAILURE;
    }
    else if (strcmp(mimetype, "image/gif") != 0 ||
             strcmp(mimetype, "image/png") != 0 ||
             strcmp(mimetype, "image/jpeg") != 0 ||
             strcmp(mimetype, "image/wbmp") != 0   )
    {
        char err[256];
        sprintf(err, "Unsupported image format: %s", mimetype);
        msSetError(MS_WMSCONNERR, err, "msWMSGetImage()");
        nStatus = MS_FAILURE;
    }
#endif

    HTRequest_delete(request);                  /* Delete the request object */
    HTProfile_delete();

    return nStatus;
}


#else

/* Non-preemptive version: uses an event loop.  
 * Works on both Linux and Windows.
 */

/*
**  We get called here from the event loop when we are done
**  loading.
*/
static int terminate_handler (HTRequest * request, HTResponse * response,
                              void * param, int status)
{

    if (status != 200)
    {
        static char errmsg[100];
        msDebug("HTTP GET request returned status %d\n", status);

        sprintf(errmsg, "HTTP GET request failed with status %d", status);
        msSetError(MS_WMSCONNERR, errmsg, "msGetImage()");
    }

    if (param) 
        *((int*)param) = status;

    HTEventList_stopLoop();
    return 0;
}


/**********************************************************************
 *                          msWMSGetImage()
 *
 * Fetch a map slide via HTTP request and save to specified temp file.
 *
 * Based on sample code from http://www.w3.org/Library/Examples/LoadToFile.c
 **********************************************************************/
int msWMSGetImage(const char *geturl, const char *outputfile)
{
    HTRequest *         request = NULL;
    int                 nHTTPStatus = 0;
    int                 nStatus = MS_SUCCESS;

    /* Initiate W3C Reference Library with a client profile */
    HTProfile_newNoCacheClient("MapServer WMS Client", "1.0");

    /* And the traces... */
#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Need our own trace and print functions */
    HTPrint_setCallback(tracer);
    HTTrace_setCallback(tracer);

    /* Add our own filter to terminate the application */
    HTNet_addAfter(terminate_handler, NULL, &nHTTPStatus, HT_ALL, HT_FILTER_LAST);

    /* Set the timeout for how long we are going to wait for a response */
    HTHost_setEventTimeout(25000);

    if (geturl == NULL || outputfile == NULL)
    {
        msSetError(MS_WMSCONNERR, "URL or output file parameter missing.", 
                   "msGetImage()");
        return(MS_FAILURE);
    }

    request = HTRequest_new();

    /* Start the load */
    if (HTLoadToFile(geturl, request, outputfile) != YES) 
    {
        msSetError(MS_WMSCONNERR, "Can't open output file.", "msGetImage()");
        HTProfile_delete();
        return(MS_FAILURE);
    }

    /* Go into the event loop... */
    HTEventList_loop(request);

    if (nHTTPStatus != 200)
    {
        // msSetError() already called in terminate_handler
        nStatus = MS_FAILURE;
    }

    HTRequest_delete(request);                  /* Delete the request object */
    HTProfile_delete();

    /* If load failed then msSetError() will have been called */
    //return (ms_error.code == 0) ? MS_SUCCESS : MS_FAILURE;
    return nStatus;
}


#endif /* USE_WMS_LYR */

#endif /* PREEMPTIVE_MODE */


/**********************************************************************
 *                          msTmpFile()
 *
 * Generate a Unique temporary filename using PID + timestamp + extension
 **********************************************************************/
const char *msTmpFile(const char *path, const char *ext)
{
    static char tmpFname[256];
    static char tmpId[128]; /* big enough for time + pid */
    static int tmpCount = -1;

    if (tmpCount == -1)
    {
        /* We'll use tmpId and tmpCount to generate unique filenames */
        sprintf(tmpId, "%ld%d",(long)time(NULL),(int)getpid());
        tmpCount = 0;
    }

    if (path == NULL) path="";
    if (ext == NULL)  ext = "";

    sprintf(tmpFname, "%s%s%d%s", path, tmpId, tmpCount++, ext);

    return tmpFname;
}


/**********************************************************************
 *                          msBuildWMSLayerURL()
 *
 * Build a GetMap or GetFeatureInfo URL.  
 *
 * Returns a reference to a newly allocated string that should be freed 
 * by the caller.
 **********************************************************************/
#define WMS_GETMAP         1
#define WMS_GETFEATUREINFO 2

char *msBuildWMSLayerURL(mapObj *map, layerObj *lp, int nRequestType,
                         int nClickX, int nClickY, int nFeatureCount,
                         const char *pszInfoFormat) 
{
#ifdef USE_WMS_LYR
    char *pszURL = NULL, *pszEPSG = NULL;
    const char *pszTmp, *pszRequestParam, *pszExceptionsParam;
    rectObj bbox;
    int nVersion;
    
    if (lp->connectiontype != MS_WMS || lp->connection == NULL)
    {
        msSetError(MS_WMSCONNERR, "Call supported only for CONNECTIONTYPE WMS",
                   "msBuildWMSLayerURL()");
        return NULL;
    }

/* ------------------------------------------------------------------
 * Find out request version
 * ------------------------------------------------------------------ */
    if ((pszTmp = strstr(lp->connection, "VERSION=")) == NULL &&
        (pszTmp = strstr(lp->connection, "version=")) == NULL &&
        (pszTmp = strstr(lp->connection, "WMTVER=")) == NULL &&
        (pszTmp = strstr(lp->connection, "wmtver=")) == NULL )
    {
        msSetError(MS_WMSCONNERR, "WMS Connection String must contain the VERSION or WMTVER parameter (with name in uppercase).", "msBuildWMSLayerURL()");
        return NULL;
    }
    pszTmp = strchr(pszTmp, '=')+1;
    if (strncmp(pszTmp, "1.0.8", 5) >= 0)    /* 1.0.8 == 1.1.0 */
        nVersion = WMS_V_1_1_0;
    else if (strncmp(pszTmp, "1.0.7", 5) >= 0)
        nVersion = WMS_V_1_0_7;
    else if (strncmp(pszTmp, "1.0.0", 5) >= 0)
        nVersion = WMS_V_1_0_0;
    else
    {
        msSetError(MS_WMSCONNERR, "MapServer supports only WMS 1.0.0 to 1.1.0 (please verify the VERSION parameter in the connection string).", "msBuildWMSLayerURL()");
        return NULL;
    }


/* ------------------------------------------------------------------
 * For GetFeatureInfo requests, make sure QUERY_LAYERS is included
 * ------------------------------------------------------------------ */
    if  (nRequestType == WMS_GETFEATUREINFO &&
         strstr(lp->connection, "QUERY_LAYERS=") == NULL &&
         strstr(lp->connection, "query_layers=") == NULL )
    {
        msSetError(MS_WMSCONNERR, "WMS Connection String must contain the QUERY_LAYERS parameter to support GetFeatureInfo requests (with name in uppercase).", "msBuildWMSLayerURL()");
        return NULL;
    }



/* ------------------------------------------------------------------
 * Figure the SRS we'll use for the request.
 * - Fetch the map SRS (if it's EPSG)
 * - Check if map SRS is listed in layer wms_srs metadata
 * - If map SRS is valid for this layer then use it
 * - Otherwise request layer in its default SRS and we'll reproject later
 * ------------------------------------------------------------------ */
    if ((pszEPSG = (char*)msWMSGetEPSGProj(&(map->projection), 
                                           NULL, TRUE)) != NULL &&
        (pszEPSG = strdup(pszEPSG)) != NULL &&
        strncasecmp(pszEPSG, "EPSG:", 5) == 0)
    {
        const char *pszLyrEPSG, *pszFound;
        int nLen;

        nLen = strlen(pszEPSG);

        pszLyrEPSG = msWMSGetEPSGProj(&(lp->projection), lp->metadata, FALSE);

        if (pszLyrEPSG == NULL ||
            (pszFound = strstr(pszLyrEPSG, pszEPSG)) == NULL ||
            ! (*(pszFound+nLen) == ' ' || *(pszFound+nLen) == '\0'))
        {
            // Not found in Layer's list of SRS (including projection object)
            free(pszEPSG);
            pszEPSG = NULL;
        }
    }

    if (pszEPSG == NULL &&
        ((pszEPSG = (char*)msWMSGetEPSGProj(&(lp->projection), 
                                            lp->metadata, TRUE)) == NULL ||
         (pszEPSG = strdup(pszEPSG)) == NULL ||
         strncasecmp(pszEPSG, "EPSG:", 5) != 0) )
    {
        msSetError(MS_WMSCONNERR, "Layer must have an EPSG projection code (in its PROJECTION object or wms_srs metadata)", "msBuildWMSLayerURL()");
        if (pszEPSG) free(pszEPSG);
        return NULL;
    }

    // We'll store the remote server's response to a tmp file.
    if (lp->data) free(lp->data);
    lp->data = strdup( msTmpFile(map->web.imagepath, "_img.tmp") );

/* ------------------------------------------------------------------
 * Set layer SRS and reproject map extents to the layer's SRS
 * ------------------------------------------------------------------ */
    // No need to set lp->proj if it's already set to the right EPSG code
    if ((pszTmp = msWMSGetEPSGProj(&(lp->projection), NULL, TRUE)) == NULL ||
        strcasecmp(pszEPSG, pszTmp) != 0)
    {
        char szProj[20];
        sprintf(szProj, "init=epsg:%s", pszEPSG+5);
        if (msLoadProjectionString(&(lp->projection), szProj) != 0)
            return NULL;
    }

    bbox = map->extent;
    if (msProjectionsDiffer(&(map->projection), &(lp->projection)))
    {
        msProjectRect(&(map->projection), &(lp->projection), &bbox);
    }

/* ------------------------------------------------------------------
 * Build the request URL.
 * At this point we set only the following parameters for GetMap:
 *   REQUEST
 *   SRS
 *   BBOX
 *
 * And for GetFeatureInfo:
 *   X
 *   Y
 *   FEATURE_COUNT
 *   INFO_FORMAT
 *
 * The connection string should contain all other required params, 
 * including:
 *   VERSION
 *   LAYERS
 *   FORMAT
 *   TRANSPARENT
 *   STYLES
 *   QUERY_LAYERS (for queriable layers only)
 * ------------------------------------------------------------------ */
    // Make sure we have a big enough buffer for the URL
    if(!(pszURL = (char *)malloc((strlen(lp->connection)+256)*sizeof(char)))) 
    {
        msSetError(MS_MEMERR, NULL, "msBuildWMSLayerURL()");
        return NULL;
    }

    // __TODO__ We have to urlencode each value... especially the BBOX values
    // because if they end up in exponent format (123e+06) the + will be seen
    // as a space by the remote server.

    if (nRequestType == WMS_GETFEATUREINFO)
    {
        if (nVersion >= WMS_V_1_0_7)
            pszRequestParam = "GetFeatureInfo";
        else
            pszRequestParam = "feature_info";

        if (nVersion >= WMS_V_1_1_0)
            pszExceptionsParam = "application/vnd.odc.se_xml";
        else if (nVersion > WMS_V_1_1_0)  /* 1.0.1 to 1.0.7 */
            pszExceptionsParam = "SE_XML";
        else
            pszExceptionsParam = "WMS_XML";

        sprintf(pszURL, 
                "%s&REQUEST=%s&WIDTH=%d&HEIGHT=%d&SRS=%s&BBOX=%f,%f,%f,%f"
                "&EXCEPTIONS=%s&X=%d&Y=%d&FEATURE_COUNT=%d&INFO_FORMAT=%s",
                lp->connection, pszRequestParam, map->width, map->height, 
                pszEPSG, bbox.minx, bbox.miny, bbox.maxx, bbox.maxy,
                pszExceptionsParam,
                nClickX, nClickY, nFeatureCount, pszInfoFormat);
    }
    else /* if (nRequestType == WMS_GETMAP) */
    {
        if (nVersion >= WMS_V_1_0_7)
            pszRequestParam = "GetMap";
        else
            pszRequestParam = "Map";

        if (nVersion >= WMS_V_1_1_0)
            pszExceptionsParam = "application/vnd.odc.se_inimage";
        else
            pszExceptionsParam = "INIMAGE";

        sprintf(pszURL, 
                "%s&REQUEST=%s&WIDTH=%d&HEIGHT=%d&SRS=%s&BBOX=%f,%f,%f,%f"
                "&EXCEPTIONS=%s",
                lp->connection, pszRequestParam, map->width, map->height, 
                pszEPSG, bbox.minx, bbox.miny, bbox.maxx, bbox.maxy,
                pszExceptionsParam);
    }

    free(pszEPSG);

    return pszURL;

#else
/* ------------------------------------------------------------------
 * WMS CONNECTION Support not included...
 * ------------------------------------------------------------------ */
  msSetError(MS_WMSCONNERR, "WMS CLIENT CONNECTION support is not available.", 
             "msBuildWMSLayerURL()");
  return NULL;

#endif /* USE_WMS_LYR */

}


/**********************************************************************
 *                          msWMSGetFeatureInfoURL()
 *
 * Build a GetFeatureInfo URL for this layer.
 *
 * Returns a reference to a newly allocated string that should be freed 
 * by the caller.
 **********************************************************************/
char *msWMSGetFeatureInfoURL(mapObj *map, layerObj *lp,
                             int nClickX, int nClickY, int nFeatureCount,
                             const char *pszInfoFormat) 
{
    return msBuildWMSLayerURL(map, lp, WMS_GETFEATUREINFO,
                              nClickX, nClickY, nFeatureCount,
                              pszInfoFormat);
}


/**********************************************************************
 *                          msDrawWMSLayer()
 *
 **********************************************************************/

int msDrawWMSLayer(mapObj *map, layerObj *lp, gdImagePtr img) 
{
#ifdef USE_WMS_LYR
    char *pszURL = NULL;
    const char *pszTmp;
    rectObj bbox;
    int status = MS_SUCCESS;
    
    if (lp->connectiontype != MS_WMS || lp->connection == NULL)
        return MS_FAILURE;

    if((lp->status != MS_ON) && (lp->status != MS_DEFAULT))
        return MS_SUCCESS;

/* ------------------------------------------------------------------
 * Check if layer overlaps current view window (using wms_latlonboundingbox)
 * ------------------------------------------------------------------ */
    if ((pszTmp = msLookupHashTable(lp->metadata, 
                                    "wms_latlonboundingbox")) != NULL)
    {
        char **tokens;
        int n;
        rectObj ext;

        tokens = split(pszTmp, ' ', &n);
        if (tokens==NULL || n != 4) {
            msSetError(MS_WMSCONNERR, "Wrong number of arguments for 'wms_latlonboundingbox' metadata.",
                       "msDrawWMSLayer()");
            return MS_FAILURE;
        }

        ext.minx = atof(tokens[0]);
        ext.miny = atof(tokens[1]);
        ext.maxx = atof(tokens[2]);
        ext.maxy = atof(tokens[3]);

        msFreeCharArray(tokens, n);

        // Reproject latlonboundingbox to the selected SRS for the layer and
        // check if it overlaps the bbox that we calculated for the request

        msProjectRect(&(map->latlon), &(lp->projection), &ext);
        if (!msRectOverlap(&bbox, &ext))
            return MS_SUCCESS;  // No overlap.
    }

/* ------------------------------------------------------------------
 * Build the request URL and download image
 * ------------------------------------------------------------------ */
    if ((pszURL = msBuildWMSLayerURL(map, lp, WMS_GETMAP, 
                                     0, 0, 0, NULL)) == NULL)
    {
        /* an error was already reported. */
        return MS_FAILURE;
    }

    msDebug("WMS GET %s\n", pszURL);
    if (msWMSGetImage(pszURL, lp->data) != MS_SUCCESS)
    {
        msDebug("WMS GET failed.\n", pszURL);
        return MS_FAILURE;
    }
    msDebug("WMS GET completed OK.\n");

    free(pszURL);

/* ------------------------------------------------------------------
 * Prepare layer for drawing, reprojecting the image received from the
 * server if needed...
 * ------------------------------------------------------------------ */
    lp->type = MS_LAYER_RASTER;

    if (!msProjectionsDiffer(&(map->projection), &(lp->projection)))
    {
        // The simple case... no reprojection needed... render layer directly.
        lp->transform = MS_FALSE;
        if (msDrawRasterLayer(map, lp, img) != 0)
            status = MS_FAILURE;
    }
    else
    {
        FILE *fp;
        char *wldfile;
        // OK, we have to resample the raster to map projection...
        lp->transform = MS_TRUE;

        // Create a world file with raster extents
        // One line per value, in this order: cx, 0, 0, cy, ulx, uly
        wldfile = strdup(lp->data);
        if (wldfile)    
            strcpy(wldfile+strlen(wldfile)-3, "wld");
        if (wldfile && (fp = fopen(wldfile, "wt")) != NULL)
        {
            fprintf(fp, "%f\n", MS_CELLSIZE(bbox.minx,bbox.maxx, map->width));
            fprintf(fp, "0\n");
            fprintf(fp, "0\n");
            fprintf(fp, "%f\n", MS_CELLSIZE(bbox.maxy,bbox.miny, map->height));
            fprintf(fp, "%f\n", bbox.minx);
            fprintf(fp, "%f\n", bbox.maxy);
            fclose(fp);

            // GDAL should be called to reproject automatically.
            if (msDrawRasterLayer(map, lp, img) != 0)
                status = MS_FAILURE;

            unlink(wldfile);
            free(wldfile);
        }
        else
        {
            msSetError(MS_WMSCONNERR, 
                       "Unable to create wld file for WMS slide.", 
                       "msDrawWMSLayer()");
            status = MS_FAILURE;
        }

    } 

    // We're done with the remote server's response... delete it.
    unlink(lp->data);

    // free(lp->data);
    lp->data = NULL;

    return status;

#else
/* ------------------------------------------------------------------
 * WMS CONNECTION Support not included...
 * ------------------------------------------------------------------ */
  msSetError(MS_WMSCONNERR, "WMS CLIENT CONNECTION support is not available.", 
             "msDrawWMSLayer()");
  return(MS_FAILURE);

#endif /* USE_WMS_LYR */

}



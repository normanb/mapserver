/**********************************************************************
 * $Id$
 *
 * mapows.h - OGC Web Services (WMS, WFS) support function definitions
 *
 **********************************************************************
 * $Log$
 * Revision 1.7  2002/11/20 21:22:32  dan
 * Added msOWSGetSchemasLocation() for use by both WFS and WMS Map Context
 *
 * Revision 1.6  2002/11/20 17:17:21  julien
 * Support version 0.1.2 of MapContext
 * Remove warning from tags
 * Encode and decode all url
 *
 * Revision 1.5  2002/10/28 20:31:20  dan
 * New support for WMS Map Context (from Julien)
 *
 * Revision 1.2  2002/10/22 20:03:57  julien
 * Add the mapcontext support
 *
 * Revision 1.4  2002/10/09 02:29:03  dan
 * Initial implementation of WFS client support.
 *
 * Revision 1.3  2002/10/08 02:40:08  dan
 * Added WFS DescribeFeatureType
 *
 * Revision 1.2  2002/10/04 21:29:41  dan
 * WFS: Added GetCapabilities and basic GetFeature (still some work to do)
 *
 * Revision 1.1  2002/09/03 03:19:51  dan
 * Set the bases for WFS Server support + moved some WMS/WFS stuff to mapows.c
 *
 **********************************************************************/

#ifndef MAPOWS_H
#define MAPOWS_H

/*====================================================================
 *   mapows.c
 *====================================================================*/
#if defined(USE_WMS_SVR) || defined (USE_WFS_SVR)

int msOWSDispatch(mapObj *map, char **names, char **values, int numentries); 
int msOWSMakeAllLayersUnique(mapObj *map);
char *msOWSGetOnlineResource(mapObj *map, const char *metadata_name);
const char *msOWSGetSchemasLocation(mapObj *map);

// OWS_NOERR and OWS_WARN passed as action_if_not_found to printMetadata()
#define OWS_NOERR   0
#define OWS_WARN    1

int msOWSPrintMetadata(FILE *stream, hashTableObj metadata, const char *name, 
                       int action_if_not_found, const char *format, 
                       const char *default_value);
int msOWSPrintEncodeMetadata(FILE *stream, hashTableObj metadata, 
                             const char *name, int action_if_not_found, 
                             const char *format, const char *default_value) ;
int msOWSPrintGroupMetadata(FILE *stream, mapObj *map, char* pszGroupName, 
                            const char *name, int action_if_not_found, 
                            const char *format, const char *default_value);
int msOWSPrintParam(FILE *stream, const char *name, const char *value, 
                    int action_if_not_found, const char *format, 
                    const char *default_value);
int msOWSPrintMetadataList(FILE *stream, hashTableObj metadata, 
                           const char *name, const char *startTag, 
                           const char *endTag, const char *itemFormat);
void msOWSPrintLatLonBoundingBox(FILE *stream, const char *tabspace, 
                                 rectObj *extent, projectionObj *srcproj);
void msOWSPrintBoundingBox(FILE *stream, const char *tabspace, 
                           rectObj *extent, 
                           projectionObj *srcproj,
                           hashTableObj metadata );
void msOWSPrintContactInfo( FILE *stream, const char *tabspace, 
                           const char *wmtver, hashTableObj metadata );
char *msEncodeHTMLEntities(const char *string);
void msDecodeHTMLEntities(const char *string);
int msOWSGetLayerExtent(mapObj *map, layerObj *lp, rectObj *ext);

#endif

/*====================================================================
 *   mapgml.c
 *====================================================================*/
int msGMLWriteQuery(mapObj *map, char *filename);

#ifdef USE_WFS_SVR
int msGMLWriteWFSQuery(mapObj *map, FILE *stream);
#endif


/*====================================================================
 *   maphttp.c
 *====================================================================*/

typedef struct http_request_info
{
    int         nLayerId;
    void      * request;  /* HTRequest * */
    char      * pszGetUrl;
    char      * pszOutputFile;
    int         nStatus;
    int         nTimeout;
    rectObj     bbox;
} httpRequestObj;

void msHTTPInitRequestObj(httpRequestObj *pasReqInfo, int numRequests);
void msHTTPFreeRequestObj(httpRequestObj *pasReqInfo, int numRequests);
int  msHTTPExecuteRequests(httpRequestObj *pasReqInfo, int numRequests);
int  msHTTPGetFile(char *pszGetUrl, char *pszOutputFile, int *pnHTTPStatus,
                   int nTimeout);


/*====================================================================
 *   mapwms.c
 *====================================================================*/
int msWMSDispatch(mapObj *map, char **names, char **values, int numentries); 



/*====================================================================
 *   mapwmslayer.c
 *====================================================================*/

int msPrepareWMSLayerRequest(int nLayerId, mapObj *map, layerObj *lp,
                             httpRequestObj *pasReqInfo, int *numRequests);
int msDrawWMSLayerLow(int nLayerId, httpRequestObj *pasReqInfo, 
                      int numRequests, mapObj *map, layerObj *lp, 
                      imageObj *img);

char *msWMSGetFeatureInfoURL(mapObj *map, layerObj *lp,
                             int nClickX, int nClickY, int nFeatureCount,
                             const char *pszInfoFormat); 


/*====================================================================
 *   mapwfs.c
 *====================================================================*/
int msWFSDispatch(mapObj *map, char **names, char **values, int numentries); 

#ifdef USE_WFS_SVR
const char *msWFSGetGeomElementName(mapObj *map, layerObj *lp);
#endif


/*====================================================================
 *   mapwfslayer.c
 *====================================================================*/

int msPrepareWFSLayerRequest(int nLayerId, mapObj *map, layerObj *lp,
                             httpRequestObj *pasReqInfo, int *numRequests);
int msDrawWFSLayerLow(int nLayerId, httpRequestObj *pasReqInfo, 
                      int numRequests, mapObj *map, layerObj *lp, 
                      imageObj *img);


/*====================================================================
 *   mapcontext.c
 *====================================================================*/

int msSaveMapContext(mapObj *map, char *filename);
int msLoadMapContext(mapObj *map, char *filename);

#endif /* MAPOWS_H */


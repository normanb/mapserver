
%include arrays_java.i

/* Mapscript library loader */

%pragma(java) jniclasscode=%{
    static {
        String  library = System.getProperty("mapserver.library.name", "mapscript");

        System.loadLibrary(library);
        /* TODO Throw when return value not MS_SUCCESS? */
        edu.umn.gis.mapscript.mapscript.msSetup();
    }
%}

%typemap(jni) gdBuffer    %{jbyteArray%}
%typemap(jtype) gdBuffer  %{byte[]%}
%typemap(jstype) gdBuffer %{byte[]%}

%typemap(out) gdBuffer
%{ $result = SWIG_JavaArrayOutSchar(jenv, $1.data, $1.size); gdFree($1.data); %}

%typemap(javain) gdBuffer "$javainput"
%typemap(javaout) gdBuffer {
    return $jnicall;
}

/* String conversion utility function */

%{
/*
	These functions taken from: http://java.sun.com/docs/books/jni/html/other.html#26018
	Umberto Nicoletti, umberto.nicoletti@gmail.com

	Fix bug: http://mapserver.gis.umn.edu/bugs/show_bug.cgi?id=1753
*/	


void JNU_ThrowByName(JNIEnv *env, const char *name, const char *msg)
{
	jclass cls = (*env)->FindClass(env, name);
	/* if cls is NULL, an exception has already been thrown */
	if (cls != NULL) {
		(*env)->ThrowNew(env, cls, msg);
	}
	/* free the local ref */
	(*env)->DeleteLocalRef(env, cls);
}

char *JNU_GetStringNativeChars(JNIEnv *env, jstring jstr) {
	jbyteArray bytes = 0;
	jthrowable exc;
	char *result = 0;
	jclass jcls_str;
	jmethodID MID_String_getBytes;

	if (jstr == NULL) {
		return NULL;
	}

	if ((*env)->EnsureLocalCapacity(env, 2) < 0) {
		return 0; /* out of memory error */
	}

  	jcls_str = (*env)->FindClass(env, "java/lang/String"); 
    	MID_String_getBytes = (*env)->GetMethodID(env, jcls_str, "getBytes", "()[B"); 
     
	bytes = (*env)->CallObjectMethod(env, jstr,
                                      MID_String_getBytes);
	exc = (*env)->ExceptionOccurred(env);
	if (!exc) {
    		jint len = (*env)->GetArrayLength(env, bytes);
    		result = (char *)malloc(len + 1);
    		if (result == 0) {
        		JNU_ThrowByName(env, "java/lang/OutOfMemoryError",0);
        		(*env)->DeleteLocalRef(env, bytes);
        		return 0;
    		}
    		(*env)->GetByteArrayRegion(env, bytes, 0, len,
                               (jbyte *)result);
	    	result[len] = 0; /* NULL-terminate */
	} else {
    		(*env)->DeleteLocalRef(env, exc);
	}
	(*env)->DeleteLocalRef(env, bytes);
	return result;
}

jstring JNU_NewStringNative(JNIEnv *env, const char *str) {
	jstring result;
	jbyteArray bytes = 0;
	int len;
	jclass jcls_str;
	jmethodID MID_String_init;

	if (str == NULL) {
		return NULL;
	}

	if ((*env)->EnsureLocalCapacity(env, 2) < 0) {
	    return NULL; /* out of memory error */
	}
	jcls_str = (*env)->FindClass(env, "java/lang/String"); 
	MID_String_init = (*env)->GetMethodID(env, jcls_str, "<init>", "([B)V"); 

	len = strlen(str);
	bytes = (*env)->NewByteArray(env, len);
	if (bytes != NULL) {
	    (*env)->SetByteArrayRegion(env, bytes, 0, len,
	                            (jbyte *)str);
	    result = (*env)->NewObject(env, jcls_str,
	                            MID_String_init, bytes);
	    (*env)->DeleteLocalRef(env, bytes);
	    return result;
	} /* else fall through */
	return NULL;
}
 
%}

%typemap(in) char * {
	$1 = JNU_GetStringNativeChars(jenv, $input);
}

%typemap(out) char * {
	$result = JNU_NewStringNative(jenv, $1);
}

/* QuesoGLC
 * A free implementation of the OpenGL Character Renderer (GLC)
 * Copyright (c) 2002, Bertrand Coconnier
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/* $Id$ */
#ifndef __glc_ocontext_h
#define __glc_ocontext_h

#ifdef _REENTRANT
#include <pthread.h>
#endif
#include <gdbm.h>

#include "GL/glc.h"
#include "constant.h"
#include "ofont.h"
#include "omaster.h"
#include "ostrlst.h"

extern "C" {
  void __glcInitLibrary(void);
  void __glcExitLibrary(void);
}

class __glcContextState;

typedef struct {
  __glcContextState* currentContext;
  GLCenum errorState;
  GLint lockState;
} threadArea;

class __glcContextState {
  static GLboolean *isCurrent;
  static __glcContextState **stateList;
#ifdef _REENTRANT
  static pthread_mutex_t mutex;
  static pthread_key_t threadKey;
  static pthread_once_t initLibraryOnce;
#else
  static GLint initOnce;
  static threadArea* area;
#endif

  static __glcContextState* getState(GLint inContext);
  static void setState(GLint inContext, __glcContextState *inState);
  static GLboolean getCurrency(GLint inContext);
  static void setCurrency(GLint inContext, GLboolean isCurrent);
  static GLboolean isContext(GLint inContext);
  static void lock(void);
  static void unlock(void);
  static threadArea* getThreadArea(void);

  GLCchar *buffer;
  GLint bufferSize;

 public:
  static FT_Memory memoryManager;
  static GDBM_FILE unidb1, unidb2;

  GLuint displayDPIx;
  GLuint displayDPIy;

  FT_Library library;
  GLint id;			/* Context ID */
  GLboolean pendingDelete;	/* Is there a pending deletion ? */
  GLCfunc callback;		/* Callback function */
  GLvoid* dataPointer;	/* GLC_DATA_POINTER */
  GLboolean autoFont;		/* GLC_AUTO_FONT */
  GLboolean glObjects;	/* GLC_GLOBJECTS */
  GLboolean mipmap;		/* GLC_MIPMAP */
  __glcStringList* catalogList;/* GLC_CATALOG_LIST */
  GLfloat resolution;		/* GLC_RESOLUTION */
  GLfloat bitmapMatrix[4];	/* GLC_BITMAP_MATRIX */
  GLint currentFontList[GLC_MAX_CURRENT_FONT];
  GLint currentFontCount;	/* GLC_CURRENT_FONT_COUNT */
  __glcFont* fontList[GLC_MAX_FONT];
  GLint fontCount;		/* GLC_FONT_COUNT */
  GLint listObjectCount;	/* GLC_LIST_OBJECT_COUNT */
  __glcMaster* masterList[GLC_MAX_MASTER];
  GLint masterCount;		/* GLC_MASTER_COUNT */
  GLint measuredCharCount;	/* GLC_MEASURED_CHAR_COUNT */
  GLint renderStyle;		/* GLC_RENDER_STYLE */
  GLint replacementCode;	/* GLC_REPLACEMENT_CODE */
  GLint stringType;		/* GLC_STRING_TYPE */
  GLuint textureObjectList[GLC_MAX_TEXTURE_OBJECT];
  GLint textureObjectCount;	/* GLC_TEXTURE_OBJECT_COUNT */
  GLint versionMajor;		/* GLC_VERSION_MAJOR */
  GLint versionMinor;		/* GLC_VERSION_MINOR */
  GLfloat measurementCharBuffer[GLC_MAX_MEASURE][12];
  GLfloat measurementStringBuffer[12];
  GLboolean isInCallbackFunc;

  __glcContextState(GLint inContext);
  ~__glcContextState();
  void addMasters(const GLCchar* catalog, GLboolean append);
  void removeMasters(GLint inIndex);
  GLint getFont(GLint code);

  static __glcContextState* getCurrent(void);
  static void raiseError(GLCenum inError);
  GLCchar* queryBuffer(int inSize);

  friend void glcContext(GLint inContext);
  friend void glcDeleteContext(GLint inContext);
  friend GLint glcGenContext(void);
  friend GLint* glcGetAllContexts(void);
  friend GLint glcGetCurrentContext(void);
  friend GLCenum glcGetError(void);
  friend GLboolean glcIsContext(GLint inContext);

  friend void __glcInitLibrary(void);
  friend void __glcExitLibrary(void);
};

#endif /* __glc_ocontext_h */

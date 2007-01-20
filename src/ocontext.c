/* QuesoGLC
 * A free implementation of the OpenGL Character Renderer (GLC)
 * Copyright (c) 2002, 2004-2007, Bertrand Coconnier
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

/* Defines the methods of an object that is intended to managed contexts */

/* Microsoft Visual C++ */
#ifdef _MSC_VER
#define GLCAPI __declspec(dllexport)
#endif

#include "internal.h"

#include <sys/stat.h>
#ifndef __WIN32__
#include <unistd.h>
#else
#include <io.h>
#endif

#include "texture.h"
#include FT_MODULE_H
#include FT_LIST_H

commonArea __glcCommonArea;

/** \file
 * defines the object __glcContext which is used to manage the contexts.
 */


/* Constructor of the object : it allocates memory and initializes the member
 * of the new object.
 */
__glcContextState* __glcCtxCreate(GLint inContext)
{
  __glcContextState *This = NULL;

  This = (__glcContextState*)__glcMalloc(sizeof(__glcContextState));
  if (!This) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }
  memset(This, 0, sizeof(__glcContextState));

  if (FT_New_Library(&__glcCommonArea.memoryManager, &This->library)) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    __glcFree(This);
    return NULL;
  }

  FT_Add_Default_Modules(This->library);

#ifdef FT_CACHE_H
  if (FTC_Manager_New(This->library, 0, 0, 0, __glcFileOpen, NULL, &This->cache)) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FT_Done_Library(This->library);
    __glcFree(This);
    return NULL;
  }
#endif

  This->node.prev = NULL;
  This->node.next = NULL;
  This->node.data = NULL;

  This->catalogList = FcStrSetCreate();
  if (!This->catalogList) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
#ifdef FT_CACHE_H
    FTC_Manager_Done(This->cache);
#endif
    FT_Done_Library(This->library);
    __glcFree(This);
    return NULL;
  }

  This->masterHashTable = __glcArrayCreate(sizeof(FcChar32));
  if (!This->masterHashTable) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcStrSetDestroy(This->catalogList);
#ifdef FT_CACHE_H
    FTC_Manager_Done(This->cache);
#endif
    FT_Done_Library(This->library);
    __glcFree(This);
    return NULL;
  }
  __glcCreateHashTable(This);
  This->currentFontList.head = NULL;
  This->currentFontList.tail = NULL;
  This->fontList.head = NULL;
  This->fontList.tail = NULL;

  This->isCurrent = GL_FALSE;
  This->id = inContext;
  This->pendingDelete = GL_FALSE;
  This->stringState.callback = GLC_NONE;
  This->stringState.dataPointer = NULL;
  This->stringState.replacementCode = 0;
  This->stringState.stringType = GLC_UCS1;
  This->enableState.autoFont = GL_TRUE;
  This->enableState.glObjects = GL_TRUE;
  This->enableState.mipmap = GL_TRUE;
  This->enableState.hinting = GL_FALSE;
  This->enableState.extrude = GL_FALSE;
  This->enableState.kerning = GL_FALSE;
  This->renderState.resolution = 0.;
  This->renderState.renderStyle = GLC_BITMAP;
  This->bitmapMatrixStackDepth = 1;
  This->bitmapMatrix = This->bitmapMatrixStack;
  This->bitmapMatrix[0] = 1.;
  This->bitmapMatrix[1] = 0.;
  This->bitmapMatrix[2] = 0.;
  This->bitmapMatrix[3] = 1.;
  This->attribStackDepth = 0;
  This->measurementBuffer = __glcArrayCreate(12 * sizeof(GLfloat));
  if (!This->measurementBuffer) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    __glcArrayDestroy(This->masterHashTable);
    FcStrSetDestroy(This->catalogList);
#ifdef FT_CACHE_H
    FTC_Manager_Done(This->cache);
#endif
    FT_Done_Library(This->library);
    __glcFree(This);
    return NULL;
  }
  This->isInCallbackFunc = GL_FALSE;
  This->buffer = NULL;
  This->bufferSize = 0;
  This->lastFontID = 1;
  This->vertexArray = __glcArrayCreate(2 * sizeof(GLfloat));
  if (!This->vertexArray) {
    __glcArrayDestroy(This->measurementBuffer);
    __glcArrayDestroy(This->masterHashTable);
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcStrSetDestroy(This->catalogList);
#ifdef FT_CACHE_H
    FTC_Manager_Done(This->cache);
#endif
    FT_Done_Library(This->library);
    __glcFree(This);
    return NULL;
  }
  This->controlPoints = __glcArrayCreate(7 * sizeof(GLfloat));
  if (!This->vertexArray) {
    __glcArrayDestroy(This->vertexArray);
    __glcArrayDestroy(This->measurementBuffer);
    __glcArrayDestroy(This->masterHashTable);
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcStrSetDestroy(This->catalogList);
#ifdef FT_CACHE_H
    FTC_Manager_Done(This->cache);
#endif
    FT_Done_Library(This->library);
    __glcFree(This);
    return NULL;
  }
  This->endContour = __glcArrayCreate(sizeof(int));
  if (!This->endContour) {
    __glcArrayDestroy(This->controlPoints);
    __glcArrayDestroy(This->vertexArray);
    __glcArrayDestroy(This->measurementBuffer);
    __glcArrayDestroy(This->masterHashTable);
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcStrSetDestroy(This->catalogList);
#ifdef FT_CACHE_H
    FTC_Manager_Done(This->cache);
#endif
    FT_Done_Library(This->library);
    __glcFree(This);
    return NULL;
  }

  This->glCapacities = 0;
  This->texture.id = 0;
  This->texture.width = 0;
  This->texture.heigth = 0;

  This->atlas.id = 0;
  This->atlas.width = 0;
  This->atlas.heigth = 0;
  This->atlasList.head = NULL;
  This->atlasList.tail = NULL;
  This->atlasWidth = 0;
  This->atlasHeight = 0;
  This->atlasCount = 0;

  return This;
}



/* This function is called from FT_List_Finalize() to destroy all
 * remaining fonts
 */
static void __glcFontDestructor(FT_Memory inMemory, void* inData, void* inUser)
{
  __glcFont *font = (__glcFont*)inData;
  /* This is wrong to get the context state there : it may be the source of
   * possible thread bugs (if the context is changed during execution of the
   * function). 
   */
  __glcContextState* state = __glcGetCurrent();

  assert(state);

  if (font)
    __glcFontDestroy(font, state);
}



/* This function is called from FT_List_Finalize() to destroy all
 * atlas elements and update the glyphs accordingly
 */
static void __glcAtlasDestructor(FT_Memory inMemory, void* inData, void* inUser)
{
  __glcAtlasElement* element = (__glcAtlasElement*)inData;
  __glcGlyph* glyph = NULL;

  assert(element);

  glyph = element->glyph;
  if (glyph)
    __glcGlyphDestroyTexture(glyph);
}



/* Destructor of the object : it first destroys all the objects (whether they
 * are internal GLC objects or GL textures or GL display lists) that have been
 * created during the life of the context. Then it releases the memory occupied
 * by the GLC state struct.
 */
void __glcCtxDestroy(__glcContextState *This)
{
  assert(This);

  /* Destroy the list of catalogs */
  FcStrSetDestroy(This->catalogList);

  /* Destroy GLC_CURRENT_FONT_LIST */
  FT_List_Finalize(&This->currentFontList, NULL,
		   &__glcCommonArea.memoryManager, NULL);

  /* Destroy GLC_FONT_LIST */
  FT_List_Finalize(&This->fontList, __glcFontDestructor,
                   &__glcCommonArea.memoryManager, NULL);

  if (This->masterHashTable)
    __glcArrayDestroy(This->masterHashTable);

  if (This->texture.id)
    glDeleteTextures(1, &This->texture.id);

  if (This->atlas.id)
    glDeleteTextures(1, &This->atlas.id);

  FT_List_Finalize(&This->atlasList, __glcAtlasDestructor,
		   &__glcCommonArea.memoryManager, NULL);

  if (This->bufferSize)
    __glcFree(This->buffer);

  if (This->measurementBuffer)
    __glcArrayDestroy(This->measurementBuffer);

  if (This->vertexArray)
    __glcArrayDestroy(This->vertexArray);

  if (This->controlPoints)
    __glcArrayDestroy(This->controlPoints);

  if (This->endContour)
    __glcArrayDestroy(This->endContour);

#ifdef FT_CACHE_H
  FTC_Manager_Done(This->cache);
#endif
  FT_Done_Library(This->library);
  __glcFree(This);
}



/* Return the ID of the first font in GLC_CURRENT_FONT_LIST that maps 'inCode'
 * If there is no such font, the function returns zero.
 * 'inCode' must be given in UCS-4 format.
 */
static GLint __glcLookupFont(GLint inCode, FT_List fontList)
{
  FT_ListNode node = NULL;

  for (node = fontList->head; node; node = node->next) {
    __glcFont* font = (__glcFont*)node->data;

    /* Check if the character identified by inCode exists in the font */
    if (__glcCharMapHasChar(font->charMap, inCode))
      return font->id;
  }
  return -1;
}



/* Calls the callback function (does various tests to determine if it is
 * possible) and returns GL_TRUE if it has succeeded or GL_FALSE otherwise.
 * 'inCode' must be given in UCS-4 format.
 */
static GLboolean __glcCallCallbackFunc(GLint inCode,
				       __glcContextState *inState)
{
  GLCfunc callbackFunc = NULL;
  GLboolean result = GL_FALSE;
  GLint aCode = 0;

  /* Recursivity is not allowed */
  if (inState->isInCallbackFunc)
    return GL_FALSE;

  callbackFunc = inState->stringState.callback;
  if (!callbackFunc)
    return GL_FALSE;

  /* Convert the character code back to the current string type */
  aCode = __glcConvertUcs4ToGLint(inState, inCode);
  /* Check if the character has been converted */
  if (aCode < 0)
    return GL_FALSE;

  inState->isInCallbackFunc = GL_TRUE;
  /* Call the callback function with the character converted to the current
   * string type.
   */
  result = (*callbackFunc)(aCode);
  inState->isInCallbackFunc = GL_FALSE;

  return result;
}



/* Returns the ID of the first font in GLC_CURRENT_FONT_LIST that maps
 * 'inCode'. If there is no such font and GLC_AUTO_FONT is enabled, the
 * function attempts to append a new font from GLC_FONT_LIST (or from a master)
 * to GLC_CURRENT_FONT_LIST. If the attempt fails the function returns zero.
 * 'inCode' must be given in UCS-4 format.
 */
GLint __glcCtxGetFont(__glcContextState *This, GLint inCode)
{
  GLint font = 0;

  /* Look for a font in the current font list */
  font = __glcLookupFont(inCode, &This->currentFontList);
  /* If a font has been found return */
  if (font >= 0)
    return font;

  /* If a callback function is defined for GLC_OP_glcUnmappedCode then call it.
   * The callback function should return GL_TRUE if it succeeds in appending to
   * GLC_CURRENT_FONT_LIST the ID of a font that maps 'inCode'.
   */
  if (__glcCallCallbackFunc(inCode, This)) {
    font = __glcLookupFont(inCode, &This->currentFontList);
    if (font >= 0)
      return font;
  }

  /* If the value of the boolean variable GLC_AUTOFONT is GL_TRUE then search
   * GLC_FONT_LIST for the first font that maps 'inCode'. If the search
   * succeeds, then append the font's ID to GLC_CURRENT_FONT_LIST.
   */
  if (This->enableState.autoFont) {
    FcPattern* pattern = NULL;
    FcCharSet* charSet = NULL;
    FcFontSet* fontSet = NULL;
    FcResult result;
    int m = 0, f = 0;
    FcChar8* family = NULL;
    FcChar8* foundry = NULL;
    int fixed = 0;
    FcChar32 hashValue = 0;
    FcChar32* hashTable = (FcChar32*)GLC_ARRAY_DATA(This->masterHashTable);
    FcChar8* style = NULL;

    font = __glcLookupFont(inCode, &This->fontList);
    if (font >= 0) {
      glcAppendFont(font);
      return font;
    }

    /* Otherwise, the function searches the GLC master list for the first
     * master that maps 'inCode'. If the search succeeds, it creates a font
     * from the master and appends its ID to GLC_CURRENT_FONT_LIST.
     */
    charSet = FcCharSetCreate();
    if (!charSet) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      return -1;
    }
    if (!FcCharSetAddChar(charSet, inCode)) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcCharSetDestroy(charSet);
      return -1;
    }
    pattern = FcPatternCreate();
    if (!pattern) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcCharSetDestroy(charSet);
      return -1;
    }
    if (!FcPatternAddCharSet(pattern, FC_CHARSET, charSet)) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcCharSetDestroy(charSet);
      FcPatternDestroy(pattern);
      return -1;
    }
    if (!FcPatternAddBool(pattern, FC_OUTLINE, FcTrue)) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcCharSetDestroy(charSet);
      FcPatternDestroy(pattern);
      return -1;
    }

    if (!FcConfigSubstitute(NULL, pattern, FcMatchPattern)) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcPatternDestroy(pattern);
      FcCharSetDestroy(charSet);
      return -1;
    }
    FcDefaultSubstitute(pattern);
    fontSet = FcFontSort(NULL, pattern, FcFalse, NULL, &result);
    FcPatternDestroy(pattern);
    FcCharSetDestroy(charSet);
    if ((!fontSet) || (result == FcResultTypeMismatch)) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      return -1;
    }

    for (f = 0; f < fontSet->nfont; f++) {
      FcBool outline = FcFalse;

      result = FcPatternGetBool(fontSet->fonts[f], FC_OUTLINE, 0, &outline);
      assert(result != FcResultTypeMismatch);
      result = FcPatternGetCharSet(fontSet->fonts[f], FC_CHARSET, 0, &charSet);
      assert(result != FcResultTypeMismatch);

      if (outline && FcCharSetHasChar(charSet, inCode))
	break;
    }

    if (f == fontSet->nfont) {
      FcFontSetDestroy(fontSet);
      return -1;
    }

    result = FcPatternGetString(fontSet->fonts[f], FC_FAMILY, 0, &family);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetString(fontSet->fonts[f], FC_FOUNDRY, 0, &foundry);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetInteger(fontSet->fonts[f], FC_SPACING, 0, &fixed);
    assert(result != FcResultTypeMismatch);

    pattern = FcPatternCreate();
    if (!pattern) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcFontSetDestroy(fontSet);
      return -1;
    }
    if (!FcPatternAddString(pattern, FC_FAMILY, family)) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcPatternDestroy(pattern);
      FcFontSetDestroy(fontSet);
      return -1;
    }
    if (!FcPatternAddString(pattern, FC_FOUNDRY, foundry)) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcPatternDestroy(pattern);
      FcFontSetDestroy(fontSet);
      return -1;
    }
    if (!FcPatternAddInteger(pattern, FC_SPACING, fixed)) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcPatternDestroy(pattern);
      FcFontSetDestroy(fontSet);
      return -1;
    }
    if (!FcPatternAddBool(pattern, FC_OUTLINE, FcTrue)) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcPatternDestroy(pattern);
      FcFontSetDestroy(fontSet);
      return -1;
    }

    hashValue = FcPatternHash(pattern);
    FcPatternDestroy(pattern);
    for (m = 0; m < GLC_ARRAY_LENGTH(This->masterHashTable); m++) {
      if (hashValue == hashTable[m])
	break;
    }
    assert(m != GLC_ARRAY_LENGTH(This->masterHashTable));

    font = glcNewFontFromMaster(glcGenFontID(), m);
    if (font) {
      result = FcPatternGetString(fontSet->fonts[f], FC_STYLE, 0, &style);
      assert(result != FcResultTypeMismatch);
      FcFontSetDestroy(fontSet);
      glcFontFace(font, style);
      /* Add the font to the GLC_CURRENT_FONT_LIST */
      glcAppendFont(font);
      return font;
    }
    FcFontSetDestroy(fontSet);
  }
  return -1;
}



/* Sometimes informations may need to be stored temporarily by a thread.
 * The so-called 'buffer' is created for that purpose. Notice that it is a
 * component of the GLC state struct hence its lifetime is the same as the
 * GLC state's lifetime.
 * __glcCtxQueryBuffer() should be called whenever the buffer is to be used
 * in order to check if it is big enough to store infos.
 * Note that the only memory management function used below is 'realloc' which
 * means that the buffer goes bigger and bigger until it is freed. No function
 * is provided to reduce its size so it should be freed and re-allocated
 * manually in case of emergency ;-)
 */
GLCchar* __glcCtxQueryBuffer(__glcContextState *This, int inSize)
{
  if (inSize > This->bufferSize) {
    This->buffer = (GLCchar*)__glcRealloc(This->buffer, inSize);
    if (!This->buffer)
      __glcRaiseError(GLC_RESOURCE_ERROR);
    else
      This->bufferSize = inSize;
  }

  return This->buffer;
}

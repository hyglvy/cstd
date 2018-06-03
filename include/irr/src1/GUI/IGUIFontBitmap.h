// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#ifndef __I_GUI_FONT_BITMAP_H_INCLUDED__
#define __I_GUI_FONT_BITMAP_H_INCLUDED__
//#include "IGUIFont.h"
class IGUISpriteBank;
//! Font interface.
class IGUIFontBitmap : public IGUIFont
{
public:
  //! Returns the type of this font
  EGUI_FONT_TYPE getType() const {
    return EGFT_BITMAP;
  }
  //! returns the parsed Symbol Information
  IGUISpriteBank* getSpriteBank() = 0;
  //! returns the sprite number from a given character
  u32 getSpriteNoFromChar(const char* c) = 0;
  //! Gets kerning values (distance between letters) for the font. If no parameters are provided,
  /** the global kerning distance is returned.
  \param thisLetter: If this parameter is provided, the left side kerning for this letter is added
  to the global kerning value. For example, a space might only be one pixel wide, but it may
  be displayed as several pixels.
  \param previousLetter: If provided, kerning is calculated for both letters and added to the global
  kerning value. For example, EGFT_BITMAP will add the right kerning value of previousLetter to the
  left side kerning value of thisLetter, then add the global value.
  */
  s32 getKerningWidth(const char* thisLetter = 0, const char* previousLetter = 0) = 0;
};
#endif

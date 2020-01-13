////////////////////////////////////////////////////////////////////////////////
//
// File: UiComponentSize.cpp
//
// Author: Raynard Brown
//
// Copyright (c) 2020 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ui/layout/UiComponentSize.h"

UiComponentSize::UiComponentSize()
{
}

UiComponentSize::~UiComponentSize()
{
}

/*static*/ int UiComponentSize::getButtonWidth()
{
  return 75;
}

/*static*/ int UiComponentSize::getButtonHeight()
{
  return 23;
}

/*static*/ int UiComponentSize::getLabelHeight()
{
  /*
   * According to the UXGuide.pdf, Text (static) controls, in my case Labels,
   * are recommended to have a height of 13 pixels (Given a Segoe UI 9 pt font).
   * However when testing I have discovered clipping in the descent. I also
   * discovered that this particular recommendation works with the font,
   * MS Shell Dlg 2 8pt font.
   *
   * With further testing, I have discovered that the correct Label height for
   * the Segoe UI 9pt font is 15. Also the tmHeight field of the TEXTMETRIC
   * structure confirms this when the font is selected into the device context.
   */
  return 15;
}

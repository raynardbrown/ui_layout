////////////////////////////////////////////////////////////////////////////////
//
// File: RowLayout.h
//
// Author: Raynard Brown
//
// Copyright (c) 2019 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef UI_LAYOUT_ROWLAYOUT_H_
#define UI_LAYOUT_ROWLAYOUT_H_

#include "ui/layout/ILayout.h"

class UiComponent;

class RowLayout : public ILayout
{
  public:

    RowLayout();

    virtual ~RowLayout();

    virtual void executeLayout(UiComponent * uiComponent);
};
#endif /* UI_LAYOUT_ROWLAYOUT_H_ */

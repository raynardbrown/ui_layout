////////////////////////////////////////////////////////////////////////////////
//
// File: ILayout.h
//
// Author: Raynard Brown
//
// Copyright (c) 2019 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef UI_LAYOUT_ILAYOUT_H_
#define UI_LAYOUT_ILAYOUT_H_

class UiComponent;

class ILayout
{
  protected:

    ILayout()
    {

    }

  public:

    virtual ~ILayout()
    {

    }

    virtual void executeLayout(UiComponent * uiComponent) = 0;
};
#endif /* UI_LAYOUT_ILAYOUT_H_ */

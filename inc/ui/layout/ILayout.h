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

#ifndef INC_ILAYOUT_H_
#define INC_ILAYOUT_H_

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

    virtual void executeLayout() = 0;
};

#endif /* INC_ILAYOUT_H_ */

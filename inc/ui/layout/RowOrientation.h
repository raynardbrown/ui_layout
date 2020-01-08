////////////////////////////////////////////////////////////////////////////////
//
// File: RowOrientation.h
//
// Author: Raynard Brown
//
// Copyright (c) 2020 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef UI_LAYOUT_ROWORIENTATION_H_
#define UI_LAYOUT_ROWORIENTATION_H_

/**
 * The alignment of all controls within a specified row.
 */
struct RowOrientation
{
  public:

    enum Value
    {
      /**
       * Left-align all controls with a specified row.
       */
      LeftRowOrientation,

      /**
       * Center-align all controls with a specified row.
       */
      CenterRowOrientation,

      /**
       * Right-align all controls with a specified row.
       */
      RightRowOrientation,
    };
};
#endif /* UI_LAYOUT_ROWORIENTATION_H_ */

////////////////////////////////////////////////////////////////////////////////
//
// File: UiComponentConstraints.cpp
//
// Author: Raynard Brown
//
// Copyright (c) 2020 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ui/layout/UiComponentConstraints.h"

void initUiComponentConstraints(UiComponentConstraints * uiComponentConstraints)
{
  uiComponentConstraints->sizeGroupId = -1;
  uiComponentConstraints->labelSizeGroupId = -1;
  uiComponentConstraints->horizontalGapLeft = -1;
  uiComponentConstraints->horizontalGapRight = -1;
  uiComponentConstraints->labelOrientation = LabelOrientation::LeftOrientation;
  uiComponentConstraints->labelVerticalAlignment = LabelVerticalAlignment::TopAlignment;

  uiComponentConstraints->growX = 0;
  uiComponentConstraints->growY = 0;
}

void copyUiComponentConstraints(UiComponentConstraints * destination,
                                const UiComponentConstraints * source)
{
  destination->sizeGroupId = source->sizeGroupId;
  destination->labelSizeGroupId = source->labelSizeGroupId;
  destination->horizontalGapLeft = source->horizontalGapLeft;
  destination->horizontalGapRight = source->horizontalGapRight;
  destination->labelOrientation = source->labelOrientation;
  destination->labelVerticalAlignment = source->labelVerticalAlignment;

  destination->growX = source->growX;
  destination->growY = source->growY;
}

////////////////////////////////////////////////////////////////////////////////
//
// File: UiComponentInternals.h
//
// Author: Raynard Brown
//
// Copyright (c) 2020 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef UI_LAYOUT_PRIVATE_UICOMPONENTINTERNALS_H_
#define UI_LAYOUT_PRIVATE_UICOMPONENTINTERNALS_H_

struct UiComponentInternals
{
    int labelPreferredHeight;
    int labelPreferredWidth;

    int uiComponentPreferredWidth;
    int uiComponentPreferredHeight;

    // used as a cache to avoid flickering
    int labelX;
    // used as a cache to avoid flickering
    int labelY;

    // used as a cache to avoid flickering
    int uiComponentX;
    // used as a cache to avoid flickering
    int uiComponentY;

    int horizontalGapLeft;
    int horizontalGapRight;
};
#endif /* UI_LAYOUT_PRIVATE_UICOMPONENTINTERNALS_H_ */

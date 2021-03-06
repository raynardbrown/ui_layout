////////////////////////////////////////////////////////////////////////////////
//
// File: UiComponentConstraints.h
//
// Author: Raynard Brown
//
// Copyright (c) 2020 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef UI_LAYOUT_UICOMPONENTCONSTRAINTS_H_
#define UI_LAYOUT_UICOMPONENTCONSTRAINTS_H_

#include "ui/layout/LabelOrientation.h"
#include "ui/layout/LabelVerticalAlignment.h"

struct UiComponentConstraints
{
    /**
     * All UiComponents with the specified group id have the same bounds.
     *
     * This field is ignored if it is less than 0.
     */
    int sizeGroupId;

    /**
     * All UiComponents with the specified group id have the same bounds.
     *
     * This field is ignored if it is less than 0.
     */
    int labelSizeGroupId;

    /**
     * The horizontal space to the left of and in between a UiComponent and
     * an adjacent UiComponent.
     *
     * If a default value of -1 is specified, then the layout determines the
     * gap between the two UiComponents based on whether or not the UiComponents
     * in question are related or unrelated.
     */
    int horizontalGapLeft;
    int horizontalGapRight;

    /**
     * The location where a label will reside relative to its associated
     * UiComponent.
     *
     * The default value is LabelOrientation::LeftOrientation.
     */
    LabelOrientation::Value labelOrientation;

    /**
     * The vertical location where a label will reside relative to its
     * associated UiComponent. This value is only valid when the UiComponent
     * associated with the label has a UiComponentConstraints::labelOrientation
     * of LabelOrientation::LeftOrientation or
     * LabelOrientation::RightOrientation.
     *
     *
     * The default value is LabelVerticalAlignment::TopAlignment.
     */
    LabelVerticalAlignment::Value labelVerticalAlignment;

    /**
     * Controls how much a UiComponent grows. The default 0 means a
     * UiComponent will not grow.
     */
    int growX;
    int growY;
};

void initUiComponentConstraints(UiComponentConstraints * uiComponentConstraints);

void copyUiComponentConstraints(UiComponentConstraints * destination,
                                const UiComponentConstraints * source);

#endif /* UI_LAYOUT_UICOMPONENTCONSTRAINTS_H_ */

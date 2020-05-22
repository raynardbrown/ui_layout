////////////////////////////////////////////////////////////////////////////////
//
// File: RowLayoutPrivate.h
//
// Author: Raynard Brown
//
// Copyright (c) 2020 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef UI_LAYOUT_PRIVATE_ROWLAYOUTPRIVATE_H_
#define UI_LAYOUT_PRIVATE_ROWLAYOUTPRIVATE_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

class Size2D;
class UiComponent;
struct UiComponentWrapper;

// Which UiComponent in the UiComponent wrapper is part of the size group
enum SizeGroupIdentifier
{
  LabelGroup,
  UiComponentGroup
};

#include "ui/layout/RowLayout.h"

class RowLayoutPrivate
{
  public:

    RowLayoutPrivate(UiComponent * rowLayoutContainer);

    ~RowLayoutPrivate();

    std::auto_ptr<Size2D> getMaxUiComponentSize(const std::vector<std::pair<SizeGroupIdentifier, UiComponentWrapper *> >& uiComponentWrappers);

    void resizeUiComponentsInSizeGroup(const std::vector<std::pair<SizeGroupIdentifier, UiComponentWrapper *> >& uiComponentWrappers,
                                       std::auto_ptr<Size2D> maxSize);

    void resizeUiComponentsInSizeGroups();

    void resizePreviousAndNextLabelRows(RowLayout * thisRowLayout);

    void preLayout(RowLayout * thisRowLayout);

    int getUiComponentGap(std::size_t rowIndex,
                          std::size_t previousUiComponentIndex,
                          std::size_t currentUiComponentIndex);

    void applyCenterRowAlignment(RowLayout * thisRowLayout,
                                 std::size_t rowIndex,
                                 int containerWidth,
                                 int containerLeftPadding,
                                 int containerRightPadding);

    void applyRightRowAlignment(RowLayout * thisRowLayout,
                                std::size_t rowIndex,
                                int containerWidth,
                                int containerLeftPadding,
                                int containerRightPadding);

    void handleVerticalAlignment(UiComponentWrapper * uiComponentWrapper,
                                 int * yLabelVAlignmentOffset,
                                 int * yUiComponentPeerVAlignmentOffset);

    void layoutAllUiComponentsInRow(RowLayout * thisRowLayout,
                                    std::size_t rowIndex,
                                    int * xOffset,
                                    int * yOffset,
                                    int * heightOfTallestUiComponentInRow,
                                    int initialXOffset,
                                    int * initialYOffset,
                                    int * containerHeight,
                                    int * maxRowWidth,
                                    bool isFirstRow);

    void layoutUiComponentInRow(RowLayout * thisRowLayout,
                                std::size_t rowIndex,
                                std::size_t uiComponentIndex,
                                int * xOffset,
                                int * yOffset,
                                int * heightOfTallestUiComponentInRow,
                                bool insertGaps);

    void moveUiComponentsInRowRight(std::size_t rowIndex,
                                    std::size_t uiComponentIndex,
                                    int remainingWidth);

    void moveUiComponentsBelowDown(std::size_t rowIndex,
                                   int remainingHeight);

    void applyAllSizes();

    void applyAllLocations();

    void setDrawingEnabled(bool flag);

    void enableAllDrawing();

    void disableAllDrawing();

    void postLayout(RowLayout * thisRowLayout, UiComponent * container);

    UiComponent * rowLayoutContainer;

    RowLayout::Row * currentRow;

    std::vector<RowLayout::Row *> rows;

    // Key   - The size group id - Tip: Clients of RowLayout should use an
    //                               enumeration to specify groups.
    //
    // Value - Collection of UiComponents that will have the same dimensions
    //         The first value of the pair specifies the UiComponent from the
    //         wrapper that is part of the size group
    std::map<int, std::vector<std::pair<SizeGroupIdentifier, UiComponentWrapper *> > > sizeGroups;

    int verticalSpaceBetweenRows;
};


#endif /* UI_LAYOUT_PRIVATE_ROWLAYOUTPRIVATE_H_ */

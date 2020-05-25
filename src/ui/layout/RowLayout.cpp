////////////////////////////////////////////////////////////////////////////////
//
// File: RowLayout.cpp
//
// Author: Raynard Brown
//
// Copyright (c) 2019 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>

#include <vector>
#include <map>
#include <memory>

#include "graphics/Size2D.h"

#include "ui/UiComponent.h"
#include "ui/Label.h"

#include "ui/layout/UiComponentConstraints.h"

#include "ui/layout/private/UiComponentWrapper.h"
#include "ui/layout/private/UiComponentInternals.h"
#include "ui/layout/private/RowLayoutPrivate.h"

#include "ui/layout/RowLayout.h"

static int getVerticalSpaceBetweenRowsToPlatformIfNeeded(int verticalSpace)
{
  if(verticalSpace < 0)
  {
    return 11; // TODO: Do not hard code this value
  }

  return verticalSpace;
}

/**
 * Struct used to hold labels associated with UiComponents in which the
 * labels are not laid out within the row of the associated UiComponent.
 *
 * For example:
 *
 *                +---------------------+
 *   Row 1        |                     |
 *  +---------->  |       Label         |
 *                |                     |
 *                +---------------------+
 *
 *                +---------------------+
 *   Row 2        |                     |
 *  +---------->  |     UiComponent     |
 *                |                     |
 *                +---------------------+
 *
 */
struct LabelRow
{
    std::size_t getLabelCount() const
    {
      return labels.size();
    }

    Label * getLabelAt(std::size_t index)
    {
      return labels.at(index)->labelPeer;
    }

    UiComponentWrapper * getWrapperForLabelAt(std::size_t index)
    {
      return labels.at(index);
    }

    void addLabel(UiComponentWrapper * labelPair)
    {
      labels.push_back(labelPair);
    }

    void removeAllLabels()
    {
      for(;!labels.empty();)
      {
        // Don't delete the UiComponent wrapper since you don't own the memory.
        labels.pop_back();
      }
    }

    std::vector<UiComponentWrapper *> labels;

    /**
     * The height of the tallest label in pixels that is in this row.
     */
    int maxHeightLabelInRow;
};

class RowLayout::Row::RowPrivate
{
  public:

    RowPrivate(RowLayout * rowLayout)
    :horizontalSpaceBetweenUiComponents(0),
     previousLabelRow(nullptr),
     nextLabelRow(nullptr),
     rowLayout(rowLayout),
     rowOrientation(RowOrientation::LeftRowOrientation),
     rowWidth(0),
     rowHeight(0)
    {

    }

    ~RowPrivate()
    {

    }

    UiComponentWrapper * getUiComponentWrapperAt(std::size_t index)
    {
      return uiComponents.at(index);
    }

    UiComponentConstraints * getUiComponentConstraintsAt(std::size_t index)
    {
      return uiComponents.at(index)->uiComponentConstraints;
    }

    void initUiComponentInternals(UiComponentWrapper * uiComponentWrapper)
    {
      uiComponentWrapper->uiComponentInternals = new UiComponentInternals;

      uiComponentWrapper->uiComponentInternals->labelPreferredWidth = 0;
      uiComponentWrapper->uiComponentInternals->labelPreferredHeight = 0;

      uiComponentWrapper->uiComponentInternals->uiComponentPreferredWidth = 0;
      uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight = 0;

      uiComponentWrapper->uiComponentInternals->labelX = 0;
      uiComponentWrapper->uiComponentInternals->labelY = 0;

      uiComponentWrapper->uiComponentInternals->uiComponentX = 0;
      uiComponentWrapper->uiComponentInternals->uiComponentY = 0;

      uiComponentWrapper->uiComponentInternals->horizontalGapLeft = -1;
      uiComponentWrapper->uiComponentInternals->horizontalGapRight = -1;
    }

    void freeUiComponentWrapper(UiComponentWrapper * uiComponentWrapper)
    {
      delete uiComponentWrapper->uiComponentConstraints;
      uiComponentWrapper->uiComponentConstraints = nullptr;

      delete uiComponentWrapper->uiComponentInternals;
      uiComponentWrapper->uiComponentInternals = nullptr;

      delete uiComponentWrapper;
      uiComponentWrapper = nullptr;
    }

    std::vector<UiComponentWrapper *> uiComponents;

    /**
     * The amount of horizontal space between uiComponents in this row,
     * excluding uiComponents and their associated labels. The space between
     * uiComponents and their associated labels is usually different.
     *
     * By default the space between uiComponents is zero unless the client
     * specifies otherwise.
     */
    std::size_t horizontalSpaceBetweenUiComponents;

    /**
     * The row of labels above this row or nullptr if there isn't a row of
     * labels above this row.
     */
    LabelRow * previousLabelRow;

    /**
     * The row of labels below this row or nullptr if there isn't a row of
     * labels below this row.
     */
    LabelRow * nextLabelRow;

    RowLayout * rowLayout;

    RowOrientation::Value rowOrientation;

    int rowWidth;

    int rowHeight;
};

RowLayout::Row::Row(RowLayout * rowLayout)
:d(new RowLayout::Row::RowPrivate(rowLayout))
{

}

RowLayout::Row::~Row()
{

}

RowOrientation::Value RowLayout::Row::getRowOrientation() const
{
  return d->rowOrientation;
}

// Note: Although UiComponents and their associated labels are individual
//       UiComponents, for the sake of this structure, they are treated
//       as "one" UiComponent and share an index.
std::size_t RowLayout::Row::getUiComponentCount() const
{
  return d->uiComponents.size();
}

UiComponent * RowLayout::Row::getUiComponentAt(std::size_t index)
{
  return d->uiComponents.at(index)->uiComponent;
}

Label * RowLayout::Row::getLabelAt(std::size_t index)
{
  return d->uiComponents.at(index)->labelPeer;
}

RowLayout::Row * RowLayout::Row::addUiComponent(UiComponent * uiComponent,
                                                const UiComponentConstraints * uiComponentConstraints)
{
  UiComponentWrapper * uiComponentWrapper = new UiComponentWrapper;
  uiComponentWrapper->uiComponent = uiComponent;
  uiComponentWrapper->labelPeer = nullptr;
  uiComponentWrapper->uiComponentConstraints = new UiComponentConstraints;
  uiComponentWrapper->uiComponentInternals = nullptr;

  ::initUiComponentConstraints(uiComponentWrapper->uiComponentConstraints);
  d->initUiComponentInternals(uiComponentWrapper);

  ::copyUiComponentConstraints(uiComponentWrapper->uiComponentConstraints,
                               uiComponentConstraints);

  d->uiComponents.push_back(uiComponentWrapper);

  d->rowLayout->d->rowLayoutContainer->addUiComponent(uiComponent);

  // Get the index of the uiComponent and add it to the size group map if and
  // only if a size group is specified.
  const int sizeGroupId = uiComponentConstraints->sizeGroupId;

  if(sizeGroupId >= 0)
  {
    d->rowLayout->d->sizeGroups[sizeGroupId].push_back(std::pair<SizeGroupIdentifier, UiComponentWrapper *>(UiComponentGroup, uiComponentWrapper));
  }

  return this;
}

RowLayout::Row * RowLayout::Row::addUiComponent(Label * label,
                                                UiComponent* uiComponent,
                                                const UiComponentConstraints * uiComponentConstraints)
{
  UiComponentWrapper * uiComponentWrapper = new UiComponentWrapper;
  uiComponentWrapper->uiComponent = uiComponent;
  uiComponentWrapper->labelPeer = label;
  uiComponentWrapper->uiComponentConstraints = new UiComponentConstraints;
  uiComponentWrapper->uiComponentInternals = nullptr;

  ::initUiComponentConstraints(uiComponentWrapper->uiComponentConstraints);
  d->initUiComponentInternals(uiComponentWrapper);

  ::copyUiComponentConstraints(uiComponentWrapper->uiComponentConstraints,
                               uiComponentConstraints);

  d->uiComponents.push_back(uiComponentWrapper);

  if(uiComponentWrapper->uiComponentConstraints->labelOrientation == LabelOrientation::TopOrientation)
  {
    if(d->previousLabelRow == nullptr)
    {
      d->previousLabelRow = new LabelRow;
      d->previousLabelRow->maxHeightLabelInRow = 0;
    }

    d->previousLabelRow->addLabel(uiComponentWrapper);
  }
  else if(uiComponentWrapper->uiComponentConstraints->labelOrientation == LabelOrientation::BottomOrientation)
  {
    if(d->nextLabelRow == nullptr)
    {
      d->nextLabelRow = new LabelRow;
      d->nextLabelRow->maxHeightLabelInRow = 0;
    }

    d->nextLabelRow->addLabel(uiComponentWrapper);
  }

  d->rowLayout->d->rowLayoutContainer->addUiComponent(label);
  d->rowLayout->d->rowLayoutContainer->addUiComponent(uiComponent);

  // Get the index of the uiComponent and add it to the size group map if and
  // only if a size group is specified.

  const int sizeGroupId = uiComponentConstraints->sizeGroupId;

  const int labelSizeGroupId = uiComponentConstraints->labelSizeGroupId;

  if(sizeGroupId >= 0)
  {
    d->rowLayout->d->sizeGroups[sizeGroupId].push_back(std::pair<SizeGroupIdentifier,
                                                       UiComponentWrapper *>(UiComponentGroup, uiComponentWrapper));
  }

  if(labelSizeGroupId >= 0)
  {
    d->rowLayout->d->sizeGroups[labelSizeGroupId].push_back(std::pair<SizeGroupIdentifier,
                                                            UiComponentWrapper *>(LabelGroup, uiComponentWrapper));
  }

  return this;
}

void RowLayout::Row::setHorizontalSpaceBetweenUiComponents(std::size_t horizontalSpace)
{
  // Now that the row exists, set the horizontal space
  d->horizontalSpaceBetweenUiComponents = horizontalSpace;
}

void RowLayout::Row::setRowOrientation(RowOrientation::Value rowOrientation)
{
  d->rowOrientation = rowOrientation;
}

void RowLayout::Row::removeAllUiComponents()
{
  for(;!d->uiComponents.empty();)
  {
    // Don't delete the UiComponent since you don't own the memory, however
    // you must delete the UiComponent wrapper and the
    // UiComponentConstraints

    UiComponentWrapper * uiComponentWrapper = d->uiComponents.back();

    d->freeUiComponentWrapper(uiComponentWrapper);

    d->uiComponents.pop_back();
  }
}

void RowLayout::Row::removeUiComponentAt(std::size_t index)
{
  UiComponentWrapper * uiComponentWrapper = d->uiComponents.at(index);

  d->uiComponents.erase(d->uiComponents.begin() + index);

  d->freeUiComponentWrapper(uiComponentWrapper);
}

RowLayoutPrivate::RowLayoutPrivate(UiComponent * rowLayoutContainer)
:rowLayoutContainer(rowLayoutContainer),
 currentRow(nullptr),
 verticalSpaceBetweenRows(-1)
{

}

RowLayoutPrivate::~RowLayoutPrivate()
{
  for(;!rows.empty();)
  {
    RowLayout::Row * row = rows.back();

    row->removeAllUiComponents();

    delete row->d->previousLabelRow;
    row->d->previousLabelRow = nullptr;

    delete row->d->nextLabelRow;
    row->d->nextLabelRow = nullptr;

    delete row;
    row = nullptr;

    rows.pop_back();
  }
}

std::auto_ptr<Size2D> RowLayoutPrivate::getMaxUiComponentSize(const std::vector<std::pair<SizeGroupIdentifier, UiComponentWrapper *> >& uiComponentWrappers)
{
  std::auto_ptr<Size2D> maxSize(new Size2D(0, 0));

  for(std::size_t i = 0; i < uiComponentWrappers.size(); ++i)
  {
    UiComponentWrapper * uiComponentWrapper = uiComponentWrappers.at(i).second;

    if(uiComponentWrappers.at(i).first == LabelGroup)
    {
      maxSize->setSize(std::max(uiComponentWrapper->uiComponentInternals->labelPreferredWidth,
                                maxSize->getWidth()),
                       std::max(uiComponentWrapper->uiComponentInternals->labelPreferredHeight,
                                maxSize->getHeight()));
    }
    else
    {
      maxSize->setSize(std::max(uiComponentWrapper->uiComponentInternals->uiComponentPreferredWidth,
                                maxSize->getWidth()),
                       std::max(uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight,
                                maxSize->getHeight()));
    }
  }

  return maxSize;
} // end RowLayoutPrivate::getMaxUiComponentSize

void RowLayoutPrivate::resizeUiComponentsInSizeGroup(const std::vector<std::pair<SizeGroupIdentifier, UiComponentWrapper *> >& uiComponentWrappers,
                                                     std::auto_ptr<Size2D> maxSize)
{
  for(std::size_t i = 0; i < uiComponentWrappers.size(); ++i)
  {
    UiComponentWrapper * uiComponentWrapper = uiComponentWrappers.at(i).second;

    if(uiComponentWrappers.at(i).first == LabelGroup)
    {
      uiComponentWrapper->uiComponentInternals->labelPreferredWidth = maxSize->getWidth();
      uiComponentWrapper->uiComponentInternals->labelPreferredHeight = maxSize->getHeight();
    }
    else
    {
      uiComponentWrapper->uiComponentInternals->uiComponentPreferredWidth = maxSize->getWidth();
      uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight = maxSize->getHeight();
    }
  }
} // end RowLayoutPrivate::resizeUiComponentsInSizeGroup

void RowLayoutPrivate::resizeUiComponentsInSizeGroups()
{
  std::map<int, std::vector<std::pair<SizeGroupIdentifier, UiComponentWrapper *> > >::iterator it = sizeGroups.begin();

  for(; it != sizeGroups.end(); ++it)
  {
    // Get the max size for all of the UiComponents within this size group
    // container
    std::auto_ptr<Size2D> maxSize(getMaxUiComponentSize(it->second));

    // Resize all of the UiComponents within this size group container
    resizeUiComponentsInSizeGroup(it->second, maxSize);
  }
} //end RowLayoutPrivate::resizeUiComponentsInSizeGroups

void RowLayoutPrivate::resizePreviousAndNextLabelRows(RowLayout * thisRowLayout)
{
  for(std::size_t rowIndex = 0; rowIndex < thisRowLayout->getRowCount(); ++rowIndex)
  {
    if(rows.at(rowIndex)->d->previousLabelRow) // TODO: Do not use the rows variable directly, access it with a function.
    {
      LabelRow * prev = rows.at(rowIndex)->d->previousLabelRow;

      for(std::size_t i = 0; i < prev->getLabelCount(); ++i)
      {
        prev->maxHeightLabelInRow = std::max(prev->maxHeightLabelInRow,
                                             prev->getWrapperForLabelAt(i)->uiComponentInternals->labelPreferredHeight);
      }
    }

    if(rows.at(rowIndex)->d->nextLabelRow) // TODO: Do not use the rows variable directly, access it with a function.
    {
      LabelRow * next = rows.at(rowIndex)->d->nextLabelRow;

      for(std::size_t i = 0; i < next->getLabelCount(); ++i)
      {
        next->maxHeightLabelInRow = std::max(next->maxHeightLabelInRow,
                                             next->getWrapperForLabelAt(i)->uiComponentInternals->labelPreferredHeight);
      }
    }
  } // end for loop
} // end RowLayoutPrivate::resizePreviousAndNextLabelRows

// This stage is only concerned with grabbing the preferred sizes and
// setting those sizes to the actual size.
//
// We also specify any gaps if gaps have not been specified. However we do
// not apply gaps, that is done in the post layout stage.
//
// We make no adjustments to the location of any uiComponent.
//
// We make no adjustments to the container during this stage.
void RowLayoutPrivate::preLayout(RowLayout * thisRowLayout)
{
  for(std::size_t rowIndex = 0; rowIndex < thisRowLayout->getRowCount(); ++rowIndex)
  {
    std::size_t componentIndex = 0;

    // Start Fencepost block, because the post fencepost loop will add in
    // the uiComponent gaps if any.
    if(thisRowLayout->getUiComponentCountAtRow(rowIndex) > 0) // there is at least one uiComponent in this row
    {
      RowLayout::Row * rowLayoutRow = rows.at(rowIndex);
      UiComponentWrapper * uiComponentWrapper = rowLayoutRow->d->getUiComponentWrapperAt(componentIndex);

      if(uiComponentWrapper->labelPeer)
      {
        // this is a label/uiComponent pair

        // Since the uiComponents were added to this layout manager, the
        // width and height are under the control of the layout manager. So
        // the width and height must be set accordingly.

        uiComponentWrapper->uiComponentInternals->labelPreferredWidth = uiComponentWrapper->labelPeer->getPreferredWidth();
        uiComponentWrapper->uiComponentInternals->labelPreferredHeight = uiComponentWrapper->labelPeer->getPreferredHeight();
      }

      // Now configure the uiComponent

      // Since the uiComponents were added to this layout manager, the width
      // and height are under the control of the layout manager. So the
      // width and height must be set accordingly.

      uiComponentWrapper->uiComponentInternals->uiComponentPreferredWidth = uiComponentWrapper->uiComponent->getPreferredWidth();
      uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight = uiComponentWrapper->uiComponent->getPreferredHeight();
    }
    // End Fencepost block

    ++componentIndex;

    // Check for all of the uiComponents in the current row.
    for(; componentIndex < thisRowLayout->getUiComponentCountAtRow(rowIndex); ++componentIndex)
    {
      RowLayout::Row * rowLayoutRow = rows.at(rowIndex);
      UiComponentWrapper * uiComponentWrapper = rowLayoutRow->d->getUiComponentWrapperAt(componentIndex);

      if(uiComponentWrapper->labelPeer)
      {
        // this is a label/uiComponent pair

        // Since the uiComponents were added to this layout manager, the
        // width and height are under the control of the layout manager. So
        // the width and height must be set accordingly.

        uiComponentWrapper->uiComponentInternals->labelPreferredWidth = uiComponentWrapper->labelPeer->getPreferredWidth();
        uiComponentWrapper->uiComponentInternals->labelPreferredHeight = uiComponentWrapper->labelPeer->getPreferredHeight();
      }

      // Now configure the uiComponent

      // Since the uiComponents were added to this layout manager, the width
      // and height are under the control of the layout manager. So the
      // width and height must be set accordingly.

      uiComponentWrapper->uiComponentInternals->uiComponentPreferredWidth = uiComponentWrapper->uiComponent->getPreferredWidth();
      uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight = uiComponentWrapper->uiComponent->getPreferredHeight();

      // Set the gap if a gap has not already been specified. Note since
      // a gap by definition is the space between two uiComponents, you will
      // need to get the gap between the previous uiComponent and the
      // current uiComponent, setting only the right gap of the previous
      // uiComponent and the left gap of the current uiComponent.
      UiComponentWrapper * previousUiComponent = rowLayoutRow->d->getUiComponentWrapperAt(componentIndex - 1);
      UiComponentWrapper * currentUiComponent = rowLayoutRow->d->getUiComponentWrapperAt(componentIndex);

      if(previousUiComponent->uiComponentConstraints->horizontalGapRight < 0)
      {
        // the client did not specify the gap, so we will need to specify
        // a gap

        // Leave the user constraint unspecified, only change the internal
        // constraint because the user left the constraint unspecified.
        previousUiComponent->uiComponentInternals->horizontalGapRight = 7; // TODO: Do not hard code, also you need to factor in related/unrelated
      }

      if(currentUiComponent->uiComponentConstraints->horizontalGapLeft < 0)
      {
        // the client did not specify the gap, so we will need to specify
        // a gap

        // Leave the user constraint unspecified, only change the internal
        // constraint because the user left the constraint unspecified.
        currentUiComponent->uiComponentInternals->horizontalGapLeft = 7; // TODO: Do not hard code, also you need to factor in related/unrelated
      }

    }// end current row loop

    // Now that we have the sizes set for all uiComponents within this row
    // check to see if there are any labels in the previous or next rows
    // so that we can calculate the maximum height of those rows if they
    // exist.
    resizePreviousAndNextLabelRows(thisRowLayout);

  }// end all rows loop
} // end RowLayoutPrivate::preLayout

int RowLayoutPrivate::getUiComponentGap(std::size_t rowIndex,
                                        std::size_t previousUiComponentIndex,
                                        std::size_t currentUiComponentIndex)
{
  RowLayout::Row * rowLayoutRow = rows.at(rowIndex);
  UiComponentWrapper * previousUiComponent = rowLayoutRow->d->getUiComponentWrapperAt(previousUiComponentIndex);
  UiComponentWrapper * currentUiComponent = rowLayoutRow->d->getUiComponentWrapperAt(currentUiComponentIndex);

  if(previousUiComponent->uiComponentConstraints->horizontalGapRight > 0)
  {
    // the client specified a gap for the previous uiComponent

    if(currentUiComponent->uiComponentConstraints->horizontalGapLeft > 0)
    {
      // the client specified a gap for the current uiComponent

      // the gap
      return std::max(previousUiComponent->uiComponentConstraints->horizontalGapRight,
                      currentUiComponent->uiComponentConstraints->horizontalGapLeft);
    }
    else
    {
      // the client did not specify a gap for the current uiComponent

      // the gap
      return std::max(previousUiComponent->uiComponentConstraints->horizontalGapRight,
                      currentUiComponent->uiComponentInternals->horizontalGapLeft);
    }
  }
  else
  {
    // the client did not specify a gap for the previous uiComponent

    if(currentUiComponent->uiComponentConstraints->horizontalGapLeft > 0)
    {
      // the client specified a gap for the current uiComponent

      // the gap
      return std::max(previousUiComponent->uiComponentInternals->horizontalGapRight,
                      currentUiComponent->uiComponentConstraints->horizontalGapLeft);
    }
    else
    {
      // the client did not specify a gap for the current uiComponent

      // the gap
      return std::max(previousUiComponent->uiComponentInternals->horizontalGapRight,
                      currentUiComponent->uiComponentInternals->horizontalGapLeft);
    }
  }
} // end RowLayoutPrivate::getUiComponentGap

void RowLayoutPrivate::applyCenterRowAlignment(RowLayout * thisRowLayout,
                                               std::size_t rowIndex,
                                               int containerWidth,
                                               int containerLeftPadding,
                                               int containerRightPadding)
{
  int remainingFreeSpace = containerWidth - rows.at(rowIndex)->d->rowWidth - containerLeftPadding - containerRightPadding;

  remainingFreeSpace  = remainingFreeSpace / 2;

  if(remainingFreeSpace > 0)
  {
    for(std::size_t uiComponentIndex = 0; uiComponentIndex < thisRowLayout->getUiComponentCountAtRow(rowIndex); ++uiComponentIndex)
    {
      UiComponentWrapper * uiComponentWrapper = rows.at(rowIndex)->d->getUiComponentWrapperAt(uiComponentIndex);

      if(uiComponentWrapper->labelPeer)
      {
        uiComponentWrapper->uiComponentInternals->labelX = uiComponentWrapper->uiComponentInternals->labelX + remainingFreeSpace;
      }

      uiComponentWrapper->uiComponentInternals->uiComponentX = uiComponentWrapper->uiComponentInternals->uiComponentX + remainingFreeSpace;
    }
  }
} // end RowLayoutPrivate::applyCenterRowAlignment

void RowLayoutPrivate::applyRightRowAlignment(RowLayout * thisRowLayout,
                                              std::size_t rowIndex,
                                              int containerWidth,
                                              int containerLeftPadding,
                                              int containerRightPadding)
{
  int remainingFreeSpace = containerWidth - rows.at(rowIndex)->d->rowWidth - containerLeftPadding - containerRightPadding;

  if(remainingFreeSpace > 0)
  {
    for(std::size_t uiComponentIndex = 0; uiComponentIndex < thisRowLayout->getUiComponentCountAtRow(rowIndex); ++uiComponentIndex)
    {
      UiComponentWrapper * uiComponentWrapper = rows.at(rowIndex)->d->getUiComponentWrapperAt(uiComponentIndex);

      if(uiComponentWrapper->labelPeer)
      {
        uiComponentWrapper->uiComponentInternals->labelX = uiComponentWrapper->uiComponentInternals->labelX + remainingFreeSpace;
      }

      uiComponentWrapper->uiComponentInternals->uiComponentX = uiComponentWrapper->uiComponentInternals->uiComponentX + remainingFreeSpace;
    }
  }
} // end RowLayoutPrivate::applyRightRowAlignment

void RowLayoutPrivate::handleVerticalAlignment(UiComponentWrapper * uiComponentWrapper,
                                               int * yLabelVAlignmentOffset,
                                               int * yUiComponentPeerVAlignmentOffset)
{
  if(uiComponentWrapper->uiComponentConstraints->labelVerticalAlignment == LabelVerticalAlignment::MiddleAlignment)
  {
    if(uiComponentWrapper->uiComponentInternals->labelPreferredHeight < uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight)
    {
      *yLabelVAlignmentOffset = (uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight - uiComponentWrapper->uiComponentInternals->labelPreferredHeight) / 2;
    }
    else if(uiComponentWrapper->uiComponentInternals->labelPreferredHeight > uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight)
    {
      *yUiComponentPeerVAlignmentOffset = (uiComponentWrapper->uiComponentInternals->labelPreferredHeight - uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight) / 2;
    }
  }
  else if(uiComponentWrapper->uiComponentConstraints->labelVerticalAlignment == LabelVerticalAlignment::BottomAlignment)
  {
    if(uiComponentWrapper->uiComponentInternals->labelPreferredHeight < uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight)
    {
      *yLabelVAlignmentOffset = (uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight - uiComponentWrapper->uiComponentInternals->labelPreferredHeight);
    }
    else if(uiComponentWrapper->uiComponentInternals->labelPreferredHeight > uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight)
    {
      *yUiComponentPeerVAlignmentOffset = (uiComponentWrapper->uiComponentInternals->labelPreferredHeight - uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight);
    }
  }
} // end RowLayoutPrivate::handleVerticalAlignment

void RowLayoutPrivate::layoutUiComponentInRow(RowLayout * thisRowLayout,
                                              std::size_t rowIndex,
                                              std::size_t uiComponentIndex,
                                              int * xOffset,
                                              int * yOffset,
                                              int * heightOfTallestUiComponentInRow,
                                              bool insertGaps)
{
  if(insertGaps)
  {
    int temp = thisRowLayout->getHorizontalSpaceBetweenUiComponents(rowIndex);

    *xOffset += temp;

    // Now we must insert the gap from the previous uiComponent if any and
    // the current uiComponent gap if any before we set the location of
    // the current uiComponent.
    temp = getUiComponentGap(rowIndex,
                             uiComponentIndex - 1, // previous uiComponent
                             uiComponentIndex);    // current uiComponent
    *xOffset += temp;
  }

  UiComponentWrapper * uiComponentWrapper = rows.at(rowIndex)->d->getUiComponentWrapperAt(uiComponentIndex);

  std::size_t finalUiComponentYOffset = 0;

  if(rows.at(rowIndex)->d->previousLabelRow)
  {
    finalUiComponentYOffset = rows.at(rowIndex)->d->previousLabelRow->maxHeightLabelInRow;
  }

  std::size_t uiComponentRowXOffset = 0;

  // 1) Handle labels on the top.
  if(uiComponentWrapper->labelPeer &&
      uiComponentWrapper->uiComponentConstraints->labelOrientation == LabelOrientation::TopOrientation)
  {
    // this is a label from the uiComponent/label pair above its uiComponent
    uiComponentWrapper->uiComponentInternals->labelX = *xOffset;
    uiComponentWrapper->uiComponentInternals->labelY = *yOffset;

    // Do not adjust the xOffset since the label is directly above its
    // uiComponent pair.
  }

  // 2) Handle uiComponents and in-line labels(labels to the left or right)
  //    Left label, uiComponent then right label in that order

  int yLabelVAlignmentOffset = 0;
  int yUiComponentPeerVAlignmentOffset = 0;

  if(uiComponentWrapper->labelPeer &&
      uiComponentWrapper->uiComponentConstraints->labelOrientation == LabelOrientation::LeftOrientation)
  {
    // this is a label from the uiComponent/label pair
    uiComponentWrapper->uiComponentInternals->labelX = *xOffset + uiComponentRowXOffset;

    // handle vertical alignment if any
    handleVerticalAlignment(uiComponentWrapper, &yLabelVAlignmentOffset, &yUiComponentPeerVAlignmentOffset);

    uiComponentWrapper->uiComponentInternals->labelY = *yOffset + yLabelVAlignmentOffset + finalUiComponentYOffset;

    uiComponentRowXOffset += uiComponentWrapper->uiComponentInternals->labelPreferredWidth;

    *heightOfTallestUiComponentInRow = std::max(*heightOfTallestUiComponentInRow, uiComponentWrapper->uiComponentInternals->labelPreferredHeight);
  }

  // Now configure the uiComponent
  uiComponentWrapper->uiComponentInternals->uiComponentX = *xOffset + uiComponentRowXOffset;
  uiComponentWrapper->uiComponentInternals->uiComponentY = *yOffset + yUiComponentPeerVAlignmentOffset + finalUiComponentYOffset;

  uiComponentRowXOffset += uiComponentWrapper->uiComponentInternals->uiComponentPreferredWidth;

  *heightOfTallestUiComponentInRow = std::max(*heightOfTallestUiComponentInRow, uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight);

  if(uiComponentWrapper->labelPeer &&
      uiComponentWrapper->uiComponentConstraints->labelOrientation == LabelOrientation::RightOrientation)
  {
    // this is a label from the uiComponent/label pair
    uiComponentWrapper->uiComponentInternals->labelX = *xOffset + uiComponentRowXOffset;

    // handle vertical alignment if any
    handleVerticalAlignment(uiComponentWrapper, &yLabelVAlignmentOffset, &yUiComponentPeerVAlignmentOffset);

    uiComponentWrapper->uiComponentInternals->labelY = *yOffset + finalUiComponentYOffset;

    uiComponentRowXOffset += uiComponentWrapper->uiComponentInternals->labelPreferredWidth;

    *heightOfTallestUiComponentInRow = std::max(*heightOfTallestUiComponentInRow, uiComponentWrapper->uiComponentInternals->labelPreferredHeight);
  }

  finalUiComponentYOffset += uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight;

  // 3) Handle labels on the bottom.

  if(uiComponentWrapper->labelPeer &&
      uiComponentWrapper->uiComponentConstraints->labelOrientation == LabelOrientation::BottomOrientation)
  {
    // this is a label from the uiComponent/label pair
    uiComponentWrapper->uiComponentInternals->labelX = *xOffset;
    uiComponentWrapper->uiComponentInternals->labelY = *yOffset + finalUiComponentYOffset;

    *heightOfTallestUiComponentInRow = std::max(*heightOfTallestUiComponentInRow, uiComponentWrapper->uiComponentInternals->labelPreferredHeight);
  }

  // Adjust the xOffset
  *xOffset += uiComponentRowXOffset;
} // end RowLayoutPrivate::layoutUiComponentInRow

void RowLayoutPrivate::moveUiComponentsInRowRight(std::size_t rowIndex,
                                                  std::size_t uiComponentIndex,
                                                  int remainingWidth)
{
  for(; uiComponentIndex < rows.at(rowIndex)->getUiComponentCount(); ++uiComponentIndex)
  {
    UiComponentWrapper * uiComponentWrapper = rows.at(rowIndex)->d->getUiComponentWrapperAt(uiComponentIndex);

    uiComponentWrapper->uiComponentInternals->uiComponentX = uiComponentWrapper->uiComponentInternals->uiComponentX + remainingWidth;
  }
} // end RowLayoutPrivate::moveUiComponentsInRowRight

void RowLayoutPrivate::moveUiComponentsBelowDown(std::size_t rowIndex,
                                                 int remainingHeight)
{
  for(; rowIndex < rows.size(); ++rowIndex)
  {
    for(std::size_t uiComponentIndex = 0; uiComponentIndex < rows.at(rowIndex)->getUiComponentCount(); ++uiComponentIndex)
    {
      UiComponentWrapper * uiComponentWrapper = rows.at(rowIndex)->d->getUiComponentWrapperAt(uiComponentIndex);

      uiComponentWrapper->uiComponentInternals->uiComponentY = uiComponentWrapper->uiComponentInternals->uiComponentY + remainingHeight;
    }
  }
} // end RowLayoutPrivate::moveUiComponentsBelowDown

void RowLayoutPrivate::applyAllSizes()
{
  for(std::size_t rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
  {
    RowLayout::Row * rowLayoutRow = rows.at(rowIndex);

    for(std::size_t uiComponentIndex = 0; uiComponentIndex < rowLayoutRow->getUiComponentCount(); ++uiComponentIndex)
    {
      UiComponentWrapper * uiComponentWrapper = rowLayoutRow->d->getUiComponentWrapperAt(uiComponentIndex);

      if(uiComponentWrapper->labelPeer)
      {
        uiComponentWrapper->labelPeer->setSize(uiComponentWrapper->uiComponentInternals->labelPreferredWidth,
                                               uiComponentWrapper->uiComponentInternals->labelPreferredHeight);
      }

      uiComponentWrapper->uiComponent->setSize(uiComponentWrapper->uiComponentInternals->uiComponentPreferredWidth,
                                               uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight);
    }
  }
} // end RowLayoutPrivate::applyAllSizes

void RowLayoutPrivate::applyAllLocations()
{
  for(std::size_t rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
  {
    RowLayout::Row * rowLayoutRow = rows.at(rowIndex);

    for(std::size_t uiComponentIndex = 0; uiComponentIndex < rowLayoutRow->getUiComponentCount(); ++uiComponentIndex)
    {
      UiComponentWrapper * uiComponentWrapper = rowLayoutRow->d->getUiComponentWrapperAt(uiComponentIndex);

      if(uiComponentWrapper->labelPeer)
      {
        uiComponentWrapper->labelPeer->setLocation(uiComponentWrapper->uiComponentInternals->labelX,
                                                   uiComponentWrapper->uiComponentInternals->labelY);
      }

      uiComponentWrapper->uiComponent->setLocation(uiComponentWrapper->uiComponentInternals->uiComponentX,
                                                   uiComponentWrapper->uiComponentInternals->uiComponentY);
    }
  }
} // end RowLayoutPrivate::applyAllLocations

void RowLayoutPrivate::setDrawingEnabled(bool flag)
{
  for(std::size_t rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
  {
    RowLayout::Row * rowLayoutRow = rows.at(rowIndex);

    for(std::size_t uiComponentIndex = 0; uiComponentIndex < rowLayoutRow->getUiComponentCount(); ++uiComponentIndex)
    {
      UiComponentWrapper * uiComponentWrapper = rowLayoutRow->d->getUiComponentWrapperAt(uiComponentIndex);

      if(uiComponentWrapper->labelPeer)
      {
        if(flag)
        {
          uiComponentWrapper->labelPeer->enableDrawing();
        }
        else
        {
          uiComponentWrapper->labelPeer->disableDrawing();
        }
      }

      if(flag)
      {
        uiComponentWrapper->uiComponent->enableDrawing();;
      }
      else
      {
        uiComponentWrapper->uiComponent->disableDrawing();
      }
    }
  }
} // end RowLayoutPrivate::setDrawingEnabled

void RowLayoutPrivate::enableAllDrawing()
{
  setDrawingEnabled(true);
}

void RowLayoutPrivate::disableAllDrawing()
{
  setDrawingEnabled(false);
}

void RowLayoutPrivate::postLayout(RowLayout * thisRowLayout, UiComponent * container)
{
  // To prevent flickering while laying out
  disableAllDrawing();

  // Apply the constant left padding if any.
  const int initialXOffset = container->getLeftPadding();
  int xOffset = initialXOffset;

  // Apply the constant top padding if any.
  int yOffset = container->getTopPadding();

  // The height of the tallest uiComponent in the current row.
  int heightOfTallestUiComponentInRow = 0;

  int initialYOffset = container->getTopPadding();

  // Take the container dimensions into account, by keeping a running
  // tally of the size of its uiComponents and padding.
  int containerWidth = container->getLeftPadding() + container->getRightPadding();

  int containerHeight = container->getTopPadding() + container->getBottomPadding();

  // Width of the largest row in the specified container.
  // Note: the width of any given row is the following.
  //
  // xOffset - initialXOffset
  int maxRowWidth = 0;

  std::size_t rowIndex = 0;

  // Start fencepost block. We need the fencepost here in order to handle
  // gaps between rows.
  if(thisRowLayout->getRowCount() > 0)
  {
    layoutAllUiComponentsInRow(thisRowLayout,
                               rowIndex,
                               &xOffset,
                               &yOffset,
                               &heightOfTallestUiComponentInRow,
                               initialXOffset,
                               &initialYOffset,
                               &containerHeight,
                               &maxRowWidth,
                               true);
  }
  // End fencepost block.

  ++rowIndex;

  // Handle the remaining rows
  for(; rowIndex < thisRowLayout->getRowCount(); ++rowIndex)
  {
    layoutAllUiComponentsInRow(thisRowLayout,
                               rowIndex,
                               &xOffset,
                               &yOffset,
                               &heightOfTallestUiComponentInRow,
                               initialXOffset,
                               &initialYOffset,
                               &containerHeight,
                               &maxRowWidth,
                               false);
  }// end all rows loop

  containerWidth += maxRowWidth;

  // Now set the preferred size of the container
  container->setPreferredSize(containerWidth, containerHeight);

  // Now that the uiComponents are in place, we need to apply orientation
  // rules if any have been specified.

  for(std::size_t i = 0; i < thisRowLayout->getRowCount(); ++i)
  {
    if(rows.at(i)->d->rowOrientation == RowOrientation::RightRowOrientation)
    {
      applyRightRowAlignment(thisRowLayout,
                             i,
                             container->getClientWidth(),
                             container->getLeftPadding(),
                             container->getRightPadding());
    }
    else if(rows.at(i)->d->rowOrientation == RowOrientation::CenterRowOrientation)
    {
      applyCenterRowAlignment(thisRowLayout,
                              i,
                              container->getClientWidth(),
                              container->getLeftPadding(),
                              container->getRightPadding());
    }
  }

  // Grow uiComponents if you have the space
  // Note: In a row layout, the uiComponents can grow wider independent of
  //       other rows. However, height is constrained by the amount of rows
  //       in the layout.
  for(std::size_t rowIndex = 0; rowIndex < thisRowLayout->getRowCount(); ++rowIndex)
  {
    int remainingWidth = container->getClientWidth() - rows.at(rowIndex)->d->rowWidth;
    int remainingHeight = container->getClientHeight() - containerHeight;

    // strip the padding if any
    remainingWidth = remainingWidth - (container->getLeftPadding() + container->getRightPadding()); // Note: Row width does not include the padding so take it out
    // Note: Nothing needs to be taken out of the height because the container height already includes the padding.

    // give the rest to children that want the space
    for(std::size_t uiComponentIndex = 0; uiComponentIndex < thisRowLayout->getUiComponentCountAtRow(rowIndex); ++uiComponentIndex)
    {
      UiComponentWrapper * uiComponentWrapper = rows.at(rowIndex)->d->getUiComponentWrapperAt(uiComponentIndex);

      if(uiComponentWrapper->uiComponentConstraints->growX > 0) // TODO: In the future allow more than one uiComponent to grow.
      {
        if(remainingWidth > 0)
        {
          uiComponentWrapper->uiComponentInternals->uiComponentPreferredWidth = uiComponentWrapper->uiComponentInternals->uiComponentPreferredWidth + remainingWidth;

          // Move the rest of the uiComponents in this row to the right by
          // remaining width
          // TODO: Don't forget about uiComponent/label pairs above or below
          moveUiComponentsInRowRight(rowIndex, uiComponentIndex + 1, remainingWidth);
        }
      }

      // Grow Y implies Grow Row Y too.
      // Grow the uiComponent in row first, make sure not greater than its
      // row.
      // Then grow it by the remaining height of the container
      if(uiComponentWrapper->uiComponentConstraints->growY > 0)
      {
        int remainingHeightInRow = rows.at(rowIndex)->d->rowHeight - uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight;

        if(remainingHeightInRow > 0)
        {
          uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight = uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight + remainingHeightInRow;
        }

        if(remainingHeight > 0)
        {
          uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight = uiComponentWrapper->uiComponentInternals->uiComponentPreferredHeight + remainingHeight;

          // Move the rest of the uiComponents in rows below this
          // uiComponent by the remaining height
          // TODO: Don't forget about uiComponent/label pairs below
          // rowIndex + 1 --> One row below
          moveUiComponentsBelowDown(rowIndex + 1, remainingHeight);

          // There is no more height left
          remainingHeight = 0;
        }
      }
    }
  } // end grow loop

  // Apply all the cached sizes in one step
  applyAllSizes();

  // Apply all the cached locations in one step
  applyAllLocations();

  enableAllDrawing();
} // end RowLayoutPrivate::postLayout

void RowLayoutPrivate::layoutAllUiComponentsInRow(RowLayout * thisRowLayout,
                                                  std::size_t rowIndex,
                                                  int * xOffset,
                                                  int * yOffset,
                                                  int * heightOfTallestUiComponentInRow,
                                                  int initialXOffset,
                                                  int * initialYOffset,
                                                  int * containerHeight,
                                                  int * maxRowWidth,
                                                  bool isFirstRow)
{
  // Reset the x-offset for the new row.
  *xOffset = initialXOffset;

  if(!isFirstRow)
  {
    // Adjust the y-offset for the new row.
    *yOffset = *initialYOffset;
  }

  // We've added the last row's tallest uiComponent's height so reset the
  // offset so we can calculate the height of the tallest uiComponent in
  // the next row.
  *heightOfTallestUiComponentInRow = 0;

  std::size_t uiComponentIndex = 0;

  if(!isFirstRow)
  {
    // Also add in the vertical gap between rows.
    *yOffset += ::getVerticalSpaceBetweenRowsToPlatformIfNeeded(thisRowLayout->getVerticalSpaceBetweenRows());
  }

  // Start Fencepost block. We need the fencepost here in order to handle
  // gaps between uiComponents in a row.

  // there is at least one uiComponent in this row
  if(thisRowLayout->getUiComponentCountAtRow(rowIndex) > 0)
  {
    layoutUiComponentInRow(thisRowLayout,
                           rowIndex,
                           uiComponentIndex,
                           xOffset,
                           yOffset,
                           heightOfTallestUiComponentInRow,
                           false);
  }
  // End Fencepost block

  ++uiComponentIndex;

  // Check for all of the uiComponents in the current row.
  for(; uiComponentIndex < thisRowLayout->getUiComponentCountAtRow(rowIndex); ++uiComponentIndex)
  {
    layoutUiComponentInRow(thisRowLayout,
                           rowIndex,
                           uiComponentIndex,
                           xOffset,
                           yOffset,
                           heightOfTallestUiComponentInRow,
                           true);
  }// end current row loop

  // For each row, we need to check its previous and next label row
  // pointer, if they are valid, offset the yOffset by the height of the
  // tallest label in each valid label row. The check is here instead of
  // earlier so that we ensure at least one uiComponent is in the current
  // row.
  if(rows.at(rowIndex)->d->previousLabelRow)
  {
    *yOffset += rows.at(rowIndex)->d->previousLabelRow->maxHeightLabelInRow;
  }

  if(rows.at(rowIndex)->d->nextLabelRow)
  {
    *yOffset += rows.at(rowIndex)->d->nextLabelRow->maxHeightLabelInRow;
  }

  rows.at(rowIndex)->d->rowWidth = *xOffset - initialXOffset;
  rows.at(rowIndex)->d->rowHeight = (*yOffset + *heightOfTallestUiComponentInRow) - *initialYOffset;

  *initialYOffset += rows.at(rowIndex)->d->rowHeight;

  *maxRowWidth = std::max(*maxRowWidth, rows.at(rowIndex)->d->rowWidth);

  *containerHeight += rows.at(rowIndex)->d->rowHeight;
}

RowLayout::RowLayout(UiComponent * rowLayoutContainer)
:ILayout(),
 d(new RowLayoutPrivate(rowLayoutContainer))
{
  rowLayoutContainer->setLayout(this);
}

RowLayout::~RowLayout()
{
  delete d;
  d = nullptr;
}

std::size_t RowLayout::getRowCount() const
{
  return d->rows.size();
}

std::size_t RowLayout::getUiComponentCountAllRows() const
{
  std::size_t totalUiComponents = 0;

  for(std::size_t i = 0; i < d->rows.size(); ++i)
  {
    Row * row = d->rows.at(i);
    totalUiComponents += row->getUiComponentCount();
  }

  return totalUiComponents;
}

std::size_t RowLayout::getUiComponentCountAtRow(std::size_t row) const
{
  // Verify that the row is valid
  if(d->rows.size() >= row + 1)
  {
    return d->rows.at(row)->getUiComponentCount();
  }

  return 0;
}

UiComponent * RowLayout::getUiComponentAtRow(std::size_t row,
                                             std::size_t index) const
{
  // Verify that the row parameter is valid
  if(d->rows.size() >= row + 1)
  {
    // Verify that the index parameter is valid
    if(d->rows.at(row)->getUiComponentCount() > 0 &&
       d->rows.at(row)->getUiComponentCount() - 1 >= index)
    {
      return d->rows.at(row)->getUiComponentAt(index);
    }
  }

  return nullptr;
}

Label * RowLayout::getLabelAtRow(std::size_t row,
                                 std::size_t index) const
{
  // Verify that the row parameter is valid
  if(d->rows.size() >= row + 1)
  {
    // Verify that the index parameter is valid
    if(d->rows.at(row)->getUiComponentCount() > 0 &&
       d->rows.at(row)->getUiComponentCount() - 1 >= index)
    {
      return d->rows.at(row)->getLabelAt(index);
    }
  }

  return nullptr;
}

std::size_t RowLayout::getHorizontalSpaceBetweenUiComponents(std::size_t row) const
{
  // Verify that the row is valid
  if(d->rows.size() >= row + 1)
  {
    return d->rows.at(row)->d->horizontalSpaceBetweenUiComponents;
  }

  return 0;
}

int RowLayout::getVerticalSpaceBetweenRows() const
{
  return d->verticalSpaceBetweenRows;
}

RowLayout::Row * RowLayout::addRow()
{
  d->currentRow = new Row(this);
  d->rows.push_back(d->currentRow);

  return d->currentRow;
}

// TODO: Adding a UiComponent should force a layout
RowLayout::Row * RowLayout::addUiComponent(UiComponent * uiComponent,
                                           UiComponentConstraints * uiComponentConstraints)
{
  // TODO: Change this note from client to our responsibility and handle it
  // Note: it is the client's responsibility to ensure that the UiComponent has
  //       not already been added to the layout. This layout does not check
  //       for or guard against duplicate UiComponents.

  // 1) If the current Row is not nullptr, use it.

  // 2) If the current Row is nullptr create a new Row, add it to the rows
  //    and use it.
  if(!d->currentRow)
  {
    d->currentRow = new Row(this);
    d->rows.push_back(d->currentRow);
  }

  // Now that the row exists, add the UiComponent to the row
  return d->currentRow->addUiComponent(uiComponent,
                                       uiComponentConstraints);
}

// TODO: Adding a UiComponent should force a layout
RowLayout::Row * RowLayout::addUiComponent(Label * label,
                                           UiComponent * uiComponent,
                                           UiComponentConstraints * uiComponentConstraints)
{
  // TODO: Change this note from client to our responsibility and handle it
  // Note: it is the client's responsibility to ensure that the uiComponent has
  //       not already been added to the layout. This layout does not check
  //       for or guard against duplicate uiComponents.

  // 1) If the current Row is not nullptr, use it.

  // 2) If the current Row is nullptr create a new Row, add it to the rows
  //    and use it.
  if(!d->currentRow)
  {
    d->currentRow = new Row(this);
    d->rows.push_back(d->currentRow);
  }

  // Now that the row exists, add the uiComponent to the row
  return d->currentRow->addUiComponent(label,
                                       uiComponent,
                                       uiComponentConstraints);
}

// TODO: Perform a layout if this changes.
void RowLayout::setVerticalSpaceBetweenRows(int verticalSpace)
{
  d->verticalSpaceBetweenRows = verticalSpace;
}

// TODO: Removing a UiComponent should force a layout
// TODO: Implement remove on the underlying container too
void RowLayout::removeUiComponent(UiComponent * uiComponent)
{
  bool removedUiComponent = false;

  for(std::size_t i = 0; i < d->rows.size(); ++i)
  {
    Row * row = d->rows.at(i);

    for(std::size_t j = 0; j < row->getUiComponentCount(); ++j)
    {
      UiComponent * rowUiComponent = row->getUiComponentAt(j);

      if(rowUiComponent == uiComponent)
      {
        // Do not forget to remove the UiComponent from the size group if it is
        // specified
        UiComponentConstraints * constraints = row->d->getUiComponentConstraintsAt(j);
        UiComponentWrapper * uiComponentWrapper = row->d->getUiComponentWrapperAt(j);

        if(constraints->sizeGroupId >= 0)
        {
          std::vector<std::pair<SizeGroupIdentifier, UiComponentWrapper *> >& tempUiComponents = d->sizeGroups[constraints->sizeGroupId];

          tempUiComponents.erase(std::remove(tempUiComponents.begin(), tempUiComponents.end(), std::pair<SizeGroupIdentifier, UiComponentWrapper *>(UiComponentGroup, uiComponentWrapper)), tempUiComponents.end());
        }

        if(constraints->labelSizeGroupId >= 0)
        {
          std::vector<std::pair<SizeGroupIdentifier, UiComponentWrapper *> >& tempUiComponents = d->sizeGroups[constraints->labelSizeGroupId];

          tempUiComponents.erase(std::remove(tempUiComponents.begin(), tempUiComponents.end(), std::pair<SizeGroupIdentifier, UiComponentWrapper *>(LabelGroup, uiComponentWrapper)), tempUiComponents.end());
        }

        // Now remove the UiComponent

        row->removeUiComponentAt(j);
        removedUiComponent = true;
        break;
      }
    }

    if(removedUiComponent)
    {
      break;
    }
  }
}

// TODO: Implement remove on the underlying container too
void RowLayout::removeAllUiComponents()
{
  // Traverse the container backwards.
  for(std::size_t i = d->rows.size(); i-- > 0;)
  {
    Row * row = d->rows.at(i);
    row->removeAllUiComponents();
  }

  // Remove all size groups
  d->sizeGroups.clear();
}

void RowLayout::executeLayout(UiComponent * container)
{
  d->preLayout(this);

  // Now that you have the actual sizes of the UiComponents, calculate the
  // new sizes of the UiComponents that are members of size groups.
  //
  // Then run a new layout loop using the new sizes, not the preferred
  // sizes.

  d->resizeUiComponentsInSizeGroups();

  // You need to resize the previous and next label rows to account for
  // potential size group adjustments
  d->resizePreviousAndNextLabelRows(this);

  d->postLayout(this, container);
}

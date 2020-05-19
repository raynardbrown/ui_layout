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

#include <cstddef>

#include "ui/layout/ILayout.h"

#include "ui/layout/RowOrientation.h"

class UiComponent;
class Label;
struct UiComponentConstraints;
class RowLayoutPrivate;

class RowLayout : public ILayout
{
  public:

    /**
     * Create a new RowLayout.
     *
     * @param[in] uiComponent the UiComponent that will be laid out.
     */
    RowLayout(UiComponent * uiComponent);

    virtual ~RowLayout();

    /**
     * Return the number of rows in this layout.
     *
     * @return the number of rows in this layout.
     */
    std::size_t getRowCount() const;

    /**
     * Return the number of uiComponents associated with this layout. Note
     * labels that are added to this layout with an associated uiComponent, are
     * treated as one whole uiComponent with their associated uiComponent peers.
     *
     * @return the number of uiComponents associated with this layout.
     */
    std::size_t getUiComponentCountAllRows() const;

    /**
     * Return the number of uiComponents associated with this layout at the
     * specified row. Note labels that are added to this layout with an
     * associated uiComponent, are treated as one whole uiComponent with their
     * associated uiComponent peers.
     *
     * @param[in] row the row within this layout whose uiComponent count will be
     * given.
     *
     * @return the number of uiComponents associated with this layout at the
     * specified row.
     */
    std::size_t getUiComponentCountAtRow(std::size_t row) const;

    /**
     * Return the UiComponent in this layout at the specified row and the
     * specified index within that row. Note that if a UiComponent is added to
     * this layout with a label, the UiComponent and its associated label shall
     * share an index. Note that although a UiComponent and its associated label
     * share an index, only the UiComponent is return by this function.
     *
     * @param[in] row a row within this layout.
     *
     * @param[in] index the index within the specified row.
     *
     * @see RowLayout::getLabelAtRow
     *
     * @return the UiComponent in this layout at the specified row and the
     * specified index within that row or nullptr if the specified row and index
     * are invalid.
     */
    UiComponent * getUiComponentAtRow(std::size_t row,
                                      std::size_t index) const;

    Label * getLabelAtRow(std::size_t row,
                          std::size_t index) const;

    std::size_t getHorizontalSpaceBetweenUiComponents(std::size_t row) const;

    /**
     * Return the amount of vertical space in pixels between each row within
     * this RowLayout or -1 if this RowLayout is using the platform default
     * value.
     *
     * @return the amount of vertical space in pixels between each row within
     * this RowLayout or -1 if this RowLayout is using the platform default
     * value.
     */
    int getVerticalSpaceBetweenRows() const;

    /**
     * Add the specified UiComponent to this layout at the specified row.
     *
     * @param[in] uiComponent the UiComponent that will be added to this layout
     * at the specified row.
     *
     * @param[in] row the row in this layout where the specified UiComponent
     * will be added.
     *
     * @param[in] uiComponentConstraints the constraints to which the specified
     * UiComponent is bound.
     */
    void addUiComponent(UiComponent * uiComponent,
                        std::size_t row,
                        UiComponentConstraints * uiComponentConstraints);

    /**
     * Add the specified UiComponent and its associated label to this layout at
     * the specified row.
     *
     * @param[in] label the label that is associated with the specified
     * UiComponent.
     *
     * @param[in] uiComponent the UiComponent that will be added to this layout
     * at the specified row.
     *
     * @param[in] row the row where the specified UiComponent and its associated
     * label will be added to this layout.
     *
     * @param[in] uiComponentConstraints the constraints to which the specified
     * UiComponent and its associated label are bound.
     */
    void addUiComponent(Label * label,
                        UiComponent * uiComponent,
                        std::size_t row,
                        UiComponentConstraints * uiComponentConstraints);

    void setHorizontalSpaceBetweenUiComponents(std::size_t row,
                                               std::size_t horizontalSpace);

    /**
     * The amount of vertical space in pixels between each row within this
     * RowLayout.
     *
     * If the specified vertical space is less than 0, then this RowLayout uses
     * a default value which is platform dependent. The platform default value
     * assumes that rows are unrelated.
     *
     * @param[in] verticalSpace the amount of vertical space in pixels between
     * each row within this RowLayout.
     */
    void setVerticalSpaceBetweenRows(int verticalSpace);

    /**
     * Set the orientation of the specified row to the specified orientation.
     * The specified orientation is applied to all uiComponents within the
     * specified row.
     *
     * @param[in] row the row whose orientation will be changed.
     *
     * @param[in] rowOrientation the new orientation of the specified row.
     */
    void setRowOrientation(std::size_t row,
                           RowOrientation::Value rowOrientation);

    /**
     * Remove the specified uiComponent from this layout. If a uiComponent was
     * added with a label using
     * RowLayout::(Label *, UiComponent *, std::size_t, UiComponentConstraints *)
     * , then the specified uiComponent and the associated label are removed
     * from this layout.
     *
     * @param[in] uiComponent the UiComponent that will be removed from this
     * layout.
     */
    void removeUiComponent(UiComponent * uiComponent);

    /**
     * Remove all uiComponents from this layout.
     */
    void removeAllUiComponents();

    virtual void executeLayout(UiComponent * uiComponent);

  private:

    RowLayoutPrivate * d;

  public:

    class Row
    {
      private:

        Row(RowLayout * rowLayout);

      public:

        ~Row();

        // Note: Although UiComponents and their associated labels are individual
        //       UiComponents, for the sake of this structure, they are treated
        //       as "one" UiComponent and share an index.
        std::size_t getUiComponentCount() const;

        UiComponent * getUiComponentAt(std::size_t index);

        Label * getLabelAt(std::size_t index);

        /**
         * Return the alignment of this Row. The alignment applies to all
         * uiComponents within this Row.
         *
         * @return the alignment of the specified row.
         */
        RowOrientation::Value getRowOrientation() const;

        Row * addUiComponent(UiComponent * uiComponent,
                             const UiComponentConstraints * uiComponentConstraints);

        Row * addUiComponent(Label * label,
                             UiComponent* uiComponent,
                             const UiComponentConstraints * uiComponentConstraints);

        void removeAllUiComponents();

        void removeUiComponentAt(std::size_t index);

      private:

        class RowPrivate;
        RowPrivate * d;
    }; // end Row class
};
#endif /* UI_LAYOUT_ROWLAYOUT_H_ */

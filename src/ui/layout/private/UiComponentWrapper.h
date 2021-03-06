////////////////////////////////////////////////////////////////////////////////
//
// File: UiComponentWrapper.h
//
// Author: Raynard Brown
//
// Copyright (c) 2020 Raynard Brown
//
// All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef UI_LAYOUT_PRIVATE_UICOMPONENTWRAPPER_H_
#define UI_LAYOUT_PRIVATE_UICOMPONENTWRAPPER_H_

class UiComponent;
class Label;
struct UiComponentConstraints;
struct UiComponentInternals;

struct UiComponentWrapper
{
    UiComponent * uiComponent;
    Label * labelPeer;
    UiComponentConstraints * uiComponentConstraints;
    UiComponentInternals * uiComponentInternals;
};
#endif /* UI_LAYOUT_PRIVATE_UICOMPONENTWRAPPER_H_ */

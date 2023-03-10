/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCE_DIALOGWINDOW_JUCEHEADER__
#define __JUCE_DIALOGWINDOW_JUCEHEADER__

#include "juce_DocumentWindow.h"


//==============================================================================
/**
    A dialog-box style window.

    This class is a convenient way of creating a DocumentWindow with a close button
    that can be triggered by pressing the escape key.

    Any of the methods available to a DocumentWindow or ResizableWindow are also
    available to this, so it can be made resizable, have a menu bar, etc.

    You can either override or use an instance of the DialogWindow class directly,
    or you can use a DialogWindow::LaunchOptions structure to quickly set up and
    launch a box containing a content component.

    If you use the class directly, you'll need to override the
    DocumentWindow::closeButtonPressed() method to handle the user clicking the close
    button - for more info, see the DocumentWindow help.

    @see DocumentWindow, ResizableWindow
*/
class JUCE_API  DialogWindow   : public DocumentWindow
{
public:
    //==============================================================================
    /** Creates a DialogWindow.

        @param name                 the name to give the component - this is also
                                    the title shown at the top of the window. To change
                                    this later, use setName()
        @param backgroundColour     the colour to use for filling the window's background.
        @param escapeKeyTriggersCloseButton if true, then pressing the escape key will cause the
                                            close button to be triggered
        @param addToDesktop         if true, the window will be automatically added to the
                                    desktop; if false, you can use it as a child component
    */
    DialogWindow (const String& name,
                  const Colour& backgroundColour,
                  bool escapeKeyTriggersCloseButton,
                  bool addToDesktop = true);

    /** Destructor.
        If a content component has been set with setContentOwned(), it will be deleted.
    */
    ~DialogWindow();

    //==============================================================================
    /** This class defines a collection of settings to be used to open a DialogWindow.

        The easiest way to open a DialogWindow is to create yourself a LaunchOptions structure,
        initialise its fields with the appropriate details, and then call its launchAsync()
        method to launch the dialog.
    */
    struct LaunchOptions
    {
        LaunchOptions() noexcept;

        /** The title to give the window. */
        String dialogTitle;

        /** The background colour for the window. */
        Colour dialogBackgroundColour;

        /** The content component to show in the window. This must not be null!
            Using an OptionalScopedPointer to hold this pointer lets you indicate whether
            you'd like the dialog to automatically delete the component when the dialog
            has terminated.
        */
        OptionalScopedPointer <Component> content;

        /** If this is not a nullptr, it indicates a component that you'd like to position this
            dialog box in front of. See the DocumentWindow::centreAroundComponent() method for
            more info about this parameter.
        */
        Component* componentToCentreAround;

        /** If true, then the escape key will trigger the dialog's close button. */
        bool escapeKeyTriggersCloseButton;
        /** If true, the dialog will use a native title bar. See TopLevelWindow::setUsingNativeTitleBar() */
        bool useNativeTitleBar;
        /** If true, the window will be resizable. See ResizableWindow::setResizable() */
        bool resizable;
        /** Indicates whether to use a border or corner resizer component. See ResizableWindow::setResizable() */
        bool useBottomRightCornerResizer;

        /** Launches a new modal dialog window.
            This will create a dialog based on the settings in this structure,
            launch it modally, and return immediately. The window that is returned
            will be automatically deleted when the modal state is terminated.

            When the dialog's close button is clicked, it'll automatically terminate its
            modal state, but you can also do this programatically by calling
            exitModalState (returnValue) on the DialogWindow.

            If your content component needs to find the dialog window that it is
            contained in, a quick trick is to do this:
            @code
            Dialogwindow* dw = contentComponent->findParentComponentOfClass<DialogWindow>();

            if (dw != nullptr)
                dw->exitModalState (1234);
            @endcode
        */
        DialogWindow* launchAsync();

        /** Creates a new DialogWindow instance with these settings.
            This method simply creates the window, it doesn't run it modally. In most cases
            you'll want to use launchAsync() or runModal() instead.
        */
        DialogWindow* create();

       #if JUCE_MODAL_LOOPS_PERMITTED || DOXYGEN
        /** Launches and runs the dialog modally, returning the status code that was
            used to terminate the modal loop.

            Note that running modal loops inline is a BAD technique. If possible, always
            use launchAsync() instead of this method.
        */
        int runModal();
       #endif
    };

    //==============================================================================
    /** Easy way of quickly showing a dialog box containing a given component.

        Note: this method has been superceded by the DialogWindow::LaunchOptions structure,
        which does the same job with some extra flexibility. The showDialog method is here
        for backwards compatibility, but please use DialogWindow::LaunchOptions in new code.

        This will open and display a DialogWindow containing a given component, making it
        modal, but returning immediately to allow the dialog to finish in its own time. If
        you want to block and run a modal loop until the dialog is dismissed, use showModalDialog()
        instead.

        To close the dialog programatically, you should call exitModalState (returnValue) on
        the DialogWindow that is created. To find a pointer to this window from your
        contentComponent, you can do something like this:
        @code
        Dialogwindow* dw = contentComponent->findParentComponentOfClass<DialogWindow>();

        if (dw != nullptr)
            dw->exitModalState (1234);
        @endcode

        @param dialogTitle          the dialog box's title
        @param contentComponent     the content component for the dialog box. Make sure
                                    that this has been set to the size you want it to
                                    be before calling this method. The component won't
                                    be deleted by this call, so you can re-use it or delete
                                    it afterwards
        @param componentToCentreAround  if this is non-zero, it indicates a component that
                                    you'd like to show this dialog box in front of. See the
                                    DocumentWindow::centreAroundComponent() method for more
                                    info on this parameter
        @param backgroundColour     a colour to use for the dialog box's background colour
        @param escapeKeyTriggersCloseButton if true, then pressing the escape key will cause the
                                            close button to be triggered
        @param shouldBeResizable    if true, the dialog window has either a resizable border, or
                                    a corner resizer
        @param useBottomRightCornerResizer     if shouldBeResizable is true, this indicates whether
                                    to use a border or corner resizer component. See ResizableWindow::setResizable()
    */
    static void showDialog (const String& dialogTitle,
                            Component* contentComponent,
                            Component* componentToCentreAround,
                            const Colour& backgroundColour,
                            bool escapeKeyTriggersCloseButton,
                            bool shouldBeResizable = false,
                            bool useBottomRightCornerResizer = false);

   #if JUCE_MODAL_LOOPS_PERMITTED || DOXYGEN
    /** Easy way of quickly showing a dialog box containing a given component.

        Note: this method has been superceded by the DialogWindow::LaunchOptions structure,
        which does the same job with some extra flexibility. The showDialog method is here
        for backwards compatibility, but please use DialogWindow::LaunchOptions in new code.

        This will open and display a DialogWindow containing a given component, returning
        when the user clicks its close button.

        It returns the value that was returned by the dialog box's runModalLoop() call.

        To close the dialog programatically, you should call exitModalState (returnValue) on
        the DialogWindow that is created. To find a pointer to this window from your
        contentComponent, you can do something like this:
        @code
        Dialogwindow* dw = contentComponent->findParentComponentOfClass<DialogWindow>();

        if (dw != nullptr)
            dw->exitModalState (1234);
        @endcode

        @param dialogTitle          the dialog box's title
        @param contentComponent     the content component for the dialog box. Make sure
                                    that this has been set to the size you want it to
                                    be before calling this method. The component won't
                                    be deleted by this call, so you can re-use it or delete
                                    it afterwards
        @param componentToCentreAround  if this is non-zero, it indicates a component that
                                    you'd like to show this dialog box in front of. See the
                                    DocumentWindow::centreAroundComponent() method for more
                                    info on this parameter
        @param backgroundColour     a colour to use for the dialog box's background colour
        @param escapeKeyTriggersCloseButton if true, then pressing the escape key will cause the
                                            close button to be triggered
        @param shouldBeResizable    if true, the dialog window has either a resizable border, or
                                    a corner resizer
        @param useBottomRightCornerResizer     if shouldBeResizable is true, this indicates whether
                                    to use a border or corner resizer component. See ResizableWindow::setResizable()
    */
    static int showModalDialog (const String& dialogTitle,
                                Component* contentComponent,
                                Component* componentToCentreAround,
                                const Colour& backgroundColour,
                                bool escapeKeyTriggersCloseButton,
                                bool shouldBeResizable = false,
                                bool useBottomRightCornerResizer = false);
   #endif


protected:
    //==============================================================================
    /** @internal */
    void resized();
    /** @internal */
    bool keyPressed (const KeyPress&);

private:
    bool escapeKeyTriggersCloseButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DialogWindow);
};

#endif   // __JUCE_DIALOGWINDOW_JUCEHEADER__

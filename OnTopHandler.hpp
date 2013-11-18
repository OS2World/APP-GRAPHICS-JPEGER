#ifndef OnTopHandler_HPP
#define OnTopHandler_HPP

#define INCL_WINMESSAGEMGR // required for WinSetVisibleRegionNotify
#include <os2.h>           // OS/2
#include <ihandler.hpp>    // IHandler

/* class OnTopHandler
 *
 * written by St‚phane Charette
 * v1.0  17 August 1998 - initial version
 * v1.0a 18 August 1998 - fixed recursion problem
 * v1.0b 7 September 1998 - fixed redraw problem and
 *             implemented ::frameWindowNeedsUpdate()
 *
 * based in part on code by William Law available at:
 *    http://people.netscape.com/law/samples/index.html
 *
 * PURPOSE:       handle the necessary event to ensure that a window can
 *                remain at the top of the Z-order; as a result, any window
 *                whose messages are handled by this handler will always
 *                remain "on top" of other windows, even when it isn't the
 *                active window
 *
 * REQUIREMENTS:  IBM VisualAge C++ version 3.0+ for OS/2 Warp version 4.0+
 *                (may also work with Warp 3 -- untested)
 *
 * USAGE:         (see the file OnTopHandler.cpp)
 *                ...
 *                IFrameWindow frame;
 *                OnTopHandler handler;
 *                handler.handleEventsFor( &frame );
 *                ...
 *                handler.stopHandlingEventsFor( &frame );
 *                ...
 *
 * NOTE:          Derived classes that need to perform their own actions when
 *                a frame window is about to be covered over can choose to
 *                overload the method:
 *
 *                   virtual Boolean frameWindowNeedsUpdate( IEvent &event )
 *
 *                This method is called when a window needs to have its z-order
 *                raised to remain "on top".  To get the frame window from the
 *                event, use:
 *
 *                   event.dispatchingWindow()
 *
 *                Unless you also take care of screen redraw, you'll most
 *                likely want to return "false" to ensure OS/2 correctly
 *                redraws the screen.
 */
class OnTopHandler : public IHandler
{
   public:

      // start handling events for the specified frame window
      virtual IHandler &handleEventsFor( IWindow *frame )
      {
         // make certain the window is a frame window
         if( frame->isFrameWindow() )
         {
            // start the OCL event handling
            IHandler::handleEventsFor( frame );

            // call the PM function to enable the WM_VRNENABLED message
            WinSetVisibleRegionNotify( frame->handle(), true );
         }
         return *this;
      }

      // stop handling events for the specified frame window
      virtual IHandler &stopHandlingEventsFor( IWindow *frame )
      {
         // make certain the window is a frame window
         if( frame->isFrameWindow() )
         {
            // call the PM function to disable the WM_VRNENABLED message
            WinSetVisibleRegionNotify( frame->handle(), false );

            // stop the OCL event handling
            IHandler::stopHandlingEventsFor( frame );
         }
         return *this;
      }

   protected:

      // handle the event and put the frame window back on top
      virtual Boolean frameWindowNeedsUpdate( IEvent &event )
      {
         // disable event processing (to prevent recursion)
         disable();

         // put the window at the top of the z-order list
         WinSetWindowPos( event.controlWindow()->handle(), HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );

         // re-enable event processing
         enable();

         // to ensure that the screen is properly redrawn, we need to
         // return false, allowing the operating system to take care of
         // redrawing the screen
         return false;
      }

      // handle event processing
      virtual Boolean dispatchHandlerEvent( IEvent &event )
      {
         // assume the event wont be handled
         Boolean result = false;

         // look for the WM_VRNENABLED message
         if( event.eventId() == WM_VRNENABLED )
         {
            // call the method that takes care of the action
            result = frameWindowNeedsUpdate( event );
         }

         event.setResult(result);
         return result;
      }
};

#endif


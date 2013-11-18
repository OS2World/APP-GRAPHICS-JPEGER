#include "jpeger.h"
#include "jpeger_banner.hpp"
#include <ifont.hpp>    // IFont
#include <istattxt.hpp> // IStaticText
#include <istring.hpp>  // IString
#include <isysmenu.hpp> // ISystemMenu
#include <ititle.hpp>   // ITitle


/*****************************************************************************
 *
 *    BannerWindow::BannerWindow (constructor #1)
 *
 * create the banner window, without a titlebar or other controls
 *
 *****************************************************************************/
BannerWindow::BannerWindow() :
   IFrameWindow(  WND_BANNER,
                  IFrameWindow::dialogBackground      |
                  IFrameWindow::dialogBorder          ),
   bitmap(        BM_BANNER, this, this, BM_BANNER, IRectangle(),
                  IBitmapControl::classDefaultStyle   |
                  IBitmapControl::sizeToBitmap        )
{
   // initialize the banner
   init().setAutoDeleteObject().setAutoDestroyWindow();

   // setup handler to bring this window to the top
   OnTopHandler::handleEventsFor( this );

   // setup handler to ignore close event
   ICommandHandler::handleEventsFor( this );

   // since the queue processing has not been started, force a repaint to happen
   show().refresh( IWindow::paintAllImmediate );

   return;
}


/*****************************************************************************
 *
 *    BannerWindow::BannerWindow (constructor #2)
 *
 * create the banner window, including titlebar and system menu
 *
 *****************************************************************************/
BannerWindow::BannerWindow(IWindow *wnd) :
   IFrameWindow(  WND_BANNER,
                  IWindow::desktopWindow(),
                  wnd,
                  IRectangle(),
                  IFrameWindow::dialogBackground      |
                  IFrameWindow::dialogBorder          |
                  IFrameWindow::systemMenu            |
                  IFrameWindow::titleBar              ,
                  JPEGER_TITLE                        ),
   bitmap(        BM_BANNER, this, this, BM_BANNER, IRectangle(),
                  IBitmapControl::classDefaultStyle   |
                  IBitmapControl::sizeToBitmap        )
{
   // this is being called as a window instead of a banner -- add a text control
   IStaticText *text = new IStaticText(0, this, this);
   text->
      setText(IString(JPEGER_NAME)+" ("+JPEGER_EMAIL+")").
      setAlignment(IStaticText::centerCenter).
      setFont(IFont("Helv", 8)).
      setForegroundColor(IColor::black).
      setBackgroundColor(BACK_COLOUR).
      setAutoDeleteObject();
   addExtension( text, IFrameWindow::belowClient, IFrameWindow::none );

   // initialize the banner
   init();

   return;
}



/*****************************************************************************
 *
 *    BannerWindow::~BannerWindow
 *
 * emtpy destructor
 *
 *****************************************************************************/
BannerWindow::~BannerWindow()
{
   return;
}


/*****************************************************************************
 *
 *    BannerWindow::init
 *
 * initialize the banner window, common to both constructors
 *
 *****************************************************************************/
BannerWindow &BannerWindow::init()
{
   // place the bitmap in the center of the screen
   bitmap.moveTo( IPoint(
      (IWindow::desktopWindow()->rect().width() - bitmap.rect().width()) / 2,
      (IWindow::desktopWindow()->rect().height() - bitmap.rect().height()) / 2 ) );

   // setup the banner frame window around the bitmap
   moveSizeToClient(bitmap.rect());
   setClient( &bitmap );

   return *this;
}


/*****************************************************************************
 *
 *    BannerWindow::systemCommand
 *
 * handle system command events; inherited from ICommandHandler
 *
 *****************************************************************************/
Boolean BannerWindow::systemCommand( ICommandEvent &event )
{
   Boolean result = false;

   if( event.commandId() == ISystemMenu::idClose )
   {
      // ignore the "close" event
      result = true;
   }

   event.setResult( result );
   return result;
}


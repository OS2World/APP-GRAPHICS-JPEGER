#include "jpeger.h"
#include "jpeger_resize.hpp"
#include "jpeger_main.hpp"
#include <istparse.hpp> // IStringParser
#include <math.h>       // atan2
#include <os2.h>


/*****************************************************************************
 *
 *    ResizeWindow::ResizeWindow
 *
 * create the compression window & start event handling
 *
 *****************************************************************************/
ResizeWindow::ResizeWindow(MainWindow *wnd, IProfile &profile) :
   IFrameWindow(WND_RESIZE, IFrameWindow::dialogResource),

   pbRatio(          PB_RATIO,            this),
   efRatio1(         EF_RATIO_1,          this),
   efRatio2(         EF_RATIO_2,          this),
   rbSizeCustom(     RB_CUSTOM_SIZE,      this),
   rbSizePrevious(   RB_PREVIOUS_SIZE,    this),
   sbSizeX(          SB_SIZE_X,           this),
   sbSizeY(          SB_SIZE_Y,           this),
   ckProportional(   CK_KEEP_PROPORTIONAL,this),
   rbPrevious1(      RB_SIZE_1,           this),
   rbPrevious2(      RB_SIZE_2,           this),
   rbPrevious3(      RB_SIZE_3,           this),
   flyText(          0,                   this),
   flyOverHelpHandler(&flyText, 100, 500),
   wnd(              wnd),
   profile(          profile),
   ratio(            true)
{
   setOwner(wnd).setAutoDeleteObject();

   // setup the spinbutton range
   sbSizeX.setRange(IRange(1, 9999)).setLimit(4);
   sbSizeY.setRange(IRange(1, 9999)).setLimit(4);

   // setup event handling
   ISelectHandler::handleEventsFor(&ckProportional);
   ISelectHandler::handleEventsFor(&rbPrevious1);
   ISelectHandler::handleEventsFor(&rbPrevious2);
   ISelectHandler::handleEventsFor(&rbPrevious3);
   ISelectHandler::handleEventsFor(&rbSizeCustom);
   ISelectHandler::handleEventsFor(&rbSizePrevious);
   IFocusHandler::handleEventsFor(&sbSizeX);
   IFocusHandler::handleEventsFor(&sbSizeY);
   ISpinHandler::handleEventsFor(&sbSizeX);
   ISpinHandler::handleEventsFor(&sbSizeY);
   ICommandHandler::handleEventsFor(this);
   flyOverHelpHandler.handleEventsFor(this);

   // read the settings from the .INI file
   readSettings();

   return;
}


/*****************************************************************************
 *
 *    ResizeWindow::~ResizeWindow
 *
 * destructor; stop event handling
 *
 *****************************************************************************/
ResizeWindow::~ResizeWindow()
{
   // stop event handling
   ISelectHandler::stopHandlingEventsFor(&ckProportional);
   ISelectHandler::stopHandlingEventsFor(&rbPrevious1);
   ISelectHandler::stopHandlingEventsFor(&rbPrevious2);
   ISelectHandler::stopHandlingEventsFor(&rbPrevious3);
   ISelectHandler::stopHandlingEventsFor(&rbSizeCustom);
   ISelectHandler::stopHandlingEventsFor(&rbSizePrevious);
   IFocusHandler::stopHandlingEventsFor(&sbSizeX);
   IFocusHandler::stopHandlingEventsFor(&sbSizeY);
   ISpinHandler::stopHandlingEventsFor(&sbSizeX);
   ISpinHandler::stopHandlingEventsFor(&sbSizeY);
   ICommandHandler::stopHandlingEventsFor(this);
   flyOverHelpHandler.stopHandlingEventsFor(this);
   return;
}


/*****************************************************************************
 *
 *    ResizeWindow::command
 *
 * inherited from ICommandHandler; handle command events
 *
 *****************************************************************************/
Boolean ResizeWindow::command( ICommandEvent &event )
{
   Boolean result = false;  // default indicates that event has not been handled

   switch( event.commandId() )
   {
      case DID_OK:
      {
         // apply the current settings
         saveSettings();
         result = true;
         dismiss(DID_OK);
         break;
      }
      case DID_CANCEL:
      {
         result = true;
         dismiss(DID_CANCEL);
         break;
      }
      case PB_RATIO:
      {
         if( ratio )
         {
            ratio = false;
            pbRatio.setText( "~Angle" );
            efRatio1.setText(calcAngle(wnd->img1->size()));
            efRatio2.setText(calcAngle(getNewSize()));
            result = true;
         }
         else
         {
            ratio = true;
            pbRatio.setText( "R~atio" );
            efRatio1.setText(calcRatio(wnd->img1->size()));
            efRatio2.setText(calcRatio(getNewSize()));
            result = true;
         }
         break;
      }
   }

   event.setResult( result );
   return result;
}



/*****************************************************************************
 *
 *    ResizeWindow::selected
 *
 * inherited from ISelectHandler; handle control selection events
 *
 *****************************************************************************/
Boolean ResizeWindow::selected( IControlEvent &event )
{
   validateControls(event.controlId());

   // indicate the event was handled
   event.setResult(true);
   return true;
}


/*****************************************************************************
 *
 *    ResizeWindow::gotFocus
 *
 * inherited from IFocusHandler; handle focus events
 *
 *****************************************************************************/
Boolean ResizeWindow::gotFocus( IControlEvent &event )
{
   validateControls(event.controlId());

   // indicate the event was handled
   event.setResult(true);
   return true;
}


/*****************************************************************************
 *
 *    ResizeWindow::lostFocus
 *
 * inherited from IFocusHandler; handle focus events
 *
 *****************************************************************************/
Boolean ResizeWindow::lostFocus( IControlEvent &event )
{
   validateControls(0);

   // indicate the event was handled
   event.setResult(true);
   return true;
}


/*****************************************************************************
 *
 *    ResizeWindow::spinEnded
 *
 * inherited from ISpinHandler; handle spinbutton events
 *
 *****************************************************************************/
Boolean ResizeWindow::spinEnded( IControlEvent &event )
{
   validateControls(event.controlId());

   // indicate the event was handled
   event.setResult(true);
   return true;
}


/*****************************************************************************
 *
 *    ResizeWindow::validateControls
 *
 * validate the state of controls
 *
 *****************************************************************************/
ResizeWindow &ResizeWindow::validateControls(const int controlId)
{
   switch( controlId )
   {
      case SB_SIZE_X:
      case SB_SIZE_Y:
      case CK_KEEP_PROPORTIONAL:
      {
         rbSizeCustom.select();
         break;
      }
      case RB_SIZE_1:
      case RB_SIZE_2:
      case RB_SIZE_3:
      {
         rbSizePrevious.select();
         break;
      }
   }

   if( ratio )
   {
      efRatio2.setText(calcRatio(getNewSize()));
   }
   else
   {
      efRatio2.setText(calcAngle(getNewSize()));
   }

   return *this;
}


/*****************************************************************************
 *
 *    ResizeWindow::readSettings
 *
 * read settings from the .INI file
 *
 *****************************************************************************/
ResizeWindow &ResizeWindow::readSettings()
{
   profile.setDefaultApplicationName("JPEGER Resize");
   if( profile.containsKeyName("Ratio") )
   {
      ratio = profile.integerWithKey( "Ratio" );
      ckProportional.select( profile.integerWithKey( "Keep Proportional")        );
      rbPrevious1.   setText(IString("~")+profile.elementWithKey( "Previous 1" ) );
      rbPrevious2.   setText(IString("~")+profile.elementWithKey( "Previous 2" ) );
      rbPrevious3.   setText(IString("~")+profile.elementWithKey( "Previous 3" ) );
   }

   // populate the X and Y spinbuttons with the current image size
   sbSizeX.spinTo(wnd->img1->size().width(), true);
   sbSizeY.spinTo(wnd->img1->size().height(), true);

   // if ratio is set to "true", then we don't have to do anything; otherwise,
   // we need to set it to "true" and create a fake click event to reset it
   if( ! ratio )
   {
      ratio = true;
      pbRatio.click();
      efRatio2.setText(calcAngle(getNewSize()));
   }
   else
   {
      // calculate the initial image ratio
      efRatio1.setText(calcRatio(wnd->img1->size()));
      efRatio2.setText(calcRatio(getNewSize()));
   }

   // always start with previous choice #1 selected
   rbPrevious1.click();

   return *this;
}


/*****************************************************************************
 *
 *    ResizeWindow::saveSettings
 *
 * save settings to the .INI file
 *
 *****************************************************************************/
ResizeWindow &ResizeWindow::saveSettings()
{
   profile.setDefaultApplicationName("JPEGER Resize").
      addOrReplaceElementWithKey( "Ratio",               ratio).
      addOrReplaceElementWithKey( "Keep Proportional",   ckProportional.   isSelected()).
      addOrReplaceElementWithKey( "Previous 1",          rbPrevious1.      text()).
      addOrReplaceElementWithKey( "Previous 2",          rbPrevious2.      text()).
      addOrReplaceElementWithKey( "Previous 3",          rbPrevious3.      text());

   return *this;
}


/*****************************************************************************
 *
 *    ResizeWindow::calcRatio
 *
 * recursively calculate the ratio from a specified size
 *
 *****************************************************************************/
IString ResizeWindow::calcRatio(const IPair pair) const
{
   IString result( IString(pair.coord1()) + ":" + IString(pair.coord2()) );

   for(  int i = (min(pair.coord1(), pair.coord2()) / 2) + 1;
         i > 1;
         i-- )
   {
      if( (pair.coord1() % i == 0) && (pair.coord2() % i == 0) )
      {
         result = calcRatio(pair / i);
         break;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    ResizeWindow::calcAngle
 *
 * calculate the angle between the given X and Y pair values
 *
 *****************************************************************************/
IString ResizeWindow::calcAngle(const IPair pair) const
{
   // calculate the angle
   float radiant = atan2(pair.coord2(), pair.coord1());

   // convert from radiant to degree
   float degrees = radiant * 180.0 / 3.141592654;

   // convert to a string
   IString result( degrees );

   // is there a decimal point?
   if( result.indexOf('.') == 0 )
   {
      // if not, add a decimal point at the end of the string
      result += ".";
   }
   else
   {
      // otherwise, we may need to round the last digit
      IString beforeDecimal;
      IString afterDecimal;
      result >> beforeDecimal >> '.' >> afterDecimal;

      IString decimal( afterDecimal.subString(1,6-beforeDecimal.length(),'0') );

      if(decimal[decimal.length()] >= '5')
      {
         // round out the number!
         int number = decimal.subString(1,5-beforeDecimal.length()).asInt() + 1;
         decimal = IString(number);
      }
      result = beforeDecimal + "." + decimal;
   }

   // change to the selected nationalization decimal point
   result.change(".", IString(wnd->sDecimal));

   // return the string
   return result.subString(1,6,'0');
}


/*****************************************************************************
 *
 *    ResizeWindow::getNewSize
 *
 * calculate the angle between the given X and Y pair values
 *
 *****************************************************************************/
IPair ResizeWindow::getNewSize(void) const
{
   IPair result;

   if( rbSizeCustom.isSelected() )
   {
      result = IPair(sbSizeX.value(), sbSizeY.value());
   }
   else
   {
      IString text;

      if( rbPrevious1.isSelected() )
      {
         text = rbPrevious1.text();
      }
      else if( rbPrevious2.isSelected() )
      {
         text = rbPrevious2.text();
      }
      else if( rbPrevious3.isSelected() )
      {
         text = rbPrevious3.text();
      }

      // convert the text into numeric values
      IString valueX;
      IString valueY;
      text >> IStringParser::skip >> valueX >> 'x' >> valueY;

      result = IPair(valueX.asInt(), valueY.asInt());
   }

   return result;
}


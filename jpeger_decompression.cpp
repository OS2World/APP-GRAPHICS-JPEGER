#include "jpeger.h"
#include "jpeger_decompression.hpp"
#include "jpeger_main.hpp"
#include <os2.h>


/*****************************************************************************
 *
 *    DecompressionWindow::DecompressionWindow
 *
 * create the decompression window & start event handling
 *
 *****************************************************************************/
DecompressionWindow::DecompressionWindow(MainWindow *wnd, IProfile &profile) :
   IFrameWindow(WND_DECOMPRESSION, IFrameWindow::dialogResource),
   rbUse24BitColour(          RB_USE_24_BIT_COLOUR,         this),
   rbQuantizeColours(         RB_QUANTIZE_COLOURS,          this),
   rbUse1PassQuantization(    RB_USE_1_PASS_QUANTIZATION,   this),
   rbUse2PassQuantization(    RB_USE_2_PASS_QUANTIZATION,   this),
   rbNoDithering(             RB_NO_DITHERING,              this),
   rbOrderedDithering(        RB_ORDERED_DITHERING,         this),
   rbFloydSteinbergDithering( RB_FLOYD_STEINBERG_DITHERING, this),
   slNumberOfColours(         SL_NUMBER_OF_COLOURS,         this),
   efNumberOfColours(         EF_NUMBER_OF_COLOURS,         this),
   txtNumberOfColours(        TXT_NUMBER_OF_COLOURS,        this),
   ckFancyUpsampling(         CK_FANCY_UPSAMPLING,          this),
   ckBlockSmoothing(          CK_BLOCK_SMOOTHING,           this),
   flyText(                   0,                            this),
   flyOverHelpHandler(       &flyText, 100, 500),
   wnd(                       wnd),
   profile(                   profile)
{
   setOwner(wnd).setAutoDeleteObject();

   // setup event handling
   ICommandHandler::handleEventsFor(this);
   ISelectHandler::handleEventsFor(this);
   ISliderArmHandler::handleEventsFor(this);
   flyOverHelpHandler.handleEventsFor(this);

   // read and validate the settings from the .INI file
   readSettings().validateControls();

   return;
}


/*****************************************************************************
 *
 *    DecompressionWindow::~DecompressionWindow
 *
 * destructor; stop event handling
 *
 *****************************************************************************/
DecompressionWindow::~DecompressionWindow()
{
   // mark this window as being closed
   wnd->decompressionWindow = (DecompressionWindow*)0;

   // stop event handling
   flyOverHelpHandler.stopHandlingEventsFor(this);
   ISliderArmHandler::stopHandlingEventsFor(this);
   ISelectHandler::stopHandlingEventsFor(this);
   ICommandHandler::stopHandlingEventsFor(this);
}


/*****************************************************************************
 *
 *    DecompressionWindow::command
 *
 * inherited from ICommandHandler; handle command events
 *
 *****************************************************************************/
Boolean DecompressionWindow::command( ICommandEvent &event )
{
   Boolean result = false;  // default indicates that event has not been handled

   switch( event.commandId() )
   {
      case DID_OK:
      {
         // apply the current settings
         saveSettings();
         result = true;
//         close();
         break;
      }
      case DID_CANCEL:
      {
         result = true;
         close();
         break;
      }
   }

   event.setResult( result );
   return result;
}


/*****************************************************************************
 *
 *    DecompressionWindow::selected
 *
 * inherited from ISelectHandler; handle selection events
 *
 *****************************************************************************/
Boolean DecompressionWindow::selected( IControlEvent &event )
{
   Boolean result = false; // default indicates that event has not been handled

   switch( event.controlId() )
   {
      case RB_USE_24_BIT_COLOUR:
      case RB_QUANTIZE_COLOURS:
      {
         validateControls();
         result = true;
         break;
      }
   }

   event.setResult( result );
   return result;
}


/*****************************************************************************
 *
 *    DecompressionWindow::positionChanged
 *
 * inherited from ISliderArmHandler; handle slider arm position events
 *
 *****************************************************************************/
Boolean DecompressionWindow::positionChanged( IControlEvent &event )
{
   Boolean result = false; // default indicates that event has not been handled

   switch( event.controlId() )
   {
      case SL_NUMBER_OF_COLOURS:
      {
         efNumberOfColours.setText( IString(slNumberOfColours.armTickOffset()+1) );
         result = true;
         break;
      }
   }

   event.setResult( result );
   return result;
}


/*****************************************************************************
 *
 *    DecompressionWindow::validateControls
 *
 * validate various controls
 *
 *****************************************************************************/
DecompressionWindow &DecompressionWindow::validateControls()
{
   // is the "Quantization" radio button selected?
   Boolean quantization_state = rbQuantizeColours.isSelected();

   // enable/disable some controls depending on "Quantization"
   rbUse1PassQuantization.    enable( quantization_state );
   rbUse2PassQuantization.    enable( quantization_state );
   rbNoDithering.             enable( quantization_state );
   rbOrderedDithering.        enable( quantization_state );
   rbFloydSteinbergDithering. enable( quantization_state );
   slNumberOfColours.         enable( quantization_state );
   efNumberOfColours.         enable( quantization_state );
   txtNumberOfColours.        enable( quantization_state );

   efNumberOfColours.setText( IString(slNumberOfColours.armTickOffset()+1) );

   return *this;
}


/*****************************************************************************
 *
 *    DecompressionWindow::readSettings
 *
 * read settings from the .INI file
 *
 *****************************************************************************/
DecompressionWindow &DecompressionWindow::readSettings()
{
   profile.setDefaultApplicationName("JPEGER Decompression");
   if( profile.containsKeyName("24-bit colour") )
   {
      rbUse24BitColour.          select( profile.integerWithKey( "24-bit colour"             ));
      rbQuantizeColours.         select( profile.integerWithKey( "Quantize colours"          ));
      slNumberOfColours.  moveArmToTick( profile.integerWithKey( "Number of colours"         ));
      rbUse1PassQuantization.    select( profile.integerWithKey( "1-pass quantization"       ));
      rbUse2PassQuantization.    select( profile.integerWithKey( "2-pass quantization"       ));
      rbNoDithering.             select( profile.integerWithKey( "No dithering"              ));
      rbOrderedDithering.        select( profile.integerWithKey( "Ordered dithering"         ));
      rbFloydSteinbergDithering. select( profile.integerWithKey( "Floyd-Steinberg dithering" ));
      ckFancyUpsampling.         select( profile.integerWithKey( "Fancy upsampling"          ));
      ckBlockSmoothing.          select( profile.integerWithKey( "Block smoothing"           ));
   }
   else
   {
      // the .INI file doesn't exist, so use default values
      rbUse24BitColour.          select();
      slNumberOfColours.         moveArmToTick(255);
      rbUse2PassQuantization.    select();
      rbFloydSteinbergDithering. select();
      ckFancyUpsampling.         select();
      ckBlockSmoothing.          select();
   }

   return *this;
}


/*****************************************************************************
 *
 *    DecompressionWindow::saveSettings
 *
 * save settings to the .INI file
 *
 *****************************************************************************/
DecompressionWindow &DecompressionWindow::saveSettings()
{
   profile.setDefaultApplicationName("JPEGER Decompression").
      addOrReplaceElementWithKey( "24-bit colour", rbUse24BitColour.isSelected() ).
      addOrReplaceElementWithKey( "Quantize colours", rbQuantizeColours.isSelected() ).
      addOrReplaceElementWithKey( "Number of colours", slNumberOfColours.armTickOffset() ).
      addOrReplaceElementWithKey( "1-pass quantization", rbUse1PassQuantization.isSelected() ).
      addOrReplaceElementWithKey( "2-pass quantization", rbUse2PassQuantization.isSelected() ).
      addOrReplaceElementWithKey( "No dithering", rbNoDithering.isSelected() ).
      addOrReplaceElementWithKey( "Ordered dithering", rbOrderedDithering.isSelected() ).
      addOrReplaceElementWithKey( "Floyd-Steinberg dithering", rbFloydSteinbergDithering.isSelected() ).
      addOrReplaceElementWithKey( "Fancy upsampling", ckFancyUpsampling.isSelected() ).
      addOrReplaceElementWithKey( "Block smoothing", ckBlockSmoothing.isSelected() );

   return *this;
}


#include "jpeger.h"
#include "jpeger_compression.hpp"
#include "jpeger_main.hpp"
#include <os2.h>


/*****************************************************************************
 *
 *    CompressionWindow::CompressionWindow
 *
 * create the compression window & start event handling
 *
 *****************************************************************************/
CompressionWindow::CompressionWindow(MainWindow *wnd, IProfile &profile) :
   IFrameWindow(WND_COMPRESSION, IFrameWindow::dialogResource),
   rbCompressSlowInteger(  RB_COMPRESS_SLOW_INTEGER,  this),
   rbCompressFastInteger(  RB_COMPRESS_FAST_INTEGER,  this),
   rbCompressFloat(        RB_COMPRESS_FLOAT,         this),
   ckHuffmanOptimization(  CK_HUFFMAN_OPTIMIZATION,   this),
   slQuality(              SL_QUALITY,                this),
   efQuality(              EF_QUALITY,                this),
   slSmoothing(            SL_SMOOTHING,              this),
   efSmoothing(            EF_SMOOTHING,              this),
   ckBaseline(             CK_FORCE_BASELINE,         this),
   ckProgressive(          CK_PROGRESSIVE_JPEG,       this),
   flyText(                0,                         this),
   flyOverHelpHandler(    &flyText, 100, 500),
   wnd(                    wnd),
   profile(                profile)
{
   setOwner(wnd).setAutoDeleteObject();

   // setup event handling
   ICommandHandler::handleEventsFor(this);
   ISliderArmHandler::handleEventsFor(this);
   flyOverHelpHandler.handleEventsFor(this);

   // read and validate the settings from the .INI file
   readSettings().validateControls();

   return;
}


/*****************************************************************************
 *
 *    CompressionWindow::~CompressionWindow
 *
 * destructor; stop event handling
 *
 *****************************************************************************/
CompressionWindow::~CompressionWindow()
{
   // mark this window as being closed
   wnd->compressionWindow = (CompressionWindow*)0;

   // stop event handling
   flyOverHelpHandler.stopHandlingEventsFor(this);
   ISliderArmHandler::stopHandlingEventsFor(this);
   ICommandHandler::stopHandlingEventsFor(this);
}


/*****************************************************************************
 *
 *    CompressionWindow::command
 *
 * inherited from ICommandHandler; handle command events
 *
 *****************************************************************************/
Boolean CompressionWindow::command( ICommandEvent &event )
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
 *    CompressionWindow::positionChanged
 *
 * inherited from ISliderArmHandler; handle slider arm position events
 *
 *****************************************************************************/
Boolean CompressionWindow::positionChanged( IControlEvent &event )
{
   Boolean result = false; // default indicates that event has not been handled

   switch( event.controlId() )
   {
      case SL_QUALITY:
      case SL_SMOOTHING:
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
 *    DecompressionWindow::validateControls
 *
 * validate various controls
 *
 *****************************************************************************/
CompressionWindow &CompressionWindow::validateControls()
{
   efQuality.setText( IString(slQuality.armTickOffset()) );
   efSmoothing.setText( IString(slSmoothing.armTickOffset()) );
   ckBaseline.enable( slQuality.armTickOffset() < 25 );

   return *this;
}


/*****************************************************************************
 *
 *    CompressionWindow::readSettings
 *
 * read settings from the .INI file
 *
 *****************************************************************************/
CompressionWindow &CompressionWindow::readSettings()
{
   profile.setDefaultApplicationName("JPEGER Compression");
   if( profile.containsKeyName("Compress slow integer") )
   {
      rbCompressSlowInteger.  select(  profile.integerWithKey( "Compress slow integer" ) );
      rbCompressFastInteger.  select(  profile.integerWithKey( "Compress fast integer" ) );
      rbCompressFloat.        select(  profile.integerWithKey( "Compress float"        ) );
      ckHuffmanOptimization.  select(  profile.integerWithKey( "Huffman optimization"  ) );
      slQuality.       moveArmToTick(  profile.integerWithKey( "Quality"               ) );
      slSmoothing.     moveArmToTick(  profile.integerWithKey( "Smoothing"             ) );
      ckBaseline.             select(  profile.integerWithKey( "Baseline"              ) );
      ckProgressive.          select(  profile.integerWithKey( "Progressive"           ) );
   }
   else
   {
      // the .INI file doesn't yet exist, so use the default values
      rbCompressFloat.        select();
      ckHuffmanOptimization.  select();
      slQuality.              moveArmToTick(75);
      ckProgressive.          select();
   }

   return *this;
}


/*****************************************************************************
 *
 *    CompressionWindow::saveSettings
 *
 * save settings to the .INI file
 *
 *****************************************************************************/
CompressionWindow &CompressionWindow::saveSettings()
{
   profile.setDefaultApplicationName("JPEGER Compression").
      addOrReplaceElementWithKey( "Compress slow integer",  rbCompressSlowInteger.  isSelected()).
      addOrReplaceElementWithKey( "Compress fast integer",  rbCompressFastInteger.  isSelected()).
      addOrReplaceElementWithKey( "Compress float",         rbCompressFloat.        isSelected()).
      addOrReplaceElementWithKey( "Huffman optimization",   ckHuffmanOptimization.  isSelected()).
      addOrReplaceElementWithKey( "Quality",                slQuality.              armTickOffset()).
      addOrReplaceElementWithKey( "Smoothing",              slSmoothing.            armTickOffset()).
      addOrReplaceElementWithKey( "Baseline",               ckBaseline.             isSelected()).
      addOrReplaceElementWithKey( "Progressive",            ckProgressive.          isSelected());

   return *this;
}


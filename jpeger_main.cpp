// OS/2 definitions
#define INCL_DOSFILEMGR
#define INCL_GPIBITMAPS
#define INCL_WINERRORS
#define INCL_WINSHELLDATA

// local includes
#include "jpeger.h"
#include "jpeger_main.hpp"
#include "jpeger_banner.hpp"
#include "jpeger_compression.hpp"
#include "jpeger_decompression.hpp"
#include "jpeger_colourcube.hpp"
#include "jpeger_resize.hpp"

// global includes
#include <memory.h>
#include <stdio.h>
#include <jpeglib.h>    // requires STDIO.H to be included first
#include <os2.h>

// OCL includes
#include <iapp.hpp>
#include <iexcbase.hpp> // IException
#include <ifiledlg.hpp> // IFileDialog
#include <imsgbox.hpp>  // IMessageBox
#include <ipopmenu.hpp> // IPopUpMenu
#include <isysmenu.hpp> // ISystemMenu
#include <ithread.hpp>  // IThread
#include <itime.hpp>    // ITime


/*****************************************************************************
 *
 *    main
 *
 * start the application and catch OCL exceptions
 *
 *****************************************************************************/
int main()
{
   int result = 0;
   try
   {
      // load the banner window
      BannerWindow *banner = new BannerWindow();

      // start the main window
      MainWindow *mainWindow = new MainWindow();
      mainWindow->show();

      // see if any "special" windows should be started
      if( ITime::now().seconds() == 25 )
      {
         // BTW, I chose "25" only because that was my age when I wrote this!
         IFrameWindow *moodies = new IFrameWindow( WND_MOODIES, IWindow::desktopWindow(), mainWindow, IFrameWindow::dialogResource );
         moodies->setAutoDeleteObject().setAutoDestroyWindow().show().setFocus();
      }
      else
      {
         mainWindow->setFocus();
      }

      // close the banner and start the event queue processing
      banner->close();
      IApplication::current().run();
   }
   catch( IException &exception )
   {
      IMessageBox msgbox(IWindow::desktopWindow());
      msgbox.
         setTitle("JPEGER - Exception Error!").
         show( exception.text(),
               IMessageBox::okButton      |
               IMessageBox::defButton1    |
               IMessageBox::errorIcon     |
               IMessageBox::moveable      |
               IMessageBox::systemModal   );
      result = -1;
   }
   return result;
}


/*****************************************************************************
 *
 *    MainWindow::MainWindow
 *
 * create the main window & setup the various canvases
 *
 *****************************************************************************/
MainWindow::MainWindow() :
   IFrameWindow(     JPEGER_TITLE, WND_MAIN,
                     IFrameWindow::classDefaultStyle  |
                     IFrameWindow::accelerator        |
                     IFrameWindow::animated           |
                     IFrameWindow::dialogBackground   |
                     IFrameWindow::menuBar            |
                     IFrameWindow::minimizedIcon      ),
   title(            this              ),
   flyText(          0, this           ),
   flyHelpHandler(  &flyText, 100, 500 ),
   profile(          "jpeger.ini"      ),

   // create the main window controls
   menubar(          this, IMenuBar::wrapper ),
   split(            0, this, this ),
   vp1(              0, &split, &split ),
   vp2(              0, &split, &split ),
   dc1(              0, &vp1, &vp1 ),
   dc2(              0, &vp2, &vp2 ),
   gl1(              ),
   gl2(              ),

   // create the controls used in the status bar
   status(           0, this, this ),
   bytes1(           EF_BYTES_1, &status, &status, IRectangle(),
                     IEntryField::classDefaultStyle   |
                     IEntryField::centerAlign         |
                     IEntryField::readOnly            &
                    ~IEntryField::leftAlign           ),
   colours1(         EF_COLOURS_1, &status, &status, IRectangle(),
                     IEntryField::classDefaultStyle   |
                     IEntryField::centerAlign         |
                     IEntryField::readOnly            &
                    ~IEntryField::leftAlign           ),
   size1(            EF_SIZE_1, &status, &status, IRectangle(),
                     IEntryField::classDefaultStyle   |
                     IEntryField::centerAlign         |
                     IEntryField::readOnly            &
                    ~IEntryField::leftAlign           ),
   bytes2(           EF_BYTES_2, &status, &status, IRectangle(),
                     IEntryField::classDefaultStyle   |
                     IEntryField::centerAlign         |
                     IEntryField::readOnly            &
                    ~IEntryField::leftAlign           ),
   colours2(         EF_COLOURS_2, &status, &status, IRectangle(),
                     IEntryField::classDefaultStyle   |
                     IEntryField::centerAlign         |
                     IEntryField::readOnly            &
                    ~IEntryField::leftAlign           ),
   size2(            EF_SIZE_2, &status, &status, IRectangle(),
                     IEntryField::classDefaultStyle   |
                     IEntryField::centerAlign         |
                     IEntryField::readOnly            &
                    ~IEntryField::leftAlign           )
{
   // delete everything if something is closed or fails
   setAutoDeleteObject().setAutoDestroyWindow();

   // setup some instance variables
   compressionWindow = (CompressionWindow*)0;
   decompressionWindow = (DecompressionWindow*)0;
   img1 = (IGBitmap*)0;
   img2 = (IGBitmap*)0;
   loadDirectory = "";
   saveDirectory = "";
   sourceFilename = "";
   filemask = "*.JPG;*.BMP;*.RLE;*.RL4;*.RL8;*.VGA;*.GIF;*.PCX;*.TIF;*.TGA;*.IFF;*.LBM;*.XBM";
   imageLoaded = false;

   // setup event handling
   IMenuHandler::handleEventsFor(&dc1);
   IMenuHandler::handleEventsFor(&dc2);
   ICommandHandler::handleEventsFor(this);
   flyHelpHandler.handleEventsFor(this);

   // setup the main window
   dc1.setGraphicList(&gl1);
   dc2.setGraphicList(&gl2);
   vp1.setBackgroundColor(IColor::black);
   vp2.setBackgroundColor(IColor::black);
   split.setSplitBarMiddleColor( BACK_COLOUR  );

   // setup the status bar
   status.  setRowHeight( 1, 1, false ).
            setRowHeight( 2, 0, false ).
            setColumnWidth( 1, 0, false ).
            setColumnWidth( 2, 0, false ).
            setColumnWidth( 3, 0, false ).
            setColumnWidth( 4, 5, true ).
            setColumnWidth( 5, 0, false ).
            setColumnWidth( 6, 0, false ).
            setColumnWidth( 7, 0, false ).
            addToCell( &bytes1,  1, 2, 1, 1 ).
            addToCell( &colours1,2, 2, 1, 1 ).
            addToCell( &size1,   3, 2, 1, 1 ).
            addToCell( &bytes2,  5, 2, 1, 1 ).
            addToCell( &colours2,6, 2, 1, 1 ).
            addToCell( &size2,   7, 2, 1, 1 );

   bytes1.  setLimit(9).
            setForegroundColor(IColor::black).
            setBackgroundColor(BACK_COLOUR).
            setHiliteForegroundColor(IColor::black).
            setHiliteBackgroundColor(BACK_COLOUR);
   colours1.setLimit(10).
            setForegroundColor(IColor::black).
            setBackgroundColor(BACK_COLOUR).
            setHiliteForegroundColor(IColor::black).
            setHiliteBackgroundColor(BACK_COLOUR);
   size1.   setLimit(9).
            setForegroundColor(IColor::black).
            setBackgroundColor(BACK_COLOUR).
            setHiliteForegroundColor(IColor::black).
            setHiliteBackgroundColor(BACK_COLOUR);
   bytes2.  setLimit(9).
            setForegroundColor(IColor::black).
            setBackgroundColor(BACK_COLOUR).
            setHiliteForegroundColor(IColor::black).
            setHiliteBackgroundColor(BACK_COLOUR);
   colours2.setLimit(10).
            setForegroundColor(IColor::black).
            setBackgroundColor(BACK_COLOUR).
            setHiliteForegroundColor(IColor::black).
            setHiliteBackgroundColor(BACK_COLOUR);
   size2.   setLimit(9).
            setForegroundColor(IColor::black).
            setBackgroundColor(BACK_COLOUR).
            setHiliteForegroundColor(IColor::black).
            setHiliteBackgroundColor(BACK_COLOUR);

   validateMenubar();

   // set the client windows
   setClient( &split );
   addExtension( &status, IFrameWindow::belowClient, IFrameWindow::thinLine );
   readSettings();

   // get some system variables:
   ULONG size = 1;

   // character used to separate the thousands
   PrfQueryProfileData(HINI_USERPROFILE, "PM_National", "sThousand", &sThousand, &size );

   // character used as a decimal point
   PrfQueryProfileData(HINI_USERPROFILE, "PM_National", "sDecimal", &sDecimal, &size );

   // character used as a list separator
   PrfQueryProfileData(HINI_USERPROFILE, "PM_National", "sList", &sList, &size );

   return;
}


/*****************************************************************************
 *
 *    MainWindow::~MainWindow
 *
 * stop event handling during the destruction
 *
 *****************************************************************************/
MainWindow::~MainWindow()
{
   // stop event handling
   flyHelpHandler.stopHandlingEventsFor(this);
   ICommandHandler::stopHandlingEventsFor(this);
   IMenuHandler::stopHandlingEventsFor(&dc1);
   IMenuHandler::stopHandlingEventsFor(&dc2);

   return;
}


/*****************************************************************************
 *
 *    MainWindow::systemCommand
 *
 * inherited from ICommandHandler; handle system commands
 *
 *****************************************************************************/
Boolean MainWindow::systemCommand( ICommandEvent &event )
{
   if( event.commandId() == ISystemMenu::idClose )
   {
      // save the settings before the window is closed
      saveSettings();
   }

   event.setResult( false );
   return false;
}


/*****************************************************************************
 *
 *    MainWindow::command
 *
 * inherited from ICommandHandler; handle commands
 *
 *****************************************************************************/
Boolean MainWindow::command( ICommandEvent &event )
{
   Boolean result = false;  // default indicates that event has not been handled

   switch( event.commandId() )
   {
      case MI_OPEN:
      {
         // open the image on a new thread
         IThread( new IThreadMemberFn<MainWindow>(*this, openImage) );
         result = true;
         break;
      }
      case MI_CLOSE:
      {
         closeImage();
         validateMenubar();
         dc1.setLayoutDistorted( IWindow::minimumSizeChanged |
                                 IWindow::childMinimumSizeChanged |
                                 IWindow::immediateUpdate, 0);
         result = true;
         break;
      }
      case MI_VERTICAL:
      {
         showVertical();
         result = true;
         break;
      }
      case MI_HORIZONTAL:
      {
         showHorizontal();
         result = true;
         break;
      }
      case MI_SOURCE:
      {
         split.setSplitWindowPercentage( &vp1, 100 );
         split.setSplitWindowPercentage( &vp2, 0 );
         split.refresh();
         result = true;
         break;
      }
      case MI_DESTINATION:
      {
         split.setSplitWindowPercentage( &vp1, 0 );
         split.setSplitWindowPercentage( &vp2, 100 );
         split.refresh();
         result = true;
         break;
      }
      case MI_ROTATE_90:
      {
         img1->rotateBy90();
         dc1.setLayoutDistorted( IWindow::minimumSizeChanged |
                                 IWindow::childMinimumSizeChanged |
                                 IWindow::immediateUpdate, 0);
         size1.setText(IString(img1->size().width())+"x"+IString(img1->size().height()));
         result = true;
         break;
      }
      case MI_FLIP_HORIZONTAL:
      {
         img1->reflectHorizontally();
         dc1.refresh();
         result = true;
         break;
      }
      case MI_FLIP_VERTICAL:
      {
         img1->reflectVertically();
         dc1.refresh();
         result = true;
         break;
      }
      case MI_TRANSPOSE:
      {
         img1->transposeXForY();
         dc1.setLayoutDistorted( IWindow::minimumSizeChanged |
                                 IWindow::childMinimumSizeChanged |
                                 IWindow::immediateUpdate, 0);
         size1.setText(IString(img1->size().width())+"x"+IString(img1->size().height()));
         result = true;
         break;
      }
      case MI_COMPRESS_OPTIONS:
      {
         // if the compression window does not yet exist...
         if(!compressionWindow)
         {
            // ...then create it...
            compressionWindow = new CompressionWindow(this, profile);
         }
         else
         {
            // ...otherwise, re-center the window at the current mouse coordinates
            compressionWindow->moveTo( IWindow::pointerPosition() - (compressionWindow->rect().size()/2) );
         }
         compressionWindow->show().setFocus();
         result = true;
         break;
      }
      case MI_DECOMPRESS_OPTIONS:
      {
         // if the decompression window does not yet exist...
         if(!decompressionWindow)
         {
            // ...then create it...
            decompressionWindow = new DecompressionWindow(this, profile);
         }
         else
         {
            // ...otherwise, re-center the window at the current mouse coordinates
            decompressionWindow->moveTo( IWindow::pointerPosition() - (decompressionWindow->rect().size()/2) );
         }
         decompressionWindow->show().setFocus();
         result = true;
         break;
      }
      case MI_RESIZE:
      {
         ResizeWindow resizeWindow(this, profile);
         resizeWindow.showModally();
         if( resizeWindow.result() == DID_OK )
         {
            img2->sizeTo(resizeWindow.getNewSize());
            dc2.setLayoutDistorted( IWindow::minimumSizeChanged |
                                    IWindow::childMinimumSizeChanged |
                                    IWindow::immediateUpdate, 0);
         }
         result = true;
         break;
      }
      case MI_ABOUT:
      {
         BannerWindow banner(this);
         banner.setFocus();
         banner.showModally();
         result = true;
         break;
      }
   }

   event.setResult( result );
   return result;
}



/*****************************************************************************
 *
 *    MainWindow::makePopUpMenu
 *
 * inherited from IMenuHandler; handle requests for popup menus
 *
 *****************************************************************************/
Boolean MainWindow::makePopUpMenu( IMenuEvent &event )
{
   // create the popup menu
   IPopUpMenu *menu = new IPopUpMenu( this, 0 );
   menu->   addText( MI_COMPRESS_OPTIONS, "JPEG ~compression options" ).
            addText( MI_DECOMPRESS_OPTIONS, "JPEG ~decompression options" ).
            setAutoDeleteObject(true);
   // show the popup menu
   menu->show( IWindow::pointerPosition() - this->position() );

   event.setResult( true );
   return true;
}


/*****************************************************************************
 *
 *    MainWindow::showVertical
 *
 * set the split canvas to vertical view (split bar set as top-to-bottom)
 *
 *****************************************************************************/
MainWindow &MainWindow::showVertical( const Boolean isVertical )
{
   if( ! isVertical )
   {
      return showHorizontal();
   }
   split.setOrientation(ISplitCanvas::verticalSplit);
   menubar.checkItem(MI_VERTICAL, true).checkItem(MI_HORIZONTAL, false);
   return *this;
}


/*****************************************************************************
 *
 *    MainWindow::showHorizontal
 *
 * set the split canvas to horizontal view (split bar set as left-to-right)
 *
 *****************************************************************************/
MainWindow &MainWindow::showHorizontal( const Boolean isHorizontal )
{
   if( ! isHorizontal )
   {
      return showVertical();
   }
   split.setOrientation(ISplitCanvas::horizontalSplit);
   menubar.checkItem(MI_VERTICAL, false).checkItem(MI_HORIZONTAL, true);
   return *this;
}


/*****************************************************************************
 *
 *    MainWindow::readSettings
 *
 * read settings from the .INI file
 *
 *****************************************************************************/
MainWindow &MainWindow::readSettings()
{
   profile.setDefaultApplicationName("JPEGER Main Window");
   if( profile.containsKeyName("x1") )
   {
      moveSizeTo( IRectangle( profile.integerWithKey("x1"),
                              profile.integerWithKey("y1"),
                              profile.integerWithKey("x2"),
                              profile.integerWithKey("y2") ) );
      showVertical( profile.integerWithKey("vertical") );
      split.setSplitWindowPercentage( &vp1, profile.integerWithKey("split size 1") );
      split.setSplitWindowPercentage( &vp2, profile.integerWithKey("split size 2") );
      loadDirectory = profile.elementWithKey("load directory");
      saveDirectory = profile.elementWithKey("save directory");
      filemask = profile.elementWithKey("filemask");
   }

   return *this;
}


/*****************************************************************************
 *
 *    MainWindow::saveSettings
 *
 * save settings to the .INI file
 *
 *****************************************************************************/
MainWindow &MainWindow::saveSettings()
{
   // get the window's frame rectangle
   IRectangle wndRect( isMinimized()||isMaximized() ? restoreRect() : rect() );

   // save the window position & other values
   profile.setDefaultApplicationName("JPEGER Main Window").
      addOrReplaceElementWithKey( "x1", wndRect.minX() ).
      addOrReplaceElementWithKey( "y1", wndRect.minY() ).
      addOrReplaceElementWithKey( "x2", wndRect.maxX() ).
      addOrReplaceElementWithKey( "y2", wndRect.maxY() ).
      addOrReplaceElementWithKey( "split size 1", split.splitWindowPercentage(&vp1) ).
      addOrReplaceElementWithKey( "split size 2", split.splitWindowPercentage(&vp2) ).
      addOrReplaceElementWithKey( "vertical", split.orientation()==ISplitCanvas::verticalSplit ).
      addOrReplaceElementWithKey( "load directory", loadDirectory ).
      addOrReplaceElementWithKey( "save directory", saveDirectory ).
      addOrReplaceElementWithKey( "filemask", filemask );

   return *this;
}


/*****************************************************************************
 *
 *    MainWindow::openImage
 *
 * show and process the "open" (load) image dialog window
 *
 *****************************************************************************/
void MainWindow::openImage()
{
   try
   {
      if( sourceFilename == "" )
      {
         sourceFilename = filemask;
      }
      IFileDialog::Settings settings;
      settings.
         setOKButtonText("Load").
         setTitle("JPEGER - Load Image").
         setFileName(loadDirectory + sourceFilename).
         setOpenDialog();

      // get the name of the file they want to open
      IFileDialog fileDialog(IWindow::desktopWindow(), this, settings);

      if( fileDialog.pressedOK() )
      {
         // disable all menu items
         validateMenubar(false);

         // set the "wait" mouse pointer since this may take a few seconds
         setMousePointer(ISystemPointerHandle(ISystemPointerHandle::wait));

         // get the filename selected in the dialog
         IString filename(fileDialog.fileName());

         // remember the name of the directory
         loadDirectory = IString(filename).remove(filename.lastIndexOf('\\') + 1);

         // is the file in a format supported natively by IGBitmap?
         IGBitmap *bitmap = readJPEGFile(filename);

         // if we weren't successful in reading the file as a JPEG...
         if( ! bitmap )
         {
            try
            {
               // ...then try one of the "built-in" supported formats
               bitmap = new IGBitmap(filename);
            }
            catch( IException &exception )
            {
               IMessageBox msgbox(IWindow::desktopWindow());
               msgbox.
                  setTitle("JPEGER - Error reading image file").
                  show( "JPEGER is unable to load the selected image file.",
                        IMessageBox::okButton      |
                        IMessageBox::defButton1    |
                        IMessageBox::errorIcon     |
                        IMessageBox::moveable      |
                        IMessageBox::systemModal   );
            }

         }

         // did we succeed in getting our IGBitmap?
         if( bitmap )
         {
            // start by closing any existing image views
            closeImage();

            // set the image pointer to point to our new bitmap
            img1 = bitmap;
            img2 = new IGBitmap(*img1, IRectangle(img1->position(), img1->size()));

            // add the image to the IGList
            gl1.addAsFirst( *img1 );
            gl2.addAsFirst( *img2 );

            // set the name of the file into the titlebar
            title.setViewText(filename);

            // get the size of the file
            FILESTATUS3 fileStatus;
            DosQueryPathInfo(filename, FIL_STANDARD, &fileStatus, sizeof(FILESTATUS3));
            bytes1.setText(formatNumber(fileStatus.cbFile));
            bytes2.setText(bytes1.text());

            // save the name of the source file
            sourceFilename = filename.remove(1, filename.lastIndexOf('\\') );

            // set the file dimensions
            size1.setText(IString(img1->size().width())+"x"+IString(img1->size().height()));
            size2.setText(size1.text());

            // mark an image as being loaded
            imageLoaded = true;

            // update the canvas
            dc1.setLayoutDistorted( IWindow::minimumSizeChanged      |
                                    IWindow::childMinimumSizeChanged |
                                    IWindow::immediateUpdate, 0      );

            dc2.setLayoutDistorted( IWindow::minimumSizeChanged      |
                                    IWindow::childMinimumSizeChanged |
                                    IWindow::immediateUpdate, 0      );

            // get the number of colours used
//            IBitmapHandle hbmp( bitmap->handle() );
//            unsigned long colours = findColours(hbmp);
//            colours1.setText(formatNumber(colours));
            colours1.setText("?");
            colours2.setText("?");

         }
         else
         {
            // since the file failed to load, reset the filemask to *.jpg
            sourceFilename = filemask;
         }

         // update the menubar
         validateMenubar();

         // reset to the default mouse pointer
         setMousePointer(IPointerHandle());
      }
   }
   catch( IException &exception )
   {
      IMessageBox msgbox(IWindow::desktopWindow());
      msgbox.
         setTitle("JPEGER - OCL Exception Error! (reader thread)").
         show( exception.text(),
               IMessageBox::okButton      |
               IMessageBox::defButton1    |
               IMessageBox::errorIcon     |
               IMessageBox::moveable      |
               IMessageBox::systemModal   );

      // close down the main window (thereby killing the application)
      close();
   }
   catch( ... )
   {
      IMessageBox msgbox(IWindow::desktopWindow());
      msgbox.
         setTitle("JPEGER - Unkown Exception Error! (reader thread)").
         show( "Unkown exception was caught.",
               IMessageBox::okButton      |
               IMessageBox::defButton1    |
               IMessageBox::errorIcon     |
               IMessageBox::moveable      |
               IMessageBox::systemModal   );

      // close down the main window (thereby killing the application)
      close();
   }

   return;
}


/*****************************************************************************
 *
 *    MainWindow::closeImage
 *
 * close any image views
 *
 *****************************************************************************/
MainWindow &MainWindow::closeImage()
{
   // is there an image in the first glist?
   if( ! gl1.isEmpty() )
   {
      // remove the image from the list
      gl1.removeFirst();

      // delete the image
      delete img1;
   }

   // is there an image in the second glist?
   if( ! gl2.isEmpty() )
   {
      // remove the image from the list
      gl2.removeFirst();

      // delete the image
      delete img2;
   }

   // blank out the status bar
   size1.setText("");
   size2.setText("");
   bytes1.setText("");
   bytes2.setText("");
   colours1.setText("");
   colours2.setText("");

   // reset the titlebar name
   title.setViewText("");

   imageLoaded = false;

   return *this;
}


/*****************************************************************************
 *
 *    MainWindow::validateMenubar
 *
 * validates items on the menubar based on whether or not an image is loaded
 *
 *****************************************************************************/
MainWindow &MainWindow::validateMenubar( const Boolean status )
{
   menubar.
      enableItem(MI_SAVE,           false ).//imageLoaded & status).
      enableItem(MI_SAVE_AS,        false ).//imageLoaded & status).
      enableItem(MI_CLOSE,          imageLoaded & status).
      enableItem(MI_ROTATE_90,      imageLoaded & status).
      enableItem(MI_FLIP_HORIZONTAL,imageLoaded & status).
      enableItem(MI_FLIP_VERTICAL,  imageLoaded & status).
      enableItem(MI_TRANSPOSE,      imageLoaded & status).
      enableItem(MI_CROP,           false ).//imageLoaded & status).
      enableItem(MI_RESIZE,         imageLoaded & status).
      enableItem(MI_SOURCE,         imageLoaded).
      enableItem(MI_DESTINATION,    imageLoaded);

   return *this;
}


/*****************************************************************************
 *
 *    MainWindow::readJPEGFile
 *
 * process the specified JPEG file and return a pointer to an IGBitmap
 *
 *****************************************************************************/
IGBitmap *MainWindow::readJPEGFile( const char *name )
{
   IGBitmap *bitmap = (IGBitmap*)0;
   unsigned char *image = (unsigned char*)0;

   // can we open the file for read access?
   FILE *inputFile = fopen(name, "rb");
   if( ! inputFile )
   {
      IMessageBox msgbox(this);
      msgbox.
         setTitle("JPEGER - File Error!").
         show( IString("The JPEG file \"")+name+"\" cannot be opened for read access.",
               IMessageBox::applicationModal |
               IMessageBox::okButton         |
               IMessageBox::defButton1       |
               IMessageBox::errorIcon        |
               IMessageBox::moveable         );

      // return immediately since we cannot open the file
      return bitmap;
   }

   // now that the file was opened, create the necessary JPEG structures
   struct jpeg_decompress_struct cinfo;
   struct jpeg_error_mgr jerr;
   JSAMPARRAY buffer;
   int row_stride;

   try
   {
      // initialize the JPEG structures
      cinfo.err = jpeg_std_error(&jerr);
      jpeg_create_decompress(&cinfo);
      jpeg_stdio_src(&cinfo, inputFile);
      jpeg_read_header(&cinfo, TRUE);

      // set the decompression settings before calling "jpeg_start_decompress"

      jpeg_start_decompress(&cinfo);

      // the following fields can now be accessed
      //cinfo.output_width;
      //cinfo.output_height;
      //cinfo.out_color_components;
      //cinfo.output_components;
      //cinfo.colormap;
      //cinfo.actual_number_of_colors;

      // allocate a 1-row jpeg work buffer
      row_stride = cinfo.output_width * cinfo.output_components;
      buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

      // get the width of the buffer required to hold the image
      unsigned long rowSize = cinfo.output_width * 3;
      // NOTE: remember that the rows have to be doubleword aligned when we call
      //       the GpiCreateBitmap() function, so some padding may be necessary
      //       (doubleword aligned means it should divide evenly into 4)
      rowSize += rowSize % 4;
      // get the total size of the buffer required to hold the image
      unsigned long imageSize = cinfo.output_height * rowSize;
      // allocate the necessary memory to hold the image
      image = new unsigned char[imageSize];

      //                    __________________
      // imageIndexBase -> |.                 |
      //                   |                  |
      //                   |                  |
      //    row, buffer -> |xxxxxxxxxxxxxxxxxx|
      //                   |                  |
      //          image -> |._________________|
      //
      // NOTE: the imageIndexBase is the start of the row at the physical top of the
      //       bitmap -- this is _NOT_ the same thing as the start of the bitmap since
      //       in OS/2, the bitmaps are from bottom-left-to-top-right!
      //
      unsigned long imageIndexBase = rowSize * (cinfo.output_height - 1);
      unsigned long row = 0;

      // loop through all of the lines (top-to-bottom), copying the RGB info into the "image"
      while( cinfo.output_scanline < cinfo.output_height)
      {
         // get the next JPEG row, decompressed into RGB values
         jpeg_read_scanlines(&cinfo, buffer, 1);

         // copy this entire JPEG-decompressed row into our image buffer

         // NOTE: damn OS/2 bitmaps...they start at the bottom and work their way
         //       up instead of the other way around, so flip each row as we go
         unsigned long imageIndex = imageIndexBase - row * rowSize;
         memcpy( &(image[imageIndex]), buffer[0], row_stride );
         row ++;
      }

      // create a bitmap with the image information we've obtained
      BITMAPINFOHEADER2 bmpHeader = {0};
      ULONG flOptions = CBM_INIT;
      BITMAPINFO2 bmpInfo = {0};

      bmpHeader.cbFix         = sizeof(BITMAPINFOHEADER2);
      bmpHeader.cx            = cinfo.output_width;
      bmpHeader.cy            = cinfo.output_height;
      bmpHeader.cPlanes       = 1;
      bmpHeader.cBitCount     = 24;
      bmpHeader.ulCompression = BCA_UNCOMP;
      bmpHeader.usUnits       = BRU_METRIC;
      bmpHeader.usRecording   = BRA_BOTTOMUP;
      bmpHeader.usRendering   = BRH_NOTHALFTONED;
      bmpHeader.ulColorEncoding=BCE_RGB;

      bmpInfo.cbFix           = sizeof(BITMAPINFO2) - 4;
      bmpInfo.cx              = bmpHeader.cx;
      bmpInfo.cy              = bmpHeader.cy;
      bmpInfo.cPlanes         = bmpHeader.cPlanes;
      bmpInfo.cBitCount       = bmpHeader.cBitCount;
      bmpInfo.ulCompression   = bmpHeader.ulCompression;
      bmpInfo.usUnits         = bmpHeader.usUnits;
      bmpInfo.usRecording     = bmpHeader.usRecording;
      bmpInfo.usRendering     = bmpHeader.usRendering;
      bmpInfo.ulColorEncoding = bmpHeader.ulColorEncoding;

      // get the presentation space
      IPresSpaceHandle ps = this->presSpace();

      // create the bitmap (...and return the bitmap handle)
      HBITMAP hbm = GpiCreateBitmap( ps, &bmpHeader, flOptions, (PBYTE)image, &bmpInfo );

      // release the presentation space
      this->releasePresSpace( ps );

      // were we successful in obtaining a bitmap handle?
      if( ! hbm )
      {
         // show the error
         ERRORID errorid = WinGetLastError( IThread::current().anchorBlock() );
         IMessageBox msgbox(this);
         msgbox.
            setTitle("JPEGER - GpiCreateBitmap Error!").
            show( IString("Error 0x" + IString(errorid).d2x().rightJustify(8, '0') + " was received from GpiQueryBitmapBits()."),
               IMessageBox::applicationModal |
               IMessageBox::okButton         |
               IMessageBox::defButton1       |
               IMessageBox::errorIcon        |
               IMessageBox::moveable         );
      }
      else
      {
         // create a new IGBitmap with the bitmap handle given to us by GpiCreateBitmap
         IBitmapHandle bitmapHandle( hbm );
         bitmap = new IGBitmap( bitmapHandle );
      }

      // finish processing JPEG file
      jpeg_finish_decompress(&cinfo);
   }
   catch( IException &exception )
   {
      // the JPEG file failed to load, so return a NULL IGBitmap
      bitmap = (IGBitmap*)0;
   }

   // release jpeg library memory
   jpeg_destroy_decompress(&cinfo);

   // close the input file
   fclose( inputFile );

   // release the 24-bit memory image
   if( image )
   {
      delete [] image;
   }

   return bitmap;
}


/*****************************************************************************
 *
 *    MainWindow::formatNumber
 *
 * places a seperator between thousands
 *
 *****************************************************************************/
IString MainWindow::formatNumber( const unsigned long number ) const
{
   IString result( number );

   // make certain that a thousands' separator has been specified
   if( sThousand != '\0' )
   {
      // insert the necessary character into the string
      for( int i = result.length() % 3; i <= result.length(); i += 4 )
      {
         result.insert( sThousand, i );
      }
      result.strip(sThousand);
   }

   return result;
}


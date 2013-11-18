#ifndef JPEGER_MAIN_H
#define JPEGER_MAIN_H

#include <icmdhdr.hpp>  // ICommandHandler
#include <idrawcv.hpp>  // IDrawingCanvas
#include <ientryfd.hpp> // IEntryField
#include <iflytext.hpp> // IFlyText
#include <iflyhhdr.hpp> // IFlyOverHelpHandler
#include <iframe.hpp>   // IFrameWindow
#include <igbitmap.hpp> // IGBitmap
#include <imcelcv.hpp>  // IMultiCellCanvas
#include <imenubar.hpp> // IMenuBar
#include <imenuhdr.hpp> // IMenuHandler
#include <iprofile.hpp> // IProfile
#include <isplitcv.hpp> // ISplitCanvas
#include <istattxt.hpp> // IStaticText
#include <istring.hpp>  // IString
#include <ititle.hpp>   // ITitle
#include <ivport.hpp>   // IViewPort


// class prototypes
class MainWindow;
class CompressionWindow;
class DecompressionWindow;
class ResizeWindow;


class MainWindow :
   public IFrameWindow,
   protected ICommandHandler,
   protected IMenuHandler
{
   friend class CompressionWindow;
   friend class DecompressionWindow;
   friend class ResizeWindow;

   public:

      // constructor/destructor
      MainWindow(void);
      virtual ~MainWindow(void);

   protected:

      // inherited methods
      virtual Boolean command( ICommandEvent &event );
      virtual Boolean systemCommand( ICommandEvent &event );
      virtual Boolean makePopUpMenu( IMenuEvent &event );

      // other methods
      MainWindow &showVertical(     const Boolean isVertical=true );
      MainWindow &showHorizontal(   const Boolean isHorizontal=true );
      MainWindow &readSettings(     void);
      MainWindow &saveSettings(     void);
      void        openImage(        void);
      MainWindow &closeImage(       void);
      MainWindow &validateMenubar(  const Boolean status=true );
      IGBitmap   *readJPEGFile(     const char *name );
      IString     formatNumber(     const unsigned long number ) const;

      // instance variables
      ITitle               title;
      IFlyText             flyText;
      IFlyOverHelpHandler  flyHelpHandler;
      IProfile             profile;
      IMenuBar             menubar;
      ISplitCanvas         split;
      IViewPort            vp1;
      IViewPort            vp2;
      IDrawingCanvas       dc1;
      IDrawingCanvas       dc2;
      IGList               gl1;
      IGList               gl2;
      IGBitmap             *img1;
      IGBitmap             *img2;

      IMultiCellCanvas     status;
      IEntryField          bytes1;
      IEntryField          colours1;
      IEntryField          size1;
      IEntryField          bytes2;
      IEntryField          colours2;
      IEntryField          size2;

      IString              loadDirectory;
      IString              saveDirectory;
      IString              sourceFilename;
      IString              filemask;

      CompressionWindow    *compressionWindow;
      DecompressionWindow  *decompressionWindow;

      Boolean              imageLoaded;

      // character used to seperate the thousands
      char                 sThousand;

      // character used as a decimal point
      char                 sDecimal;

      // character used as a list separator
      char                 sList;
};

#endif


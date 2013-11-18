#ifndef JPEGER_COMPRESSION_H
#define JPEGER_COMPRESSION_H

#include <icheckbx.hpp> // ICheckBox
#include <icmdhdr.hpp>  // ICommandHandler
#include <ientryfd.hpp> // IEntryField
#include <iflytext.hpp> // IFlyText
#include <iflyhhdr.hpp> // IFlyOverHelpHandler
#include <iframe.hpp>   // IFrameWindow
#include <iprofile.hpp> // IProfile
#include <iradiobt.hpp> // IRadioButton
#include <isldahdr.hpp> // ISliderArmHandler
#include <islider.hpp>  // ISlider
#include <istattxt.hpp> // IStaticText


// class prototypes
class CompressionWindow;
class MainWindow;


class CompressionWindow :
   public IFrameWindow,
   protected ICommandHandler,
   protected ISliderArmHandler
{
   public:

      // constructor/destructor
      CompressionWindow(MainWindow *wnd, IProfile &profile);
      virtual ~CompressionWindow(void);

   protected:

      // inherited from ICommandHandler
      virtual Boolean command( ICommandEvent &event );

      // inherited from ISliderArmHandler
      virtual Boolean positionChanged( IControlEvent &event );

      // other methods
      virtual CompressionWindow &validateControls(void);
      virtual CompressionWindow &readSettings(void);
      virtual CompressionWindow &saveSettings(void);

      // instance variables
      IFlyText             flyText;
      IFlyOverHelpHandler  flyOverHelpHandler;
      MainWindow          *wnd;
      IProfile            &profile;
      IRadioButton         rbCompressSlowInteger;
      IRadioButton         rbCompressFastInteger;
      IRadioButton         rbCompressFloat;
      ICheckBox            ckHuffmanOptimization;
      ISlider              slQuality;
      IEntryField          efQuality;
      ISlider              slSmoothing;
      IEntryField          efSmoothing;
      ICheckBox            ckBaseline;
      ICheckBox            ckProgressive;
};

#endif


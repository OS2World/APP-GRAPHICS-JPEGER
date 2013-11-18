#ifndef JPEGER_DECOMPRESSION_H
#define JPEGER_DECOMPRESSION_H

#include <icheckbx.hpp> // ICheckBox
#include <icmdhdr.hpp>  // ICommandHandler
#include <ientryfd.hpp> // IEntryField
#include <iflytext.hpp> // IFlyText
#include <iflyhhdr.hpp> // IFlyOverHelpHandler
#include <iframe.hpp>   // IFrameWindow
#include <iprofile.hpp> // IProfile
#include <iradiobt.hpp> // IRadioButton
#include <iselhdr.hpp>  // ISelectHandler
#include <isldahdr.hpp> // ISliderArmHandler
#include <islider.hpp>  // ISlider
#include <istattxt.hpp> // IStaticText


// class prototypes
class DecompressionWindow;
class MainWindow;


class DecompressionWindow :
   public IFrameWindow,
   protected ICommandHandler,
   protected ISelectHandler,
   protected ISliderArmHandler
{
   public:

      // constructor/destructor
      DecompressionWindow(MainWindow *wnd, IProfile &profile);
      virtual ~DecompressionWindow(void);

   protected:

      // inherited from ICommandHandler
      virtual Boolean command( ICommandEvent &event );

      // inherited from ISelectHandler
      virtual Boolean selected( IControlEvent &event );

      // inherited from ISliderArmHandler
      virtual Boolean positionChanged( IControlEvent &event );

      // other methods
      virtual DecompressionWindow &validateControls(void);
      virtual DecompressionWindow &readSettings(void);
      virtual DecompressionWindow &saveSettings(void);

      // instance variables
      IFlyText             flyText;
      IFlyOverHelpHandler  flyOverHelpHandler;
      MainWindow          *wnd;
      IProfile            &profile;
      IRadioButton         rbUse24BitColour;
      IRadioButton         rbQuantizeColours;
      IRadioButton         rbUse1PassQuantization;
      IRadioButton         rbUse2PassQuantization;
      IRadioButton         rbNoDithering;
      IRadioButton         rbOrderedDithering;
      IRadioButton         rbFloydSteinbergDithering;
      ISlider              slNumberOfColours;
      IEntryField          efNumberOfColours;
      IStaticText          txtNumberOfColours;
      ICheckBox            ckFancyUpsampling;
      ICheckBox            ckBlockSmoothing;
};

#endif


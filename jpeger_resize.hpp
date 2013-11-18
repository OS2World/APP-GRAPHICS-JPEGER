#ifndef JPEGER_RESIZE_H
#define JPEGER_RESIZE_H

#include <icheckbx.hpp> // ICheckBox
#include <icmdhdr.hpp>  // ICommandHandler
#include <ientryfd.hpp> // IEntryField
#include <iflytext.hpp> // IFlyText
#include <iflyhhdr.hpp> // IFlyOverHelpHandler
#include <ifocshdr.hpp> // IFocusHandler
#include <iframe.hpp>   // IFrameWindow
#include <iprofile.hpp> // IProfile
#include <ipushbut.hpp> // IPushButton
#include <iradiobt.hpp> // IRadioButton
#include <iselhdr.hpp>  // ISelectHandler
#include <ispinhdr.hpp> // ISpinHandler
#include <ispinnum.hpp> // INumericSpinButton


// class prototypes
class ResizeWindow;
class MainWindow;


class ResizeWindow :
   public IFrameWindow,
   protected ICommandHandler,
   protected ISelectHandler,
   protected IFocusHandler,
   protected ISpinHandler
{
   public:

      // constructor/destructor
      ResizeWindow(MainWindow *wnd, IProfile &profile);
      virtual ~ResizeWindow(void);

      virtual IPair getNewSize(void) const;

   protected:

      // inherited from OCL handlers
      virtual Boolean command( ICommandEvent &event );
      virtual Boolean selected( IControlEvent &event );
      virtual Boolean gotFocus( IControlEvent &event );
      virtual Boolean lostFocus( IControlEvent &event );
      virtual Boolean spinEnded( IControlEvent &event );

      // other methods
      virtual ResizeWindow &readSettings(void);
      virtual ResizeWindow &saveSettings(void);
      virtual ResizeWindow &validateControls(const int controlId);
      virtual IString calcRatio(const IPair pair) const;
      virtual IString calcAngle(const IPair pair) const;

      // instance variables
      IFlyText             flyText;
      IFlyOverHelpHandler  flyOverHelpHandler;
      MainWindow          *wnd;
      IProfile            &profile;
      Boolean              ratio;

      // controls
      IPushButton          pbRatio;
      IEntryField          efRatio1;
      IEntryField          efRatio2;
      IRadioButton         rbSizeCustom;
      IRadioButton         rbSizePrevious;
      INumericSpinButton   sbSizeX;
      INumericSpinButton   sbSizeY;
      ICheckBox            ckProportional;
      IRadioButton         rbPrevious1;
      IRadioButton         rbPrevious2;
      IRadioButton         rbPrevious3;
};

#endif


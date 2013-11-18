#include <ibmpctl.hpp>  // IBitmapControl
#include <icmdhdr.hpp>  // ICommandHandler
#include <iframe.hpp>   // IFrameWindow

#include "ontophandler.hpp"

class BannerWindow :
   public IFrameWindow,
   protected OnTopHandler,
   protected ICommandHandler
{
   public:

      // constructor/destructor
      BannerWindow(void);
      BannerWindow(IWindow *wnd);
      virtual ~BannerWindow(void);

   protected:

      BannerWindow &init(void);
      virtual Boolean systemCommand( ICommandEvent &event );

      // instance variables
      IBitmapControl bitmap;
};


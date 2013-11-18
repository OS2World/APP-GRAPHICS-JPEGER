#define INCL_GPIBITMAPS
#define INCL_GPICONTROL
#define INCL_WINERRORS

#include "jpeger_colourcube.hpp"
#include <imsgbox.hpp>  // IMessageBox
#include <iset.h>       // ISet
#include <ithread.hpp>  // IThread
#include <iwindow.hpp>  // IWindow
#include <os2.h>        // OS/2


/*****************************************************************************
 *
 *    ColourCubeElement::ColourCubeElement
 *
 * various constructors for the colour cube elements
 *
 *****************************************************************************/
ColourCubeElement::ColourCubeElement()
{
   red = 0;
   green = 0;
   blue = 0;
   return;
}

ColourCubeElement::ColourCubeElement( const char red, const char green, const char blue )
{
   this->red = red;
   this->green = green;
   this->blue = blue;
   return;
}

ColourCubeElement::ColourCubeElement( const ColourCubeElement &rhs )
{
   red = rhs.red;
   green = rhs.green;
   blue = rhs.blue;
   return;
}


/*****************************************************************************
 *
 *    ColourCubeElement::~ColourCubeElement
 *
 * destructor for the colour cube elements
 *
 *****************************************************************************/
ColourCubeElement::~ColourCubeElement()
{
   // empty destructor
   return;
}


/*****************************************************************************
 *
 *    ColourCubeElement::operator ==
 *    ColourCubeElement::operator <
 *    ColourCubeElement::operator >
 *
 * various operators required for use with the ISet template
 *
 *****************************************************************************/
Boolean ColourCubeElement::operator == ( const ColourCubeElement &rhs ) const
{
   return (red==rhs.red && green==rhs.green && blue==rhs.blue);
}

Boolean ColourCubeElement::operator < ( const ColourCubeElement &rhs ) const
{
   return (red <= rhs.red && green <= rhs.green && blue < rhs.blue);
}

Boolean ColourCubeElement::operator > ( const ColourCubeElement &rhs ) const
{
   return (red >= rhs.red && green >= rhs.green && blue > rhs.blue);
}


/*****************************************************************************
 *
 *    findColours
 *
 * determine the number of colours used in a bitmap
 *
 *****************************************************************************/
unsigned long findColours( const HBITMAP hbmp )
{
   // create a "set" to keep track of which colours are being used
   ISet<ColourCubeElement> colourSet;

   // get the bitmap info header
   BITMAPINFO2 bmpInfo;
   bmpInfo.cbFix = sizeof(BITMAPINFO2) - 4; //24;
   GpiQueryBitmapInfoHeader(hbmp, (PBITMAPINFOHEADER2)&bmpInfo);

   // allocate a buffer to hold an entire row of pixels
   RGB2 *rgb = new RGB2[bmpInfo.cx];

   // open the device context
   DEVOPENSTRUC devopenstruc = {NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL};
   HDC hdc = DevOpenDC( IThread::current().anchorBlock(), OD_MEMORY, "*", 5, (PDEVOPENDATA)&devopenstruc, NULLHANDLE );

   // get a handle to a presentation space
   SIZEL sizel = {0, 0};
   HPS hps = GpiCreatePS( IThread::current().anchorBlock(), hdc, &sizel, PU_PELS | GPIT_MICRO | GPIA_ASSOC );

   // select the bitmap
   GpiSetBitmap( hps, hbmp );

   // loop through the entire bitmap and find the number of colours used
   for( int row=0; row < bmpInfo.cy; row++ )
   {
      // get a row of pixels
      long result = GpiQueryBitmapBits(hps, row, 1, (PBYTE)rgb, &bmpInfo);

      if( result != 1 )
      {
         // show the error
         ERRORID errorid = WinGetLastError( IThread::current().anchorBlock() );
         IMessageBox msgbox(IWindow::desktopWindow());
         msgbox.
            setTitle("JPEGER - GpiQueryBitmapBits Error!").
            show( IString("Error 0x") + IString(errorid).d2x().rightJustify(8, '0') + " was received from GpiQueryBitmapBits().",
               IMessageBox::systemModal   |
               IMessageBox::okButton      |
               IMessageBox::defButton1    |
               IMessageBox::errorIcon     |
               IMessageBox::moveable      );
         break;
      }

      for( int col=0; col < bmpInfo.cx; col++ )
      {
         ColourCubeElement element( rgb[col].bRed, rgb[col].bGreen, rgb[col].bBlue );
         colourSet.add(element);
      }
   }

   // destroy the device context and presentation space
   GpiAssociate( hps, NULLHANDLE );
   GpiDestroyPS( hps );
   DevCloseDC( hdc );

   delete [] rgb;

   return colourSet.numberOfElements();
}


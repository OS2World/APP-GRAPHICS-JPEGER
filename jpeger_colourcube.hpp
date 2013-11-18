#ifndef JPEGER_COLOURCUBE_H
#define JPEGER_COLOURCUBE_H


// global includes
#include <ibase.hpp>    // IBase (required for "Boolean")
#include <os2.h>


// function prototype
unsigned long findColours( const HBITMAP hbmp );


class ColourCubeElement
{
   public:

      // constructors
      ColourCubeElement( void );
      ColourCubeElement( const char red, const char green, const char blue );
      ColourCubeElement( const ColourCubeElement &rhs );

      // destructor
      virtual ~ColourCubeElement( void );

      // operators
      virtual Boolean operator == ( const ColourCubeElement &rhs ) const;
      virtual Boolean operator < ( const ColourCubeElement &rhs ) const;
      virtual Boolean operator > ( const ColourCubeElement &rhs ) const;

      // instance variables
      char red;
      char green;
      char blue;
};

#endif


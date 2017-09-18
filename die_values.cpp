#include "die_values.h"

namespace eote
{

    DieValues::DieValues()
    : success( 0 ), advantage( 0 ), triumph( 0 ), force( 0 )
    { }

    DieValues::DieValues( int s, int a, int t, int f )
    : success( s ), advantage( a ), triumph( t ), force( f )
    { }

    namespace Value
    {
        DieValues
        Blank()
        {
            return DieValues();
        }

        DieValues
        Success( uint8_t v )
        {
            return DieValues( v, 0, 0, 0 );
        }

        DieValues
        Failure( uint8_t v )
        {
            return DieValues( -v, 0, 0, 0 );
        }

        DieValues
        Advantage( uint8_t v )
        {
            return DieValues( 0, v, 0, 0 );
        }

        DieValues
        Disadvantage( uint8_t v )
        {
            return DieValues( 0, -v, 0, 0 );
        }

        DieValues
        Triumph( uint8_t v )
        {
            return DieValues( 0, 0, v, 0 );
        }

        DieValues
        Despair( uint8_t v )
        {
            return DieValues( 0, 0, -v, 0 );
        }

        DieValues
        Light( uint8_t v )
        {
            return DieValues( 0, 0, 0, v );
        }

        DieValues
        Dark( uint8_t v )
        {
            return DieValues( 0, 0, 0, -v );
        }
    }

}


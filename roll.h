#ifndef ROLL_H_INCLUDED
#define ROLL_H_INCLUDED

#include "die_values.h"

namespace eote
{
    namespace Roll
    {
        enum Die { Blue, Black, Red, Yellow, Green, Purple, White };

        DieValues _Blue();
        DieValues _Black();
        DieValues _Red();
        DieValues _Yellow();
        DieValues _Green();
        DieValues _Purple();
        DieValues _White();
    }

    template < typename list_type >
    inline DieValues RollDice( const list_type& l );

    template < typename iterator_type >
    inline DieValues RollDice( const iterator_type& begin, const iterator_type& end );

    template < typename list_type >
    inline std::string DiceListToString( const list_type& l );

    template < typename iterator_type >
    inline std::string DiceListToString( const iterator_type& begin, const iterator_type& end );
}

#include "roll.inl"

#endif // ROLL_H_INCLUDED

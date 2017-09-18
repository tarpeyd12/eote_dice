#ifndef ROLL_H_INCLUDED
#define ROLL_H_INCLUDED

#include "random.h"

#include "die_values.h"

namespace eote
{
    namespace Roll
    {
        enum Die { Blue, Black, Red, Yellow, Green, Purple, White };

        DieValues _Blue( Random::RandomFunctor * _rand = nullptr );
        DieValues _Black( Random::RandomFunctor * _rand = nullptr );
        DieValues _Red( Random::RandomFunctor * _rand = nullptr );
        DieValues _Yellow( Random::RandomFunctor * _rand = nullptr );
        DieValues _Green( Random::RandomFunctor * _rand = nullptr );
        DieValues _Purple( Random::RandomFunctor * _rand = nullptr );
        DieValues _White( Random::RandomFunctor * _rand = nullptr );
    }

    template < typename list_type >
    inline DieValues RollDice( const list_type& l, Random::RandomFunctor * _rand = nullptr );

    template < typename iterator_type >
    inline DieValues RollDice( const iterator_type& begin, const iterator_type& end, Random::RandomFunctor * _rand = nullptr );

    template < typename list_type >
    inline std::string DiceListToString( const list_type& l );

    template < typename iterator_type >
    inline std::string DiceListToString( const iterator_type& begin, const iterator_type& end );

    template < typename list_type >
    inline uint64_t DiceListTotalPossibleSideCombinations( const list_type& l );

    template < typename iterator_type >
    inline uint64_t DiceListTotalPossibleSideCombinations( const iterator_type& begin, const iterator_type& end );
}

#include "roll.inl"

#endif // ROLL_H_INCLUDED

#ifndef ROLL_INL_INCLUDED
#define ROLL_INL_INCLUDED

namespace eote
{
    template < typename list_type >
    DieValues
    RollDice( const list_type& l, Random::RandomFunctor * _rand )
    {
        return RollDice( l.begin(), l.end(), _rand );
    }

    template < typename iterator_type >
    DieValues
    RollDice( const iterator_type& begin, const iterator_type& end, Random::RandomFunctor * _rand )
    {
        DieValues out;

        for( auto it = begin; it != end; ++it )
        {
            switch( *it )
            {
                case Roll::Blue:   out += Roll::_Blue( _rand );   break;
                case Roll::Black:  out += Roll::_Black( _rand );  break;
                case Roll::Red:    out += Roll::_Red( _rand );    break;
                case Roll::Yellow: out += Roll::_Yellow( _rand ); break;
                case Roll::Green:  out += Roll::_Green( _rand );  break;
                case Roll::Purple: out += Roll::_Purple( _rand ); break;
                case Roll::White:  out += Roll::_White( _rand );  break;
            }
        }
        return out;
    }

    template < typename list_type >
    inline DieValues _IterateDice( const list_type& l, uintmax_t count )
    {
        return _IterateDice( l.begin(), l.end(), count );
    }

    template < typename iterator_type >
    inline DieValues _IterateDice( const iterator_type& begin, const iterator_type& end, uintmax_t count )
    {
        DieValues out;

        for( auto it = begin; it != end; ++it )
        {
            switch( *it )
            {
                case Roll::Blue:   out += Roll::_Iterate::_Blue( count, &count );   break;
                case Roll::Black:  out += Roll::_Iterate::_Black( count, &count );  break;
                case Roll::Red:    out += Roll::_Iterate::_Red( count, &count );    break;
                case Roll::Yellow: out += Roll::_Iterate::_Yellow( count, &count ); break;
                case Roll::Green:  out += Roll::_Iterate::_Green( count, &count );  break;
                case Roll::Purple: out += Roll::_Iterate::_Purple( count, &count ); break;
                case Roll::White:  out += Roll::_Iterate::_White( count, &count );  break;
            }
        }
        return out;
    }

    template < typename list_type >
    std::string
    DiceListToString( const list_type& l )
    {
        return DiceListToString( l.begin(), l.end() );
    }

    template < typename iterator_type >
    std::string
    DiceListToString( const iterator_type& begin, const iterator_type& end )
    {
        uintmax_t blue = 0, black = 0, red = 0, yellow = 0, green = 0, purple = 0, white = 0;

        for( auto it = begin; it != end; ++it )
        {
            switch( *it )
            {
                case Roll::Blue:   ++blue;   break;
                case Roll::Black:  ++black;  break;
                case Roll::Red:    ++red;    break;
                case Roll::Yellow: ++yellow; break;
                case Roll::Green:  ++green;  break;
                case Roll::Purple: ++purple; break;
                case Roll::White:  ++white;  break;
            }
        }

        std::stringstream ss;

        if( green )  { ss << green  << " Green, ";  }
        if( yellow ) { ss << yellow << " Yellow, "; }
        if( purple ) { ss << purple << " Purple, "; }
        if( red )    { ss << red    << " Red, ";    }
        if( black )  { ss << black  << " Black, ";  }
        if( blue )   { ss << blue   << " Blue, ";   }
        if( white )  { ss << white  << " White, ";  }

        std::string str( ss.str() );

        //if( str.size() && str.back() == ' ' ) { str = str.substr( 0, str.size()-1 ); }
        //if( str.size() && str.back() == ',' ) { str = str.substr( 0, str.size()-1 ); }
        while( str.size() && ( str.back() == ' ' || str.back() == ',' ) ) { str = str.substr( 0, str.size()-1 ); }

        return str;
    }

    template < typename list_type >
    inline uint64_t DiceListTotalPossibleSideCombinations( const list_type& l )
    {
        return DiceListTotalPossibleSideCombinations( l.begin(), l.end() );
    }

    template < typename iterator_type >
    inline uint64_t DiceListTotalPossibleSideCombinations( const iterator_type& begin, const iterator_type& end )
    {
        uint64_t count = 1;

        if( begin == end )
        {
            return 0;
        }

        for( auto it = begin; it != end; ++it )
        {
            uint64_t factor = 0;
            switch( *it )
            {
                case Roll::Blue:
                case Roll::Black:  factor = 6;  break;

                case Roll::Green:
                case Roll::Purple: factor = 8;  break;

                case Roll::Yellow:
                case Roll::Red:
                case Roll::White:  factor = 12; break;
            }

            uint64_t next = count * factor;

            if( next < count ) // overflow
            {
                return 0;
            }

            count = next;
        }

        return count;
    }
}

#endif // ROLL_INL_INCLUDED

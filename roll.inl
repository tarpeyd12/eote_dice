#ifndef ROLL_INL_INCLUDED
#define ROLL_INL_INCLUDED

namespace eote
{
    inline
    DieValues
    RollDie( const Roll::Die die, Random::RandomFunctor * _rand )
    {
        switch( die )
        {
            case Roll::Blue:   return Roll::_Blue( _rand );
            case Roll::Black:  return Roll::_Black( _rand );
            case Roll::Red:    return Roll::_Red( _rand );
            case Roll::Yellow: return Roll::_Yellow( _rand );
            case Roll::Green:  return Roll::_Green( _rand );
            case Roll::Purple: return Roll::_Purple( _rand );
            case Roll::White:  return Roll::_White( _rand );

            default: break;
        }

        return Value::Blank();
    }

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
            out += RollDie( *it, _rand );
            /*switch( *it )
            {
                case Roll::Blue:   out += Roll::_Blue( _rand );   break;
                case Roll::Black:  out += Roll::_Black( _rand );  break;
                case Roll::Red:    out += Roll::_Red( _rand );    break;
                case Roll::Yellow: out += Roll::_Yellow( _rand ); break;
                case Roll::Green:  out += Roll::_Green( _rand );  break;
                case Roll::Purple: out += Roll::_Purple( _rand ); break;
                case Roll::White:  out += Roll::_White( _rand );  break;
            }*/
        }
        return out;
    }

    inline
    DieValues
    _IterateDie( const Roll::Die die, uintmax_t count, uintmax_t * carry )
    {
        switch( die )
        {
            case Roll::Blue:   return Roll::_Iterate::_Blue( count, carry );
            case Roll::Black:  return Roll::_Iterate::_Black( count, carry );
            case Roll::Red:    return Roll::_Iterate::_Red( count, carry );
            case Roll::Yellow: return Roll::_Iterate::_Yellow( count, carry );
            case Roll::Green:  return Roll::_Iterate::_Green( count, carry );
            case Roll::Purple: return Roll::_Iterate::_Purple( count, carry );
            case Roll::White:  return Roll::_Iterate::_White( count, carry );

            default: break;
        }

        return Value::Blank();
    }

    template < typename list_type >
    inline
    DieValues
    _IterateDice( const list_type& l, uintmax_t count )
    {
        return _IterateDice( l.begin(), l.end(), count );
    }

    template < typename iterator_type >
    inline
    DieValues
    _IterateDice( const iterator_type& begin, const iterator_type& end, uintmax_t count )
    {
        DieValues out;

        for( auto it = begin; it != end; ++it )
        {
            out += _IterateDie( *it, count, &count );
            /*switch( *it )
            {
                case Roll::Blue:   out += Roll::_Iterate::_Blue( count, &count );   break;
                case Roll::Black:  out += Roll::_Iterate::_Black( count, &count );  break;
                case Roll::Red:    out += Roll::_Iterate::_Red( count, &count );    break;
                case Roll::Yellow: out += Roll::_Iterate::_Yellow( count, &count ); break;
                case Roll::Green:  out += Roll::_Iterate::_Green( count, &count );  break;
                case Roll::Purple: out += Roll::_Iterate::_Purple( count, &count ); break;
                case Roll::White:  out += Roll::_Iterate::_White( count, &count );  break;
            }*/
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

    inline
    std::string
    DiceListToString( const std::map< Roll::Die, uintmax_t >& dieMap )
    {
        std::map< Roll::Die, uintmax_t > tmp( dieMap );

        std::stringstream ss;

        if( uintmax_t green = tmp[Roll::Green] )   { ss << green  << " Green, ";  }
        if( uintmax_t yellow = tmp[Roll::Yellow] ) { ss << yellow << " Yellow, "; }
        if( uintmax_t purple = tmp[Roll::Purple] ) { ss << purple << " Purple, "; }
        if( uintmax_t red = tmp[Roll::Red] )       { ss << red    << " Red, ";    }
        if( uintmax_t black = tmp[Roll::Black] )   { ss << black  << " Black, ";  }
        if( uintmax_t blue = tmp[Roll::Blue] )     { ss << blue   << " Blue, ";   }
        if( uintmax_t white = tmp[Roll::White] )   { ss << white  << " White, ";  }

        std::string str( ss.str() );

        while( str.size() && ( str.back() == ' ' || str.back() == ',' ) ) { str = str.substr( 0, str.size()-1 ); }

        return str;
    }

    inline
    uintmax_t
    DiceListTotalPossibleSideCombinations( const Roll::Die die )
    {
        switch( die )
        {
            case Roll::Blue:
            case Roll::Black:  return 6;

            case Roll::Green:
            case Roll::Purple: return 8;

            case Roll::Yellow:
            case Roll::Red:
            case Roll::White:  return 12;

            default: break;
        }

        return 0;
    }

    template < typename list_type >
    inline
    uintmax_t
    DiceListTotalPossibleSideCombinations( const list_type& l )
    {
        return DiceListTotalPossibleSideCombinations( l.begin(), l.end() );
    }

    template < typename iterator_type >
    inline
    uintmax_t
    DiceListTotalPossibleSideCombinations( const iterator_type& begin, const iterator_type& end )
    {
        uintmax_t count = 1;

        if( begin == end )
        {
            return 0;
        }

        for( auto it = begin; it != end; ++it )
        {
            uintmax_t factor = DiceListTotalPossibleSideCombinations( *it );

            uintmax_t next = count * factor;

            if( next < count ) // overflow
            {
                return 0;
            }

            count = next;
        }

        return count;
    }

    inline
    uintmax_t
    DiceListTotalPossibleSideCombinations( const std::map< Roll::Die, uintmax_t >& dieMap )
    {
        uintmax_t count = 1;

        for( auto it = dieMap.begin(); it != dieMap.end(); ++it )
        {
            uintmax_t faces = DiceListTotalPossibleSideCombinations( it->first );
            uintmax_t numDie = it->second;

            while( numDie-- )
            {
                uintmax_t next = count * faces;

                if( next < count ) // overflow
                {
                    return 0;
                }

                count = next;
            }
        }

        return count;
    }
}

#endif // ROLL_INL_INCLUDED

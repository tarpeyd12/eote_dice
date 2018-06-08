#ifndef DIE_VALUES_H_INCLUDED
#define DIE_VALUES_H_INCLUDED

#include <sstream>

namespace eote
{
    struct DieValues
    {
        int success;
        int advantage;
        int triumph;
        int force;

        DieValues();
        DieValues( int s, int a, int t, int f );

        inline DieValues& operator+=( const DieValues& other );
        inline DieValues& operator-=( const DieValues& other );

        friend inline DieValues operator+( DieValues lhs, const DieValues& rhs );
        friend inline DieValues operator-( DieValues lhs, const DieValues& rhs );

        inline operator std::string() const;
        inline std::string toString( bool csv = false ) const;

        inline int operator[]( std::size_t i ) const;
        inline void setByIndex( std::size_t i, int v );

        friend inline bool operator==( const DieValues& lhs, const DieValues& rhs )
        {
            return lhs.success == rhs.success && lhs.advantage == rhs.advantage && lhs.triumph == rhs.triumph && lhs.force == rhs.force;
        }
    };


    namespace Value
    {
        DieValues Blank();

        DieValues Success( uint8_t v = 1 );
        DieValues Failure( uint8_t v = 1 );
        DieValues Advantage( uint8_t v = 1 );
        DieValues Disadvantage( uint8_t v = 1 );
        DieValues Triumph( uint8_t v = 1 );
        DieValues Despair( uint8_t v = 1 );
        DieValues Light( uint8_t v = 1 );
        DieValues Dark( uint8_t v = 1 );
    }

}

#include "die_values.inl"

#endif // DIE_VALUES_H_INCLUDED

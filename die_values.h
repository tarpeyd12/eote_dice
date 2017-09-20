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


    DieValues&
    DieValues::operator+=( const DieValues& other )
    {
        success   += other.success;
        advantage += other.advantage;
        triumph   += other.triumph;
        force     += other.force;
        return *this;
    }

    DieValues&
    DieValues::operator-=( const DieValues& other )
    {
        success   -= other.success;
        advantage -= other.advantage;
        triumph   -= other.triumph;
        force     -= other.force;
        return *this;
    }

    DieValues
    operator+( DieValues lhs, const DieValues& rhs )
    {
        lhs += rhs;
        return lhs;
    }

    DieValues
    operator-( DieValues lhs, const DieValues& rhs )
    {
        lhs -= rhs;
        return lhs;
    }

    DieValues::operator std::string() const
    {
        std::stringstream s;

        if( triumph > 0 )
        {
            s << "triumph:" << triumph << " ";
        }
        else if( triumph < 0 )
        {
            s << "despair:" << -triumph << " ";
        }

        if( success > 0 )
        {
            s << "success:" << success << " ";
        }
        else if( success < 0 )
        {
            s << "failure:" << -success << " ";
        }

        if( advantage > 0 )
        {
            s << "advantage:" << advantage << " ";
        }
        else if( advantage < 0 )
        {
            s << "disadvantage:" << -advantage << " ";
        }

        if( force > 0 )
        {
            s << "light:" << force << " ";
        }
        else if( force < 0 )
        {
            s << "dark:" << -force << " ";
        }

        return s.str();
    }

    std::string
    DieValues::toString( bool csv ) const
    {
        std::stringstream s;

        {
            s << "triumph:" << (csv?",":"") << std::max( triumph, 0 ) << ",";
            s << "despair:" << (csv?",":"") << std::max( -triumph, 0 ) << ",";
        }

        {
            s << "success:" << (csv?",":"") << std::max( success, 0 ) << ",";
            s << "failure:" << (csv?",":"") << std::max( -success, 0 ) << ",";
        }

        {
            s << "advantage:" << (csv?",":"") << std::max( advantage, 0 ) << ",";
            s << "disadvantage:" << (csv?",":"") << std::max( -advantage, 0 ) << ",";
        }

        {
            s << "light:" << (csv?",":"") << std::max( force, 0 ) << ",";
            s << "dark:" << (csv?",":"") << std::max( -force, 0 ) << "";
        }

        return s.str();
    }

    int
    DieValues::operator[]( std::size_t i ) const
    {
        switch( i )
        {
            default:
            case 0:
                return triumph;
            case 1:
                return success;
            case 2:
                return advantage;
            case 3:
                return force;
        }
        return 0;
    }

    void
    DieValues::setByIndex( std::size_t i, int v )
    {
        switch( i )
        {
            default:
            case 0:
                triumph = v;
                break;
            case 1:
                success = v;
                break;
            case 2:
                advantage = v;
                break;
            case 3:
                force = v;
                break;
        }
    }


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

#endif // DIE_VALUES_H_INCLUDED

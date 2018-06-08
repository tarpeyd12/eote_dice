#ifndef DIE_VALUES_INL_INCLUDED
#define DIE_VALUES_INL_INCLUDED

namespace std
{
    template< >
    struct less< eote::DieValues >
    {
        inline
        bool
        operator()( const eote::DieValues& lhs, const eote::DieValues& rhs ) const
        {
            if( lhs.triumph != rhs.triumph ) { return lhs.triumph > rhs.triumph; } // most significant
            if( lhs.success != rhs.success ) { return lhs.success > rhs.success; }
            if( lhs.advantage != rhs.advantage ) { return lhs.advantage > rhs.advantage; }
            if( lhs.force != rhs.force ) { return lhs.force > rhs.force; } // least significant
            return false;
        }
    };

    template< >
    struct hash< eote::DieValues >
    {
        inline
        std::size_t
        operator()( const eote::DieValues& dv ) const
        {
            return ( (dv[0] & 0xff) << 8*3 ) | ( (dv[1] & 0xff) << 8*2 ) | ( (dv[2] & 0xff) << 8*1 ) | ( (dv[3] & 0xff) << 8*0 );
        }
    };
}

namespace eote
{
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
}

#endif // DIE_VALUES_INL_INCLUDED

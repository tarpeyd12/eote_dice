#ifndef RANDOM_FUNCTOR_INL_INCLUDED
#define RANDOM_FUNCTOR_INL_INCLUDED

namespace Rand
{
    double
    RandomFunctor::operator()( double min, double max )
    {
        return Float( min, max );
    }

    extern Random_Unsafe __random;

    double
    Float( double min, double max )
    {
        return __random.Float( min, max );
    }

    float
    Float( float min, float max )
    {
        return __random.Float( min, max );
    }

    unsigned
    Int( unsigned min, unsigned max )
    {
        return __random.Int( min, max );
    }
}

#endif // RANDOM_FUNCTOR_INL_INCLUDED

#ifndef RANDOM_FUNCTOR_H_INCLUDED
#define RANDOM_FUNCTOR_H_INCLUDED

#include <algorithm>
#include <random>
#include <mutex>

namespace Rand
{
    class RandomFunctor
    {
        public:
            virtual ~RandomFunctor() = default;

            virtual double Float( double min, double max ) = 0; // [min,max)
            virtual unsigned Int( unsigned min, unsigned max ) = 0; // [min,max]

            inline double operator()( double min = -1.0, double max = 1.0 );
    };

    inline double Float( double min = 0.0, double max = 1.0 );
    inline float Float( float min = 0.0, float max = 1.0 );

    inline unsigned Int( unsigned min = 0, unsigned max = RAND_MAX-1 );

    // TODO(dean): template the generator type
    class Random_Safe : public RandomFunctor
    {
        private:
            std::mutex randLock;

            //std::mt19937 generator;
            std::mt19937_64 generator;

        public:
            Random_Safe()
            : randLock(), generator()
            {
                std::lock_guard<std::mutex> lock( randLock );
                //std::random_device rd;
                //generator.seed( rd() );
                generator.seed( time(nullptr) );
            }

            Random_Safe( unsigned _seed )
            : randLock(), generator( _seed )
            {

            }

            virtual ~Random_Safe() = default;

            inline
            double
            Float( double min = 0.0, double max = 1.0 ) override
            {
                std::lock_guard<std::mutex> lock( randLock );
                return std::uniform_real_distribution<double>( min, max )( generator );
            }

            inline
            unsigned
            Int( unsigned min, unsigned max ) override
            {
                std::lock_guard<std::mutex> lock( randLock );
                return std::uniform_int_distribution<unsigned>( min, max )( generator );
            }
    };

    class Random_Unsafe : public RandomFunctor
    {
        private:
            //std::mt19937 generator;
            std::mt19937_64 generator;

        public:
            Random_Unsafe()
            : generator()
            {
                //std::random_device rd;
                //generator.seed( rd() );
                generator.seed( time(nullptr) );
            }

            Random_Unsafe( unsigned _seed )
            : generator( _seed )
            {

            }

            virtual ~Random_Unsafe() = default;

            inline
            double
            Float( double min = 0.0, double max = 1.0 ) override
            {
                return std::uniform_real_distribution<double>( min, max )( generator );
            }

            inline
            unsigned
            Int( unsigned min, unsigned max ) override
            {
                return std::uniform_int_distribution<unsigned>( min, max )( generator );
            }
    };
}

#include "random_functor.inl"

#endif // RANDOM_FUNCTOR_H_INCLUDED

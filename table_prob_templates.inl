#ifndef TABLE_PROB_TEMPLATES_INL_INCLUDED
#define TABLE_PROB_TEMPLATES_INL_INCLUDED

#include "die_values.h"
#include "roll.h"

namespace eote
{
    template < uint16_t NUM_THREADS, bool roll_randomly, bool print_progress >
    std::map< eote::DieValues, uintmax_t >
    _AproximateProbabilityTable( std::vector< eote::Roll::Die > dieList, uint64_t count )
    {
        // make sure we have at least one thread
        static_assert( NUM_THREADS != 0, "cannot have 0 threads." );

        // if rolling randomly and count is zero make it a big number that is achievable in this lifetime
        if( roll_randomly && !count )
        {
            count = 10000000000ull;
        }
        // if rolling iteratively, calculate how many we need to do
        if( !roll_randomly )
        {
            count = eote::DiceListTotalPossibleSideCombinations( dieList );
        }

        // create an array of maps, one per thread
        std::unordered_map< eote::DieValues, uintmax_t > table_array[ NUM_THREADS ];

        // create and seed a random number generator per thread
        Random::Random_Unsafe random_array[ NUM_THREADS ];
        if( roll_randomly ) for( uint64_t i = 0; i < NUM_THREADS; ++i ) { random_array[ i ] = Random::Random_Unsafe( Random::Int() ); }

        std::cout.precision( 4 );

        // define the thread function
        auto _RunThreadDieRolls =
        [&]( uint64_t threadID, uint64_t numthreads, std::unordered_map< eote::DieValues, uintmax_t > * _table, Random::RandomFunctor * _rand, uint64_t _count )
        {
            for( uint64_t i = threadID; i < _count; i += numthreads )
            {
                auto die = roll_randomly ? eote::RollDice( dieList, _rand ) : eote::_IterateDice( dieList, i );
                (*_table)
                [ die ]
                += 1;

                if( print_progress && threadID == 0 && i % 100000 == 0 ) std::cout << double(i)/double(_count)*100.0 << "% #unique=" << _table->size() << " itteration=" << i << "\r" << std::flush;
            }
        };

        // list of threads
        std::vector< std::thread * > _thread_list;

        // create the threads
        // TODO: use std::async
        for( uint64_t i = 1; i < NUM_THREADS; ++i )
        {
            _thread_list.push_back( new std::thread( _RunThreadDieRolls, i, NUM_THREADS, &table_array[ i ], &random_array[ i ], count ) );
            //_thread_list.back().start();
        }
        // this thread
        _RunThreadDieRolls( 0, NUM_THREADS, &table_array[ 0 ], &random_array[ 0 ], count );

        // cleanup the threads
        for( uint64_t i = 0; i < _thread_list.size(); ++i )
        {
            _thread_list[ i ]->join();
            delete _thread_list[ i ];
            _thread_list[ i ] = nullptr;
        }

        // the combined table to output
        std::map< eote::DieValues, uintmax_t > table; // must be map

        if( print_progress ) std::cout << "\nConsolidating ...\n" << std::flush;

        // for each map that we passed to the threads
        for( uint64_t i = 0; i < NUM_THREADS; ++i )
        {
            // add all the entries into the output table
            for( auto dp : table_array[ i ] )
            {
                table[ dp.first ] += dp.second;
                if( print_progress ) std::cout << " " << table.size() << "\r";
            }
            table_array[ i ].clear();
        }

        return table;
    }
}

#endif // TABLE_PROB_TEMPLATES_INL_INCLUDED

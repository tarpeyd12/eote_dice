#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

#include <algorithm>
#include <random>
#include <mutex>
#include <map>
#include <unordered_map>
#include <memory>

#include <thread>

#include "die_values.h"
#include "roll.h"

namespace std
{
    template< >
    struct less< eote::DieValues >
    {
        bool
        operator()( const eote::DieValues& lhs, const eote::DieValues& rhs ) const
        {
            if( lhs.triumph != rhs.triumph ) { return lhs.triumph > rhs.triumph; } // most significant
            if( lhs.success != rhs.success ) { return lhs.success > rhs.success; }
            if( lhs.advantage != rhs.advantage ) { return lhs.advantage > rhs.advantage; }
            if( lhs.force != rhs.force ) { return lhs.force > rhs.force; } // least significant
            return false;

            /*if( lhs[0] != rhs[0] ) { return lhs[0] > rhs[0]; }
            if( lhs[1] != rhs[1] ) { return lhs[1] > rhs[1]; }
            if( lhs[2] != rhs[2] ) { return lhs[2] > rhs[2]; }
            if( lhs[3] != rhs[3] ) { return lhs[3] > rhs[3]; }
            return false;

            for( int i = 0; i < 4; ++i )
            {
                if( lhs[i] == rhs[i] )
                {
                    continue;
                }
                return lhs[i] > rhs[i];
            }
            return false;*/
        }
    };

    template< >
    struct hash< eote::DieValues >
    {
        std::size_t
        operator()( const eote::DieValues& dv ) const
        {
            /*std::size_t out = 0;

            for( int i = 0; i < 4; ++i )
            {
                out <<= 8;
                out |= ( dv[i] & 0xff );
            }
            return out;*/

            return ( (dv[0] & 0xff) << 8*3 ) | ( (dv[1] & 0xff) << 8*2 ) | ( (dv[2] & 0xff) << 8*1 ) | ( (dv[3] & 0xff) << 8*0 );
        }
    };

}

class comma_numpunct : public std::numpunct< char >
{
  protected:
    virtual
    char
    do_thousands_sep() const
    {
        return ',';
    }

    virtual
    std::string
    do_grouping() const
    {
        return "\03";
    }
};

int main()
{
    std::ios::sync_with_stdio( false );

    std::vector< eote::Roll::Die > dieList;

    /*dieList.push_back( eote::Roll::Die::Yellow );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Black );

    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Red );

    dieList.push_back( eote::Roll::Die::Blue );
    dieList.push_back( eote::Roll::Die::White );*/

    dieList.push_back( eote::Roll::Die::Yellow );
    dieList.push_back( eote::Roll::Die::Yellow );
    dieList.push_back( eote::Roll::Die::Red );
    dieList.push_back( eote::Roll::Die::White );

    //dieList.push_back( eote::Roll::Die::Yellow );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Black );
    dieList.push_back( eote::Roll::Die::Black );
    dieList.push_back( eote::Roll::Die::Blue );
    dieList.push_back( eote::Roll::Die::Blue );
    //dieList.push_back( eote::Roll::Die::White );


     uintmax_t count = 500000000000;

    std::cout << std::fixed;

    //std::cout.unsetf( std::ios_base::floatfield );

    std::cout.imbue( std::locale( std::locale(""), new comma_numpunct() ) ); // format integers with commas.

    std::cout << "Rolling: " << eote::DiceListToString( dieList ) << " dice.\n";
    std::cout << count << " times ..." << std::endl;

    uint64_t combo_possibilities = eote::DiceListTotalPossibleSideCombinations( dieList );

    if( combo_possibilities )
    {
        count = combo_possibilities;

        std::cout << "" << combo_possibilities << " combinations possible." << std::endl;
        std::cout << (long double)(count) / (long double)(combo_possibilities) << " random rolls per actual possible face combination." << std::endl;
        std::cout << std::endl;
    }
    else
    {
        std::cout << "ERROR: number of possibilities exceeds " << ( ~1ull ) << std::endl;
    }
    //std::cout << "" << (combo_possibilities ? combo_possibilities : "ERROR: too many possibilities to hold in uint64_t" ) << " combinations possible.\n" << std::endl;

    //std::cout.imbue(std::locale(""));

    #define NUM_THREADS 4

    std::map< eote::DieValues, uintmax_t > table; // must be map
    std::unordered_map< eote::DieValues, uintmax_t > table_array[ NUM_THREADS ];

    Random::Random_Unsafe random_array[ NUM_THREADS ];
    for( uint64_t i = 0; i < NUM_THREADS; ++i ) { random_array[ i ] = Random::Random_Unsafe( Random::Int() ); }

    std::cout.precision( 4 );

    auto _RunThreadDieRolls = [&]( uint64_t threadID, uint64_t numthreads, std::unordered_map< eote::DieValues, uintmax_t > * _table, Random::RandomFunctor * _rand, uint64_t _count )
    {
        for( uint64_t i = threadID; i < _count; i += numthreads )
        {
            //auto die = eote::RollDice( dieList, _rand );
            auto die = eote::_IterateDice( dieList, i );
            (*_table)[ die ] += 1;

            if( threadID == 0 && i % 100000 == 0 ) std::cout << double(i)/double(_count)*100.0 << "% #unique=" << _table->size() << " itteration=" << i << "\r" << std::flush;
            //if( i % 100000 == 0 ) std::cout << double(i)/double(count)*100.0 << "% c=" << table.size() << " i=" << i << "\r" << std::flush;
        }
    };
    /*for( uint64_t i = 0; i < count; ++i )
    {
        auto die = eote::RollDice( dieList );
        table[ die ] += 1;

        if( i % 100000 == 0 ) std::cout << double(i)/double(count)*100.0 << "% c=" << table.size() << " i=" << i << "\r" << std::flush;
    }*/

    //_RunThreadDieRolls( 0, 1, &table, &random_array[0], count );

    std::vector< std::thread * > _thread_list;

    for( uint64_t i = 1; i < NUM_THREADS; ++i )
    {
        _thread_list.push_back( new std::thread( _RunThreadDieRolls, i, NUM_THREADS, &table_array[ i ], &random_array[ i ], count ) );
        //_thread_list.back().start();
    }
    _RunThreadDieRolls( 0, NUM_THREADS, &table_array[ 0 ], &random_array[ 0 ], count );
    for( uint64_t i = 1; i < _thread_list.size(); ++i )
    {
        _thread_list[ i ]->join();
        delete _thread_list[ i ];
        _thread_list[ i ] = nullptr;
    }

    std::cout << "\nConsolidating ...\n" << std::flush;
    for( uint64_t i = 0; i < NUM_THREADS; ++i )
    {
        for( auto dp : table_array[ i ] )
        {
            table[ dp.first ] += dp.second;
            std::cout << " " << table.size() << "\r";
        }
        table_array[i].clear();
    }

    std::cout << std::endl;

    std::cout.precision( 8 );

    std::vector< std::pair< uintmax_t, eote::DieValues > > probList;

    std::ofstream out_allProb( "./allProb.csv" );

    std::cout << "\nProbability Sorted By Advantageousness:\n" << std::endl;

    for( auto dp : table )
    {
        out_allProb << dp.first.toString(true) << ",prob:," << std::scientific <<  double(dp.second)/double(count)*100.0 << "%" << std::endl;
        std::cout << "p:" << double(dp.second)/double(count)*100.0 << "%  " << std::string(dp.first) << "\n";
        probList.push_back(std::pair<uintmax_t, eote::DieValues>( dp.second, dp.first ));
    }

    out_allProb.close();

    std::sort( probList.begin(), probList.end(), []( const std::pair<uintmax_t, eote::DieValues>& a, const std::pair<uintmax_t, eote::DieValues>& b ){ return a.first>b.first; } );

    std::map< uintmax_t, std::map<eote::DieValues, uintmax_t> > buckets;

    std::ofstream out_sortProb( "./sortProb.csv" );

    const int numBuckets = 10;

    std::cout << "\nProbability Sorted By Probability:\n" << std::endl;
    double cuumprob = 0.0;
    for( auto dp : probList )
    {
        double percent = double(dp.first)/double(count)*100.0;
        cuumprob += percent;
        out_sortProb << dp.second.toString(true) << ",prob:," << std::scientific << percent << "%,cuumProb:," << std::scientific << cuumprob << "%" << std::endl;
        std::cout << "p:" << percent << "%(" << cuumprob <<  "%)  " << std::string(dp.second) << "\n";

        buckets[ std::min<int>(int(double(cuumprob)/100.0*double(numBuckets)),numBuckets-1) ][ dp.second ] += dp.first;
    }

    out_sortProb.close();

    std::cout << "\nAt least one of:\n\n";

    //auto defaultDieValue = eote::DieValues();

    for( auto bucket : buckets )
    {
        auto bucketid = bucket.first;
        uintmax_t numInBucket = 0;

        uintmax_t numSuccessInBucket = 0;
        uintmax_t numAdvantageInBucket = 0;
        uintmax_t numTriumphInBucket = 0;
        uintmax_t numForceInBucket = 0;

        uintmax_t numFailInBucket = 0;
        uintmax_t numDisadvantageInBucket = 0;
        uintmax_t numDispareInBucket = 0;
        uintmax_t numDarkInBucket = 0;

        for( auto rollStatPair : bucket.second )
        {
            auto dieRoll = rollStatPair.first;
            auto bcount = rollStatPair.second;

            numInBucket += bcount;
            if( dieRoll.success > 0 )   { numSuccessInBucket   += bcount; }
            if( dieRoll.advantage > 0 ) { numAdvantageInBucket += bcount; }
            if( dieRoll.triumph > 0 )   { numTriumphInBucket   += bcount; }
            if( dieRoll.force > 0 )     { numForceInBucket     += bcount; }

            if( dieRoll.success < 0 )   { numFailInBucket   += bcount; }
            if( dieRoll.advantage < 0 ) { numDisadvantageInBucket += bcount; }
            if( dieRoll.triumph < 0 )   { numDispareInBucket   += bcount; }
            if( dieRoll.force < 0 )     { numDarkInBucket     += bcount; }

            //std::cout << "bucketid:" << bucketid << " count:" << bcount << " dieRoll:" << std::string(dieRoll) << std::endl;
        }

        std::cout << std::setw(3) << std::setprecision(0) << std::right << int((bucketid+1)/double(buckets.size())*100.0) << "th percentile: ";
        //std::cout << "count:" << std::setw(3) << std::left << numInBucket;
        std::cout << " t:" << std::setw(5) << std::right << std::setprecision(numTriumphInBucket==numInBucket?1:2)   << double(numTriumphInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " s:" << std::setw(5) << std::right << std::setprecision(numSuccessInBucket==numInBucket?1:2)   << double(numSuccessInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " a:" << std::setw(5) << std::right << std::setprecision(numAdvantageInBucket==numInBucket?1:2) << double(numAdvantageInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " lf:" << std::setw(5) << std::right << std::setprecision(numForceInBucket==numInBucket?1:2)    << double(numForceInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " |";
        std::cout << " d:" << std::setw(5) << std::right << std::setprecision(numDispareInBucket==numInBucket?1:2)      << double(numDispareInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " f:" << std::setw(5) << std::right << std::setprecision(numFailInBucket==numInBucket?1:2)         << double(numFailInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " v:" << std::setw(5) << std::right << std::setprecision(numDisadvantageInBucket==numInBucket?1:2) << double(numDisadvantageInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " df:" << std::setw(5) << std::right << std::setprecision(numDarkInBucket==numInBucket?1:2)        << double(numDarkInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " (" << numInBucket << ")";

        std::cout << std::endl;
    }

    {
        uintmax_t numInBucket = 0;

        uintmax_t numSuccessInBucket = 0;
        uintmax_t numAdvantageInBucket = 0;
        uintmax_t numTriumphInBucket = 0;
        uintmax_t numForceInBucket = 0;

        uintmax_t numFailInBucket = 0;
        uintmax_t numDisadvantageInBucket = 0;
        uintmax_t numDispareInBucket = 0;
        uintmax_t numDarkInBucket = 0;

        for( auto rollStatPair : probList )
        {
            auto dieRoll = rollStatPair.second;
            auto bcount = rollStatPair.first;

            numInBucket += bcount;
            if( dieRoll.success > 0 )   { numSuccessInBucket   += bcount; }
            if( dieRoll.advantage > 0 ) { numAdvantageInBucket += bcount; }
            if( dieRoll.triumph > 0 )   { numTriumphInBucket   += bcount; }
            if( dieRoll.force > 0 )     { numForceInBucket     += bcount; }

            if( dieRoll.success < 0 )   { numFailInBucket   += bcount; }
            if( dieRoll.advantage < 0 ) { numDisadvantageInBucket += bcount; }
            if( dieRoll.triumph < 0 )   { numDispareInBucket   += bcount; }
            if( dieRoll.force < 0 )     { numDarkInBucket     += bcount; }

            //std::cout << "bucketid:" << bucketid << " count:" << bcount << " dieRoll:" << std::string(dieRoll) << std::endl;
        }

        std::cout << "\n           total: ";
        //std::cout << "count:" << std::setw(3) << std::left << numInBucket;
        std::cout << " t:" << std::setw(5) << std::right << std::setprecision(numTriumphInBucket==numInBucket?1:2)   << double(numTriumphInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " s:" << std::setw(5) << std::right << std::setprecision(numSuccessInBucket==numInBucket?1:2)   << double(numSuccessInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " a:" << std::setw(5) << std::right << std::setprecision(numAdvantageInBucket==numInBucket?1:2) << double(numAdvantageInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " lf:" << std::setw(5) << std::right << std::setprecision(numForceInBucket==numInBucket?1:2)    << double(numForceInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " |";
        std::cout << " d:" << std::setw(5) << std::right << std::setprecision(numDispareInBucket==numInBucket?1:2)      << double(numDispareInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " f:" << std::setw(5) << std::right << std::setprecision(numFailInBucket==numInBucket?1:2)         << double(numFailInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " v:" << std::setw(5) << std::right << std::setprecision(numDisadvantageInBucket==numInBucket?1:2) << double(numDisadvantageInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " df:" << std::setw(5) << std::right << std::setprecision(numDarkInBucket==numInBucket?1:2)        << double(numDarkInBucket)/double(numInBucket)*100.0 << "%";
        std::cout << " (" << numInBucket << ")";

        std::cout << "\n\ntotal unique combinations: " << table.size() << std::endl;
    }


    std::cout << std::endl;

    return 0;
}

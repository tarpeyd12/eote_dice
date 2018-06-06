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
    template < typename Type >
    Type
    clamp( const Type& v, const Type& a, const Type& b )
    {
        return std::max<Type>( std::min<Type>( v, b ), a );
    }

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
        }
    };

    template< >
    struct hash< eote::DieValues >
    {
        std::size_t
        operator()( const eote::DieValues& dv ) const
        {
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


template < uint16_t NUM_THREADS = 2, bool roll_randomly = false, bool print_progress = true >
std::map< eote::DieValues, uintmax_t >
GetProbabilityTable( std::vector< eote::Roll::Die > dieList, uint64_t count = 0 )
{
    static_assert( NUM_THREADS != 0, "cannot have 0 threads." );

    if( roll_randomly && !count )
    {
        count = 10000000000ull;
    }
    if( !roll_randomly )
    {
        count = eote::DiceListTotalPossibleSideCombinations( dieList );
    }

    std::unordered_map< eote::DieValues, uintmax_t > table_array[ NUM_THREADS ];

    Random::Random_Unsafe random_array[ NUM_THREADS ];
    if( roll_randomly ) for( uint64_t i = 0; i < NUM_THREADS; ++i ) { random_array[ i ] = Random::Random_Unsafe( Random::Int() ); }

    std::cout.precision( 4 );

    auto _RunThreadDieRolls = [&]( uint64_t threadID, uint64_t numthreads, std::unordered_map< eote::DieValues, uintmax_t > * _table, Random::RandomFunctor * _rand, uint64_t _count )
    {
        for( uint64_t i = threadID; i < _count; i += numthreads )
        {
            auto die = roll_randomly ? eote::RollDice( dieList, _rand ) : eote::_IterateDice( dieList, i );
            (*_table)[ die ] += 1;

            if( print_progress && threadID == 0 && i % 100000 == 0 ) std::cout << double(i)/double(_count)*100.0 << "% #unique=" << _table->size() << " itteration=" << i << "\r" << std::flush;
        }
    };

    std::vector< std::thread * > _thread_list;

    for( uint64_t i = 1; i < NUM_THREADS; ++i )
    {
        _thread_list.push_back( new std::thread( _RunThreadDieRolls, i, NUM_THREADS, &table_array[ i ], &random_array[ i ], count ) );
        //_thread_list.back().start();
    }
    _RunThreadDieRolls( 0, NUM_THREADS, &table_array[ 0 ], &random_array[ 0 ], count );
    for( uint64_t i = 0; i < _thread_list.size(); ++i )
    {
        _thread_list[ i ]->join();
        delete _thread_list[ i ];
        _thread_list[ i ] = nullptr;
    }

    std::map< eote::DieValues, uintmax_t > table; // must be map

    if( print_progress ) std::cout << "\nConsolidating ...\n" << std::flush;
    for( uint64_t i = 0; i < NUM_THREADS; ++i )
    {
        for( auto dp : table_array[ i ] )
        {
            table[ dp.first ] += dp.second;
            if( print_progress ) std::cout << " " << table.size() << "\r";
        }
        table_array[ i ].clear();
    }

    return table;
}

void
PrintProbabilityTable_Counts( std::map< eote::DieValues, uintmax_t > table, std::ostream& out = std::cout )
{
    for( auto dp : table )
    {
        out << "p:" << dp.second << "  " << std::string( dp.first ) << "\n";
    }
}

std::map< eote::DieValues, uintmax_t >
CombineTableProbabilities( std::map< eote::DieValues, uintmax_t > tableA, std::map< eote::DieValues, uintmax_t > tableB )
{
    // enforce that there are fewer entries in tableB
    if( tableA.size() < tableB.size() )
    {
        return CombineTableProbabilities( tableB, tableA );
    }

    if( tableA.size() && !tableB.size() )
    {
        return tableA;
    }

    std::map< eote::DieValues, uintmax_t > table_out;

    for( auto entryB : tableB )
    {
        for( auto entryA : tableA )
        {
            table_out[ entryA.first + entryB.first ] += entryA.second * entryB.second;
        }
    }

    return table_out;
}

std::map< eote::DieValues, uintmax_t >
DieToProbabilityTable( eote::Roll::Die die )
{
    return GetProbabilityTable< 1, false, false >( std::vector< eote::Roll::Die >{ die } );
}

std::map< eote::DieValues, uintmax_t >
GetProbabilityTable_TableCombineMethod( std::vector< eote::Roll::Die > dieList )
{
    std::map< eote::DieValues, uintmax_t > table;

    for( auto die : dieList )
    {
        table = CombineTableProbabilities( table, DieToProbabilityTable( die ) );
    }

    return table;
}

std::map< eote::DieValues, uintmax_t >
SimplifyTableForAxies( std::map< eote::DieValues, uintmax_t > table, uint8_t axisA, uint8_t axisB )
{
    axisA %= 4;
    axisB %= 4;

    std::map< eote::DieValues, uintmax_t > out;

    for( auto entry : table )
    {
        eote::DieValues dummy;
        dummy.setByIndex( axisA, entry.first[axisA] );
        dummy.setByIndex( axisB, entry.first[axisB] );

        out[ dummy ] += entry.second;
    }

    return out;
}

void
ProbabilityTableToAreaCSV( std::map< eote::DieValues, uintmax_t > table, uint8_t axisA, uint8_t axisB, std::ostream& out )
{
    axisA %= 4;
    axisB %= 4;

    int minAxisA = table.begin()->first[axisA];
    int maxAxisA = table.begin()->first[axisA];

    int minAxisB = table.begin()->first[axisB];
    int maxAxisB = table.begin()->first[axisB];

    std::map< eote::DieValues, uintmax_t > simplifiyed_table;

    for( auto entry : table )
    {
        minAxisA = std::min<int>( minAxisA, entry.first[axisA] );
        maxAxisA = std::max<int>( maxAxisA, entry.first[axisA] );

        minAxisB = std::min<int>( minAxisB, entry.first[axisB] );
        maxAxisB = std::max<int>( maxAxisB, entry.first[axisB] );

        eote::DieValues dummy;
        dummy.setByIndex( axisA, entry.first[axisA] );
        dummy.setByIndex( axisB, entry.first[axisB] );

        simplifiyed_table[ dummy ] += entry.second;
    }

    //std::map< eote::DieValues, uintmax_t > simplifiyed_table = SimplifyTableForAxies( table, axisA, axisB );

    switch( axisA )
    {
        case 0: out << "Horizontal triumph "; break;
        case 1: out << "Horizontal success "; break;
        case 2: out << "Horizontal advantage "; break;
        case 3: out << "Horizontal force "; break;
    }
    switch( axisB )
    {
        case 0: out << "Vertical triumph,"; break;
        case 1: out << "Vertical success,"; break;
        case 2: out << "Vertical advantage,"; break;
        case 3: out << "Vertical force,"; break;
    }
    for( int A = minAxisA; A <= maxAxisA; ++A )
    {
        out << A << ",";
    }

    for( int B = maxAxisB; B >= minAxisB; --B )
    {
        out << "\n" << B << ",";
        for( int A = minAxisA; A <= maxAxisA; ++A )
        {
            eote::DieValues dummy;

            dummy.setByIndex( axisA, A );
            dummy.setByIndex( axisB, B );

            auto s = simplifiyed_table.find( dummy );

            if( s == simplifiyed_table.end() )
            {
                out << ",";
                continue;
            }

            out << s->second << ",";
        }
    }
}

void
GetMinMaxValues( const std::map< eote::DieValues, uintmax_t >& inputProbList, eote::DieValues& min, eote::DieValues& max, uint64_t& numprobs )
{
    for( std::size_t i = 0; i < 4; ++i )
    {
        min.setByIndex( i, inputProbList.begin()->first[i] );
        max.setByIndex( i, inputProbList.begin()->first[i] );
    }

    for( std::pair< eote::DieValues, uintmax_t > pair: inputProbList )
    {
        auto dieResult = pair.first;
        auto probValue = pair.second;

        numprobs += probValue;

        for( std::size_t i = 0; i < 4; ++i )
        {
            if( dieResult[i] < min[i] ) { min.setByIndex( i, dieResult[i] ); }
            if( dieResult[i] > max[i] ) { max.setByIndex( i, dieResult[i] ); }
        }
    }
}

void
PrintAtLeast( std::size_t axis, const std::map< eote::DieValues, uintmax_t >& inputProbList, const eote::DieValues& min, const eote::DieValues& max, uint64_t numProbs, std::ostream& out = std::cout )
{
    //assert( axis < 4 );
    if( axis >= 4 )
    {
        return;
    }

    for( int i = min[axis]; i <= max[axis]; ++i )
    {
        uint64_t current = 0;

        for( std::pair< eote::DieValues, uintmax_t > pair : inputProbList )
        {
            auto dieResult = pair.first;
            auto probValue = pair.second;

            if( dieResult[axis] >= i )
            {
                current += probValue;
            }
        }

        switch(axis)
        {
            case 0: out << "At least " << i << " Triumph: " << (long double)(current)/(long double)(numProbs)*100.0 << " (" << current << "/" << numProbs << ")\n"; break;
            case 1: out << "At least " << i << " Success: " << (long double)(current)/(long double)(numProbs)*100.0 << " (" << current << "/" << numProbs << ")\n"; break;
            case 2: out << "At least " << i << " Advantage: " << (long double)(current)/(long double)(numProbs)*100.0 << " (" << current << "/" << numProbs << ")\n"; break;
            case 3: out << "At least " << i << " Force: " << (long double)(current)/(long double)(numProbs)*100.0 << " (" << current << "/" << numProbs << ")\n"; break;
        }
    }
}

int
main()
{
    std::ios::sync_with_stdio( false );

    std::vector< eote::Roll::Die > dieList;

    //dieList.push_back( eote::Roll::Die::Yellow );
    //dieList.push_back( eote::Roll::Die::Yellow );
    //dieList.push_back( eote::Roll::Die::Yellow );
    //dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Green );
    //dieList.push_back( eote::Roll::Die::Green );
    //dieList.push_back( eote::Roll::Die::Green );

    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Black );

    /*dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Green );*/


    /*do
    {
        dieList.push_back( eote::Roll::Die::White );
    }
    while( eote::DiceListTotalPossibleSideCombinations( dieList ) != 0 );
    dieList.pop_back();*/

    /*dieList.push_back( eote::Roll::Die::Yellow );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Green );
    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Purple );
    dieList.push_back( eote::Roll::Die::Blue );
    dieList.push_back( eote::Roll::Die::Blue );
    dieList.push_back( eote::Roll::Die::Blue );
    dieList.push_back( eote::Roll::Die::Black );
    dieList.push_back( eote::Roll::Die::White );*/


    uintmax_t count = 500000000000ull;

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
        return 0;
    }
    //std::cout << "" << (combo_possibilities ? combo_possibilities : "ERROR: too many possibilities to hold in uint64_t" ) << " combinations possible.\n" << std::endl;

    //std::cout.imbue(std::locale(""));


    //auto table = GetProbabilityTable<8>( dieList );

    auto table = GetProbabilityTable_TableCombineMethod( dieList );


    /*std::vector< eote::Roll::Die > dieList_A;
    std::vector< eote::Roll::Die > dieList_B;

    dieList_A.push_back( eote::Roll::Die::Blue );

    dieList_B.push_back( eote::Roll::Die::Black );


    std::cout << "\n Table Method:\n\n";

    PrintProbabilityTable_Counts( CombineTableProbabilities( GetProbabilityTable< 1, false, false >( dieList_A ), GetProbabilityTable< 1, false, false >( dieList_B ) ) );

    std::cout << "\n Iterative Method:\n\n";

    dieList_A.insert( dieList_A.end(), dieList_B.begin(), dieList_B.end() );

    PrintProbabilityTable_Counts( GetProbabilityTable< 1, false, false >( dieList_A ) );

    return 0;*/

    // output section
    {
        std::cout << std::endl;

        std::cout.precision( 8 );

        std::vector< std::pair< uintmax_t, eote::DieValues > > probList;

        std::ofstream out_allProb( "./allProb.csv" );
        std::ofstream out_rawProb( "./rawProb.csv" );


        {
            /*std::ofstream out_areaProb_0_1( "./areaProb_0_1.csv" );
            ProbabilityTableToAreaCSV( table, 0, 1, out_areaProb_0_1 );
            out_areaProb_0_1.close();

            std::ofstream out_areaProb_2_0( "./areaProb_2_0.csv" );
            ProbabilityTableToAreaCSV( table, 2, 0, out_areaProb_2_0 );
            out_areaProb_2_0.close();

            std::ofstream out_areaProb_0_3( "./areaProb_0_3.csv" );
            ProbabilityTableToAreaCSV( table, 0, 3, out_areaProb_0_3 );
            out_areaProb_0_3.close();*/

            std::ofstream out_areaProb_1_2( "./areaProb_1_2.csv" );
            ProbabilityTableToAreaCSV( table, 1, 2, out_areaProb_1_2 );
            out_areaProb_1_2.close();

            /*std::ofstream out_areaProb_1_3( "./areaProb_1_3.csv" );
            ProbabilityTableToAreaCSV( table, 1, 3, out_areaProb_1_3 );
            out_areaProb_1_3.close();

            std::ofstream out_areaProb_2_3( "./areaProb_2_3.csv" );
            ProbabilityTableToAreaCSV( table, 2, 3, out_areaProb_2_3 );
            out_areaProb_2_3.close();*/
        }

        std::cout << "\nProbability Sorted By Advantageousness:\n" << std::endl;

        for( auto dp : table )
        {
            out_allProb << dp.first.toString(true) << ",prob:," << std::scientific <<  (long double)(dp.second)/(long double)(count)*100.0 << "%,occurrences:," << dp.second << std::endl;
            out_rawProb << dp.first[0] << "," << dp.first[1] << "," << dp.first[2] << "," << dp.first[3] << "," << dp.second << std::endl;
            std::cout << "p:" << (long double)(dp.second)/(long double)(count)*100.0 << "%  " << std::string(dp.first) << "\n";
            probList.push_back( std::pair< uintmax_t, eote::DieValues >( dp.second, dp.first ) );
        }

        out_allProb.close();
        out_rawProb.close();

        std::sort( probList.begin(), probList.end(), []( const std::pair<uintmax_t, eote::DieValues>& a, const std::pair<uintmax_t, eote::DieValues>& b ){ return a.first>b.first; } );

        std::map< uintmax_t, std::map<eote::DieValues, uintmax_t> > buckets;

        std::ofstream out_sortProb( "./sortProb.csv" );

        const uintmax_t numBuckets = 12;

        std::cout << "\nProbability Sorted By Probability:\n" << std::endl;
        uintmax_t cuumprob = 0;
        for( auto dp : probList )
        {
            long double percent = (long double)(dp.first)/(long double)(count)*100.0;
            cuumprob += dp.first;
            long double cumprob_f = (long double)(cuumprob)/(long double)(count)*100.0;
            out_sortProb << dp.second.toString(true) << ",prob:," << std::scientific << percent << "%,occurrences:," << dp.first << ",cuumProb:," << std::scientific << cumprob_f << "%" << std::endl;
            std::cout << "p:" << percent << "%(" << cumprob_f <<  "%)  " << std::string(dp.second) << "\n";

            //buckets[ std::min<int>(int((long double)(cumprob_f)/100.0*(long double)(numBuckets)),numBuckets-1) ][ dp.second ] += dp.first;
            //buckets[ std::min<uintmax_t>((cuumprob*numBuckets)/(count),numBuckets-1) ][ dp.second ] += dp.first;
            buckets[ std::clamp<uintmax_t>( (cuumprob*numBuckets)/(count), 0, numBuckets-1) ][ dp.second ] += dp.first;
        }

        out_sortProb.close();

        // TODO: redo the bucket system.

        std::cout << "\nAt least one of:\n\n";

        //auto defaultDieValue = eote::DieValues();
        /*
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

            //std::cout << std::setw(3) << std::setprecision(0) << std::right << int((bucketid+1)/double(buckets.size())*100.0) << "th percentile: ";
            std::cout << std::setw(3) << std::setprecision(0) << std::right << (bucketid+1) << (bucketid<3?((bucketid==0)?("st"):(bucketid<2?"nd":"rd")):"th");
            std::cout << " bucket (" << std::setw(numInBucket>=count?3:2) << int((long double)(numInBucket)/(long double)(count)*100.0) << "%ofRolls): ";
            //std::cout << "count:" << std::setw(3) << std::left << numInBucket;
            std::cout << " t:" << std::setw(5) << std::right << std::setprecision(numTriumphInBucket>=numInBucket?1:2)   << double(numTriumphInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " s:" << std::setw(5) << std::right << std::setprecision(numSuccessInBucket>=numInBucket?1:2)   << double(numSuccessInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " a:" << std::setw(5) << std::right << std::setprecision(numAdvantageInBucket>=numInBucket?1:2) << double(numAdvantageInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " lf:" << std::setw(5) << std::right << std::setprecision(numForceInBucket>=numInBucket?1:2)    << double(numForceInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " |";
            std::cout << " d:" << std::setw(5) << std::right << std::setprecision(numDispareInBucket>=numInBucket?1:2)      << double(numDispareInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " f:" << std::setw(5) << std::right << std::setprecision(numFailInBucket>=numInBucket?1:2)         << double(numFailInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " v:" << std::setw(5) << std::right << std::setprecision(numDisadvantageInBucket>=numInBucket?1:2) << double(numDisadvantageInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " df:" << std::setw(5) << std::right << std::setprecision(numDarkInBucket>=numInBucket?1:2)        << double(numDarkInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " (" << numInBucket << ")";

            std::cout << std::endl;
        }
        */
        /*
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
            std::cout << " t:" << std::setw(5) << std::right << std::setprecision(numTriumphInBucket>=numInBucket?1:2)   << double(numTriumphInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " s:" << std::setw(5) << std::right << std::setprecision(numSuccessInBucket>=numInBucket?1:2)   << double(numSuccessInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " a:" << std::setw(5) << std::right << std::setprecision(numAdvantageInBucket>=numInBucket?1:2) << double(numAdvantageInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " lf:" << std::setw(5) << std::right << std::setprecision(numForceInBucket>=numInBucket?1:2)    << double(numForceInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " |";
            std::cout << " d:" << std::setw(5) << std::right << std::setprecision(numDispareInBucket>=numInBucket?1:2)      << double(numDispareInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " f:" << std::setw(5) << std::right << std::setprecision(numFailInBucket>=numInBucket?1:2)         << double(numFailInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " v:" << std::setw(5) << std::right << std::setprecision(numDisadvantageInBucket>=numInBucket?1:2) << double(numDisadvantageInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " df:" << std::setw(5) << std::right << std::setprecision(numDarkInBucket>=numInBucket?1:2)        << double(numDarkInBucket)/double(numInBucket)*100.0 << "%";
            std::cout << " (" << numInBucket << ")";

            std::cout << "\n\ntotal unique combinations: " << table.size() << std::endl;
        }
        */
    }

    {
        std::cout << "\n";
        std::cout << "\n";

        eote::DieValues min, max;
        uint64_t numprobs;
        GetMinMaxValues( table, min, max, numprobs );

        PrintAtLeast( 0, table, min, max, combo_possibilities, std::cout );
        std::cout << "\n";
        PrintAtLeast( 1, table, min, max, combo_possibilities, std::cout );
        std::cout << "\n";
        PrintAtLeast( 2, table, min, max, combo_possibilities, std::cout );
        std::cout << "\n";
        PrintAtLeast( 3, table, min, max, combo_possibilities, std::cout );
        std::cout << "\n";
    }

    std::cout << table.size() << " unique outcomes from " << combo_possibilities << " permutations\n";

    std::cout << std::endl;

    return 0;
}

#include "includes.h"

#include "die_values.h"
#include "roll.h"
#include "table_prob.h"


namespace std
{
    template < typename Type >
    Type
    clamp( const Type& v, const Type& a, const Type& b )
    {
        return std::max<Type>( std::min<Type>( v, b ), a );
    }

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


void PrintProbabilityTable_Counts( std::map< eote::DieValues, uintmax_t > table, std::ostream& out = std::cout );
std::map< eote::DieValues, uintmax_t > SimplifyTableForAxies( std::map< eote::DieValues, uintmax_t > table, uint8_t axisA, uint8_t axisB );
void ProbabilityTableToAreaCSV( std::map< eote::DieValues, uintmax_t > table, uint8_t axisA, uint8_t axisB, std::ostream& out );
void GetMinMaxValues( const std::map< eote::DieValues, uintmax_t >& inputProbList, eote::DieValues& min, eote::DieValues& max, uint64_t& numprobs );
void PrintAtLeast( std::size_t axis, const std::map< eote::DieValues, uintmax_t >& inputProbList, const eote::DieValues& min, const eote::DieValues& max, uint64_t numProbs, std::ostream& out = std::cout );


int
main()
{
    std::ios::sync_with_stdio( false );

    std::map< eote::Roll::Die, uintmax_t > dieList;

    dieList[ eote::Roll::Die::Blue ] =   1;
    dieList[ eote::Roll::Die::Yellow ] = 1;
    dieList[ eote::Roll::Die::Green ] =  1;

    dieList[ eote::Roll::Die::Black ] =  1;
    dieList[ eote::Roll::Die::Red ] =    1;
    dieList[ eote::Roll::Die::Purple ] = 1;

    //dieList[ eote::Roll::Die::White ] =  1;

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

    auto table = eote::GetProbabilityTable( dieList );

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

        std::ofstream out_sortProb( "./sortProb.csv" );

        std::cout << "\nProbability Sorted By Probability:\n" << std::endl;
        uintmax_t cuumprob = 0;
        for( auto dp : probList )
        {
            long double percent = (long double)(dp.first)/(long double)(count)*100.0;
            cuumprob += dp.first;
            long double cumprob_f = (long double)(cuumprob)/(long double)(count)*100.0;
            out_sortProb << dp.second.toString(true) << ",prob:," << std::scientific << percent << "%,occurrences:," << dp.first << ",cuumProb:," << std::scientific << cumprob_f << "%" << std::endl;
            std::cout << "p:" << percent << "%(" << cumprob_f <<  "%)  " << std::string(dp.second) << "\n";
        }

        out_sortProb.close();
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



void
PrintProbabilityTable_Counts( std::map< eote::DieValues, uintmax_t > table, std::ostream& out )
{
    for( auto dp : table )
    {
        out << "p:" << dp.second << "  " << std::string( dp.first ) << "\n";
    }
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
PrintAtLeast( std::size_t axis, const std::map< eote::DieValues, uintmax_t >& inputProbList, const eote::DieValues& min, const eote::DieValues& max, uint64_t numProbs, std::ostream& out )
{
    axis %= 4;

    std::string axis_str;

    switch( axis )
    {
        case 0: axis_str = "Triumph"; break;
        case 1: axis_str = "Success"; break;
        case 2: axis_str = "Advantage"; break;
        case 3: axis_str = "Force"; break;
    }

    for( int i = min[axis]; i <= max[axis]; ++i )
    {
        uint64_t current = 0;

        for( auto pair : inputProbList )
        {
            auto dieResult = pair.first;
            auto probValue = pair.second;

            if( dieResult[axis] >= i )
            {
                current += probValue;
            }
        }

        out << "At least " << i << " " << axis_str << ": " << (long double)(current)/(long double)(numProbs)*100.0 << " (" << current << "/" << numProbs << ")\n";
    }
}

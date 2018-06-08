#ifndef TABLE_PROB_H_INCLUDED
#define TABLE_PROB_H_INCLUDED

#include "includes.h"

#include "die_values.h"

namespace eote
{
    inline std::map< DieValues, uintmax_t > CombineTableProbabilities( std::map< DieValues, uintmax_t > tableA, std::map< DieValues, uintmax_t > tableB );

    inline std::map< DieValues, uintmax_t > GetProbabilityTable( Roll::Die die );
    inline std::map< DieValues, uintmax_t > GetProbabilityTable( Roll::Die die, uintmax_t count );
    inline std::map< DieValues, uintmax_t > GetProbabilityTable( std::vector< Roll::Die > dieList );
    inline std::map< DieValues, uintmax_t > GetProbabilityTable( std::map< Roll::Die, uintmax_t > dieMap );
    inline std::map< DieValues, uintmax_t > GetProbabilityTable_MonteCarlo( std::vector< Roll::Die > dieList, uintmax_t count = 0 );
    inline std::map< DieValues, uintmax_t > GetProbabilityTable_Iterative( std::vector< Roll::Die > dieList );


    template < uint16_t NUM_THREADS = 2, bool roll_randomly = false, bool print_progress = true >
    inline std::map< DieValues, uintmax_t > _AproximateProbabilityTable( std::vector< Roll::Die > dieList, uintmax_t count = 0 );
}

#include "table_prob_templates.inl"
#include "table_prob.inl"


#endif // TABLE_PROB_H_INCLUDED

#ifndef TABLE_PROB_INL_INCLUDED
#define TABLE_PROB_INL_INCLUDED

namespace eote
{
    std::map< DieValues, uintmax_t >
    CombineTableProbabilities( std::map< DieValues, uintmax_t > tableA, std::map< DieValues, uintmax_t > tableB )
    {
        // enforce that there are fewer entries in tableB
        if( tableA.size() < tableB.size() )
        {
            return CombineTableProbabilities( tableB, tableA );
        }

        // if one table is empty return the other. (don't need the switched logic because of the above enforced size rules)
        if( tableA.size() && !tableB.size() )
        {
            return tableA;
        }

        std::map< DieValues, uintmax_t > table_out;

        for( auto entryB : tableB )
        {
            for( auto entryA : tableA )
            {
                table_out[ entryA.first + entryB.first ] += entryA.second * entryB.second;
            }
        }

        return table_out;
    }

    std::map< DieValues, uintmax_t >
    GetProbabilityTable( Roll::Die die )
    {
        //return GetProbabilityTable_Iterative( std::vector< Roll::Die >{ die } );

        std::map< DieValues, uintmax_t > table;

        uintmax_t count = DiceListTotalPossibleSideCombinations( die );

        for( uintmax_t i = 0; i < count; ++i )
        {
            table[ _IterateDie( die, i ) ] += 1;
        }

        return table;
    }

    std::map< DieValues, uintmax_t >
    GetProbabilityTable( Roll::Die die, uintmax_t count )
    {
        //return GetProbabilityTable_Iterative( std::vector< Roll::Die >{ die } );

        std::map< DieValues, uintmax_t > table;

        while( count-- )
        {
            table = CombineTableProbabilities( table, GetProbabilityTable( die ) );
        }

        return table;
    }

    std::map< DieValues, uintmax_t >
    GetProbabilityTable( std::vector< Roll::Die > dieList )
    {
        std::map< DieValues, uintmax_t > table;

        for( auto die : dieList )
        {
            table = CombineTableProbabilities( table, GetProbabilityTable( die ) );
        }

        return table;
    }

    std::map< DieValues, uintmax_t >
    GetProbabilityTable( std::map< Roll::Die, uintmax_t > dieMap )
    {
        std::map< DieValues, uintmax_t > table;

        for( auto diePair : dieMap )
        {
            Roll::Die die = diePair.first;
            uintmax_t count = diePair.second;

            table = CombineTableProbabilities( table, GetProbabilityTable( die, count ) );
        }

        return table;
    }

    std::map< DieValues, uintmax_t >
    GetProbabilityTable_MonteCarlo( std::vector< Roll::Die > dieList, uintmax_t count )
    {
        return _AproximateProbabilityTable< 2, true, false >( dieList, count );
    }

    std::map< DieValues, uintmax_t >
    GetProbabilityTable_Iterative( std::vector< Roll::Die > dieList )
    {
        return _AproximateProbabilityTable< 2, false, false >( dieList );
    }
}

#endif // TABLE_PROB_INL_INCLUDED

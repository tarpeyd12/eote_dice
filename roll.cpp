#include "roll.h"

#include "random.h"

namespace eote
{
    namespace Roll
    {
        DieValues
        _Blue()
        {
            switch( Random::Int( 0, 5 ) )
            {
                case 0:
                case 1:
                    return Value::Blank(); // blank
                case 2:
                    return Value::Success(); // 1 success
                case 3:
                    return Value::Advantage(); // 1 advantage
                case 4:
                    return Value::Advantage( 2 ); // 2 advantage
                case 5:
                    return Value::Success() + Value::Advantage(); // 1 success 1 advantage
                default:
                    return Value::Blank();
            }
            return Value::Blank();
        }

        DieValues
        _Black()
        {
            switch( Random::Int( 0, 5 ) )
            {
                default:
                case 0:
                case 1:
                    return Value::Blank(); // blank
                case 2:
                case 3:
                    return Value::Disadvantage(); // 1 disadvantage
                case 4:
                case 5:
                    return Value::Failure(); // 1 failure
            }
            return Value::Blank();
        }

        DieValues
        _Red()
        {
            switch( Random::Int( 0, 11 ) )
            {
                default:
                case 0:
                    return Value::Blank(); // blank
                case 1:
                case 2:
                    return Value::Failure(2);
                case 3:
                case 4:
                    return Value::Failure();
                case 5:
                case 6:
                    return Value::Disadvantage();
                case 7:
                case 8:
                    return Value::Disadvantage(2);
                case 9:
                case 10:
                    return Value::Failure() + Value::Disadvantage(); // 1 success 1 advantage
                case 11:
                    return Value::Despair();
            }
            return Value::Blank();
        }

        DieValues
        _Yellow()
        {
            switch( Random::Int( 0, 11 ) )
            {
                default:
                case 0:
                    return Value::Blank(); // blank
                case 1:
                case 2:
                    return Value::Success(2); // 2 success
                case 3:
                case 4:
                    return Value::Advantage(2); // 2 advantage
                case 5:
                case 6:
                    return Value::Success(); // 1 success
                case 7:
                    return Value::Advantage(); // 1 advantage
                case 8:
                case 9:
                case 10:
                    return Value::Success() + Value::Advantage(); // 1 success 1 advantage
                case 11:
                    return Value::Triumph();
            }
            return Value::Blank();
        }

        DieValues
        _Green()
        {
            switch( Random::Int( 0, 7 ) )
            {
                default:
                case 0:
                    return Value::Blank();
                case 1:
                    return Value::Success(2);
                case 2:
                case 3:
                    return Value::Success();
                case 4:
                case 5:
                    return Value::Advantage();
                case 6:
                    return Value::Advantage(2);
                case 7:
                    return Value::Success() + Value::Advantage();
            }
            return Value::Blank();
        }

        DieValues
        _Purple()
        {
            switch( Random::Int( 0, 7 ) )
            {
                default:
                case 0:
                    return Value::Blank();
                case 1:
                case 2:
                case 3:
                    return Value::Disadvantage();
                case 4:
                    return Value::Disadvantage(2);
                case 5:
                    return Value::Failure();
                case 6:
                    return Value::Failure(2);
                case 7:
                    return Value::Failure() + Value::Disadvantage();
            }
            return Value::Blank();
        }

        DieValues
        _White()
        {
            switch( Random::Int( 0, 11 ) )
            {
                default:
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    return Value::Dark();
                case 6:
                case 7:
                case 8:
                    return Value::Light(2);
                case 9:
                case 10:
                    return Value::Light();
                case 11:
                    return Value::Dark(2);
            }
            return Value::Blank();
        }

    }

}

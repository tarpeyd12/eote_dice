#include "roll.h"

#include "random.h"

namespace eote
{
    namespace Roll
    {
        namespace _Iterate
        {
            DieValues _Blue( uintmax_t count, uintmax_t * carry )
            {
                if( carry )
                {
                    *carry = count / 6;
                }

                switch( count % 6 )
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

            DieValues _Black( uintmax_t count, uintmax_t * carry )
            {
                if( carry )
                {
                    *carry = count / 6;
                }

                switch( count % 6 )
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

            DieValues _Red( uintmax_t count, uintmax_t * carry )
            {
                if( carry )
                {
                    *carry = count / 12;
                }

                switch( count % 12 )
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

            DieValues _Yellow( uintmax_t count, uintmax_t * carry )
            {
                if( carry )
                {
                    *carry = count / 12;
                }

                switch( count % 12 )
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

            DieValues _Green( uintmax_t count, uintmax_t * carry )
            {
                if( carry )
                {
                    *carry = count / 8;
                }

                switch( count % 8 )
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

            DieValues _Purple( uintmax_t count, uintmax_t * carry )
            {
                if( carry )
                {
                    *carry = count / 8;
                }

                switch( count % 8 )
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

            DieValues _White( uintmax_t count, uintmax_t * carry )
            {
                if( carry )
                {
                    *carry = count / 12;
                }

                switch( count % 12)
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

        DieValues
        _Blue( Random::RandomFunctor * _rand )
        {
            return _Iterate::_Blue( _rand != nullptr ? _rand->Int( 0, 5 ) : Random::Int( 0, 5 ) );
        }

        DieValues
        _Black( Random::RandomFunctor * _rand )
        {
            return _Iterate::_Black( _rand != nullptr ? _rand->Int( 0, 5 ) : Random::Int( 0, 5 ) );
        }

        DieValues
        _Red( Random::RandomFunctor * _rand )
        {
            return _Iterate::_Red( _rand != nullptr ? _rand->Int( 0, 11 ) : Random::Int( 0, 11 ) );
        }

        DieValues
        _Yellow( Random::RandomFunctor * _rand )
        {
            return _Iterate::_Yellow( _rand != nullptr ? _rand->Int( 0, 11 ) : Random::Int( 0, 11 ) );
        }

        DieValues
        _Green( Random::RandomFunctor * _rand )
        {
            return _Iterate::_Green( _rand != nullptr ? _rand->Int( 0, 7 ) : Random::Int( 0, 7 ) );
        }

        DieValues
        _Purple( Random::RandomFunctor * _rand )
        {
            return _Iterate::_Purple( _rand != nullptr ? _rand->Int( 0, 7 ) : Random::Int( 0, 7 ) );
        }

        DieValues
        _White( Random::RandomFunctor * _rand )
        {
            return _Iterate::_White( _rand != nullptr ? _rand->Int( 0, 11 ) : Random::Int( 0, 11 ) );
        }

    }

}

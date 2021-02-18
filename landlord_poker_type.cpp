/*
 * @Descripttion: 
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-10 11:30:13
 * @LastEditors: huazy
 * @LastEditTime: 2020-10-14 21:15:07
 */
#include "landlord_poker_type.h"

namespace ccserver
{
    namespace landlord
    {

        int LandlordTypeScore::get_poker_type_score(PokerType poker)
        {
            int score = 0;
            int poker_type = poker.poker_type;
            if (!poker_type)
            {
                LOG_error("error poker type!");
                return score;
            }

            if (poker_type == Landlord2Type::Single1)
                score = get_poker_single_one_score(poker);
            if (poker_type == Landlord2Type::Double1)
                score = get_poker_double_one_score(poker);
            if (poker_type >= Landlord2Type::Single5 && poker_type <= Landlord2Type::Single12)
                score = get_poker_single_score(poker);
            if (poker_type >= Landlord2Type::Double3 && poker_type <= Landlord2Type::Double10)
                score = get_poker_double_score(poker);
            if (poker_type == Landlord2Type::ThreeSingle1 || poker_type == Landlord2Type::ThreeDouble1 ||
                poker_type == Landlord2Type::Three1)
                score = get_poker_three_one_score(poker);
            if ((poker_type >= Landlord2Type::ThreeSingle2 && poker_type <= Landlord2Type::ThreeSingle5) ||
                (poker_type >= Landlord2Type::ThreeDouble2 && poker_type <= Landlord2Type::ThreeDouble4))
                score = get_poker_plane_score(poker);
            if (poker_type >= Landlord2Type::Three2 && poker_type <= Landlord2Type::Three6)
                score = get_poker_three_socre(poker);
            if (poker_type == Landlord2Type::Bomb)
                score = get_poker_bomb_score(poker);
            if (poker_type == Landlord2Type::Rocket)
                score = get_poker_rocket_score(poker);
            if (poker_type == Landlord2Type::FourSingle1 || poker_type == Landlord2Type::FourDouble1)
                score == get_poker_four_score(poker);
            else
            {
                LOG_warn("wrong poker type: {}", poker_type);
                return score;
            }
        }

        int LandlordTypeScore::get_poker_base_score(int value)
        {
            return value - SCORE_OFFSET;
        }

        int LandlordTypeScore::get_poker_bomb_score(PokerType poker)
        {
            return poker.value - utils::PokerValue::P3 + SCORE_OFFSET - utils::PokerValue::P3;
        }

        int LandlordTypeScore::get_poker_double_one_score(PokerType poker)
        {
            return poker.value - SCORE_OFFSET;
        }

        int LandlordTypeScore::get_poker_double_score(PokerType poker)
        {
            return poker.value - SCORE_OFFSET;
        }

        int LandlordTypeScore::get_poker_four_score(PokerType poker)
        {
            return (poker.value - utils::PokerValue::P3) / 2;
        }

        int LandlordTypeScore::get_poker_plane_score(PokerType poker)
        {
            return (poker.value - utils::PokerValue::P3) / 2;
        }

        int LandlordTypeScore::get_poker_rocket_score(PokerType poker)
        {
            return 20;
        }

        int LandlordTypeScore::get_poker_single_one_score(PokerType poker)
        {
            return get_poker_base_score(poker.value);
        }

        int LandlordTypeScore::get_poker_single_score(PokerType poker)
        {
            return get_poker_base_score(poker.value);
        }

        int LandlordTypeScore::get_poker_three_one_score(PokerType poker)
        {
            return get_poker_base_score(poker.value);
        }

        int LandlordTypeScore::get_poker_three_socre(PokerType poker)
        {
            return get_poker_base_score(poker.value);
        }

        void LandlordTypeCheck::remove_no_compose(PokerSet data)
        {
            //手牌中是否有王和2

            //找出手牌中的单牌
        }
    } // namespace landlord
} // namespace ccserver
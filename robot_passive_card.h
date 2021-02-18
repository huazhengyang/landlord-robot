/*
 * @Descripttion: 机器人被动出牌逻辑
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-14 20:26:35
 * @LastEditors: huazy
 * @LastEditTime: 2021-02-18 17:32:45
 */
#ifndef ROBOT_PASSIVE_CARD_H
#define ROBOT_PASSIVE_CARD_H
#include "core/utils/poker.h"
//#include "hand_card_data.h"
#include "rule.h"

//todo 做一个ccpb::landlord::Landlord2PlayPokers到CardGroupData的转换，Landlord2PlayPokers所包含的信息太少了
namespace ccserver
{
    namespace landlord
    {
       //被动出牌执行类
       class PassiveCard
       {
       public:
           PassiveCard() {}
           virtual ~PassiveCard() {}
       public:
           //被动出牌
           static void passive_card(CardGroupData last_poker, HandCardData hand_card_data);

       };

       //被动出牌策略类
       class PassiveCardStrategy
       {
       public:
           PassiveCardStrategy() {}
           virtual ~PassiveCardStrategy() {}

       public:
           //获取被动出牌的出牌序列
           static void get_passive_put_card_list(CardGroupData last_poker, HandCardData& hand_card_data);

           //尝试打出最后一手牌
           static bool try_out_last_hand(CardGroupData last_poker, HandCardData& hand_card_data);

           //尝试打出相同的牌型
           static bool try_out_same_type(CardGroupData last_poker, HandCardData& hand_card_data, HandCardValue& best_card_value);

           //尝试打出炸弹
           static bool try_out_bomb(HandCardData& hand_card_data, HandCardValue& best_card_value);

           //尝试打出火箭
           static bool try_out_rocket(HandCardData& hand_card_data, HandCardValue& best_card_value);

           //单牌的被动出牌策略
           static bool single_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //对牌的被动出牌策略
           static bool double_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //三牌的被动出牌策略
           static bool three_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //单连的被动出牌策略
           static bool single_line_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //双连的被动出牌策略
           static bool double_line_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //三连的被动出牌策略
           static bool three_line_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //三带一单的被动出牌策略
           static bool three_one_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //三带一对的被动出牌策略
           static bool three_pair_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //飞机带单的被动出牌策略
           static bool plane_one_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //飞机带对的被动出牌策略
           static bool plane_pair_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //四带两单的被动出牌策略
           static bool four_one_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //四带两对的被动出牌策略
           static bool four_pair_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);

           //炸弹的被动出牌策略
           static bool bomb_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data);
       };
    }
}
#endif
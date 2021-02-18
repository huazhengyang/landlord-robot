/*
 * @Descripttion: 机器人主动出牌逻辑 
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-14 20:13:49
 * @LastEditors: huazy
 * @LastEditTime: 2021-02-18 17:32:16
 */
#ifndef ROBOT_INITIATIVE_CARD_H
#define ROBOT_INITIATIVE_CARD_H
#include <vector>
#include "card_define.h"
//#include "hand_card_data.h"
#include "rule.h"

using std::vector;

namespace ccserver
{
namespace landlord
{
//主动出牌
class InitiativeCard
{
public:
   InitiativeCard() {}
   virtual ~InitiativeCard() {}

public:
   //主动出牌逻辑
   /*
   1.优先找到能带牌出的，三带一，三带对，飞机，这些牌型会把低价值的牌带出去，达到手牌价值最大化，注意查找这些牌型时会全枚举，找到最佳的，
     这个策略还有优化的空间，可以将10以上的先保留
   2.上面的找过后，没有获取到最佳价值牌型，则由小面值牌开始遍历，当一个面值的牌型中有，单牌，对牌，三牌，单连，对连，三连中的一种，
     并获取到最佳牌型(即剩余手牌最大值)，返回，因为这个是牌值最小的
   3.上面的没有，找单王，双王的情况，函数开始时已经做了筛选
   4.最后找炸弹
   */
   static void initiative_card(HandCardData& hand_card_data);

   //获取主动出牌的序列,尝试所有牌型，找出最优解
   static void get_initiative_put_card_list(HandCardData& hand_card_data);

   //获取手牌价值，基于主动出牌逻辑，和get_initiative_put_card_list互相递归调用
   static HandCardValue get_hand_card_value(HandCardData& hand_card_data);
};

//主动出牌策略
class InitiativeCardStrategy
{
public:
   //尝试出三带一
   static void try_out_three_take_one_card(HandCardData& hand_card_data, int card_num, HandCardValue& best_hand_card_value,
                                           CardGroupData& best_card_group, vector<int>& additional);

   //尝试出三带一对
   static void try_out_three_take_pair_card(HandCardData& hand_card_data, int card_num, HandCardValue& best_hand_card_value,
                                           CardGroupData& best_card_group, vector<int>& additional);

   //尝试出四带两单
   static void try_out_four_take_one_card(HandCardData& hand_card_data);

   //尝试出四带两对
   static void try_out_four_take_pair_card(HandCardData& hand_card_data);

   //尝试出飞机带单
   static void try_out_plane_take_one_card(HandCardData& hand_card_data, int card_num, HandCardValue& best_hand_card_value,
                                           CardGroupData& best_card_group, vector<int>& additional);
   //尝试出飞机带对
   static void try_out_plane_take_pair_card(HandCardData& hand_card_data, int card_num, HandCardValue& best_hand_card_value,
                                           CardGroupData& best_card_group, vector<int>& additional);

public:
   //尝试出单牌
   static void try_out_single_card(HandCardData& hand_card_data, int card_num,
                                   HandCardValue& best_hand_card_value, CardGroupData& best_card_group);

   //尝试出对牌
   static void try_out_double_card(HandCardData& hand_card_data, int card_num,
                                   HandCardValue& best_hand_card_value, CardGroupData& best_card_group);

   //尝试出三张不带
   static void try_out_three_card(HandCardData& hand_card_data, int card_num,
                                   HandCardValue& best_hand_card_value, CardGroupData& best_card_group);

   //尝试出单顺
   static void try_out_single_line_card(HandCardData& hand_card_data, int card_num,
                                   HandCardValue& best_hand_card_value, CardGroupData& best_card_group);

   //尝试出双顺
   static void try_out_double_line_card(HandCardData& hand_card_data, int card_num,
                                   HandCardValue& best_hand_card_value, CardGroupData& best_card_group);

   //尝试出三顺
   static void try_out_three_line_card(HandCardData& hand_card_data, int card_num,
                                   HandCardValue& best_hand_card_value, CardGroupData& best_card_group);
};
}
}
#endif
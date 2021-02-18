/*
 * @Descripttion: 机器人被动出牌逻辑,包含牌型检测
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-14 21:00:57
 * @LastEditors: huazy
 * @LastEditTime: 2021-02-18 17:32:33
 */
#include <vector>

#include "robot_passive_card.h"
#include "robot_initiative_card.h"
#include "card_type.h"

namespace ccserver
{
namespace landlord
{
//每种牌型都分了牌型判断，有角色的策略可以在判断中添加
void PassiveCardStrategy::get_passive_put_card_list(CardGroupData last_poker, HandCardData& hand_card_data)
{
   //对手出的是王炸
   if (last_poker.cgType == cgKING_CARD)
       return;

   /*王炸——当前策略只处理王炸作为倒数第二手的优先出牌逻辑，后续版本会在此基础上优化*/
   if (hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] == 1 &&
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] == 1)
   {

       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1]--;
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2]--;
       hand_card_data.m_card_cnt -= 2;
       HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1]++;
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2]++;
       hand_card_data.m_card_cnt += 2;
       if (tmpHandCardValue.NeedRound == 1)
       {
           hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker1);
           hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker2);
           hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgKING_CARD,
                                                       ccpb::common::PokerValue::PJoker2, 2);
           return;
       }
   }

   //错误牌型  不出
   if (last_poker.cgType == cgERROR)
   {
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgERROR, 0, 0);
       return;
   }

   //不出牌型，在被动出牌策略里也是错误数据 不出
   if (last_poker.cgType == cgZERO)
   {
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
       return;
   }

   //先判断一手牌
   bool b_put_cards = try_out_last_hand(last_poker, hand_card_data);
   if (b_put_cards)
       return;

   //单牌
   if (last_poker.cgType == cgSINGLE)
   {
       if (single_card_strategy(last_poker, hand_card_data))
           return;
   }

   //对牌
   if (last_poker.cgType == cgDOUBLE)
   {
       if (double_card_strategy(last_poker, hand_card_data))
           return;
   }

   //三牌
   if (last_poker.cgType == cgTHREE)
   {
       if (three_card_strategy(last_poker, hand_card_data))
           return;
   }

   //单连
   if (last_poker.cgType == cgSINGLE_LINE)
   {
       if (single_line_card_strategy(last_poker, hand_card_data))
           return;
   }

   //对连
   if (last_poker.cgType == cgDOUBLE_LINE)
   {
       if (double_line_card_strategy(last_poker, hand_card_data))
           return;
   }

   //三连
   if (last_poker.cgType == cgTHREE_LINE)
   {
       if (three_line_card_strategy(last_poker, hand_card_data))
           return;
   }

   //三带一单
   if (last_poker.cgType == cgTHREE_TAKE_ONE)
   {
       if (three_one_card_strategy(last_poker, hand_card_data))
           return;
   }

   //三带一对
   if (last_poker.cgType == cgTHREE_TAKE_TWO)
   {
       if (three_pair_card_strategy(last_poker, hand_card_data))
           return;
   }

   //飞机带单
   if (last_poker.cgType == cgTHREE_TAKE_ONE_LINE)
   {
       if (plane_one_card_strategy(last_poker, hand_card_data))
           return;
   }

   //飞机带对
   if (last_poker.cgType == cgTHREE_TAKE_TWO_LINE)
   {
       if (plane_pair_card_strategy(last_poker, hand_card_data))
           return;
   }

   //四带两单
   if (last_poker.cgType == cgFOUR_TAKE_ONE)
   {
       if (four_one_card_strategy(last_poker, hand_card_data))
           return;
   }

   //四带两对
   if (last_poker.cgType == cgFOUR_TAKE_TWO)
   {
       if (four_pair_card_strategy(last_poker, hand_card_data))
           return;
   }

   //炸弹
   if (last_poker.cgType == cgBOMB_CARD)
   {
       if (bomb_card_strategy(last_poker, hand_card_data))
           return;
   }
}

bool PassiveCardStrategy::try_out_last_hand(CardGroupData last_poker, HandCardData& hand_card_data)
{
   CardGroupData SurCardGroupData = OneHandOutCard::one_hand_output_card(hand_card_data.value_aHandCardList,
                                                                           hand_card_data.m_card_cnt);
   if (SurCardGroupData.cgType != cgERROR)
   {
       //todo Landlord2PlayPokers last_poker这个结构中的数据需要做转换，现在先用同类型的数据替代了
       if (SurCardGroupData.cgType == last_poker.cgType && SurCardGroupData.nMaxCard > last_poker.nMaxCard &&
           last_poker.nCount == SurCardGroupData.nCount)
       {
           //出牌
           //Put_All_SurCards(clsGameSituation, clsHandCardData, SurCardGroupData);
           hand_card_data.m_put_out_card_type = SurCardGroupData;
           return true;
       }
       else if (SurCardGroupData.cgType == cgBOMB_CARD || SurCardGroupData.cgType == cgKING_CARD)
       {
           //出牌
           //Put_All_SurCards(clsGameSituation, clsHandCardData, SurCardGroupData);
           //todo 这个地方的返回值要改一下
           hand_card_data.m_put_out_card_type = SurCardGroupData;
           return true;
       }
   }

   return false;
}

bool PassiveCardStrategy::try_out_same_type(CardGroupData last_poker, HandCardData& hand_card_data, HandCardValue& best_card_value)
{
   return true;
}

bool PassiveCardStrategy::try_out_bomb(HandCardData& hand_card_data, HandCardValue& best_card_value)
{
   //最佳的牌号
   int best_max_card = 0;
   //是否出牌的标志
   bool b_put_cards = false;

   for (int i = ccpb::common::PokerValue::P3; i < ccpb::common::PokerValue::PJoker1; i++)
   {
       if (hand_card_data.value_aHandCardList[i] == 4)
       {
           //尝试打出炸弹，估算剩余手牌价值,因为炸弹可以参与顺子，不能因为影响顺子而任意出炸
           hand_card_data.value_aHandCardList[i] -= 4;
           hand_card_data.m_card_cnt -= 4;
           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
           hand_card_data.value_aHandCardList[i] += 4;
           hand_card_data.m_card_cnt += 4;

           //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
           if ((best_card_value.SumValue - (best_card_value.NeedRound * 7)) <= (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7))
               //如果剩余手牌价值为正，证明出去的几率很大， 那么可以用炸获得先手
               || tmpHandCardValue.SumValue > 0)
           {
               best_card_value = tmpHandCardValue;
               best_max_card = i;
               b_put_cards = true;
           }
       }
   }
   if (b_put_cards)
   {
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgBOMB_CARD, best_max_card, 4);
   }

   return b_put_cards;
}

bool PassiveCardStrategy::try_out_rocket(HandCardData& hand_card_data, HandCardValue& best_card_value)
{
   if (hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] == 1 &&
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] == 1)
   {
       //如果剩余手牌价值为正，证明出去的几率很大，那么可以用炸获得先手，王炸20分
       if (best_card_value.SumValue > 20)
       {
           hand_card_data.m_put_card_list.push_back(17);
           hand_card_data.m_put_card_list.push_back(16);
           hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgKING_CARD, ccpb::common::PokerValue::PJoker2, 2);
           return true;
       }
   }

   return false;
}

bool PassiveCardStrategy::single_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   //是否可以出牌的标志
   bool b_put_cards = false;

   //act 2 选取手牌中的最佳牌型
   //暂存最佳的价值
   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);

   //我们认为不出牌的话会让对手一个轮次，即加一轮（权值减少7）便于后续的对比参考。
   best_hand_card_value.NeedRound += 1;

   //暂存最佳的牌号
   int best_max_card = 0;

   for (int i = last_poker.nMaxCard + 1; i < 18; i++)
   {
       if (hand_card_data.value_aHandCardList[i] > 0)
       {
           //尝试打出一张牌，估算剩余手牌价值
           hand_card_data.value_aHandCardList[i]--;
           hand_card_data.m_card_cnt--;
           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
           hand_card_data.value_aHandCardList[i]++;
           hand_card_data.m_card_cnt++;

           //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
           if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <= (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
           {
               best_hand_card_value = tmpHandCardValue;
               best_max_card = i;
               b_put_cards = true;
           }
       }
   }
   if (b_put_cards)
   {
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_out_card_type  = CardTypeCheck::get_group_data(cgSINGLE, best_max_card, 1);
       return b_put_cards;
   }

   //act 3
   //尝试出炸弹
   b_put_cards = try_out_bomb(hand_card_data, best_hand_card_value);
   if (b_put_cards)
       return b_put_cards;

   //尝试出王炸
   b_put_cards = try_out_rocket(hand_card_data, best_hand_card_value);
   if (b_put_cards)
       return b_put_cards;

   //act 4
   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::double_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;

   for (int i = last_poker.nMaxCard + 1; i < 18; i++)
   {
       if (hand_card_data.value_aHandCardList[i] > 0)
       {
           hand_card_data.value_aHandCardList[i] -= 2;
           hand_card_data.m_card_cnt -= 2;
           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
           hand_card_data.value_aHandCardList[i] += 2;
           hand_card_data.m_card_cnt += 2;

           if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <= (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
           {
               best_hand_card_value = tmpHandCardValue;
               best_max_card = i;
               b_put_cards = true;
           }
       }
   }
   if (b_put_cards)
   {
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_out_card_type  = CardTypeCheck::get_group_data(cgDOUBLE, best_max_card, 2);
       return b_put_cards;
   }

   b_put_cards = try_out_bomb(hand_card_data, best_hand_card_value);
   if (b_put_cards)
       return b_put_cards;

   b_put_cards = try_out_rocket(hand_card_data, best_hand_card_value);
   if (b_put_cards)
       return b_put_cards;

   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::three_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;

   for (int i = last_poker.nMaxCard + 1; i < 18; i++)
   {
       if (hand_card_data.value_aHandCardList[i] > 0)
       {
           hand_card_data.value_aHandCardList[i] -= 3;
           hand_card_data.m_card_cnt -= 3;
           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
           hand_card_data.value_aHandCardList[i] += 3;
           hand_card_data.m_card_cnt += 3;

           if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <= (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
           {
               best_hand_card_value = tmpHandCardValue;
               best_max_card = i;
               b_put_cards = true;
           }
       }
   }
   if (b_put_cards)
   {
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_out_card_type  = CardTypeCheck::get_group_data(cgTHREE, best_max_card, 3);
       return b_put_cards;
   }

   b_put_cards = try_out_bomb(hand_card_data, best_hand_card_value);
   if (b_put_cards)
       return b_put_cards;

   b_put_cards = try_out_rocket(hand_card_data, best_hand_card_value);
   if (b_put_cards)
       return b_put_cards;

   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::single_line_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;

   //对手出牌已经是最大的顺子，尝试炸弹或者火箭
   if (last_poker.nMaxCard == ccpb::common::PokerValue::PA)
   {
       b_put_cards = try_out_bomb(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;

       b_put_cards = try_out_rocket(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;
   }

   //验证顺子的标志
   int prov = 0;
   //顺子起点
   int start_i = 0;
   //顺子终点
   int end_i = 0;
   //顺子长度
   int length = last_poker.nCount;

   //找出所有的顺子
   for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PM2; ++i)
   {
       if (hand_card_data.value_aHandCardList[i] >= 1)
           prov++;
       else
           prov = 0;

       if (prov >= length)
       {
           end_i = i;
           start_i = end_i - length + 1;

           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j]--;
           }
           hand_card_data.m_card_cnt -= last_poker.nCount;
           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j]++;
           }
           hand_card_data.m_card_cnt += last_poker.nCount;

           //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
           if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <= (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
           {
               best_hand_card_value = tmpHandCardValue;
               best_max_card = end_i;
               b_put_cards = true;
           }
       }
   }

   if (b_put_cards)
   {
       for (int j = start_i; j <= end_i; j++)
       {
           hand_card_data.m_put_card_list.push_back(j);
       }
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgSINGLE_LINE, best_max_card, last_poker.nCount);
       return b_put_cards;
   }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}
bool PassiveCardStrategy::double_line_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;

   //对手出牌已经是最大的顺子，尝试炸弹或者火箭
   if (last_poker.nMaxCard == ccpb::common::PokerValue::PA)
   {
       b_put_cards = try_out_bomb(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;

       b_put_cards = try_out_rocket(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;
   }

   //验证顺子的标志
   int prov = 0;
   //顺子起点
   int start_i = 0;
   //顺子终点
   int end_i = 0;
   //顺子长度
   int length = last_poker.nCount / 2;

   //找出所有的顺子
   for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PM2; ++i)
   {
       if (hand_card_data.value_aHandCardList[i] >= 2)
           prov++;
       else
           prov = 0;

       if (prov >= length)
       {
           end_i = i;
           start_i = end_i - length + 1;

           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j] -= 2;
           }
           hand_card_data.m_card_cnt -= last_poker.nCount;
           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j] =+ 2;
           }
           hand_card_data.m_card_cnt += last_poker.nCount;

           //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
           if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <= (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
           {
               best_hand_card_value = tmpHandCardValue;
               best_max_card = end_i;
               b_put_cards = true;
           }
       }
   }

   if (b_put_cards)
   {
       for (int j = start_i; j <= end_i; j++)
       {
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
       }
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgDOUBLE_LINE, best_max_card, last_poker.nCount);
       return b_put_cards;
   }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::three_line_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;

   //对手出牌已经是最大的顺子，尝试炸弹或者火箭
   if (last_poker.nMaxCard == ccpb::common::PokerValue::PA)
   {
       b_put_cards = try_out_bomb(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;

       b_put_cards = try_out_rocket(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;
   }

   //验证顺子的标志
   int prov = 0;
   //顺子起点
   int start_i = 0;
   //顺子终点
   int end_i = 0;
   //顺子长度
   int length = last_poker.nCount / 3;

   //找出所有的顺子
   for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PM2; ++i)
   {
       if (hand_card_data.value_aHandCardList[i] >= 3)
           prov++;
       else
           prov = 0;

       if (prov >= length)
       {
           end_i = i;
           start_i = end_i - length + 1;

           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j] -= 3;
           }
           hand_card_data.m_card_cnt -= last_poker.nCount;
           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j] += 3;
           }
           hand_card_data.m_card_cnt += last_poker.nCount;

           //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
           if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <= (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
           {
               best_hand_card_value = tmpHandCardValue;
               best_max_card = end_i;
               b_put_cards = true;
           }
       }
   }

   if (b_put_cards)
   {
       for (int j = start_i; j <= end_i; j++)
       {
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
       }
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgTHREE_LINE, best_max_card, last_poker.nCount);
       return b_put_cards;
   }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::three_one_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;

   //顺带出去的牌
   int tmp_1 = 0;
   //三带一
   for (int i = last_poker.nMaxCard + 1; i < ccpb::common::PokerValue::PJoker1; i++)
   {
       if (hand_card_data.value_aHandCardList[i] >= 3)
       {
           for (int j = ccpb::common::PokerValue::P3; j < ccpb::common::PokerValue::PJoker2; j++)
           {
               //选出一张以上的牌且不是选择三张的那个牌
               if (hand_card_data.value_aHandCardList[j] > 0 && j != i)
               {
                   hand_card_data.value_aHandCardList[i] -= 3;
                   hand_card_data.value_aHandCardList[j] -= 1;
                   hand_card_data.m_card_cnt -= 4;
                   HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                   hand_card_data.value_aHandCardList[i] += 3;
                   hand_card_data.value_aHandCardList[j] += 1;
                   hand_card_data.m_card_cnt += 4;
                   //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
                   if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <= (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                   {
                       best_hand_card_value = tmpHandCardValue;
                       best_max_card = i;
                       tmp_1 = j;
                       b_put_cards = true;
                   }
               }
           }
       }
   }
   if (b_put_cards)
   {
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(tmp_1);
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgTHREE_TAKE_ONE, best_max_card, 4);
       return b_put_cards;
   }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::three_pair_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;

   //顺带出去的牌
   int tmp_1 = 0;
   //三带一
   for (int i = last_poker.nMaxCard + 1; i < ccpb::common::PokerValue::PJoker1; i++)
   {
       if (hand_card_data.value_aHandCardList[i] >= 3)
       {
           for (int j = ccpb::common::PokerValue::P3; j < ccpb::common::PokerValue::PJoker2; j++)
           {
               //选出一张以上的牌且不是选择三张的那个牌
               if (hand_card_data.value_aHandCardList[j] >= 2 && j != i)
               {
                   hand_card_data.value_aHandCardList[i] -= 3;
                   hand_card_data.value_aHandCardList[j] -= 2;
                   hand_card_data.m_card_cnt -= 5;
                   HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                   hand_card_data.value_aHandCardList[i] += 3;
                   hand_card_data.value_aHandCardList[j] += 2;
                   hand_card_data.m_card_cnt += 5;
                   //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
                   if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <= (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                   {
                       best_hand_card_value = tmpHandCardValue;
                       best_max_card = i;
                       tmp_1 = j;
                       b_put_cards = true;
                   }
               }
           }
       }
   }
   if (b_put_cards)
   {
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(tmp_1);
       hand_card_data.m_put_card_list.push_back(tmp_1);
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgTHREE_TAKE_TWO, best_max_card, 5);
       return b_put_cards;
   }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::plane_one_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;
   int prov = 0;
   //顺子起点
   int start_i = 0;
   //顺子终点
   int end_i = 0;
   //飞机长度
   int length = last_poker.nCount / 4;

   //飞机最多四连所以副牌部分就四个
   int tmp_1 = 0;
   int tmp_2 = 0;
   int tmp_3 = 0;
   int tmp_4 = 0;

   //对手出牌已经是最大的顺子，尝试炸弹或者火箭
   if (last_poker.nMaxCard == ccpb::common::PokerValue::PA)
   {
       b_put_cards = try_out_bomb(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;

       b_put_cards = try_out_rocket(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;
   }

   //飞机最大四连，一个一个处理，效率高些，不搞全枚举了,先找主体部分
   for (int i = last_poker.nMaxCard - length + 2; i < ccpb::common::PokerValue::PMA; i++)
   {
       if (hand_card_data.value_aHandCardList[i] >= 3)
           prov++;
       else
           prov = 0;

       if (prov >= length)
       {
           end_i = i;
           start_i = end_i - length + 1;

           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j] -= 3;
           }
           hand_card_data.m_card_cnt -= last_poker.nCount;

           /*本来想做全排列选取带出的牌然后枚举出最高价值的，但考虑到当飞机长度也就是在2-4之间
           所以干脆做三个分支处理算了*/
           //为两连飞机
           if (length == 2)
           {
               for (int j = ccpb::common::PokerValue::P3; j != ccpb::common::PokerValue::PJoker2 + 1; j++)
               {
                   //组合所有的牌，计算手牌价值，确定第一张牌
                   if (hand_card_data.value_aHandCardList[j] >= 1)
                   {
                       hand_card_data.value_aHandCardList[j] -= 1;
                       //确定第二张牌
                       for (int k = ccpb::common::PokerValue::P3; k < ccpb::common::PokerValue::PJoker2 + 1; k++)
                       {
                           if (hand_card_data.value_aHandCardList[k] >= 1)
                           {
                               hand_card_data.value_aHandCardList[k] -= 1;
                               HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                               hand_card_data.value_aHandCardList[k] += 1;

                               //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，
                               //若轮次牌型很大（如炸弹） 则其-7的价值也会为正
                               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                   (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                               {
                                   best_hand_card_value = tmpHandCardValue;
                                   best_max_card = end_i;
                                   tmp_1 = j;
                                   tmp_2 = k;
                                   b_put_cards = true;
                               }
                           }
                       }
                       hand_card_data.value_aHandCardList[j] += 1;
                   }
               }
           }
           //为三连飞机, 循环三层查找带牌部分
           if (length == 3)
           {
               for (int j = ccpb::common::PokerValue::P3; j != ccpb::common::PokerValue::PJoker2 + 1; j++)
               {
                   if (hand_card_data.value_aHandCardList[j] >= 1)
                   {
                       hand_card_data.value_aHandCardList[j] -= 1;
                       for (int k = ccpb::common::PokerValue::P3; k != ccpb::common::PokerValue::PJoker2 + 1; k++)
                       {
                           if (hand_card_data.value_aHandCardList[k] >= 1)
                           {
                               hand_card_data.value_aHandCardList[k] -= 1;
                               for (int l = ccpb::common::PokerValue::P3; l != ccpb::common::PokerValue::PJoker2 + 1; l++)
                               {
                                   if (hand_card_data.value_aHandCardList[l] >= 1)
                                   {
                                       hand_card_data.value_aHandCardList[l] -= 1;
                                       HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                                       //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
                                       if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                           (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                                       {
                                           best_hand_card_value = tmpHandCardValue;
                                           best_max_card = end_i;
                                           tmp_1 = j;
                                           tmp_2 = k;
                                           tmp_3 = l;
                                           b_put_cards = true;
                                       }
                                       hand_card_data.value_aHandCardList[l] += 1;
                                   }
                               }
                               hand_card_data.value_aHandCardList[k] += 1;
                           }
                       }
                       hand_card_data.value_aHandCardList[j] += 1;
                   }
               }
           }
           //为四连飞机
           if (length == 4)
           {
               for (int j = ccpb::common::PokerValue::P3; j != ccpb::common::PokerValue::PJoker2 + 1; ++j)
               {
                   if (hand_card_data.value_aHandCardList[j] > 0)
                   {
                       hand_card_data.value_aHandCardList[j] -= 1;
                       for (int k = ccpb::common::PokerValue::P3; k != ccpb::common::PokerValue::PJoker2 + 1; ++k)
                       {
                           if (hand_card_data.value_aHandCardList[k] > 0)
                           {
                               hand_card_data.value_aHandCardList[k] -= 1;
                               for (int l = ccpb::common::PokerValue::P3; l != ccpb::common::PokerValue::PJoker2 + 1; ++l)
                               {
                                   if (hand_card_data.value_aHandCardList[l] > 0)
                                   {
                                       hand_card_data.value_aHandCardList[l] -= 1;
                                       for (int m = ccpb::common::PokerValue::P3; m < ccpb::common::PokerValue::PJoker2 + 1; ++m)
                                       {
                                           if (hand_card_data.value_aHandCardList[m] > 0)
                                           {
                                               hand_card_data.value_aHandCardList[m] -= 1;
                                               HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                                               //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
                                               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                                   (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                                               {
                                                   best_hand_card_value = tmpHandCardValue;
                                                   best_max_card = end_i;
                                                   tmp_1 = j;
                                                   tmp_2 = k;
                                                   tmp_3 = l;
                                                   tmp_4 = m;
                                                   b_put_cards = true;
                                               }
                                               hand_card_data.value_aHandCardList[m] += 1;
                                           }
                                       }
                                       hand_card_data.value_aHandCardList[l] += 1;
                                   }
                               }
                               hand_card_data.value_aHandCardList[k] += 1;
                           }
                       }
                       hand_card_data.value_aHandCardList[j] += 1;
                   }
               }
           }

           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j] += 3;
           }
           hand_card_data.m_card_cnt += last_poker.nCount;
       }
   }

   if (b_put_cards)
   {
       for (int j = start_i; j <= end_i; j++)
       {
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
       }

       if (length == 2)
       {
           hand_card_data.m_put_card_list.push_back(tmp_1);
           hand_card_data.m_put_card_list.push_back(tmp_2);
       }
       if (length == 3)
       {
           hand_card_data.m_put_card_list.push_back(tmp_1);
           hand_card_data.m_put_card_list.push_back(tmp_2);
           hand_card_data.m_put_card_list.push_back(tmp_3);

       }
       if (length == 4)
       {
           hand_card_data.m_put_card_list.push_back(tmp_1);
           hand_card_data.m_put_card_list.push_back(tmp_2);
           hand_card_data.m_put_card_list.push_back(tmp_3);
           hand_card_data.m_put_card_list.push_back(tmp_4);
       }

       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgTHREE_TAKE_ONE_LINE, best_max_card, last_poker.nCount);
       return b_put_cards;
   }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::plane_pair_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;
   int prov = 0;
   //顺子起点
   int start_i = 0;
   //顺子终点
   int end_i = 0;
   //飞机长度
   int length = last_poker.nCount / 5;

   //飞机带对最多三连所以副牌部分就三个，地主可能出4连飞机带对，20张牌，但是人家一次出20张都赢了，还管个毛线啊
   int tmp_1 = 0;
   int tmp_2 = 0;
   int tmp_3 = 0;

   //对手出牌已经是最大的顺子，尝试炸弹或者火箭
   if (last_poker.nMaxCard == ccpb::common::PokerValue::PA)
   {
       b_put_cards = try_out_bomb(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;

       b_put_cards = try_out_rocket(hand_card_data, best_hand_card_value);
       if (b_put_cards)
           return b_put_cards;
   }

   //飞机带对最大三连，一个一个处理，效率高些，不搞全枚举了,先找主体部分
   for (int i = last_poker.nMaxCard - length + 2; i < ccpb::common::PokerValue::PMA; i++)
   {
       if (hand_card_data.value_aHandCardList[i] >= 3)
           prov++;
       else
           prov = 0;

       if (prov >= length)
       {
           end_i = i;
           start_i = end_i - length + 1;

           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j] -= 3;
           }
           hand_card_data.m_card_cnt -= last_poker.nCount;

           //为两连飞机
           if (length == 2)
           {
               for (int j = ccpb::common::PokerValue::P3; j != ccpb::common::PokerValue::PJoker2 + 1; j++)
               {
                   //组合所有的牌，计算手牌价值，确定第一张牌
                   if (hand_card_data.value_aHandCardList[j] >= 2)
                   {
                       hand_card_data.value_aHandCardList[j] -= 2;
                       //确定第二张牌
                       for (int k = ccpb::common::PokerValue::P3; k < ccpb::common::PokerValue::PJoker2 + 1; k++)
                       {
                           if (hand_card_data.value_aHandCardList[k] >= 2)
                           {
                               hand_card_data.value_aHandCardList[k] -= 2;
                               HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                               hand_card_data.value_aHandCardList[k] += 2;

                               //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，
                               //若轮次牌型很大（如炸弹） 则其-7的价值也会为正
                               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                   (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                               {
                                   best_hand_card_value = tmpHandCardValue;
                                   best_max_card = end_i;
                                   tmp_1 = j;
                                   tmp_2 = k;
                                   b_put_cards = true;
                               }
                           }
                       }
                       hand_card_data.value_aHandCardList[j] += 2;
                   }
               }
           }
           //为三连飞机, 循环三层查找带牌部分
           if (length == 3)
           {
               for (int j = ccpb::common::PokerValue::P3; j != ccpb::common::PokerValue::PJoker2 + 1; j++)
               {
                   if (hand_card_data.value_aHandCardList[j] >= 2)
                   {
                       hand_card_data.value_aHandCardList[j] -= 2;
                       for (int k = ccpb::common::PokerValue::P3; k != ccpb::common::PokerValue::PJoker2 + 1; k++)
                       {
                           if (hand_card_data.value_aHandCardList[k] >= 2)
                           {
                               hand_card_data.value_aHandCardList[k] -= 2;
                               for (int l = ccpb::common::PokerValue::P3; l != ccpb::common::PokerValue::PJoker2 + 1; l++)
                               {
                                   if (hand_card_data.value_aHandCardList[l] >= 2)
                                   {
                                       hand_card_data.value_aHandCardList[l] -= 2;
                                       HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                                       //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
                                       if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                           (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                                       {
                                           best_hand_card_value = tmpHandCardValue;
                                           best_max_card = end_i;
                                           tmp_1 = j;
                                           tmp_2 = k;
                                           tmp_3 = l;
                                           b_put_cards = true;
                                       }
                                       hand_card_data.value_aHandCardList[l] += 2;
                                   }
                               }
                               hand_card_data.value_aHandCardList[k] += 2;
                           }
                       }
                       hand_card_data.value_aHandCardList[j] += 2;
                   }
               }
           }

           for (int j = start_i; j <= end_i; j++)
           {
               hand_card_data.value_aHandCardList[j] += 3;
           }
           hand_card_data.m_card_cnt += last_poker.nCount;
       }
   }

   if (b_put_cards)
   {
       for (int j = start_i; j <= end_i; j++)
       {
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
       }

       if (length == 2)
       {
           hand_card_data.m_put_card_list.push_back(tmp_1);
           hand_card_data.m_put_card_list.push_back(tmp_1);
           hand_card_data.m_put_card_list.push_back(tmp_2);
           hand_card_data.m_put_card_list.push_back(tmp_2);
       }
       if (length == 3)
       {
           hand_card_data.m_put_card_list.push_back(tmp_1);
           hand_card_data.m_put_card_list.push_back(tmp_1);
           hand_card_data.m_put_card_list.push_back(tmp_2);
           hand_card_data.m_put_card_list.push_back(tmp_2);
           hand_card_data.m_put_card_list.push_back(tmp_3);
           hand_card_data.m_put_card_list.push_back(tmp_3);
       }

       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgTHREE_TAKE_TWO_LINE, best_max_card, last_poker.nCount);
       return b_put_cards;
   }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::four_one_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;

   int tmp_1 = 0;
   int tmp_2 = 0;

   /*也就是对于四带二这种牌型我的出牌策略是：先看自己的手牌实力如何，如果较好直接炸丫挺的。如果不太好，那么再看看以四带二形式管出后实力如何，
   如果较好，可以四带二管上，如果还是不太好，那么也是要炸，输就输了！*/
   /*本策略对于这种牌型有炸必炸，四带二相比炸弹来说会多带出两手牌，即最多提高14权值的价值
   若当前手牌价值大于14，即认为我们能炸即炸，不必考虑四带二的收益*/
   if (best_hand_card_value.SumValue > 14)
   {
       for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker1; ++i)
       {
           if (hand_card_data.value_aHandCardList[i] == 4)
           {
               hand_card_data.m_put_card_list.push_back(i);
               hand_card_data.m_put_card_list.push_back(i);
               hand_card_data.m_put_card_list.push_back(i);
               hand_card_data.m_put_card_list.push_back(i);

               hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgBOMB_CARD, i, 4);

               return true;
           }
       }
       //王炸
       if (hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] == 1 &&
           hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] == 1)
       {
           hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker1);
           hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker2);

           hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgKING_CARD, ccpb::common::PokerValue::PJoker2, 2);

           return b_put_cards;
       }
   }

   for (int i = last_poker.nMaxCard + 1; i != ccpb::common::PokerValue::PJoker1; ++i)
   {
       if (hand_card_data.value_aHandCardList[i] == 4)
       {
           for (int j = ccpb::common::PokerValue::P3; j != ccpb::common::PokerValue::PJoker2 + 1; ++j)
           {
               //先选出一张以上的牌且不是选择四张的那个牌
               if (hand_card_data.value_aHandCardList[j] >= 1 && j != i)
               {
                   //再选出一张以上的牌且不是选择四张的那个牌且不是第一次选的两张内个牌（策略里四带二不允许带一对,还不如炸）
                   for (int k = j + 1; k < ccpb::common::PokerValue::PJoker2 + 1; ++k)
                   {
                       if (hand_card_data.value_aHandCardList[k] >= 1 && k != i)
                       {
                           hand_card_data.value_aHandCardList[i] -= 4;
                           hand_card_data.value_aHandCardList[j] -= 1;
                           hand_card_data.value_aHandCardList[k] -= 1;
                           hand_card_data.m_card_cnt -= 6;
                           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                           hand_card_data.value_aHandCardList[i] += 4;
                           hand_card_data.value_aHandCardList[j] += 1;
                           hand_card_data.value_aHandCardList[k] += 1;
                           hand_card_data.m_card_cnt += 6;

                           //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
                           if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                               (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                           {
                               best_hand_card_value = tmpHandCardValue;
                               best_max_card = i;
                               tmp_1 = j;
                               tmp_2 = k;
                               b_put_cards = true;
                           }
                       }
                   }
               }
           }
       }
   }

   if (b_put_cards)
   {
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(tmp_1);
       hand_card_data.m_put_card_list.push_back(tmp_2);
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgFOUR_TAKE_ONE, best_max_card, 6);
       return b_put_cards;
   }

   //这里是在拍权值较小的情况，且没有选择出良好的四带二牌型，那么也要炸，因为信仰。
   // for (int i = 3; i < 16; i++)
   // {
   //     if (hand_card_data.value_aHandCardList[i] == 4)
   //     {
   //         hand_card_data.value_nPutCardList.push_back(i);
   //         hand_card_data.value_nPutCardList.push_back(i);
   //         hand_card_data.value_nPutCardList.push_back(i);
   //         hand_card_data.value_nPutCardList.push_back(i);

   //         hand_card_data.uctPutCardType = clsGameSituation.uctNowCardGroup = get_GroupData(cgBOMB_CARD, i, 4);

   //         return;
   //     }
   // }

   // //王炸
   // if (hand_card_data.value_aHandCardList[17] > 0 && hand_card_data.value_aHandCardList[16] > 0)
   // {
   //     hand_card_data.value_nPutCardList.push_back(17);
   //     hand_card_data.value_nPutCardList.push_back(16);

   //     hand_card_data.uctPutCardType = clsGameSituation.uctNowCardGroup = get_GroupData(cgKING_CARD, 17, 2);

   //     return;
   // }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::four_pair_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;

   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   best_hand_card_value.NeedRound += 1;
   int best_max_card = 0;

   int tmp_1 = 0;
   int tmp_2 = 0;

   /*也就是对于四带二这种牌型我的出牌策略是：先看自己的手牌实力如何，如果较好直接炸丫挺的。如果不太好，那么再看看以四带二形式管出后实力如何，
   如果较好，可以四带二管上，如果还是不太好，那么也是要炸，输就输了！*/
   /*本策略对于这种牌型有炸必炸，四带二相比炸弹来说会多带出两手牌，即最多提高14权值的价值
   若当前手牌价值大于14，即认为我们能炸即炸，不必考虑四带二的收益*/
   if (best_hand_card_value.SumValue > 14)
   {
       for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker1; ++i)
       {
           if (hand_card_data.value_aHandCardList[i] == 4)
           {
               hand_card_data.m_put_card_list.push_back(i);
               hand_card_data.m_put_card_list.push_back(i);
               hand_card_data.m_put_card_list.push_back(i);
               hand_card_data.m_put_card_list.push_back(i);

               hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgBOMB_CARD, i, 4);

               return true;
           }
       }
       //王炸
       if (hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] == 1 &&
           hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] == 1)
       {
           hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker1);
           hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker2);

           hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgKING_CARD, ccpb::common::PokerValue::PJoker2, 2);

           return b_put_cards;
       }
   }

   for (int i = last_poker.nMaxCard + 1; i != ccpb::common::PokerValue::PJoker1; ++i)
   {
       if (hand_card_data.value_aHandCardList[i] == 4)
       {
           for (int j = ccpb::common::PokerValue::P3; j != ccpb::common::PokerValue::PJoker2 + 1; ++j)
           {
               //先选出一张以上的牌且不是选择四张的那个牌
               if (hand_card_data.value_aHandCardList[j] >= 2 && j != i)
               {
                   //再选出一张以上的牌且不是选择四张的那个牌且不是第一次选的两张内个牌（策略里四带二不允许带一对,还不如炸）
                   for (int k = j + 1; k < ccpb::common::PokerValue::PJoker2 + 1; ++k)
                   {
                       if (hand_card_data.value_aHandCardList[k] >= 2 && k != i)
                       {
                           hand_card_data.value_aHandCardList[i] -= 4;
                           hand_card_data.value_aHandCardList[j] -= 2;
                           hand_card_data.value_aHandCardList[k] -= 2;
                           hand_card_data.m_card_cnt -= 8;
                           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                           hand_card_data.value_aHandCardList[i] += 4;
                           hand_card_data.value_aHandCardList[j] += 2;
                           hand_card_data.value_aHandCardList[k] += 2;
                           hand_card_data.m_card_cnt += 8;

                           //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
                           if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                               (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                           {
                               best_hand_card_value = tmpHandCardValue;
                               best_max_card = i;
                               tmp_1 = j;
                               tmp_2 = k;
                               b_put_cards = true;
                           }
                       }
                   }
               }
           }
       }
   }

   if (b_put_cards)
   {
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(tmp_1);
       hand_card_data.m_put_card_list.push_back(tmp_1);
       hand_card_data.m_put_card_list.push_back(tmp_2);
       hand_card_data.m_put_card_list.push_back(tmp_2);
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgFOUR_TAKE_TWO, best_max_card, 8);
       return b_put_cards;
   }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return b_put_cards;
}

bool PassiveCardStrategy::bomb_card_strategy(CardGroupData last_poker, HandCardData& hand_card_data)
{
   bool b_put_cards = false;
   HandCardValue best_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
   int best_max_card = 0;
   //更大的炸弹——这里直接炸，不考虑拆分后果。因为信仰。
   for (int i = last_poker.nMaxCard + 1; i != ccpb::common::PokerValue::PJoker1; i++)
   {
       if (hand_card_data.value_aHandCardList[i] == 4)
       {
           hand_card_data.value_aHandCardList[i] -= 4;
           hand_card_data.m_card_cnt -= 4;
           HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
           hand_card_data.value_aHandCardList[i] += 4;
           hand_card_data.m_card_cnt += 4;
           //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
           if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
               (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
           {
               best_hand_card_value = tmpHandCardValue;
               best_max_card = i;
               b_put_cards = true;
               //优先最小牌面的炸弹
               break;
           }
       }
   }

   if (b_put_cards)
   {
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);
       hand_card_data.m_put_card_list.push_back(best_max_card);

       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgBOMB_CARD, best_max_card, 4);
       return b_put_cards;
   }

   //王炸
   if (hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] == 1 &&
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] == 1)
   {
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] -= 1;
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] -= 1;
       hand_card_data.m_card_cnt -= 2;
       HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] += 1;
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] += 1;
       hand_card_data.m_card_cnt += 2;

       //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
       if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
           (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
       {
           hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker1);
           hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker2);

           hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgKING_CARD, ccpb::common::PokerValue::PJoker2, 2);
           return true;
       }
   }

   //管不上
   hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgZERO, 0, 0);
   return false;
}

}
}
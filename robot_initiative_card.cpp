/*
* @Descripttion: 机器人主动出牌逻辑
* @version: 
* @Author: huazy
* @Date: 2020-10-14 20:14:34
 * @LastEditors: huazy
 * @LastEditTime: 2021-02-18 17:32:06
*/
#include <vector>

#include "robot_initiative_card.h"
#include "card_type.h"

namespace ccserver
{
namespace landlord
{
void InitiativeCard::initiative_card(HandCardData& hand_card_data)
{

}

HandCardValue InitiativeCard::get_hand_card_value(HandCardData& hand_card_data)
{

}

void InitiativeCard::get_initiative_put_card_list(HandCardData& hand_card_data)
{
   //清空数据
   hand_card_data.clear_card_data();

   //剪枝：如果能出去最后一手牌直接出
   CardGroupData SurCardGroupData = OneHandOutCard::one_hand_output_card(hand_card_data.value_aHandCardList, hand_card_data.m_card_cnt);
   //如果能一次性出去且不是四带二，因为主动出牌若手上剩四带二牌的话可以考虑先打一手然后炸，获得双倍积分
   if (SurCardGroupData.cgType != cgERROR &&
       SurCardGroupData.cgType != cgFOUR_TAKE_ONE &&
       SurCardGroupData.cgType != cgFOUR_TAKE_TWO)
   {
   }

   /*王炸——当前策略只处理王炸作为倒数第二手的优先出牌逻辑，后续版本会在此基础上优化*/
   if (hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] == 1 &&
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] == 1)
   {
   }

   //暂存最佳的价值
   HandCardValue best_hand_card_value;
   best_hand_card_value.NeedRound = 20;
   best_hand_card_value.SumValue = MinCardsValue;
   //我们认为不出牌的话会让对手一个轮次，即加一轮（权值减少7）便于后续的对比参考。
   best_hand_card_value.NeedRound += 1;

   //暂存最佳的组合
   CardGroupData best_card_group;

   //带牌数组
   vector<int> additional;

   //先处理优先级较高的牌，全部枚举，找出最适合的牌，可以找不到
   for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker1; i++)
   {
       //主动出牌暂时不拆分炸弹
       if (hand_card_data.value_aHandCardList[i] != 4)
       {
           //出三带一
           InitiativeCardStrategy::try_out_three_take_one_card(hand_card_data, i, best_hand_card_value,
                                                               best_card_group, additional);
           //出三带二
           InitiativeCardStrategy::try_out_three_take_pair_card(hand_card_data, i, best_hand_card_value,
                                                               best_card_group, additional);
           //出四带二单
           if (hand_card_data.value_aHandCardList[i] == 4)
           {
               //todo, 本策略主动出牌不拆分炸弹,以后可以修改
           }

           //出四带二对
           if (hand_card_data.value_aHandCardList[i] ==4)
           {
               //todo, 本策略主动出牌不拆分炸弹,以后可以修改
           }

           //出三带一单连
           InitiativeCardStrategy::try_out_plane_take_one_card(hand_card_data, i, best_hand_card_value,
                                                               best_card_group, additional);
           //出三带一双连
           InitiativeCardStrategy::try_out_plane_take_pair_card(hand_card_data, i, best_hand_card_value,
                                                               best_card_group, additional);
       }
   }

   //对上面的牌型做分析,不需要做循环，个数少，一个一个放更快
   if (best_card_group.cgType == cgTHREE_TAKE_ONE)
   {
       hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
       hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
       hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
       hand_card_data.m_put_card_list.push_back(additional[0]);
       hand_card_data.m_put_out_card_type = best_card_group;
       return;
   }
   else if (best_card_group.cgType == cgTHREE_TAKE_TWO)
   {
       hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
       hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
       hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
       hand_card_data.m_put_card_list.push_back(additional[0]);
       hand_card_data.m_put_card_list.push_back(additional[1]);
       hand_card_data.m_put_out_card_type = best_card_group;
       return;
   }
   else if (best_card_group.cgType == cgTHREE_TAKE_ONE_LINE)
   {
       for (int j = best_card_group.nMaxCard - (best_card_group.nCount / 4) + 1; j <= best_card_group.nMaxCard; j++)
       {
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
       }
       //两连飞机带单
       if (best_card_group.nCount / 4 == 2)
       {
           hand_card_data.m_put_card_list.push_back(additional[0]);
           hand_card_data.m_put_card_list.push_back(additional[1]);
       }
       //三连飞机带单
       if (best_card_group.nCount / 4 == 3)
       {
           hand_card_data.m_put_card_list.push_back(additional[0]);
           hand_card_data.m_put_card_list.push_back(additional[1]);
           hand_card_data.m_put_card_list.push_back(additional[2]);
       }
       //四连飞机带单
       if (best_card_group.nCount / 4 == 4)
       {
           hand_card_data.m_put_card_list.push_back(additional[0]);
           hand_card_data.m_put_card_list.push_back(additional[1]);
           hand_card_data.m_put_card_list.push_back(additional[2]);
           hand_card_data.m_put_card_list.push_back(additional[3]);
       }

       hand_card_data.m_put_out_card_type = best_card_group;
       return;
   }
   else if (best_card_group.cgType == cgTHREE_TAKE_TWO_LINE)
   {
       for (int j = best_card_group.nMaxCard - (best_card_group.nCount / 5) + 1; j <= best_card_group.nMaxCard; j++)
       {
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
           hand_card_data.m_put_card_list.push_back(j);
       }
       if (best_card_group.nCount / 5 == 2)
       {
           hand_card_data.m_put_card_list.push_back(additional[0]);
           hand_card_data.m_put_card_list.push_back(additional[0]);
           hand_card_data.m_put_card_list.push_back(additional[1]);
           hand_card_data.m_put_card_list.push_back(additional[1]);
       }
       if (best_card_group.nCount / 5 == 3)
       {
           hand_card_data.m_put_card_list.push_back(additional[0]);
           hand_card_data.m_put_card_list.push_back(additional[0]);
           hand_card_data.m_put_card_list.push_back(additional[1]);
           hand_card_data.m_put_card_list.push_back(additional[1]);
           hand_card_data.m_put_card_list.push_back(additional[2]);
           hand_card_data.m_put_card_list.push_back(additional[2]);
       }
       hand_card_data.m_put_out_card_type = best_card_group;
       return;
   }

   //次之处理当前价值最低的牌，现在不必再考虑这张牌可能被三牌带出等情况
   //从低价值牌型找起，找到后即可打出，因为是从
   for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker1; ++i)
   {
       //排除炸弹和没有牌的情况
       if (hand_card_data.value_aHandCardList[i] != 0 && hand_card_data.value_aHandCardList[i] != 4)
       {
           //尝试出单牌，更新手牌最大价值
           InitiativeCardStrategy::try_out_single_card(hand_card_data, i, best_hand_card_value, best_card_group);
           //出对牌，更新手牌最大价值
           InitiativeCardStrategy::try_out_double_card(hand_card_data, i, best_hand_card_value, best_card_group);
           //出三牌，更新手牌最大价值
           InitiativeCardStrategy::try_out_three_card(hand_card_data, i, best_hand_card_value, best_card_group);
           //出单顺，更新手牌最大价值
           InitiativeCardStrategy::try_out_single_line_card(hand_card_data, i, best_hand_card_value, best_card_group);
           //出双顺，更新手牌最大价值
           InitiativeCardStrategy::try_out_double_line_card(hand_card_data, i, best_hand_card_value, best_card_group);
           //出三顺，更新手牌最大价值
           InitiativeCardStrategy::try_out_three_line_card(hand_card_data, i, best_hand_card_value, best_card_group);

           //放在if内是因为若此时i有值那么必须要返回一个结果
           if (best_card_group.cgType == cgERROR)
           {
           }
           else if (best_card_group.cgType == cgSINGLE)
           {
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_out_card_type = best_card_group;
           }
           else if (best_card_group.cgType == cgDOUBLE)
           {
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_out_card_type = best_card_group;
           }
           else if (best_card_group.cgType == cgTHREE)
           {
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_out_card_type = best_card_group;
           }
           else if (best_card_group.cgType == cgSINGLE_LINE)
           {
               for (int j = best_card_group.nMaxCard - best_card_group.nCount + 1; j <= best_card_group.nMaxCard; j++)
               {
                   hand_card_data.m_put_card_list.push_back(j);
               }
               hand_card_data.m_put_out_card_type = best_card_group;
           }
           else if (best_card_group.cgType == cgDOUBLE_LINE)
           {
               for (int j = best_card_group.nMaxCard - (best_card_group.nCount / 2) + 1; j <= best_card_group.nMaxCard; j++)
               {
                   hand_card_data.m_put_card_list.push_back(j);
                   hand_card_data.m_put_card_list.push_back(j);
               }
               hand_card_data.m_put_out_card_type = best_card_group;
           }
           else if (best_card_group.cgType == cgTHREE_LINE)
           {
               for (int j = best_card_group.nMaxCard - (best_card_group.nCount / 3) + 1; j <= best_card_group.nMaxCard; j++)
               {
                   hand_card_data.m_put_card_list.push_back(j);
                   hand_card_data.m_put_card_list.push_back(j);
                   hand_card_data.m_put_card_list.push_back(j);
               }
               hand_card_data.m_put_out_card_type = best_card_group;
           }
           else if (best_card_group.cgType == cgTHREE_TAKE_ONE)
           {
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_card_list.push_back(additional[0]);
               hand_card_data.m_put_out_card_type = best_card_group;
           }
           else if (best_card_group.cgType == cgTHREE_TAKE_TWO)
           {
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_card_list.push_back(best_card_group.nMaxCard);
               hand_card_data.m_put_card_list.push_back(additional[0]);
               hand_card_data.m_put_card_list.push_back(additional[0]);
               hand_card_data.m_put_out_card_type = best_card_group;
           }
           else if (best_card_group.cgType == cgTHREE_TAKE_ONE_LINE)
           {
               for (int j = best_card_group.nMaxCard - (best_card_group.nCount / 4) + 1; j <= best_card_group.nMaxCard; j++)
               {
                   hand_card_data.m_put_card_list.push_back(j);
                   hand_card_data.m_put_card_list.push_back(j);
                   hand_card_data.m_put_card_list.push_back(j);
               }

               if (best_card_group.nCount / 4 == 2)
               {
                   hand_card_data.m_put_card_list.push_back(additional[0]);
                   hand_card_data.m_put_card_list.push_back(additional[1]);
               }
               if (best_card_group.nCount / 4 == 3)
               {
                   hand_card_data.m_put_card_list.push_back(additional[0]);
                   hand_card_data.m_put_card_list.push_back(additional[1]);
                   hand_card_data.m_put_card_list.push_back(additional[2]);
               }
               if (best_card_group.nCount / 4 == 4)
               {
                   hand_card_data.m_put_card_list.push_back(additional[0]);
                   hand_card_data.m_put_card_list.push_back(additional[1]);
                   hand_card_data.m_put_card_list.push_back(additional[2]);
                   hand_card_data.m_put_card_list.push_back(additional[3]);
               }

               hand_card_data.m_put_out_card_type = best_card_group;
           }
           else if (best_card_group.cgType == cgTHREE_TAKE_TWO_LINE)
           {
               for (int j = best_card_group.nMaxCard - (best_card_group.nCount / 5) + 1; j <= best_card_group.nMaxCard; j++)
               {
                   hand_card_data.m_put_card_list.push_back(j);
                   hand_card_data.m_put_card_list.push_back(j);
                   hand_card_data.m_put_card_list.push_back(j);
               }
               if (best_card_group.nCount / 5 == 2)
               {
                   hand_card_data.m_put_card_list.push_back(additional[0]);
                   hand_card_data.m_put_card_list.push_back(additional[0]);
                   hand_card_data.m_put_card_list.push_back(additional[1]);
                   hand_card_data.m_put_card_list.push_back(additional[1]);
               }
               if (best_card_group.nCount / 5 == 3)
               {
                   hand_card_data.m_put_card_list.push_back(additional[0]);
                   hand_card_data.m_put_card_list.push_back(additional[0]);
                   hand_card_data.m_put_card_list.push_back(additional[1]);
                   hand_card_data.m_put_card_list.push_back(additional[1]);
                   hand_card_data.m_put_card_list.push_back(additional[2]);
                   hand_card_data.m_put_card_list.push_back(additional[2]);
               }
               hand_card_data.m_put_out_card_type = best_card_group;
           }
           return;
       }
   }

   //上面的牌型都没有，找下面的大牌吧
   //如果没有从3到2的非炸牌，则看看有没有单王
   if (hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] == 1 &&
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] == 0)
   {
       hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker1);
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgSINGLE, ccpb::common::PokerValue::PJoker1, 1);
       return;
   }
   if (hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker1] == 0 &&
       hand_card_data.value_aHandCardList[ccpb::common::PokerValue::PJoker2] == 1)
   {
       hand_card_data.m_put_card_list.push_back(ccpb::common::PokerValue::PJoker2);
       hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgSINGLE, ccpb::common::PokerValue::PJoker2, 1);
       return;
   }

   //单王也没有，出炸弹
   for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker1; ++i)
   {
       if (hand_card_data.value_aHandCardList[i] == 4)
       {
           hand_card_data.m_put_card_list.push_back(i);
           hand_card_data.m_put_card_list.push_back(i);
           hand_card_data.m_put_card_list.push_back(i);
           hand_card_data.m_put_card_list.push_back(i);

           hand_card_data.m_put_out_card_type = CardTypeCheck::get_group_data(cgBOMB_CARD, i, 4);

           return;
       }
   }
}

/**
* @description: 尝试出三带一
* @param {
* hand_card_data： 手牌
* card_num： 当前的牌的值
* best_hand_card_value： 当前暂存的手牌的最佳价值
* best_hand_card_group： 当前最佳的出牌组合
* additional： 带牌部分
* }
* @return {是否成功}
*/
void InitiativeCardStrategy::try_out_three_take_one_card(HandCardData& hand_card_data, int card_num,
                                                       HandCardValue& best_hand_card_value,
                                                       CardGroupData& best_card_group, vector<int>& additional)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 3)
   {
       hand_card_data.value_aHandCardList[card_num] -= 3;
       //从最小的开始找
       for (int j = ccpb::common::PokerValue::P3; j != ccpb::common::PokerValue::PJoker2 + 1; ++j)
       {
           //选出三带一的主体后，找出剩余牌中最小的牌
           if (hand_card_data.value_aHandCardList[j] >= 1)
           {
               //在牌集中去掉这个牌
               hand_card_data.value_aHandCardList[j] -= 1;
               //手牌总数扣除三带一的牌数
               hand_card_data.m_card_cnt -= 4;
               //递归剩余的牌
               HandCardValue tmp_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
               //递归完成后将三带一的带牌部分加回去
               hand_card_data.value_aHandCardList[j] += 1;
               //牌的总数加回去
               hand_card_data.m_card_cnt += 4;
               //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                   (tmp_hand_card_value.SumValue - (tmp_hand_card_value.NeedRound * 7)))
               {
                   best_hand_card_value = tmp_hand_card_value;
                   best_card_group = CardTypeCheck::get_group_data(cgTHREE_TAKE_ONE, card_num, 4);
                   additional.clear();
                   additional.push_back(j);
               }
           }
       }
       //三带对的主体部分加回去
       hand_card_data.value_aHandCardList[card_num] += 3;
   }

}

void InitiativeCardStrategy::try_out_three_take_pair_card(HandCardData& hand_card_data, int card_num,
                                                       HandCardValue& best_hand_card_value,
                                                       CardGroupData& best_card_group, vector<int>& additional)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 3)
   {
       hand_card_data.value_aHandCardList[card_num] -= 3;
       //从最小的开始找
       for (int j = ccpb::common::PokerValue::P3; j != ccpb::common::PokerValue::PJoker2 + 1; ++j)
       {
           //选出三带一的主体后，找出剩余牌中最小的牌
           if (hand_card_data.value_aHandCardList[j] >= 2)
           {
               //在牌集中去掉这个牌
               hand_card_data.value_aHandCardList[j] -= 2;
               //手牌总数扣除三带一的牌数
               hand_card_data.m_card_cnt -= 5;
               //递归剩余的牌
               HandCardValue tmp_hand_card_value = InitiativeCard::get_hand_card_value(hand_card_data);
               //递归完成后将三带一的带牌部分加回去
               hand_card_data.value_aHandCardList[j] += 2;
               //牌的总数加回去
               hand_card_data.m_card_cnt += 5;
               //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                   (tmp_hand_card_value.SumValue - (tmp_hand_card_value.NeedRound * 7)))
               {
                   best_hand_card_value = tmp_hand_card_value;
                   best_card_group = CardTypeCheck::get_group_data(cgTHREE_TAKE_TWO, card_num, 5);
                   additional.clear();
                   additional.push_back(j);
               }
           }
       }
       //三带一对的主体部分加回去
       hand_card_data.value_aHandCardList[card_num] += 3;
   }

}

void InitiativeCardStrategy::try_out_plane_take_one_card(HandCardData& hand_card_data, int card_num,
                                                       HandCardValue& best_hand_card_value,
                                                       CardGroupData& best_card_group, vector<int>& additional)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 3)
   {
       int prov = 0;
       //飞机连最大到A
       for (int j = card_num; j != ccpb::common::PokerValue::PM2; ++j)
       {
           if (hand_card_data.value_aHandCardList[j] >= 3)
               prov++;
           else
               break;

           /*本来想做全排列选取带出的牌然后枚举出最高价值的，但考虑到当飞机长度也就是在2-4之间
           所以干脆做三个分支处理算了*/
           //为两连飞机
           if (prov == 2)
           {
               //在手牌中去除飞机的主体三连部分
               for (int k = card_num; k <= j; ++k)
               {
                   hand_card_data.value_aHandCardList[k] -= 3;
               }
               hand_card_data.m_card_cnt -= prov * 4;
               //处理带牌的部分
               for (int tmp1 = ccpb::common::PokerValue::P3; tmp1 != ccpb::common::PokerValue::PJoker2 + 1; ++tmp1)
               {
                   //带牌的第一张牌
                   if (hand_card_data.value_aHandCardList[tmp1] >= 1)
                   {
                       hand_card_data.value_aHandCardList[tmp1] -= 1;
                       //tmp1之前的都遍历过了，所以从tmp1开始
                       for (int tmp2 = tmp1; tmp2 != ccpb::common::PokerValue::PJoker2 + 1; ++tmp2)
                       {
                           if (hand_card_data.value_aHandCardList[tmp2] >= 1)
                           {
                               hand_card_data.value_aHandCardList[tmp2] -= 1;
                               //手牌除去要出的牌型后，计算手牌价值
                               HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                               //是否比当前暂存的手牌价值大，是的话覆盖掉，继续遍历查找
                               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                   (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                               {
                                   best_hand_card_value = tmpHandCardValue;
                                   best_card_group = CardTypeCheck::get_group_data(cgTHREE_TAKE_ONE_LINE, j, prov * 4);
                                   additional.clear();
                                   additional.push_back(tmp1);
                                   additional.push_back(tmp2);
                               }
                               hand_card_data.value_aHandCardList[tmp2] += 1;
                           }
                       }
                       hand_card_data.value_aHandCardList[tmp1] += 1;
                   }
               }
               //主体部分加回去
               for (int k = card_num; k <= j; k++)
               {
                   hand_card_data.value_aHandCardList[k] += 3;
               }
               hand_card_data.m_card_cnt += prov * 4;
           }
           //为三连飞机
           if (prov == 3)
           {
               for (int k = card_num; k <= j; k++)
               {
                   hand_card_data.value_aHandCardList[k] -= 3;
               }
               hand_card_data.m_card_cnt -= prov * 4;
               for (int tmp1 = ccpb::common::PokerValue::P3; tmp1 != ccpb::common::PokerValue::PJoker2 + 1; ++tmp1)
               {
                   if (hand_card_data.value_aHandCardList[tmp1] >= 1)
                   {
                       hand_card_data.value_aHandCardList[tmp1] -= 1;
                       for (int tmp2 = tmp1; tmp2 != ccpb::common::PokerValue::PJoker2 + 1; ++tmp2)
                       {
                           if (hand_card_data.value_aHandCardList[tmp2] >= 1)
                           {
                               hand_card_data.value_aHandCardList[tmp2] -= 1;
                               for (int tmp3 = tmp2; tmp3 != ccpb::common::PokerValue::PJoker2 + 1; ++tmp3)
                               {
                                   if (hand_card_data.value_aHandCardList[tmp3] >= 1)
                                   {
                                       hand_card_data.value_aHandCardList[tmp3] -= 1;

                                       HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                                       if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                           (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                                       {

                                           best_hand_card_value = tmpHandCardValue;
                                           best_card_group = CardTypeCheck::get_group_data(cgTHREE_TAKE_ONE_LINE, j, prov * 4);
                                           additional.clear();
                                           additional.push_back(tmp1);
                                           additional.push_back(tmp2);
                                           additional.push_back(tmp3);
                                       }
                                       hand_card_data.value_aHandCardList[tmp3] += 1;
                                   }
                               }
                               hand_card_data.value_aHandCardList[tmp2] += 1;
                           }
                       }
                       hand_card_data.value_aHandCardList[tmp1] += 1;
                   }
               }
               //主体部分加回去
               for (int k = card_num; k <= j; k++)
               {
                   hand_card_data.value_aHandCardList[k] += 3;
               }
               hand_card_data.m_card_cnt += prov * 4;
           }
           //为四连飞机
           if (prov == 4)
           {
               for (int k = card_num; k <= j; ++k)
               {
                   hand_card_data.value_aHandCardList[k] -= 3;
               }
               hand_card_data.m_card_cnt -= prov * 4;
               for (int tmp1 = ccpb::common::PokerValue::P3; tmp1 != ccpb::common::PokerValue::PJoker2 + 1; ++tmp1)
               {
                   if (hand_card_data.value_aHandCardList[tmp1] >= 1)
                   {
                       hand_card_data.value_aHandCardList[tmp1] -= 1;
                       for (int tmp2 = tmp1; tmp2 != ccpb::common::PokerValue::PJoker2 + 1; ++tmp2)
                       {
                           if (hand_card_data.value_aHandCardList[tmp2] >= 1)
                           {
                               hand_card_data.value_aHandCardList[tmp2] -= 1;
                               for (int tmp3 = tmp2; tmp3 != ccpb::common::PokerValue::PJoker2 + 1; ++tmp3)
                               {
                                   if (hand_card_data.value_aHandCardList[tmp3] >= 1)
                                   {
                                       hand_card_data.value_aHandCardList[tmp3] -= 1;
                                       for (int tmp4 = tmp3; tmp4 != ccpb::common::PokerValue::PJoker2 + 1; ++tmp4)
                                       {
                                           if (hand_card_data.value_aHandCardList[tmp4] >= 1)
                                           {
                                               hand_card_data.value_aHandCardList[tmp4] -= 1;
                                               HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                                               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                                   (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                                               {
                                                   best_hand_card_value = tmpHandCardValue;
                                                   best_card_group = CardTypeCheck::get_group_data(cgTHREE_TAKE_ONE_LINE, j, prov * 4);
                                                   additional.clear();
                                                   additional.push_back(tmp1);
                                                   additional.push_back(tmp2);
                                                   additional.push_back(tmp3);
                                                   additional.push_back(tmp4);
                                               }
                                               hand_card_data.value_aHandCardList[tmp4] += 1;
                                           }
                                       }
                                       hand_card_data.value_aHandCardList[tmp3] += 1;
                                   }
                               }
                               hand_card_data.value_aHandCardList[tmp2] += 1;
                           }
                       }
                       hand_card_data.value_aHandCardList[tmp1] += 1;
                   }
               }
               for (int k = card_num; k <= j; ++k)
               {
                   hand_card_data.value_aHandCardList[k] += 3;
               }
               hand_card_data.m_card_cnt += prov * 4;
           }
           //若prov==5，则是地主可以直接出去，在剪枝部分已经处理
       }
   }

}

void InitiativeCardStrategy::try_out_plane_take_pair_card(HandCardData& hand_card_data, int card_num,
                                                       HandCardValue& best_hand_card_value,
                                                       CardGroupData& best_card_group, vector<int>& additional)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 3)
   {
       int prov = 0;
       for (int j = card_num; j != ccpb::common::PokerValue::PM2; ++j)
       {
           if (hand_card_data.value_aHandCardList[j] >= 3)
               prov++;
           else
               break;

           /*本来想做全排列选取带出的牌然后枚举出最高价值的，但考虑到当飞机长度也就是在2-4之间
           所以干脆做三个分支处理算了, 而且飞机带对最多四连，四连飞机在一手出牌就做了判断*/
           //为两连飞机
           if (prov == 2)
           {

               for (int k = card_num; k <= j; ++k)
               {
                   hand_card_data.value_aHandCardList[k] -= 3;
               }
               hand_card_data.m_card_cnt -= prov * 5;
               //带对子可以带到对2
               for (int tmp1 = ccpb::common::PokerValue::P3; tmp1 != ccpb::common::PokerValue::PJoker1; ++tmp1)
               {
                   if (hand_card_data.value_aHandCardList[tmp1] >= 2)
                   {
                       hand_card_data.value_aHandCardList[tmp1] -= 2;
                       for (int tmp2 = tmp1; tmp2 != ccpb::common::PokerValue::PJoker1; ++tmp2)
                       {
                           if (hand_card_data.value_aHandCardList[tmp2] >= 2)
                           {
                               hand_card_data.value_aHandCardList[tmp2] -= 2;
                               HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                   (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                               {
                                   best_hand_card_value = tmpHandCardValue;
                                   best_card_group = CardTypeCheck::get_group_data(cgTHREE_TAKE_TWO_LINE, j, prov * 5);
                                   additional.clear();
                                   additional.push_back(tmp1);
                                   additional.push_back(tmp2);
                               }
                               hand_card_data.value_aHandCardList[tmp2] += 2;
                           }
                       }
                       hand_card_data.value_aHandCardList[tmp1] += 2;
                   }
               }
               //加回去
               for (int k = card_num; k <= j; ++k)
               {
                   hand_card_data.value_aHandCardList[k] += 3;
               }
               hand_card_data.m_card_cnt += prov * 5;
           }
           //为三连飞机
           if (prov == 3)
           {
               for (int k = card_num; k <= j; ++k)
               {
                   hand_card_data.value_aHandCardList[k] -= 3;
               }
               hand_card_data.m_card_cnt -= prov * 5;
               for (int tmp1 = ccpb::common::PokerValue::P3; tmp1 != ccpb::common::PokerValue::PJoker1; ++tmp1)
               {
                   if (hand_card_data.value_aHandCardList[tmp1] >= 2)
                   {
                       hand_card_data.value_aHandCardList[tmp1] -= 2;
                       for (int tmp2 = tmp1; tmp2 != ccpb::common::PokerValue::PJoker1; ++tmp2)
                       {
                           if (hand_card_data.value_aHandCardList[tmp2] >= 2)
                           {
                               hand_card_data.value_aHandCardList[tmp2] -= 2;
                               for (int tmp3 = tmp2; tmp3 != ccpb::common::PokerValue::PJoker1; ++tmp3)
                               {
                                   if (hand_card_data.value_aHandCardList[tmp3] >= 2)
                                   {
                                       hand_card_data.value_aHandCardList[tmp3] -= 2;
                                       HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
                                       if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                                           (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
                                       {
                                           best_hand_card_value = tmpHandCardValue;
                                           best_card_group = CardTypeCheck::get_group_data(cgTHREE_TAKE_TWO_LINE, j, prov * 5);
                                           additional.clear();
                                           additional.push_back(tmp1);
                                           additional.push_back(tmp2);
                                           additional.push_back(tmp3);
                                       }
                                       hand_card_data.value_aHandCardList[tmp3] += 2;
                                   }
                               }
                               hand_card_data.value_aHandCardList[tmp2] += 2;
                           }
                       }
                       hand_card_data.value_aHandCardList[tmp1] += 2;
                   }
               }
               for (int k = card_num; k <= j; ++k)
               {
                   hand_card_data.value_aHandCardList[k] += 3;
               }
               hand_card_data.m_card_cnt += prov * 5;
           }
           //若prov==4，则是地主可以直接出去，在剪枝部分已经处理
       }
   }

}

void InitiativeCardStrategy::try_out_single_card(HandCardData& hand_card_data, int card_num,
                                               HandCardValue& best_hand_card_value, CardGroupData& best_card_group)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 1)
   {
       hand_card_data.value_aHandCardList[card_num]--;
       hand_card_data.m_card_cnt--;
       HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
       hand_card_data.value_aHandCardList[card_num]++;
       hand_card_data.m_card_cnt++;
       if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
           (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
       {
           best_hand_card_value = tmpHandCardValue;
           best_card_group = CardTypeCheck::get_group_data(cgSINGLE, card_num, 1);
       }
   }
}

void InitiativeCardStrategy::try_out_double_card(HandCardData& hand_card_data, int card_num,
                                               HandCardValue& best_hand_card_value, CardGroupData& best_card_group)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 2)
   {
       //尝试打出一对牌，估算剩余手牌价值
       hand_card_data.value_aHandCardList[card_num] -= 2;
       hand_card_data.m_card_cnt -= 2;
       HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
       hand_card_data.value_aHandCardList[card_num] += 2;
       hand_card_data.m_card_cnt += 2;

       //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
       if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
       (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
       {
           best_hand_card_value = tmpHandCardValue;
           best_card_group = CardTypeCheck::get_group_data(cgDOUBLE, card_num, 2);
       }
   }
}

void InitiativeCardStrategy::try_out_three_card(HandCardData& hand_card_data, int card_num,
                                           HandCardValue& best_hand_card_value, CardGroupData& best_card_group)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 3)
   {
       hand_card_data.value_aHandCardList[card_num] -= 3;
       hand_card_data.m_card_cnt -= 3;
       HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
       hand_card_data.value_aHandCardList[card_num] += 3;
       hand_card_data.m_card_cnt += 3;

       //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
       if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
           (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
       {
           best_hand_card_value = tmpHandCardValue;
           best_card_group = CardTypeCheck::get_group_data(cgTHREE, card_num, 3);
       }
   }
}

void InitiativeCardStrategy::try_out_single_line_card(HandCardData& hand_card_data, int card_num,
                                           HandCardValue& best_hand_card_value, CardGroupData& best_card_group)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 1)
   {
       int prov = 0;
       //记录顺子长度
       for (int j = card_num; j != ccpb::common::PokerValue::PM2; ++j)
       {
           if (hand_card_data.value_aHandCardList[j] >= 1)
               prov++;
           else
               break;
           //是否达到可出牌标准
           if (prov >= 5)
           {
               //每个顺子的牌减一
               for (int k = card_num; k <= j; ++k)
                   hand_card_data.value_aHandCardList[k]--;
               //手牌减去顺子长度
               hand_card_data.m_card_cnt -= prov;
               HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
               //加回去
               for (int k = card_num; k <= j; ++k)
                   hand_card_data.value_aHandCardList[k]++;

               hand_card_data.m_card_cnt += prov;

               //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                   (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
               {
                   best_hand_card_value = tmpHandCardValue;
                   best_card_group = CardTypeCheck::get_group_data(cgSINGLE_LINE, j, prov);
               }
           }
       }
   }
}

void InitiativeCardStrategy::try_out_double_line_card(HandCardData& hand_card_data, int card_num,
                                   HandCardValue& best_hand_card_value, CardGroupData& best_card_group)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 2)
   {
       int prov = 0;
       for (int j = card_num; j != ccpb::common::PokerValue::PM2; ++j)
       {
           if (hand_card_data.value_aHandCardList[j] >= 2)
               prov++;
           else
               break;
           if (prov >= 3)
           {
               for (int k = card_num; k <= j; ++k)
               {
                   hand_card_data.value_aHandCardList[k] -= 2;
               }
               hand_card_data.m_card_cnt -= prov * 2;
               HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
               for (int k = card_num; k <= j; k++)
               {
                   hand_card_data.value_aHandCardList[k] += 2;
               }
               hand_card_data.m_card_cnt += prov * 2;

               //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                   (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
               {
                   best_hand_card_value = tmpHandCardValue;
                   best_card_group = CardTypeCheck::get_group_data(cgDOUBLE_LINE, j, prov * 2);
               }
           }
       }
   }
}
void InitiativeCardStrategy::try_out_three_line_card(HandCardData& hand_card_data, int card_num,
                                   HandCardValue& best_hand_card_value, CardGroupData& best_card_group)
{
   if (hand_card_data.value_aHandCardList[card_num] >= 3)
   {
       int prov = 0;
       for (int j = card_num; j != ccpb::common::PokerValue::PM2; ++j)
       {
           if (hand_card_data.value_aHandCardList[j] >= 3)
               prov++;
           else
               break;

           if (prov >= 2)
           {
               for (int k = card_num; k <= j; k++)
               {
                   hand_card_data.value_aHandCardList[k] -= 3;
               }
               hand_card_data.m_card_cnt -= prov * 3;
               HandCardValue tmpHandCardValue = InitiativeCard::get_hand_card_value(hand_card_data);
               for (int k = card_num; k <= j; k++)
               {
                   hand_card_data.value_aHandCardList[k] += 3;
               }
               hand_card_data.m_card_cnt += prov * 3;

               //选取总权值-轮次*7值最高的策略  因为我们认为剩余的手牌需要n次控手的机会才能出完，若轮次牌型很大（如炸弹） 则其-7的价值也会为正
               if ((best_hand_card_value.SumValue - (best_hand_card_value.NeedRound * 7)) <=
                   (tmpHandCardValue.SumValue - (tmpHandCardValue.NeedRound * 7)))
               {
                   best_hand_card_value = tmpHandCardValue;
                   best_card_group = CardTypeCheck::get_group_data(cgTHREE_LINE, j, prov * 3);
               }
           }
       }
   }
}
} // namespace 
}
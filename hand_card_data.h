/*
 * @Descripttion: 手牌管理
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-14 21:16:59
 * @LastEditors: huazy
 * @LastEditTime: 2021-02-18 17:31:43
 */
#ifndef HAND_CARD_DATA_H
#define HAND_CARD_DATA_H
//#include <vector>
//
//#include "rule.h"
//#include "core/utils/poker.h"
//#include "card_define.h"

//#include "robot_passive_card.h"

//using std::vector;

namespace ccserver
{
    namespace landlord
    {
       class HandCardData
       {
       public:
           HandCardData() {}
           virtual ~HandCardData() {}

       public:
           friend class PassiveCardStrategy;
           friend class InitiativeCard;
           friend class InitiativeCardStrategy;
           //获取手牌中每个值的拍张个数
           void get_num_hand_card(ccserver::landlord::PokerSet hand_pokers);

           //清空数据
           void clear_card_data();

           //拷贝拍张数组
           inline void clone_num_card(int *src) {memcpy(src, value_aHandCardList, ccpb::common::PokerValue_ARRAYSIZE); }

           //
           void set_num_hand_card(int index, int value);

           int get_num_hand_card_one(int index);

           void add_num_hand_card(int index, int n = 0);

           void sub_num_hand_card(int index, int n = 0);

       private:
           //手牌序列——状态记录，便于一些计算，值域为该index牌对应的数量0~4
           int value_aHandCardList[ccpb::common::PokerValue_ARRAYSIZE] = {0};
           //vector<int> value_aHandCardList;
           //玩家要打出去的牌类型
           CardGroupData m_put_out_card_type;
           //手牌的张数
           int m_card_cnt = 0;
           //要打出去的牌——无花色
           vector <int> m_put_card_list;
       };
    } // namespace landlord
} // namespace ccserver

#endif
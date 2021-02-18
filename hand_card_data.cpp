/*
 * @Descripttion: 手牌管理
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-14 21:17:21
 * @LastEditors: huazy
 * @LastEditTime: 2021-02-18 17:31:36
 */
//#include "core/bits/bits.h"
//#include "hand_card_data.h"

namespace ccserver
{
    namespace landlord
    {
       void HandCardData::get_num_hand_card(ccserver::landlord::PokerSet hand_pokers)
       {
           u_int64_t rawdata = hand_pokers.data();
           memset(value_aHandCardList, 0x00, sizeof(value_aHandCardList));

           int j = 0;
           for (int i = PokerSet::min_poker_value; i != PokerSet::max_poker_value + 1; ++i) {
               value_aHandCardList[i] = ccserver::bits::count8(uint8_t(0xf & rawdata >> i));
               j += 4;
           }
       }

       void HandCardData::clear_card_data()
       {

       }

       void HandCardData::set_num_hand_card(int index, int n)
       {
           if (index >= 0 && index <= ccpb::common::PokerValue_ARRAYSIZE && n <= 3)
               value_aHandCardList[index] = n;
       }

       int HandCardData::get_num_hand_card_one(int index)
       {
           if (index >= 0 && index <= ccpb::common::PokerValue_ARRAYSIZE && index <= 3)
               return value_aHandCardList[index];
       }

       void HandCardData::add_num_hand_card(int index, int n)
       {
           if (index >= 0 && index <= ccpb::common::PokerValue_ARRAYSIZE && index <= 3
           )
           {
               if (n)
                   value_aHandCardList[index] += n;
               ++value_aHandCardList[index];
           }
       }

       void HandCardData::sub_num_hand_card(int index, int n)
       {
           if (index >= 0 && index <= ccpb::common::PokerValue_ARRAYSIZE && index <= 3)
           {
               if (value_aHandCardList[index] >= n)
               {
                   if (n)
                       value_aHandCardList[index] -= n;
                   --value_aHandCardList[index];
               }
           }
       }
    } // namespace landlord
} // namespace ccserver

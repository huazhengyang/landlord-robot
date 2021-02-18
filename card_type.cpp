/*
 * @Descripttion: 
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-16 21:47:25
 * @LastEditors: huazy
 * @LastEditTime: 2020-11-03 16:13:10
 */
#include "card_type.h"
#include "rule.h"
#include "core/proto/landlord_factory.h"
#include "core/utils/poker.h"

namespace ccserver
{
    namespace landlord
    {
        CardGroupData OneHandOutCard::one_hand_output_card(const int * hand_card_type_num, const int hand_card_num)
        {
            CardGroupData ret_card_group_data;
            ret_card_group_data.nCount = hand_card_num;

            bool b_ret = false;
            //单牌类型
            if (1 == hand_card_num)
            {
                b_ret = CardTypeCheck::is_single_card_type(hand_card_type_num, ret_card_group_data);
                if (b_ret)
                    return ret_card_group_data;
            }

            //对牌or火箭
            if (2 == hand_card_num)
            {
                b_ret = CardTypeCheck::is_double_card_type(hand_card_type_num, ret_card_group_data);
                if (b_ret)
                    return ret_card_group_data;

                b_ret = CardTypeCheck::is_rocket_card_type(hand_card_type_num, ret_card_group_data);
                if (b_ret)
                    return ret_card_group_data;
            }

            //三牌
            if (3 == hand_card_num)
            {
                b_ret = CardTypeCheck::is_three_card_type(hand_card_type_num, ret_card_group_data);
                if (b_ret)
                    return ret_card_group_data;
            }

            //三带一单or炸弹
            if (4 == hand_card_num)
            {
                //三带一单
                b_ret = CardTypeCheck::is_three_one_card_type(hand_card_type_num, ret_card_group_data);
                if (b_ret)
                    return ret_card_group_data;

                //炸弹
                b_ret = CardTypeCheck::is_bomb_card_type(hand_card_type_num, ret_card_group_data);
                if (b_ret)
                    return ret_card_group_data;
            }

            //三带一对or单顺
            if (5 == hand_card_num)
            {
                //三带一对
                b_ret = CardTypeCheck::is_three_pair_card_type(hand_card_type_num, ret_card_group_data);
                if (b_ret)
                    return ret_card_group_data;
            }

            //单顺
            if (hand_card_num >= 5)
            {
                b_ret = CardTypeCheck::is_single_line_card_type(hand_card_type_num, ret_card_group_data, hand_card_num);
                if (b_ret)
                    return ret_card_group_data;
            }

            //四带两单
            if (6 == hand_card_num)
            {
                b_ret = CardTypeCheck::is_four_one_card_type(hand_card_type_num, ret_card_group_data);
                if (b_ret)
                    return ret_card_group_data;
            }

            //对连or三连
            if (hand_card_num >= 6)
            {
                //对连
                b_ret = CardTypeCheck::is_double_line_card_type(hand_card_type_num, ret_card_group_data, hand_card_num);
                if (b_ret)
                    return ret_card_group_data;

                //三连
                b_ret = CardTypeCheck::is_three_line_card_type(hand_card_type_num, ret_card_group_data, hand_card_num);
                if (b_ret)
                    return ret_card_group_data;
            }

            //四带两对
            if (8 == hand_card_num)
            {
                b_ret = CardTypeCheck::is_four_pair_card_type(hand_card_type_num, ret_card_group_data);
                if (b_ret)
                    return ret_card_group_data;
            }

            //飞机(带单)
            if (hand_card_num >= 8)
            {
                b_ret = CardTypeCheck::is_three_line_one_card_type(hand_card_type_num, ret_card_group_data, hand_card_num);
                if (b_ret)
                    return ret_card_group_data;
            }

            //飞机(带对)
            if (hand_card_num >= 10)
            {
                b_ret = CardTypeCheck::is_three_line_pair_card_type(hand_card_type_num, ret_card_group_data, hand_card_num);
                if (b_ret)
                    return ret_card_group_data;
            }

            ret_card_group_data.cgType = cgERROR;
            return ret_card_group_data;
        }

//hand_card_list: 手牌序列，所有手牌
        CardGroupData OneHandOutCard::one_hand_output_card(vector<int> hand_card_list, const int hand_card_num)
        {
            int arr[18];
            memset(arr, 0, sizeof(arr));
            for (vector<int>::iterator iter = hand_card_list.begin(); iter != hand_card_list.end(); iter++)
                arr[*iter]++;
            return one_hand_output_card(arr, hand_card_list.size());
        }

        bool CardTypeCheck::is_single_card_type(const int * hand_card_type_num, CardGroupData& ret_data)
        {
            //牌型数
            int card_num = 0;
            //牌型价值
            int card_type_value = 0;
            for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker2 + 1; i++)
            {
                if (hand_card_type_num[i] == 1)
                {
                    card_type_value = i - 10;
                    card_num++;
                    ret_data.nMaxCard = i;
                }
            }

            if (1 == card_num)
            {
                ret_data.cgType = cgSINGLE;
                ret_data.nValue = card_type_value;
                return true;
            }

            return false;
        }

        bool CardTypeCheck::is_double_card_type(const int * hand_card_type_num, CardGroupData& ret_data)
        {
            //牌型数
            int card_num = 0;
            //牌型价值
            int card_type_value = 0;

            //对子不算火箭
            for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker1; i++)
            {
                if (hand_card_type_num[i] == 2)
                {
                    card_type_value = i - 10;
                    card_num++;
                    ret_data.nMaxCard = i;
                }
            }
            if (card_num == 1)
            {
                ret_data.cgType = cgDOUBLE;
                ret_data.nValue = card_type_value;
                return true;
            }

            return false;
        }

        bool CardTypeCheck::is_three_card_type(const int * hand_card_type_num, CardGroupData& ret_data)
        {
            int card_num = 0;
            int card_type_value = 0;

            for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker1; i++)
            {
                if (hand_card_type_num[i] == 3)
                {
                    card_type_value = i - 10;
                    card_num++;
                    ret_data.nMaxCard = i;
                }
            }
            if (card_num == 1)
            {
                ret_data.cgType = cgTHREE;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_three_one_card_type(const int * hand_card_type_num, CardGroupData& ret_data)
        {
            //牌型的主体部分
            int subject = 0;
            //牌型的带牌部分
            int additional = 0;

            int card_type_value = 0;
            for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker2 + 1; i++)
            {
                if (hand_card_type_num[i] == 3)
                {
                    card_type_value = i - 10;
                    subject++;
                    ret_data.nMaxCard = i;
                }
                if (hand_card_type_num[i] == 1)
                    additional++;
            }
            if (subject == 1 && additional == 1)
            {
                ret_data.cgType = cgTHREE_TAKE_ONE;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_three_pair_card_type(const int * hand_card_type_num, CardGroupData& ret_data)
        {
            int subject = 0;
            int additional = 0;
            int card_type_value = 0;

            for (int i = ccpb::common::PokerValue::P3; i != ccpb::common::PokerValue::PJoker1; ++i)
            {
                if (hand_card_type_num[i] == 3)
                {
                    card_type_value = i - 10;
                    subject++;
                    ret_data.nMaxCard = i;
                }
                if (hand_card_type_num[i] == 2)
                    additional++;
            }

            if (subject == 1 && additional == 1)
            {
                ret_data.cgType = cgTHREE_TAKE_TWO;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_four_one_card_type(const int * hand_card_type_num, CardGroupData& ret_data)
        {
            int subject = 0;
            int additional = 0;
            int card_type_value = 0;
            for (int i = ccpb::common::PokerValue::P3; i < ccpb::common::PokerValue::PJoker2 + 1; i++)
            {
                if (hand_card_type_num[i] == 4)
                {
                    card_type_value = (i - 3) / 2;
                    subject++;
                    ret_data.nMaxCard = i;
                }
                if (hand_card_type_num[i] == 1 || hand_card_type_num[i] == 2)
                    additional += hand_card_type_num[i];
            }

            if (subject == 1 && additional == 2)
            {
                ret_data.cgType = cgFOUR_TAKE_ONE;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_four_pair_card_type(const int * hand_card_type_num, CardGroupData& ret_data)
        {
            int subject = 0;
            int additional = 0;
            int card_type_value = 0;
            for (int i = ccpb::common::PokerValue::P3; i < ccpb::common::PokerValue::PJoker1; i++)
            {
                if (hand_card_type_num[i] == 4)
                {
                    card_type_value = (i - 3) / 2;
                    subject++;
                    ret_data.nMaxCard = i;
                    continue;
                }
                if (hand_card_type_num[i] == 2 || hand_card_type_num[i] == 4)
                    additional += hand_card_type_num[i] / 2;
            }
            //注意这里additional==4因为四牌也是两个对
            if (subject == 1 && additional == 2)
            {
                ret_data.cgType = cgFOUR_TAKE_TWO;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_bomb_card_type(const int * hand_card_type_num, CardGroupData& ret_data)
        {
            int subject = 0;
            int card_type_value = 0;
            for (int i = ccpb::common::PokerValue::P3; i < ccpb::common::PJoker1; i++)
            {
                if (hand_card_type_num[i] == 4)
                {
                    card_type_value += i - 3 + 7;
                    subject++;
                    ret_data.nMaxCard = i;
                }
            }
            if (subject == 1)
            {
                ret_data.cgType = cgBOMB_CARD;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_rocket_card_type(const int * hand_card_type_num, CardGroupData& ret_data)
        {
            int card_type_value = 0;
            if (hand_card_type_num[17] > 0 && hand_card_type_num[16] > 0)
            {
                card_type_value = 20;
                ret_data.nMaxCard = ccpb::common::PokerValue::PJoker2;
                ret_data.cgType = cgKING_CARD;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_single_line_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num)
        {
            int subject = 0;
            int card_type_value = 0;
            int i;
            for (i = ccpb::common::PokerValue::P3; i < ccpb::common::PokerValue::PM2; i++)
            {
                //第一个进入if的是单连的第一张，从这时prov开始加加
                if (hand_card_type_num[i] == 1)
                    subject++;
                else
                {
                    //因为是判断是否可以一手出完，所以当出现某张牌没有的时候，即到了单连的结尾，这时候跳出循环即可
                    if (subject != 0)
                        break;
                }
            }
            //计算单连的价值
            card_type_value = i - 10;
            if (subject == card_num)
            {
                ret_data.nMaxCard = i - 1;
                ret_data.cgType = cgSINGLE_LINE;
                ret_data.nValue = card_num;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_double_line_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num)
        {
            int subject = 0;
            int card_type_value = 0;
            int i;
            for (i = ccpb::common::PokerValue::P3; i < ccpb::common::PokerValue::PM2; i++)
            {
                if (hand_card_type_num[i] == 2)
                    subject++;
                else
                {
                    if (subject != 0)
                        break;
                }
            }
            card_type_value = i - 10;
            if (subject * 2 == card_num)
            {
                ret_data.nMaxCard = i - 1;
                ret_data.cgType = cgDOUBLE_LINE;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_three_line_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num)
        {
            int subject = 0;
            int card_type_value = 0;
            int i;
            for (i = ccpb::common::PokerValue::P3; i < ccpb::common::PokerValue::PM2; i++)
            {
                if (hand_card_type_num[i] == 3)
                    subject++;
                else
                {
                    if (subject != 0)
                        break;
                }
            }
            card_type_value = (i - 3) / 2;

            if (subject * 3 == card_num)
            {
                ret_data.nMaxCard = i - 1;
                ret_data.cgType = cgTHREE_LINE;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_three_line_one_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num)
        {
            int subject = 0;
            int card_type_value = 0;
            int i;
            for (i = ccpb::common::PokerValue::P3; i < ccpb::common::PokerValue::PM2; i++)
            {
                //飞机中有炸弹也可以组成飞机，44445555这样也可以算成飞机
                if (hand_card_type_num[i] >= 3)
                    subject++;
                else
                {
                    if (subject != 0)
                        break;
                }
            }
            card_type_value = (i - 3) / 2;
            if (subject * 4 == card_num)
            {
                ret_data.nMaxCard = i - 1;
                ret_data.cgType = cgTHREE_TAKE_ONE_LINE;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        bool CardTypeCheck::is_three_line_pair_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num)
        {
            int subject = 0;
            int additional = 0;
            int card_type_value = 0;
            int i;
            //循环主体部分
            for (i = ccpb::common::PokerValue::P3; i < ccpb::common::PokerValue::PM2; i++)
            {
                if (hand_card_type_num[i] == 3)
                    subject++;
                else
                {
                    if (subject != 0)
                        break;
                }
            }
            //带牌部分，考虑四个的情况
            for (int j = ccpb::common::PokerValue::P3; j < ccpb::common::PokerValue::PJoker1; j++)
            {
                if (hand_card_type_num[j] == 2 || hand_card_type_num[j] == 4)
                    additional += hand_card_type_num[j] / 2;
            }
            card_type_value = (i - 3) / 2;
            if (subject == additional && subject * 5 == card_num)
            {
                ret_data.nMaxCard = i - 1;
                ret_data.cgType = cgTHREE_TAKE_TWO_LINE;
                ret_data.nValue = card_type_value;
                return true;
            }
            return false;
        }

        CardGroupData CardTypeCheck::get_group_data(CardGroupType card_type, int max_card, int count)
        {
            CardGroupData ret_card_group_data;

            ret_card_group_data.cgType = card_type;
            ret_card_group_data.nCount = count;
            ret_card_group_data.nMaxCard = max_card;

            //不出牌型
            if (card_type == cgZERO)
                ret_card_group_data.nValue = 0;
                //单牌类型
            else if (card_type == cgSINGLE)
                ret_card_group_data.nValue = max_card - 10;
                //对牌类型
            else if (card_type == cgDOUBLE)
                ret_card_group_data.nValue = max_card - 10;
                //三条类型
            else if (card_type == cgTHREE)
                ret_card_group_data.nValue = max_card - 10;
                //单连类型
            else if (card_type == cgSINGLE_LINE)
                ret_card_group_data.nValue = max_card - 10 + 1;
                //对连类型
            else if (card_type == cgDOUBLE_LINE)
                ret_card_group_data.nValue = max_card - 10 + 1;
                //三连类型
            else if (card_type == cgTHREE_LINE)
                ret_card_group_data.nValue = (max_card - 3 + 1) / 2;
                //三带一单
            else if (card_type == cgTHREE_TAKE_ONE)
                ret_card_group_data.nValue = max_card - 10;
                //三带一对
            else if (card_type == cgTHREE_TAKE_TWO)
                ret_card_group_data.nValue = max_card - 10;
                //三带一单连
            else if (card_type == cgTHREE_TAKE_ONE_LINE)
                ret_card_group_data.nValue = (max_card - 3 + 1) / 2;
                //三带一对连
            else if (card_type == cgTHREE_TAKE_TWO_LINE)
                ret_card_group_data.nValue = (max_card - 3 + 1) / 2;
                //四带两单
            else if (card_type == cgFOUR_TAKE_ONE)
                ret_card_group_data.nValue = (max_card - 3) / 2;
                //四带两对
            else if (card_type == cgFOUR_TAKE_TWO)
                ret_card_group_data.nValue = (max_card - 3) / 2;
                //炸弹类型
            else if (card_type == cgBOMB_CARD)
                ret_card_group_data.nValue = max_card - 3 + 7;
                //王炸类型
            else if (card_type == cgKING_CARD)
                ret_card_group_data.nValue = 20;
                //错误牌型
            else
                ret_card_group_data.nValue = 0;

            return ret_card_group_data;
        }
    }
}

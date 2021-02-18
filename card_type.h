/*
 * @Descripttion: 牌型及分值定义，牌型判断的基础是牌张数
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-16 21:47:14
 * @LastEditors: huazy
 * @LastEditTime: 2020-10-23 21:22:40
 */

#ifndef CARD_TYPE_H
#define CARD_TYPE_H
#include <vector>

#include "card_define.h"

using std::vector;
namespace ccserver
{
    namespace landlord
    {
        class OneHandOutCard
        {
        public:
            OneHandOutCard() {}
            virtual ~OneHandOutCard() {}
        public:
            //判断是否可以一手出完
            static CardGroupData one_hand_output_card(const int *hand_card_type_num, const int hand_card_num);
            //判断是否可以一手出完
            static CardGroupData one_hand_output_card(vector<int> hand_card_type_num, const int hand_card_num);
        };

        class CardTypeCheck
        {
        public:
            //生成牌型结构
            static CardGroupData get_group_data(CardGroupType card_type, int max_card, int count);
            //单牌类型检测
            static bool is_single_card_type(const int * hand_card_type_num, CardGroupData& ret_data);

            //对牌类型检测
            static bool is_double_card_type(const int * hand_card_type_num, CardGroupData& ret_data);

            //三牌牌类型检测
            static bool is_three_card_type(const int * hand_card_type_num, CardGroupData& ret_data);

            //三带一类型检测
            static bool is_three_one_card_type(const int * hand_card_type_num, CardGroupData& ret_data);

            //三带一对类型检测
            static bool is_three_pair_card_type(const int * hand_card_type_num, CardGroupData& ret_data);

            //四带两单类型检测
            static bool is_four_one_card_type(const int * hand_card_type_num, CardGroupData& ret_data);

            //四带两对类型检测
            static bool is_four_pair_card_type(const int * hand_card_type_num, CardGroupData& ret_data);

            //炸弹类型检测
            static bool is_bomb_card_type(const int * hand_card_type_num, CardGroupData& ret_data);

            //火箭类型检测
            static bool is_rocket_card_type(const int * hand_card_type_num, CardGroupData& ret_data);

            //单连类型检测
            static bool is_single_line_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num);

            //对连类型检测
            static bool is_double_line_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num);

            //三连类型检测
            static bool is_three_line_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num);

            //三连带一单(飞机)类型检测
            static bool is_three_line_one_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num);

            //三连带一对(飞机)类型检测
            static bool is_three_line_pair_card_type(const int * hand_card_type_num, CardGroupData& ret_data, int card_num);
        };
    }
}
#endif
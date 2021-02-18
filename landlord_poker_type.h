/*
 * @Descripttion: 
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-10 11:29:49
 * @LastEditors: huazy
 * @LastEditTime: 2020-10-14 21:14:03
 */

#ifndef _CCSERVER_LANDLORD_POKER_TYPE_H
#define _CCSERVER_LANDLORD_POKER_TYPE_H

#include <map>
#include <vector>

#include "core/proto/landlord_factory.h"
#include "core/utils/poker.h"
#include "rule.h"

#define SCORE_OFFSET 10;

namespace ccserver
{
    namespace landlord
    {
        //扑克基础分值定义
        std::map<int, int> PokerScore = {
            {ccpb::common::PokerValue::P3, -7},
            {ccpb::common::PokerValue::P4, -6},
            {ccpb::common::PokerValue::P5, -5},
            {ccpb::common::PokerValue::P6, -4},
            {ccpb::common::PokerValue::P7, -3},
            {ccpb::common::PokerValue::P8, -2},
            {ccpb::common::PokerValue::P9, -1},
            {ccpb::common::PokerValue::P10, 0},
            {ccpb::common::PokerValue::PJ, 1},
            {ccpb::common::PokerValue::PQ, 2},
            {ccpb::common::PokerValue::PK, 3},
            {ccpb::common::PokerValue::PMA, 4},
            {ccpb::common::PokerValue::PM2, 5},
            {ccpb::common::PokerValue::PJoker1, 6},
            {ccpb::common::PokerValue::PJoker2, -7}};

        //
        // std::map<int, PokersKind> PokerTypeScore = {
        // 	{Landlord2Type::None, 0},
        // 	{Landlord2Type::Single1, PokersKind(1, 1, 0, 0)},
        // 	{Landlord2Type::Single5, PokersKind(1, 5, 0, 0)},
        // 	{Landlord2Type::Single6, PokersKind(1, 6, 0, 0)},
        // 	{Landlord2Type::Single7, PokersKind(1, 7, 0, 0)},
        // 	{Landlord2Type::Single8, PokersKind(1, 8, 0, 0)},
        // 	{Landlord2Type::Single9, PokersKind(1, 9, 0, 0)},
        // 	{Landlord2Type::Single10, PokersKind(1, 10, 0, 0)},
        // 	{Landlord2Type::Single11, PokersKind(1, 11, 0, 0)},
        // 	{Landlord2Type::Single12, PokersKind(1, 12, 0, 0)},
        // 	{Landlord2Type::Double1, PokersKind(2, 1, 0, 0)},
        // 	{Landlord2Type::Double3, PokersKind(2, 3, 0, 0)},
        // 	{Landlord2Type::Double4, PokersKind(2, 4, 0, 0)},
        // 	{Landlord2Type::Double5, PokersKind(2, 5, 0, 0)},
        // 	{Landlord2Type::Double6, PokersKind(2, 6, 0, 0)},
        // 	{Landlord2Type::Double7, PokersKind(2, 7, 0, 0)},
        // 	{Landlord2Type::Double8, PokersKind(2, 8, 0, 0)},
        // 	{Landlord2Type::Double9, PokersKind(2, 9, 0, 0)},
        // 	{Landlord2Type::Double10, PokersKind(2, 10, 0, 0)},
        // 	{Landlord2Type::ThreeSingle1, PokersKind(3, 1, 1, 1)},
        // 	{Landlord2Type::ThreeSingle2, PokersKind(3, 2, 1, 2)},
        // 	{Landlord2Type::ThreeSingle3, PokersKind(3, 3, 1, 3)},
        // 	{Landlord2Type::ThreeSingle4, PokersKind(3, 4, 1, 4)},
        // 	{Landlord2Type::ThreeSingle5, PokersKind(3, 5, 1, 5)},
        // 	{Landlord2Type::ThreeDouble1, PokersKind(3, 1, 2, 1)},
        // 	{Landlord2Type::ThreeDouble2, PokersKind(3, 2, 2, 2)},
        // 	{Landlord2Type::ThreeDouble3, PokersKind(3, 3, 2, 3)},
        // 	{Landlord2Type::ThreeDouble4, PokersKind(3, 4, 2, 4)},
        // 	{Landlord2Type::Three1, PokersKind(3, 1, 0, 0)},
        // 	{Landlord2Type::Three2, PokersKind(3, 2, 0, 0)},
        // 	{Landlord2Type::Three3, PokersKind(3, 3, 0, 0)},
        // 	{Landlord2Type::Three4, PokersKind(3, 4, 0, 0)},
        // 	{Landlord2Type::Three5, PokersKind(3, 5, 0, 0)},
        // 	{Landlord2Type::Three6, PokersKind(3, 6, 0, 0)},
        // 	{Landlord2Type::FourSingle1, PokersKind(4, 1, 1, 2)},
        // 	{Landlord2Type::FourDouble1, PokersKind(4, 1, 2, 2)},
        // 	{Landlord2Type::Bomb, PokersKind(4, 1, 0, 0)},
        // 	{Landlord2Type::Rocket, PokersKind(1, 2, 0, 0)},
        // };

        //牌型结构体
        typedef struct _poker_type
        {
            //牌型
            int poker_type = Landlord2Type::None;
            //牌的主体
            int value = 0;
            //评分
            int score = 0;
            //牌型矩阵
            PokersKind kind = PokersKind(0, 0, 0, 0);
            //含有的牌
            PokerSet data;

        } PokerType;

        typedef std::vector<PokerType> PokerTypeGroup;

        //牌型权值处理类
        class LandlordTypeScore
        {
        public:
            //计算牌型的分值
            static int get_poker_type_score(PokerType poker);

            //计算单张牌的权重
            static int get_poker_single_one_score(PokerType poker);

            //计算单顺的权重
            static int get_poker_single_score(PokerType poker);

            //计算对子的权重
            static int get_poker_double_one_score(PokerType poker);

            //计算双顺的权重
            static int get_poker_double_score(PokerType poker);

            //计算三带的权重
            static int get_poker_three_one_score(PokerType poker);

            //计算三张不带的权重
            static int get_poker_three_socre(PokerType poker);

            //计算飞机的权重
            static int get_poker_plane_score(PokerType poker);

            //炸弹
            static int get_poker_bomb_score(PokerType poker);

            //火箭
            static int get_poker_rocket_score(PokerType poker);

            //四带单或一对
            static int get_poker_four_score(PokerType poker);

            //poker的基础权重
            static int get_poker_base_score(int value);
        };

        //牌型检测处理类
        class LandlordTypeCheck
        {
        public:
            //去除王和2和单张，不能组合的牌
            void remove_no_compose(PokerSet data);

            //
            static bool has_poker(utils::Poker poker, PokerSet data);

            //组合去掉王和2及单牌后的手牌
            PokerTypeGroup poker_type_compose(PokerSet data);

            //获取最终的牌型组合
            PokerTypeGroup get_last_poker_type_group();
        };

    } // namespace landlord
} // namespace ccserver

#endif
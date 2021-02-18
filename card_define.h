/*
 * @Descripttion: 手牌的一些定义 
 * @version: 
 * @Author: huazy
 * @Date: 2020-10-16 15:08:28
 * @LastEditors: huazy
 * @LastEditTime: 2020-10-19 17:50:38
 */
#ifndef UNTITLED4_CARD_DEFINE_H
#define UNTITLED4_CARD_DEFINE_H
//最多手牌
#define HandCardMaxLen 20
//价值最小值
#define MinCardsValue -25
//价值最大值
#define MaxCardsValue 106

namespace ccserver
{
    namespace landlord
    {
        //手牌权值结构
        struct HandCardValue
        {
            int SumValue;  //手牌总价值
            int NeedRound; // 需要打几手牌
        };

        //手牌组合枚举
        enum CardGroupType
        {
            cgERROR = -1,               //错误类型
            cgZERO = 0,                 //不出类型
            cgSINGLE = 1,               //单牌类型
            cgDOUBLE = 2,               //对牌类型
            cgTHREE = 3,                //三条类型
            cgSINGLE_LINE = 4,          //单连类型
            cgDOUBLE_LINE = 5,          //对连类型
            cgTHREE_LINE = 6,           //三连类型
            cgTHREE_TAKE_ONE = 7,       //三带一单
            cgTHREE_TAKE_TWO = 8,       //三带一对
            cgTHREE_TAKE_ONE_LINE = 9,  //三带一单连，飞机带单
            cgTHREE_TAKE_TWO_LINE = 10, //三带一对连，飞机带对
            cgFOUR_TAKE_ONE = 11,       //四带两单
            cgFOUR_TAKE_TWO = 12,       //四带两对，暂时考虑四带四的情况
            cgBOMB_CARD = 13,           //炸弹类型
            cgKING_CARD = 14           //王炸类型
        };

        //牌型组合数据结构
        struct CardGroupData
        {
            //枚举类型
            CardGroupType cgType = cgERROR;
            //该牌的价值
            int nValue = 0;
            //含牌的个数
            int nCount = 0;
            //牌中决定大小的牌值，用于对比
            int nMaxCard = 0;
        };
    } // namespace landlord
} // namespace ccserver

#endif //UNTITLED4_CARD_DEFINE_H

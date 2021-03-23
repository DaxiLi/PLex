/*****************************************************************************
Copyright: 2021, Yuan Jie.
File name: main.cpp
Description: PL/0 语法的词法分析器
Author: Yuan Jie
Version: 1.0
Date:
History:
*****************************************************************************/

/**
* 变量名称最多支持50个字符，超过50个字符得部分将被忽略
* 所以需要保证变量名得前50字符不重复
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stack>

#ifndef INFILE
#define INFILE "D:\\wksps\\c\\PLex\\example.pl0"
#endif // INFILE

#ifndef ISNUM(val)
#define ISNUM(val) (val <= '9' && val >= '0')
#endif // ISNUM

#ifndef ISALP(val)
#define ISALP(val) (val >= 'A' && val <= 'Z' && val >= 'a' && val <= 'z')
#endif // ISALP

#ifndef ISSPACE(val)
#define ISSPACE(val) (val == ' ' || val == '\n' || val == 0x9 || val == 0)
#endif // ISSPACE
using namespace std;

enum WordType{
    TYPE_DEF,
    TYPE_ERROR = -1,
    TYPE_CONST,
    TYPE_OPT,
    TYPE_SPLIT,
    TYPE_INTEGER,
    TYPE_ID,
    TYPE_UNKNOW

};

enum ErrorCode{
    ERROR_UNKNOW,           // 未知错误
    ERROR_UNEXPECT_TERMINATION,  // 意外终止
    ERROR_UNKNOW_CHAR = -1,       // 不能识别的非法字符
    ERROR_OK_NOERROR,                 //正确不包含错误
    ERROR_EMPTY                 // 空 即未识别到有效字符
};

typedef struct sWordNode sWordNode;

/* 单词类型结构体 */
struct sWordNode{
    int     iLine; //   the row line
    int     iAddres; // the colum
    enum WordType eType; // word type
    enum ErrorCode eErrorCode;
    char    *pcValue;
    sWordNode* pNext;
};




char type_string[10][20] = {
    "CONST CHAR",
    "OPTION CHAR",
    "SPLIT CHAR",
    "INTEGER CHAR",
    "ID CHAR",
    "ERROR:UNKNOW TYPE"
};

/** const char **/
char const_chars[14][10] =  {
    "begin",
    "call",
    "const",
    "do",
    "end",
    "if",
    "odd",
    "procedur",
    "read",
    "then",
    "var",
    "while",
    "write"
};

/** 操作符 **/
char opt_chars[11][5] =
 {
    "+",
    "-",
    "*",
    "/",
    "<",
    "<=",
    ">",
    ">=",
    "#",
    "=",
    ":="
    };

/** 分隔符 主要用来取词和识别标点 **/
char split_chars[5] = {
    '(',
    ')',
    ',',
    ';',
    '.'
};

char other_split_chars[] = {
    '\0',
    ' ',
    '\n',
    0x9
};

void test();
enum WordType lex(char* str);
void wordAnalyze(sWordNode* val);

/* 初始化 头节点 等操作*/
void init();

/* 利用此函数获取下一个字符，这样不论文件输入还是字符串输入都只需要修改此函数即可 */
char getch(); // 在写getword的时候没有用到，本来要用的，。。。。闲置辽，改用重写fgetWord（） 了

/* 输出报错信息 传入错误类型得枚举值 */
void error(enum ErrorCode code);

/* 分词函数，将输入分隔为单词单位 由lex 函数调用，负责将字符串按照分隔符分割为词单位
* 将全局变量 BUF[] 作为输入源
*/
sWordNode* getWord();

/* 从文件读取的分词函数，与getWord功能相同，输入换位文件指针 留备函数*/
sWordNode* fgetWord();

sWordNode* creatDefNode();

/* 程序结束，释放所有节点 */
void freeAllNode();

/* check c is split char or not */
int isSplit(char c);


/* 根节点 和 尾节点
*尾节点只有在双向链表时才有用，但是没想好双向链表有什么用，先用单链表
*/
sWordNode* psRoot,*psEnd;

/* 全局变量 BUF ，如一次性输入所有字符，将存到这里 */
char *pBUF = NULL;

/* func wordAnlyze
* 把 结构体链表 的词语进行依次分析词性并写入结构体
* 遇到错误时终止分析并打印错误信息
* */
void wordAnalyze(sWordNode* val)
{

}

/*func lex
* 字符串词性分析，对字符串进行单纯的词法分析操作
* 递归调用
* */
enum WordType lex(char* str)
{
    if (ISSPACE(*str)) // 结束字符 错误
    {
        return TYPE_ERROR;
    }
    int iL = 0;
    int iR = strlen(const_chars);
    while (iL <= iR) // 折半查找 比对 CONST
    {
        if (strcmp(const_chars[(int)(iL + iR)],str) < 0)
        {
            iL = (int)(iL + iR)/2 + 1;
        }else if (strcmp(const_chars[(int)(iL + iR)],str) > 0)
        {
            iR = (int)(iL + iR)/2 - 1;
        }else
        {
            return TYPE_CONST;
        }
    }
    if (ISALP(*str)) // 字母开头
    {
        while (ISALP(*str))
        {
            str++;
        }
        if (ISSPACE(*str)) //遇到非字母判断是否
        {
            return TYPE_ID
        }
    }else if (ISNUM(*str))
    {

    }else
    {
        return TYPE_UNKNOW;
    }
}


sWordNode*
getWord(){
    sWordNode *ret = creatDefNode();
    ret->eErrorCode = ERROR_EMPTY;
    if (pBUF == NULL)
    {
        ret->eErrorCode = ERROR_UNEXPECT_TERMINATION;
        return ret;
    }
    static char *pIndex = pBUF;     // 当前字符指针，指向当前处理字符地址
    static char *pLineFlag = pBUF;  // 指向当前已处理字符的最后一个换行符
    static int line = 1;            // 行号计数
//    static int addres = 1;
    char *pFlag = pIndex;
    while (*pIndex != '\0')
    {
        if(*pIndex == ' ' || *pIndex == 0x9)
        {
            pIndex++;
            continue;
        }
        if (*pIndex == '\n')
        {
            line++;
            pLineFlag = pIndex;
            pIndex++;
            continue;
        }
        ret->iLine = line;
        ret->iAddres = pIndex - pLineFlag + 1;  //当前地址减去回车地址，得到字符数
        pFlag = pIndex;
       int flag = isSplit(*pIndex); // 第一个字符的类型吧
       while (flag == isSplit(*pIndex) // 当和第一个字符一样的类型，则属于同一个词
              && !ISSPACE(*pIndex))    // 不是 空格 TAB 换行 0 等字符
       {
            pIndex++;
       }
       ret->pcValue = (char*)malloc(pIndex - pFlag + 1); // 为字符串申请空间
       int i; // 将词复制到 申请空间内
       for ( i = 0;pFlag < pIndex;i++,pFlag++)
        {
            ret->pcValue[i] = *pFlag;
        }
        ret->pcValue[i] = '\0';
        ret->eErrorCode = ERROR_OK_NOERROR;// 打上标签 准备送出
        return ret;
    }
    return ret;
}

/* func isSplit
* 是否分隔符
* 主要检查两组数据 opt_chars split_chars
* 因为当中的分割符，可以存在 ");   和=(
* 这种连用情况，所以根据单词起始状态分词不再有效
* 为了解决这种情况，应该将 单字符 操作符和多字符操作符 分开
* 在处理单字符 操作符时，不同的字符将返回不同的 true 值
* 这样在根据其实状态分词时，就能够把连用的 单字符操作符 分开
* 而多字符操作符任然保持连贯，而不用将所有操作符单个分开后期
* 二次处理，但是，这样带来一个麻烦时类似c语言中的++ 这样的
* 单目运算符后期处理变得困难，因为可能出现i+++++++这样的连用情况
* 所以如果出现这样的操作符，还是应该把所有操作符分别取词，
* 后期语法分析时，再进行多字符操作符的分析
* */
int isSplit(char c)
{
    int i = 1;
    for (auto str : opt_chars)
    {
        if (str[0] == c)
        {
             return 1;
        }
    }
    for (auto val : split_chars)
    {
        i++;
        if (val == c)
        {
             return i;
        }
    }
    return 0;
}

void error(enum ErrorCode code)
{
    ErrorCode ec;
    switch (code)
    {
    case ERROR_OK_NOERROR :
        cout << "cdc";
        break;
    case ERROR_UNEXPECT_TERMINATION :

        break;
    default:
        cout << "lex can not reach here!";
    }
}


sWordNode*
creatDefNode()
{
    static sWordNode* pIndex = NULL; // index 指向链表最末端节点，用以向尾部插入数据
    sWordNode* ret = (sWordNode*)malloc(sizeof(sWordNode));
    ret->iLine = -1;
    ret->iAddres = -1;
    ret->eType = TYPE_UNKNOW;
    ret->eErrorCode = ERROR_OK_NOERROR;
    ret->pcValue = NULL;
    ret->pNext = NULL;
    if (pIndex) //当链表中存在节点 才插入，否则直接赋值给头节点
    {
        pIndex->pNext = ret;
    }else
    {
        psRoot = ret;
    }
    pIndex = ret;
    return ret;
}

void init()
{
    psRoot = creatDefNode();
    psEnd = creatDefNode();

    /* do other some things */
}

void freeAllNode()
{
    sWordNode* pIndex = psRoot;
    while(psRoot)
    {
        psRoot = pIndex->pNext;
        free(pIndex->pcValue);
        free(pIndex);
        pIndex = psRoot;
    }
}

int main()
{
    test();
    cout << "Hello world!" << endl;
    return 0;
}


void test()
{
    /** 测试分词程序 */
    char str[300] =
    "const a=10;\n\
var b,c;\n\
\n\
procedure p\n\
begin\n\
    c:=b+a;\n\
end;\n\
\n\
begin\n\
    read(b);\n\
    while b#0 do\n\
    begin\n\
        call p;\n\
        write(2*c);\n\
        read(b);\n\
    end\n\
end.\0";
    pBUF = str;
    sWordNode* node = NULL;
    node = getWord();
    while ( node->eErrorCode != ERROR_EMPTY)
    {
        cout << "line: " << setw(3) <<node->iLine << " No." << setw(5) << node->iAddres <<":";
        cout << node->pcValue <<endl;
        node = getWord();
    }
    /** 测试链表是否正常 */

    sWordNode* tmp = psRoot;
    while(tmp)
    {
        cout << tmp->pcValue <<endl;
        tmp = tmp->pNext;
    }
    freeAllNode();
}

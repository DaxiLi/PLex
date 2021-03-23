/*****************************************************************************
Copyright: 2021, Yuan Jie.
File name: main.cpp
Description: PL/0 �﷨�Ĵʷ�������
Author: Yuan Jie
Version: 1.0
Date:
History:
*****************************************************************************/

/**
* �����������֧��50���ַ�������50���ַ��ò��ֽ�������
* ������Ҫ��֤��������ǰ50�ַ����ظ�
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
    ERROR_UNKNOW,           // δ֪����
    ERROR_UNEXPECT_TERMINATION,  // ������ֹ
    ERROR_UNKNOW_CHAR = -1,       // ����ʶ��ķǷ��ַ�
    ERROR_OK_NOERROR,                 //��ȷ����������
    ERROR_EMPTY                 // �� ��δʶ����Ч�ַ�
};

typedef struct sWordNode sWordNode;

/* �������ͽṹ�� */
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

/** ������ **/
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

/** �ָ��� ��Ҫ����ȡ�ʺ�ʶ���� **/
char split_chars[7] = {
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
/* ��ʼ�� ͷ�ڵ� �Ȳ���*/
void init();

/* ���ô˺�����ȡ��һ���ַ������������ļ����뻹���ַ������붼ֻ��Ҫ�޸Ĵ˺������� */
char getch();

/* ���������Ϣ ����������͵�ö��ֵ */
void error(enum ErrorCode code);

/* �ִʺ�����������ָ�Ϊ���ʵ�λ ��lex �������ã������ַ������շָ����ָ�Ϊ�ʵ�λ
* ��ȫ�ֱ��� BUF[] ��Ϊ����Դ
*/
sWordNode* getWord();

/* ���ļ���ȡ�ķִʺ�������getWord������ͬ�����뻻λ�ļ�ָ�� ��������*/
sWordNode* fgetWord();

sWordNode* creatDefNode();

/* ����������ͷ����нڵ� */
void freeAllNode();

/* check c is split char or not */
int isSplit(char c);


/* ���ڵ� �� β�ڵ�
*β�ڵ�ֻ����˫������ʱ�����ã�����û���˫��������ʲô�ã����õ�����
*/
sWordNode* psRoot,*psEnd;

/* ȫ�ֱ��� BUF ����һ�������������ַ������浽���� */
char *pBUF = NULL;

sWordNode* getWord(){
    sWordNode *ret = creatDefNode();
    ret->eErrorCode = ERROR_EMPTY;
    if (pBUF == NULL)
    {
        ret->eErrorCode = ERROR_UNEXPECT_TERMINATION;
        return ret;
    }
    static char *pIndex = pBUF;     // ��ǰ�ַ�ָ�룬ָ��ǰ�����ַ���ַ
    static char *pLineFlag = pBUF;  // ָ��ǰ�Ѵ����ַ������һ�����з�
    static int line = 1;            // �кż���
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
        ret->iAddres = pIndex - pLineFlag + 1;  //��ǰ��ַ��ȥ�س���ַ���õ��ַ���
        pFlag = pIndex;
       int flag = isSplit(*pIndex); // ��һ���ַ������Ͱ�
       while (flag == isSplit(*pIndex) // ���͵�һ���ַ�һ�������ͣ�������ͬһ����
              && !ISSPACE(*pIndex))    // ���� �ո� TAB ���� 0 ���ַ�
       {
            pIndex++;
       }
       ret->pcValue = (char*)malloc(pIndex - pFlag + 1); // Ϊ�ַ�������ռ�
       int i; // ���ʸ��Ƶ� ����ռ���
       for ( i = 0;pFlag < pIndex;i++,pFlag++)
        {
            ret->pcValue[i] = *pFlag;
        }
        ret->pcValue[i] = '\0';
        ret->eErrorCode = ERROR_OK_NOERROR;// ���ϱ�ǩ ׼���ͳ�
        return ret;
    }
    return ret;
}

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
    static sWordNode* pIndex = NULL; // index ָ��������ĩ�˽ڵ㣬������β����������
    sWordNode* ret = (sWordNode*)malloc(sizeof(sWordNode));
    ret->iLine = -1;
    ret->iAddres = -1;
    ret->eType = TYPE_UNKNOW;
    ret->eErrorCode = ERROR_OK_NOERROR;
    ret->pcValue = NULL;
    ret->pNext = NULL;
    if (pIndex) //�������д��ڽڵ� �Ų��룬����ֱ�Ӹ�ֵ��ͷ�ڵ�
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
    /** ���Էִʳ��� */
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
    /** ���������Ƿ����� */

    sWordNode* tmp = psRoot;
    while(tmp)
    {
        cout << tmp->pcValue <<endl;
        tmp = tmp->pNext;
    }
    freeAllNode();
}

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
#define ISALP(val) ( (val >= 'A' && val <= 'Z') || (val >= 'a' && val <= 'z') )
#endif // ISALP

#ifndef ISSPACE(val)
#define ISSPACE(val) (val == ' ' || val == '\n' || val == 0x9 || val == 0)
#endif // ISSPACE
using namespace std;

enum WordType{
    TYPE_DEF = 100,
    TYPE_ERROR,   // ���� ����
    TYPE_CONST, // const
    TYPE_OPT, // option char
    TYPE_SPLIT, // split char
    TYPE_INTEGER, // uint
    TYPE_ID,        // variable
    TYPE_UNKNOW_CHAR,     // unknow char error: stray '@' in program|
    TYPE_UNLEGAL_UINT, //unable to find numeric literal operator 'operator""pj'
    TYPE_PARASE_FAILED
};

enum ErrorCode{
    ERROR_UNKNOW = 200,           // δ֪����
    ERROR_UNEXPECT_TERMINATION,  // ������ֹ
    ERROR_UNKNOW_CHAR,       // ����ʶ��ķǷ��ַ�
    ERROR_OK_NOERROR,                 //��ȷ����������
    ERROR_EMPTY, // ,               // �� ��δʶ����Ч�ַ�
    ERROR_PARASE_FAILED         // ʶ�𵽷Ƿ�
};

typedef struct sWordNode sWordNode;

/* �������ͽṹ�� */
struct sWordNode{
    int     iLine; //   the row line
    int     iAddres; // the colum
    int eType; // word type
    int  eErrorCode;
    char    *pcValue;
    sWordNode* pNext;
};

typedef struct sMsg Msg;

struct sMsg{
    int type;
    char *msg;
    char *errorChar;
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


void showType(sWordNode *node);

Msg*
newMsg();

void freeMsg(Msg *msg);

void test();
Msg* lex(char* str);
void wordAnalyze(sWordNode* val);

/* ��ʼ�� ͷ�ڵ� �Ȳ���*/
void init();

/* ���ô˺�����ȡ��һ���ַ������������ļ����뻹���ַ������붼ֻ��Ҫ�޸Ĵ˺������� */
char getch(); // ��дgetword��ʱ��û���õ�������Ҫ�õģ��������������ɣ�������дfgetWord���� ��

/* ���������Ϣ ����������͵�ö��ֵ */
void error(int code);

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

void freeMsg(Msg *msg)
{
        if (msg == NULL)
        {
            return;
        }
        free(msg->errorChar);
        free(msg->msg);
        free(msg);
}

Msg* newMsg()
{
    Msg* ret = (Msg*)malloc(sizeof(Msg));
    ret->errorChar = NULL;
    ret->msg = NULL;
    ret->type = TYPE_DEF;
    return ret;
}

/* func wordAnlyze
* �� �ṹ������ �Ĵ���������η������Բ�д��ṹ��
* ��������ʱ��ֹ��������ӡ������Ϣ
* */
void wordAnalyze(sWordNode* val)
{
    if (val == NULL)
    {
        return;
    }
    Msg *msg = lex(val->pcValue);
    val->eType = msg->type;
    switch (val->eType)
    {
    case TYPE_CONST:
        cout << "(   �ؼ���  ," << val->pcValue<<")" <<endl;
        break;
    case TYPE_OPT:
        cout << "(   ������  ,"<< val->pcValue<<")" <<endl;
        break;
    case TYPE_SPLIT:
        cout << "(   �ָ���  ,"<< val->pcValue<<")" <<endl;
        break;
    case TYPE_ID:
        cout << "(   ������  ,"<< val->pcValue<<")" <<endl;
        break;
    case TYPE_INTEGER:
        cout << "( �޷�������,"<< val->pcValue<<")" <<endl;
        break;
    case TYPE_UNKNOW_CHAR:
        cout << "error:stray \'" << msg->errorChar <<"\' in program";
        break;
    case TYPE_UNLEGAL_UINT:
        cout << "error:unable to find numeric literal operator 'operator";
        break;
    default:
        cout << "lex can not reach here!"<<endl;
    }
}

/*func lex
* �ַ������Է��������ַ������е����Ĵʷ���������
* �ݹ����
* */
Msg* lex(char* str)
{
    Msg* ret = newMsg();
    if (ISSPACE(*str)) // �����ַ� ����
    {
        //int @mkd = 1;
        ret->type = TYPE_ERROR;
        return ret;
    }

    if (ISALP(*str)) // ��ĸ��ͷ
    {
        for (auto val : const_chars)
        {
            if (strcmp(val,str) == 0)
            {
                ret->type = TYPE_CONST;
                return ret;
            }
        }

        while (*(++str) != '\0')
        {
            if ( !ISNUM(*str) && !ISALP(*str))
            {
                ret->type = TYPE_UNKNOW_CHAR;
                ret->errorChar = (char*) malloc(strlen(str) + 1);
                strcpy(ret->errorChar,str);
                return ret;
            }
//            str++;
        }
        ret->type = TYPE_ID;
        return ret;
    }else if (ISNUM(*str))
    {
        while (*(++str) != '\0')
        {
            if (!ISNUM(*str))
            {
                ret->type = TYPE_UNLEGAL_UINT;
                ret->errorChar = (char*) malloc(strlen(str) + 1);
                cout <<str;
                strcpy(ret->errorChar,str);
                return ret;
            }
//            str++;
        }
        ret->type = TYPE_INTEGER;
        return ret;
    }else
    {
        for (auto val : opt_chars)
        {
            if (strcmp(val,str) == 0)
            {
                ret->type = TYPE_OPT;
                return ret;
            }
        }
        for (auto val : split_chars)
        {
            if (strcmp(&val,str) == 0)
            {
                ret->type = TYPE_SPLIT;
                return ret;
            }
        }
    }
    ret->type = TYPE_UNKNOW_CHAR;
    ret->msg = (char*) malloc(30);
    strcpy(ret->msg,"lex can not reach here!");
    return ret;
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

/* func isSplit
* �Ƿ�ָ���
* ��Ҫ����������� opt_chars split_chars
* ��Ϊ���еķָ�������Դ��� ");   ��=(
* ����������������Ը��ݵ�����ʼ״̬�ִʲ�����Ч
* Ϊ�˽�����������Ӧ�ý� ���ַ� �������Ͷ��ַ������� �ֿ�
* �ڴ����ַ� ������ʱ����ͬ���ַ������ز�ͬ�� true ֵ
* �����ڸ�����ʵ״̬�ִ�ʱ�����ܹ������õ� ���ַ������� �ֿ�
* �����ַ���������Ȼ�������ᣬ�����ý����в����������ֿ�����
* ���δ������ǣ���������һ���鷳ʱ����c�����е�++ ������
* ��Ŀ��������ڴ��������ѣ���Ϊ���ܳ���i+++++++�������������
* ����������������Ĳ�����������Ӧ�ð����в������ֱ�ȡ�ʣ�
* �����﷨����ʱ���ٽ��ж��ַ��������ķ���
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

void showType(sWordNode *node)
{
    if (node->eErrorCode != ERROR_OK_NOERROR)
    {
        error(node->eErrorCode);
        return;
    }

}

void error(int code)
{

    switch (code)
    {
    case ERROR_UNKNOW :
        cout << "cdc";
        break;
    case ERROR_UNEXPECT_TERMINATION :

        break;
    case ERROR_UNKNOW_CHAR:

        break;
    case ERROR_OK_NOERROR:

        break;
    case ERROR_EMPTY:

        break;
    case TYPE_DEF:

        break;
    case TYPE_ERROR:

        break;
    case TYPE_UNKNOW_CHAR:

        break;
    case TYPE_UNLEGAL_UINT:

        break;
    case TYPE_PARASE_FAILED:

        break;
    default:
        cout << "all thing looks good!";
    }
}


sWordNode*
creatDefNode()
{
    static sWordNode* pIndex = NULL; // index ָ��������ĩ�˽ڵ㣬������β����������
    sWordNode* ret = (sWordNode*)malloc(sizeof(sWordNode));
    ret->iLine = -1;
    ret->iAddres = -1;
    ret->eType = TYPE_DEF;
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



    /* ����lex */
    char lex_test[10][20] = {
        "const",
        "var",
        "a",
        "123",
        "123pl",
        "abc@",
        "8520"
    };
    for (auto val : lex_test)
    {
        Msg* msg = lex(val);
        cout << val <<" " << msg->type <<endl;
    }
   // return;
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
//
//    sWordNode* tmp = psRoot;
//    while(tmp)
//    {
//        cout << tmp->pcValue <<endl;
//        tmp = tmp->pNext;
//    }
   /* ���Դʷ����� */
    sWordNode* tmp = psRoot->pNext;
    while(tmp&& tmp->eErrorCode == ERROR_OK_NOERROR)
    {
        wordAnalyze(tmp);
        tmp = tmp->pNext;
    }


    freeAllNode();
}

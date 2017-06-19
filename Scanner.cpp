#include <iostream>
#include <fstream>
#include <Windows.h>
#include <cstring>
#include<sstream>
#include <set>
#include <algorithm>
using namespace std;
ifstream fin;
ofstream fout;
string FileName,ScannerText;
char StrLine[8];
set<char> G_AlphaTable,G_NumTable,G_SymbolTable;
set<string>G_KeyWords;
stringstream stream;
void Read_SourceCode(char[]);
void Scanner(string,int);
void OutFile(string);
bool isCOM;
int LineNum;
void init()
{
    LineNum=1;
    for(int i=65;i<=90;i++)G_AlphaTable.insert(char(i));
    for(int i=97;i<=122;i++)G_AlphaTable.insert(char(i));//字母表
    
    for(int i=48;i<=57;i++)G_NumTable.insert(char(i));//数字表
    
    G_SymbolTable.insert('+');G_SymbolTable.insert('-');G_SymbolTable.insert('*');//符号表
    G_SymbolTable.insert('/');G_SymbolTable.insert('<');G_SymbolTable.insert('>');
    G_SymbolTable.insert('=');G_SymbolTable.insert(';');G_SymbolTable.insert(',');
    G_SymbolTable.insert('(');G_SymbolTable.insert(')');G_SymbolTable.insert('[');
    G_SymbolTable.insert(']');G_SymbolTable.insert('{');G_SymbolTable.insert('}');
    G_SymbolTable.insert(' ');G_SymbolTable.insert('\n');G_SymbolTable.insert('\t');
    G_SymbolTable.insert('!');G_SymbolTable.insert('\r');
    
    G_KeyWords.insert("if");G_KeyWords.insert("else");G_KeyWords.insert("int");//关键词表
    G_KeyWords.insert("return");G_KeyWords.insert("void");G_KeyWords.insert("while");
    
}

int main(int argc,char* argv[])//入口函数
{
    init();//标志初始化
    isCOM=false;
    char L_SourcePath[256];//源代码路径
    if (argv[1]!=0)
    {
        strcpy(L_SourcePath,argv[1]);//获取源代码路径
        FileName=L_SourcePath;
        int pos = FileName.find_last_of('.');
        FileName=FileName.substr(0,pos);
        Read_SourceCode(L_SourcePath);
    }
    return 0;
}

void Read_SourceCode(char Path[])//源代码读取
{
    char L_CodeCont;//代码内容
    string CodeString="",SymbolString="";
    fin.open(Path);//打开源代码
    ScannerText="";
    while(fin>>noskipws>>L_CodeCont)//不跳过空白字符的读取
    {
        if(G_SymbolTable.count(L_CodeCont))//符号
        {
            SymbolString+=L_CodeCont;
            if(CodeString!="")
            {
                Scanner(CodeString,0);
                CodeString="";
            }
        }
        else//关键字串 （变量，数字，关键字）
        {
            if(SymbolString!="")
            {
                Scanner(SymbolString,1);
                SymbolString="";
            }
            CodeString+=L_CodeCont;
        }
    }
    if(CodeString!="")Scanner(CodeString,0);
    if(SymbolString!="")Scanner(SymbolString,1);
    OutFile(FileName+".tok");
}

void OutToken(string Type,string KeyStr,int LNum)
{
    if(KeyStr!="")
        ScannerText+=Type+KeyStr+" "+itoa(LineNum,StrLine,10)+"\n";
}

void getToken(string KeyString,int Kind)
{
    bool Accepted_Num=true;
    char prech;
    int i=0,Len=KeyString.length();
    if(Kind==0)
    {
        //	ScannerText+=KeyString;//输出源代码
        if(G_KeyWords.count(KeyString))//关键字
        {
            OutToken("<Key> ",KeyString,LineNum);
            
        }
        else//数字，变量
        {
            
            for(int i=0;i<Len;i++)
                if(!G_NumTable.count(KeyString[i]))Accepted_Num=false;
            if(Accepted_Num)OutToken("<Num> ",KeyString,LineNum);
            else OutToken("<ID> ",KeyString,LineNum);
        }
        
    }
    else//识别符号
    {
        if(KeyString=="==")OutToken("<Sym> ","==",LineNum);
        else if(KeyString==">=")OutToken("<Sym> ",">=",LineNum);
        else if(KeyString=="<=")OutToken("<Sym> ","<=",LineNum);
        else if(KeyString=="!=")OutToken("<Sym> ","!=",LineNum);
        else
        {
            while(i<Len)
            {
                switch(KeyString[i])
                {
                    case '+'://识别+
                    {
                        OutToken("<Sym> ","+",LineNum);
                        break;
                    }
                    case '-'://识别-
                    {
                        OutToken("<Sym> ","-",LineNum);
                        break;
                    }
                    case '*'://识别*,*/ ,/*
                    {
                        if(Len==1)
                        {
                            OutToken("<Sym> ","*",LineNum);
                        }
                        else if(prech=='/')
                        {
                            prech='\0';
                            isCOM=true;//标志之后的为注释内容
                        }
                        else prech='*';
                        break;
                    }
                    case '/'://识别/,*/ ,/*
                    {
                        if(Len==1)
                        {
                            OutToken("<Sym> ","/",LineNum);
                        }
                        else if(prech=='*')
                        {
                            prech='\0';
                            isCOM=false;//标志注释内容完成
                        }
                        else prech='/';
                        break;
                    }
                    case ','://识别,
                    {
                        OutToken("<Sym> ",",",LineNum);
                        break;
                    }
                    case ';'://识别;
                    {
                        OutToken("<Sym> ",";",LineNum);
                        break;
                    }
                    case '('://识别(
                    {
                        OutToken("<Sym> ","(",LineNum);
                        break;
                    }
                    case ')'://识别)
                    {
                        OutToken("<Sym> ",")",LineNum);
                        break;
                    }
                    case '{'://识别{
                    {
                        OutToken("<Sym> ","{",LineNum);
                        break;
                    }
                    case '}'://识别}
                    {
                        OutToken("<Sym> ","}",LineNum);
                        break;
                    }
                    case '\n'://识别回车
                    {
                        LineNum++;
                        break;
                    }
                    case '['://识别[
                    {
                        OutToken("<Sym> ","[",LineNum);
                        break;
                    }
                    case ']'://识别]
                    {
                        OutToken("<Sym> ","]",LineNum);
                        break;
                    }
                    case '<'://识别]<,<=
                    {
                        if(KeyString[i+1]=='=')
                        {
                            OutToken("<Sym> ","<=",LineNum);
                            i++;
                        } 
                        else OutToken("<Sym> ","<",LineNum);
                        break;
                    }
                    case '>'://识别]>,>=
                    {
                        if(KeyString[i+1]=='=')
                        {
                            OutToken("<Sym> ",">=",LineNum);
                            i++;
                        } 
                        else OutToken("<Sym> ",">",LineNum);
                        break;
                    }
                    case '='://识别]<=,==,=,>=,!=
                    {
                        if(KeyString[i+1]=='=')
                        {
                            OutToken("<Sym> ","==",LineNum);
                            i++;
                        } 
                        else OutToken("<Sym> ","=",LineNum);
                        break;
                    }
                    case '!'://识别!=
                    { 
                        if(KeyString[i+1]=='=')
                        {
                            OutToken("<Sym> ","!=",LineNum);
                            i++;
                        } 
                        else OutToken("<Sym> ","!",LineNum);
                        break;
                    }
                    default:break;
                }
                i++;
            }
        }
    }
}
void Scanner(string KeyString,int Kind)
{
    if(!isCOM || KeyString.find("*/")!=-1)//对于注释内容不进行解析 
    {
        if(Kind==0)//关键字串（变量，数字，关键字） 
        {
            getToken(KeyString,0);	
        }
        else if(Kind==1)//符号
        {
            getToken(KeyString,1);		
        }
    }
} 

void OutFile(string OutPath)
{
    char Path[256];
    ScannerText+="<EOF> <EOF> -1";
    strcpy(Path,OutPath.c_str());
    fout.open(Path);
    fout<<ScannerText;//输出Scanner结果 
    fout.close();
}

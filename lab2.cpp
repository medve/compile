#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

struct descr_item
{
    int table;
    int idx;
};

using namespace std;

const int delims_num = 21;
const int keywords_num = 10;
const int funcs_num = 4;
char* NOT_PAIR_BRACKETS = "Some brackets are not closed\n";
char* FILE_NOT_OPEN = "File can not be oppenned!\n";
char* DELIM_NOT_FOUND = "Delimeter is not find\n";
char* INVALID_NUMBER = "Invalid number format\n";

string key_words[keywords_num] = {"PROGRAMM","IF","ELSE","SWITCH","CASE","DEFAULT","BREAK","PRINT","BEGIN","END"};//2
string delims_t[delims_num]  = {"+","-","*",":","/","(",")","{","}","=","<",">","[","]",";","\"","'",",","\\"," "," "};//0
string funcs[funcs_num]     = {"SQRT","LOG","LN","NEARBY"};//3
vector<string> idents;//4
vector<string> constants;//1

string delim = "+-*:/(){}=<>[];',\"\\   ";
vector<char*> errors;
int brackets_1num = 0;
int brackets_2num = 0;

void scan_file(char* filename,vector<vector<descr_item> > &descriptors);
void scan_string(string& s,vector<descr_item> &str);
string find_ident(string &s,int& pos,int& cl);
void out_descriptors(vector<vector<descr_item> > &table);
void process_ident(string& ident,int cl,vector<descr_item>& str_descr);
void out_errors();
void check_backets()
{
    if(brackets_1num != 0 || brackets_2num != 0)
        errors.push_back(NOT_PAIR_BRACKETS);
}

int main(int argc,char** argv)
{
    char filename[] = "/Users/antonmedvedev/Work/university/compile/programm.txt";
    vector<vector<descr_item> > table;
    
    scan_file(filename,table);
    
    out_descriptors(table);
    out_errors();
    
    return 0;
}

void scan_file(char* filename,vector<vector<descr_item> > &descriptors)
{
    //открываем файл
    fstream file(filename,fstream::in);
    if(!file)
    {
        errors.push_back(FILE_NOT_OPEN);
        return ;
    }
    //построчно читаем
    string s;
    while(getline(file,s))
    {
        vector<descr_item> tmp_descr;
        //запускаем scan_string
        scan_string(s,tmp_descr);
        //добавляем дескриторы из строки в общий список дескрипторов
        descriptors.push_back(tmp_descr);
    }
    //проверяем парность скобок
    check_backets();
    
    file.close();
}

void scan_string(string& s,vector<descr_item>& str_descr)
{
    string ident;
    //удаляем стоящие вначале пробелы
    int i = 0;
    while((s[i] == ' '||s[i] == '	') && i < s.length())
        i++;
    s.erase(0,i);
    //удаляем комменты
    int c = s.find_first_of("#");
    if(c == -1)
        c = s.length();
    s.erase(c,s.length() - c);
    int pos = 0,cl;
    while(pos < s.length())
    {
        //находим очередной идентификатор
        ident = find_ident(s,pos,cl);
        //идентифицируем
        process_ident(ident,cl,str_descr);
    }
}

void process_ident(string& ident,int cl,vector<descr_item>& str_descr)
{
    //ищем элемент в талицах в соответствии с его классом
    int i;
    //разделители сразу
    if(cl == 0)
    {
        for(i = 0; i < delims_num; i++)
        {
            if(ident.compare(delims_t[i])==0)
            {
                descr_item descr;
                descr.table=0;
                descr.idx=i;
                str_descr.push_back(descr);
                return ;
            }
        }
        errors.push_back(DELIM_NOT_FOUND);
        return ;
    }
    //константы добавляются сразу
    if(cl == 2)
    {
        descr_item descr;
        descr.table=1;
        descr.idx=constants.size();
        constants.push_back(ident.c_str());
        str_descr.push_back(descr);
        return ;
    }
    if(cl == 1)
    {
        //начинаем с ключевых слов
        for(i = 0; i < keywords_num; i++)
        {
            if(ident.compare(key_words[i])==0)
            {
                descr_item descr;
                descr.table=2;
                descr.idx=i;
                str_descr.push_back(descr);
                return ;
            }
            
        }
        //потом табличные функции
        for(i = 0; i < funcs_num; i++)
        {
            if(ident.compare(funcs[i])==0)
            {
                descr_item descr;
                descr.table=3;
                descr.idx=i;
                str_descr.push_back(descr);
                return ;
            }
        }
        //потом пользовательские идентификаторы
        for(i = 0; i < idents.size(); i++)
        {
            if(ident.compare(idents[i])==0)
            {
                descr_item descr;
                descr.table=4;
                descr.idx=i;
                str_descr.push_back(descr);
                return ;
            }
        }
        //если ничего, то добавляем идентификатор
        descr_item descr;
        descr.table=4;
        descr.idx=i;
        idents.push_back(ident.c_str());
        str_descr.push_back(descr);
    }
}

string find_ident(string &s,int& pos,int& cl)
{
    string buffer;
    bool stop = false;
    int i,j;
    //проверяем 1й символ
    //если идент-р, то сразу возвр-ем
    //если число, то обр-ем, как число
    //большая буква, то как слово
    if(strchr( delim.c_str(), s[pos] ) != NULL)
    {
        cl = 0;
        buffer.push_back(s[pos]);
        
        if(s[pos] == '{')
            brackets_1num++;
        if(s[pos] == '(')
            brackets_2num++;
        if(s[pos] == '}')
            brackets_1num--;
        if(s[pos] == ')')
            brackets_2num--;	
        pos++;
        return buffer;
    }
    
    if(s[pos] > 64&&s[pos] < 91)
    {
        cl = 1;
    }
    
    if(s[pos] > 47&&s[pos] < 58) 
    {
        cl = 2;
    }
    
    for(i = pos;i<s.length()&&!stop;i++)
    {
        for(j = 0; j < delim.length()&&!stop; j++)
        {
            if(s[i] == delim[j])
                stop = true;
        }
        if(!stop)
        {
            if(cl == 2 && !(s[i] > 47&&s[i] < 58))
            {
                errors.push_back(INVALID_NUMBER);
                cl = 3;
            }
            buffer.push_back(s[i]);
        }
    }
    pos = stop ? i-1 : i;
    return buffer;
}


void out_descriptors(vector<vector<descr_item> > &table)
{
    for(int i = 0; i < table.size(); i++)
    {
        for (int j = 0; j < table[i].size(); j++)
        {
            cout<<"(K"<<table[i][j].table<<", "<<table[i][j].idx<<")";
        }
        cout<<endl;
    }
    
}

void out_errors()
{
    for(int i = 0; i < errors.size(); i++)
        cout<<errors[i];
}

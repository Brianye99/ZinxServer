#pragma once
#include "zinx.h"
#include <string>


class CmdMsg :
    public UserData
{
public:
    //��Ա������ʾҪ���Ե��ַ���1
    std::string szUserData;
    //���������־
    bool isOpen = true;
    //����Ϣ������
    bool isCmd = false;
    //Ҫ��ǰ׺
    bool needDatePre = false;

    CmdMsg();
    virtual ~CmdMsg();

};


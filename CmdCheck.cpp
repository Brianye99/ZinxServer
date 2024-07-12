#include "CmdCheck.h"
#include "CmdMsg.h"
#include "EchoRoles.h"


CmdCheck* CmdCheck::poSingle = new CmdCheck();

CmdCheck::CmdCheck() {

}

CmdCheck::~CmdCheck() {

}
UserData* CmdCheck::raw2request(std::string _szInput)
{
    if ("exit" == _szInput) {
        ZinxKernel::Zinx_Exit();
        return NULL;
    }
    

    //赋值原始数据字符串到用户数据中字符串字段
    CmdMsg* pret = new CmdMsg();
    if ("open" == _szInput) {
        pret->isCmd = true;
        pret->isOpen = true;

    }
    if ("close" == _szInput) {
        pret->isCmd = true;
        pret->isOpen = false;
    }
    if ("date" == _szInput) {
        pret->needDatePre = true;
        pret->isCmd = true;
    }
    if ("cleardate" == _szInput) {
        pret->needDatePre = false;
        pret->isCmd = true;
    }

    pret->szUserData = _szInput;
    return pret;
}

std::string* CmdCheck::response2raw(UserData& _oUserData)
{
    //业务数据转换为原始数据
    //数据转换，将父类转换为子类
    GET_REF2DATA(CmdMsg, output, _oUserData);

    return new std::string(output.szUserData);
}

Irole* CmdCheck::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    szOutChannel = _oUserDataMsg.szInfo;
    if (szOutChannel == "stdin") {
        szOutChannel = "stdout";
    }
    //根据命令不同，交给不同的处理role对象
    auto rolelist = ZinxKernel::Zinx_GetAllRole();

    auto pCmdMsg = dynamic_cast<CmdMsg*>(_oUserDataMsg.poUserData);
    //当前消息是否为命令
    bool isCmd = pCmdMsg->isCmd;

    Irole* pRetRole = NULL;

    for (Irole* prole : rolelist) {
        if (isCmd) {
            auto pOutCtrl = dynamic_cast<OutputCtrl*>(prole);
            if (pOutCtrl != NULL) {
                // 动态类型转换成功-- > 当前 prole 就是 OutputCtrl * 子类对象
                //因为dynamic_cast转换失败 就会返回nullptr
                pRetRole = pOutCtrl;
                break;
            }
        }
        else {
            auto pDate = dynamic_cast<DatePreRole*>(prole);
            // 动态类型转换成功 --> 当前 prole 就是 EchoRole* 子类对象
            if (NULL != pDate) {
                pRetRole = pDate;
                break;
            }
        }
    }

    return pRetRole;
}

Ichannel* CmdCheck::GetMsgSender(BytesMsg& _oBytes)
{
    //指定数据通过标准输出输出
    //return ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
    return ZinxKernel::Zinx_GetChannel_ByInfo(szOutChannel);
}

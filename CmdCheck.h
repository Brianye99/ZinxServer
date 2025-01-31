#pragma once
#include "zinx.h"
class CmdCheck :
    public Iprotocol
{
private:
    CmdCheck();
    virtual ~CmdCheck();
    static CmdCheck* poSingle;//单例模式
public:
    // 通过 Iprotocol 继承
    virtual UserData* raw2request(std::string _szInput) override;

    virtual std::string* response2raw(UserData& _oUserData) override;

    virtual Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;

    virtual Ichannel* GetMsgSender(BytesMsg& _oBytes) override;

    static CmdCheck* GetInstance() {
        return poSingle;
    }
    std::string szOutChannel;
};


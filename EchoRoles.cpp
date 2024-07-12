#include "EchoRoles.h"
#include "CmdMsg.h"
#include "CmdCheck.h"


bool EchoRole::Init()
{
	return true;
}

UserData* EchoRole::ProcMsg(UserData& _poUserData)
{
	//读出要回显的内容
	//写出去
	GET_REF2DATA(CmdMsg, input, _poUserData);
	CmdMsg* pout = new CmdMsg(input);
	ZinxKernel::Zinx_SendOut(*pout,*(CmdCheck::GetInstance()));
	return nullptr;
}

void EchoRole::Fini()
{
}



bool OutputCtrl::Init()
{
	Irole* pRetRole = NULL;
	////设置下一个处理环节是 Date 对象
	for (auto pRole : ZinxKernel::Zinx_GetAllRole())
	{
		auto pEcho = dynamic_cast<DatePreRole*>(pRole);

		if (nullptr != pEcho)
		{
			pRetRole = pEcho;
			break;
		}
	}
	if (nullptr != pRetRole)
	{
		///*设置下一个处理者函数，
		//开发者可以调用该函数，在运行时对流程进行拆分嫁接*/
		//可以通过这个函数 直接等到数据处理完成后 
		//将返回的数据传递给指定的数据处理类
		OutputCtrl::SetNextProcessor(*pRetRole);
	}
	return true;
}

UserData* OutputCtrl::ProcMsg(UserData& _poUserData)
{
	//读取用户设置的开关标志
	GET_REF2DATA(CmdMsg, input, _poUserData);
	
	if (true == input.isOpen) {
		//开输出
		if (pOut != NULL) {
			//这里open状态再次输入open会发生段错误，所以添加判断
			ZinxKernel::Zinx_Add_Channel(*(pOut));
			pOut = NULL;
		}
	}
	else {
		//同理设置判断来避免段错误
		//关输出
		auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
		if (pchannel != NULL) {
			ZinxKernel::Zinx_Del_Channel(*(pchannel));
			pOut = pchannel;
		}
		
	}
	auto retmsg = new CmdMsg(input);
	return retmsg;
	//return new CmdMsg(input);
}



void OutputCtrl::Fini()
{
}


bool DatePreRole::Init()
{
	Irole* pRetRole = NULL;
	//设置下一个处理环节是echo对象
	for (auto pRole : ZinxKernel::Zinx_GetAllRole()) {
		auto pEcho = dynamic_cast<EchoRole*>(pRole);
		if (NULL != pEcho) {
			pRetRole = pEcho;
			break;
		}
	}
	if (NULL != pRetRole) {
		DatePreRole::SetNextProcessor(*pRetRole);
	}
	return true;
}

UserData* DatePreRole::ProcMsg(UserData& _poUserData)
{
	//如果是命令，改变标志位
	CmdMsg* pret = NULL;
	GET_REF2DATA(CmdMsg, input, _poUserData);
	if (input.isCmd) {
		needAdd = input.needDatePre;
	}
	else {//如果是需要回显的内容，添加字符串交给回显对象
		pret = new CmdMsg(input);
		if (needAdd) {
			time_t tmp;
			time(&tmp);
			std::string szNew = std::string(ctime(&tmp)) + input.szUserData;
			pret->szUserData = szNew;
		}
	}
	return pret;
	
}

void DatePreRole::Fini()
{
}

#include "EchoRoles.h"
#include "CmdMsg.h"
#include "CmdCheck.h"


bool EchoRole::Init()
{
	return true;
}

UserData* EchoRole::ProcMsg(UserData& _poUserData)
{
	//����Ҫ���Ե�����
	//д��ȥ
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
	////������һ���������� Date ����
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
		///*������һ�������ߺ�����
		//�����߿��Ե��øú�����������ʱ�����̽��в�ּ޽�*/
		//����ͨ��������� ֱ�ӵȵ����ݴ�����ɺ� 
		//�����ص����ݴ��ݸ�ָ�������ݴ�����
		OutputCtrl::SetNextProcessor(*pRetRole);
	}
	return true;
}

UserData* OutputCtrl::ProcMsg(UserData& _poUserData)
{
	//��ȡ�û����õĿ��ر�־
	GET_REF2DATA(CmdMsg, input, _poUserData);
	
	if (true == input.isOpen) {
		//�����
		if (pOut != NULL) {
			//����open״̬�ٴ�����open�ᷢ���δ�����������ж�
			ZinxKernel::Zinx_Add_Channel(*(pOut));
			pOut = NULL;
		}
	}
	else {
		//ͬ�������ж�������δ���
		//�����
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
	//������һ����������echo����
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
	//���������ı��־λ
	CmdMsg* pret = NULL;
	GET_REF2DATA(CmdMsg, input, _poUserData);
	if (input.isCmd) {
		needAdd = input.needDatePre;
	}
	else {//�������Ҫ���Ե����ݣ�����ַ����������Զ���
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

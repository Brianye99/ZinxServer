#include<iostream>
#include<zinx.h>
#include "StdInOutChannel.h"
#include "EchoRoles.h"


/*����׼���� ���쵽��׼���*/
/*
Echo
	���� exit ʱ�������� �˳�����
	���� close �������ԣ� ֱ������ open ֮��������ԣ� �����ڶ�̬����
	���� date ������ǰ׺����	cleardate �ر�
*/

//ͨ���࣬���ͨ��
class TestStdout :public Ichannel {
	// ͨ�� Ichannel �̳�
	virtual bool Init() override
	{
		return true;

	}
	virtual bool ReadFd(std::string& _input) override
	{
		return false;

	}
	virtual bool WriteFd(std::string& _output) override
	{
		//��׼���
		std::cout << _output << std::endl;
		return true;
	}
	virtual void Fini() override
	{
	}
	virtual int GetFd() override
	{
		return 1;
	}
	std::string GetChannelInfo() override
	{
		return "stdout";
	}
	AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return nullptr;
	}
}*poOut = new TestStdout();

//auto* poOut = new StdOutChannel();

/*2-д���ܴ�����*/
//����д��������
//AZinxHandler ��д������Ϣ�����ͻ�ȡ��һ�������ŷ���
class Echo :public AZinxHandler {
	// ͨ�� AZinxHandler �̳�
	/*��Ϣ����������������д�ú���ʵ����Ϣ�Ĵ���
	������Ҫһ�����ڼ�������ʱ��Ӧ�ô����µ���Ϣ���󣨶Ѷ��󣩲�����ָ��*/
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		//���Ե����
		GET_REF2DATA(BytesMsg, input, _oInput);
		////��׼���ͨ��
		//Ichannel* poSendOut = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
		//if (NULL != poSendOut) {
		//	ZinxKernel::Zinx_SendOut(input.szData, *poSendOut);
		//}
		ZinxKernel::Zinx_SendOut(input.szData, *poOut);

		return nullptr;
		
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return nullptr;
	}
}*poEcho = new Echo();


////exit �˳���Ϣ������ 
//Framework ���
class ExitFramwork :public AZinxHandler {
	AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}

	// ͨ�� AZinxHandler �̳�
	IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, obyte, _oInput);

		if (obyte.szData == "exit") {
			ZinxKernel::Zinx_Exit();
			return NULL;
		}
		//����������һ�����ڴ��������
		return new BytesMsg(obyte);
	}
}*poExit = new ExitFramwork();


//��Ϣ����ǰ׺ ��������
class AddData :public AZinxHandler {
	AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}

	// ͨ�� AZinxHandler �̳�
	IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, oBytes, _oInput);
		time_t tmp;
		time(&tmp);
		std::string szNew = std::string(ctime(&tmp)) + oBytes.szData;
		BytesMsg* pret = new BytesMsg(oBytes);
		pret->szData = szNew;
		return pret;
	}
}*poAddData = new AddData();


////���ݷַ� ��������
class CmdHandler :public AZinxHandler {
	int status = 0;
	// ͨ�� AZinxHandler �̳�
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		//�ж������Ƿ���open/close��ִ�в�ͬ�Ĳ���
		GET_REF2DATA(BytesMsg, oBytes, _oInput);
		if (oBytes.szData == "open") {
			ZinxKernel::Zinx_Add_Channel(*poOut);
		}
		else if (oBytes.szData == "close") {
			ZinxKernel::Zinx_Del_Channel(*poOut);
		}
		else if (oBytes.szData == "date") {
			status = 1;
			return nullptr;
		}
		else if (oBytes.szData == "cleardate") {
			status = 0;
		}
		return new BytesMsg(oBytes);
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		GET_REF2DATA(BytesMsg, oBytes, _oNextMsg);
		if (oBytes.szData == "exit") {
			return poExit;
		}
		else {
			if (status == 0) return poEcho;
			else return poAddData;
		}
	}
}*poCmd = new CmdHandler();

///*3-дͨ����*/
//�ܵ���
//������Ϣ��?
//Ichannel ͨ���࣬�����Ի����������࣬�ṩ����ϵͳ���õ������շ�����
//һ��أ��û�Ӧ�ø��ݴ�����ļ�����ϢԴ����ͬ��
//����ͨ����������ѡ�ú��ʵ�ʵ���ࣨ�Ѿ��ṩ��ͨ�������ࣩ�����ϵͳ���ļ�IO
//ͨ����,����ͨ��
class TestStdin :public Ichannel {
	// ͨ�� Ichannel �̳�
	//����֮��Ĵ��麯����virtual�ؼ�����Ϊ�˴���ɶ���
	
	/*ͨ����ʼ��������Ӧ����д�������ʵ�ִ��ļ���һ���Ĳ�������
	�ú�������ͨ��������ӵ�zinxkernelʱ������
	Ҳ���� ��ʼ��  Ҫ�����Ϊtrue*/
	virtual bool Init() override
	{
		return true;
	}
	/*��ȡ���ݣ� ������Ӧ�ø���Ŀ���ļ���ͬ����д�����������ʵ�ֽ�fd�е����ݶ�ȡ������_string��
	�ú�����������Դ����Ӧ���ļ������ݵ���ʱ������
	ReadFd  Ϊ ��ȡ��Ϣ Ҳ����������ĵص�*/
	virtual bool ReadFd(std::string& _input) override
	{
		//��׼����
		std::cin >> _input;
		return true;
	}
	/*д�����ݣ� ������Ӧ�ý�����д���Ĳ���ͨ����д�ú���ʵ��
	�ú������ڵ���zinxkernel::sendout�����󱻵��ã�ͨ������ֱ�ӵ��ö���ͨ����·���õķ�ѹ���Ƶ��ã�*/
	virtual bool WriteFd(std::string& _output) override
	{
		return false;
	}
	/*ͨ��ȥ��ʼ����������Ӧ���ڸú������������Դ
	�ú�������ͨ�������zinxkernel��ժ��ʱ����
	��Դ����  */
	virtual void Fini() override
	{
	}
	/*��ȡ�ļ������������� ������Ӧ���ڸú������ص�ǰ���ĵ��ļ���
	һ��أ�������Ӧ�����������ж�������������¼��������¼��ǰIO���õ��ļ����ڴ˺�����ֻ�Ƿ��ظ�����
	��ȡ��Ϣ��λ�� �������Ǵ������Ϣ�ص�����Ļ �����ļ���������0  Ҳ���� STDIN_FILENO*/
	virtual int GetFd() override
	{
		return 0;
	}
	/*��ȡͨ����Ϣ�����������߿����ڸú����з��ظ�ͨ����ص�һЩ�����ַ���������������Һ͹���*/
	virtual std::string GetChannelInfo() override
	{
		return "stdin";
	}
	/*��ȡ��һ�������ڣ�������Ӧ����д�ú�����ָ����һ��������
	һ��أ�������Ӧ���ڸú�������һ��Э��������������ȡ�����ֽ���
	�������  �Ǿ��� ��ȡ������Ϣ��  ������Ҫȥ�Ǹ�������  */
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return poCmd;
	}
};

int main() {
	//1*
	// ��ʼ�����
	ZinxKernel::ZinxKernelInit();


	//4*
	//��������ӵ����
	ZinxKernel::Zinx_Add_Channel(*(new StdInChannel()));
	ZinxKernel::Zinx_Add_Channel(*(new StdOutChannel()));
	ZinxKernel::Zinx_Add_Role(*(new EchoRole()));
	ZinxKernel::Zinx_Add_Role(*(new OutputCtrl()));
	ZinxKernel::Zinx_Add_Role(*(new DatePreRole()));

	/*ZinxKernel::Zinx_Add_Channel(*(new TestStdin()));
	ZinxKernel::Zinx_Add_Channel(*poOut);*/


	//5*���п��
	ZinxKernel::Zinx_Run();


	//�����Դ�Ļ��� �������
	ZinxKernel::ZinxKernelFini();
	std::cout << " ssh test " << std::endl;
	return 0;
}
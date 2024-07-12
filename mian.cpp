#include<iostream>
#include<zinx.h>
#include "StdInOutChannel.h"
#include "EchoRoles.h"


/*读标准输入 回响到标准输出*/
/*
Echo
	输入 exit 时不做回显 退出程序
	输入 close 不做回显， 直至输入 open 之后才做回显， 类似于动态开关
	输入 date 打开日期前缀功能	cleardate 关闭
*/

//通道类，输出通道
class TestStdout :public Ichannel {
	// 通过 Ichannel 继承
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
		//标准输出
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

/*2-写功能处理类*/
//负责写操作的类
//AZinxHandler 重写处理信息方法和获取下一个处理着方法
class Echo :public AZinxHandler {
	// 通过 AZinxHandler 继承
	/*信息处理函数，开发者重写该函数实现信息的处理，
	当有需要一个环节继续处理时，应该创建新的信息对象（堆对象）并返回指针*/
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		//回显到输出
		GET_REF2DATA(BytesMsg, input, _oInput);
		////标准输出通道
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


////exit 退出信息处理类 
//Framework 框架
class ExitFramwork :public AZinxHandler {
	AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}

	// 通过 AZinxHandler 继承
	IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, obyte, _oInput);

		if (obyte.szData == "exit") {
			ZinxKernel::Zinx_Exit();
			return NULL;
		}
		//创建交给下一个环节处理的数据
		return new BytesMsg(obyte);
	}
}*poExit = new ExitFramwork();


//消息日期前缀 处理功能类
class AddData :public AZinxHandler {
	AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}

	// 通过 AZinxHandler 继承
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


////数据分发 处理功能类
class CmdHandler :public AZinxHandler {
	int status = 0;
	// 通过 AZinxHandler 继承
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		//判断输入是否是open/close，执行不同的操作
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

///*3-写通道类*/
//管道类
//接收信息类?
//Ichannel 通道类，派生自基础处理者类，提供基于系统调用的数据收发功能
//一般地，用户应该根据处理的文件（信息源）不同而
//创建通道类的子类或选用合适的实用类（已经提供的通道类子类）来完成系统级文件IO
//通道类,输入通道
class TestStdin :public Ichannel {
	// 通过 Ichannel 继承
	//重载之后的纯虚函数加virtual关键字是为了代码可读性
	
	/*通道初始化函数，应该重写这个函数实现打开文件和一定的参数配置
	该函数会在通道对象添加到zinxkernel时被调用
	也就是 初始化  要将其改为true*/
	virtual bool Init() override
	{
		return true;
	}
	/*读取数据， 开发者应该根据目标文件不同，重写这个函数，以实现将fd中的数据读取到参数_string中
	该函数会在数据源所对应的文件有数据到达时被调用
	ReadFd  为 获取信息 也就是最初步的地点*/
	virtual bool ReadFd(std::string& _input) override
	{
		//标准输入
		std::cin >> _input;
		return true;
	}
	/*写出数据， 开发者应该将数据写出的操作通过重写该函数实现
	该函数会在调用zinxkernel::sendout函数后被调用（通常不是直接调用而是通过多路复用的反压机制调用）*/
	virtual bool WriteFd(std::string& _output) override
	{
		return false;
	}
	/*通道去初始化，开发者应该在该函数回收相关资源
	该函数会在通道对象从zinxkernel中摘除时调用
	资源回收  */
	virtual void Fini() override
	{
	}
	/*获取文件描述符函数， 开发者应该在该函数返回当前关心的文件，
	一般地，开发者应该在派生类中定义属性用来记录数据来记录当前IO所用的文件，在此函数中只是返回该属性
	读取信息的位置 现在我们传入的信息地点是屏幕 所以文件描述符是0  也就是 STDIN_FILENO*/
	virtual int GetFd() override
	{
		return 0;
	}
	/*获取通道信息函数，开发者可以在该函数中返回跟通道相关的一些特征字符串，方便后续查找和过滤*/
	virtual std::string GetChannelInfo() override
	{
		return "stdin";
	}
	/*获取下一个处理环节，开发者应该重写该函数，指定下一个处理环节
	一般地，开发者应该在该函数返回一个协议对象，用来处理读取到的字节流
	这个函数  是决定 获取数据信息后  接下来要去那个处理函数  */
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return poCmd;
	}
};

int main() {
	//1*
	// 初始化框架
	ZinxKernel::ZinxKernelInit();


	//4*
	//将对象添加到框架
	ZinxKernel::Zinx_Add_Channel(*(new StdInChannel()));
	ZinxKernel::Zinx_Add_Channel(*(new StdOutChannel()));
	ZinxKernel::Zinx_Add_Role(*(new EchoRole()));
	ZinxKernel::Zinx_Add_Role(*(new OutputCtrl()));
	ZinxKernel::Zinx_Add_Role(*(new DatePreRole()));

	/*ZinxKernel::Zinx_Add_Channel(*(new TestStdin()));
	ZinxKernel::Zinx_Add_Channel(*poOut);*/


	//5*运行框架
	ZinxKernel::Zinx_Run();


	//框架资源的回收 结束框架
	ZinxKernel::ZinxKernelFini();
	std::cout << " ssh test " << std::endl;
	return 0;
}
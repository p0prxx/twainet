#ifdef WIN32
#	ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#		define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#	endif/*_WIN32_WINNT*/
#	define WIN32_LEAN_AND_MEAN     
#	include <winsock2.h>
#	include <windows.h>
#	include "connector_lib/ppp/ppp_library.h"
#else
#endif/*WIN32*/
#ifdef WIN32
#	include "udt.h"
#else
#	include "udt/udt.h"
#endif/*WIN32*/
#include "twainet.h"
#include "application.h"
#include "utils/utils.h"

#pragma warning(disable: 4273)

extern "C" void Twainet::InitLibrary(const Twainet::TwainetCallback& twainet)
{
	UDT::startup();
	Application::GetInstance().Init(twainet);
#ifdef WIN32
	PPPLibrary::GetInstance().InitLibrary();
#endif/*WIN32*/
}

extern "C" void Twainet::FreeLibrary()
{
	ManagersContainer::GetInstance().Join();
	UDT::cleanup();
#ifdef WIN32
	PPPLibrary::GetInstance().FreeLibrary();
#endif/*WIN32*/
}

extern "C" Twainet::Module Twainet::CreateModule(const Twainet::ModuleName& moduleName, bool isCoordinator, bool isPPPListener)
{
	TwainetModule* module = Application::GetInstance().CreateModule(moduleName, isPPPListener);
	isCoordinator ?  module->StartAsCoordinator() : module->Start();
	return (Twainet::Module*)module;
}

extern "C" void Twainet::DeleteModule(const Twainet::Module module)
{
	Application::GetInstance().DeleteModule((TwainetModule*)module);
}

extern "C" void Twainet::CreateServer(const Twainet::Module module, int port)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->StartServer(port);
}

extern "C" void Twainet::ConnectToServer(const Twainet::Module module, const char* host, int port)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->Connect(host, port);
}

extern "C" void Twainet::DisconnectFromServer(const Twainet::Module module)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->Disconnect();
}

extern "C" void Twainet::DisconnectFromClient(const Twainet::Module module, const char* sessionId)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->DisconnectModule(IPCObjectName(ClientServerModule::m_clientIPCName, sessionId));
}

extern "C" void Twainet::ConnectToModule(const Twainet::Module module, const Twainet::ModuleName& moduleName)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->ConnectTo(IPCObjectName(moduleName.m_name, moduleName.m_host, moduleName.m_suffix));
}

extern "C" void DisconnectFromModule(const Twainet::Module module, const Twainet::ModuleName& moduleName)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->DisconnectModule(IPCObjectName(moduleName.m_name, moduleName.m_host, moduleName.m_suffix));
}

extern "C" void Twainet::CreateTunnel(const Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->InitNewTunnel(sessionId, (TunnelConnector::TypeConnection)type);
}

extern "C" void Twainet::DisconnectTunnel(const Twainet::Module module, const char* sessionId)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->DestroyTunnel(sessionId);
}

extern "C" void Twainet::SendMessage(const Twainet::Module module, const Twainet::Message& msg)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	IPCMessage message;
	message.set_message_name(msg.m_typeMessage);
	message.set_message(msg.m_data, msg.m_dataLen);
	for(int i = 0; i < msg.m_pathLen; i++)
	{
		*message.add_ipc_path() = IPCObjectName(msg.m_path[i].m_name, msg.m_path[i].m_host, msg.m_path[i].m_suffix);
	}

	IPCMessageSignal msgSignal(message);
	twainetModule->SendMsg(msgSignal);
}

extern "C" Twainet::ModuleName Twainet::GetModuleName(const Twainet::Module module)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	const IPCObjectName& name = twainetModule->GetModuleName();
	Twainet::ModuleName retName = {0};
#ifdef WIN32
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, name.module_name().c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, name.host_name().c_str());
	strcpy_s(retName.m_suffix, MAX_NAME_LENGTH, name.suffix().c_str());
#else
	strcpy(retName.m_name, name.module_name().c_str());
	strcpy(retName.m_host, name.host_name().c_str());
	strcpy(retName.m_suffix, name.suffix().c_str());
#endif/*WIN32*/
	return retName;
}

extern "C" const char* Twainet::GetSessionId(const Twainet::Module module)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	return twainetModule->GetSessionId().c_str();
}

extern "C" int Twainet::GetExistingModules(const Twainet::Module module, Twainet::ModuleName* modules, int& sizeModules)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	std::vector<IPCObjectName> objects = twainetModule->GetIPCObjects();
	if(sizeModules < (int)objects.size())
	{
		sizeModules = objects.size();
		return 0;
	}

	int i = 0;
	for(std::vector<IPCObjectName>::iterator it = objects.begin();
		it != objects.end(); it++, i++)
	{
#ifdef WIN32
		strcpy_s(modules[i].m_name, MAX_NAME_LENGTH, it->module_name().c_str());
		strcpy_s(modules[i].m_host, MAX_NAME_LENGTH, it->host_name().c_str());
		strcpy_s(modules[i].m_suffix, MAX_NAME_LENGTH, it->suffix().c_str());
#else
		strcpy(modules[i].m_name, it->module_name().c_str());
		strcpy(modules[i].m_host, it->host_name().c_str());
		strcpy(modules[i].m_suffix, it->suffix().c_str());
#endif/*WIN32*/
	}
	return objects.size();
}

extern "C" void Twainet::SetTunnelType(const Twainet::Module module, const char* oneSessionId, const char* twoSessionId, Twainet::TypeConnection type)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->SetTypeTunnel(oneSessionId, twoSessionId, (TunnelConnector::TypeConnection)type);
}
#include "ipc_handler.h"
#include "ipc_connector.h"
//#include "utils/logger.h"
//#include "include/ref.h"
//#include "module/ipc_module.h"

IPCHandler::IPCHandler(IPCConnector* connector)
: m_connector(connector)
{
}

IPCHandler::~IPCHandler()
{
}
	
void IPCHandler::onMessage(const _Ipc__ModuleName& msg)
{
 	if(m_connector->m_checker)
 	{
 		m_connector->m_checker->StopThread();
 		m_connector->m_checker = 0;
 	}
	
 	IPCObjectName ipcName(*msg.ipc_name);
 	m_connector->SetId(ipcName.GetModuleNameString());

// 	LOG_INFO("ModuleName message: m_id-%s, m_module-%s\n",
// 		 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 
// 	AddIPCObjectMessage aoMsg(this);
// 	aoMsg.set_ip(msg.ip());
// 	aoMsg.set_port(msg.port());
// 	m_connector->m_isCoordinator ? aoMsg.set_access_id(msg.access_id()) : aoMsg.set_access_id(m_connector->m_accessId);
// 	*aoMsg.mutable_ipc_name() = msg.ipc_name();
// 	m_connector->onSignal(aoMsg);
// 
// 	ModuleNameMessage mnMsg(this, msg);
// 	mnMsg.set_is_exist(false);
// 	mnMsg.set_conn_id(m_connector->m_connectorId);
// 	m_connector->m_isCoordinator ? mnMsg.set_access_id(msg.access_id()) : mnMsg.set_access_id(m_connector->m_accessId);
// 	m_connector->onSignal(mnMsg);
 
 	ModuleStateMessage msMsg(this, ipc__module_state__descriptor);
 	msMsg.GetMessage()->rndval = "";
 	msMsg.GetMessage()->exist = m_connector->m_isExist;
 	msMsg.GetMessage()->rndval = (char*)m_connector->m_rand.c_str();
 	m_connector->toMessage(msMsg);
 
 	if(m_connector->m_isSendIPCObjects)
 	{
 		IPCObjectListMessage ipcolMsg(this, ipc__ipcobject_list__descriptor);
 		ipcolMsg.GetMessage()->access_id = msg.access_id;
// 		m_connector->onSignal(ipcolMsg);
 		m_connector->toMessage(ipcolMsg);
 	}
 	
 	if(m_connector->m_isExist)
 	{
// 		LOG_INFO("Module exists: m_id-%s, m_module-%s\n", m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 		return;
 	}
 
 	//TODO: fix problem:
 	//	if two modules with same names is trying to connect in same time to coordinator,
 	//	another modules cann't get correct creadentials of new module.
 	if(m_connector->m_isSendIPCObjects)
 	{
// 		m_connector->onIPCSignal(mnMsg);
 	}
 	m_connector->OnConnected();
}

void IPCHandler::onMessage(const _Ipc__ModuleState& msg)
{
 	if(msg.exist && m_connector->m_isExist)
 	{
// 		LOG_INFO("Module exists: m_id-%s, m_module-%s\n",
// 			 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 		m_connector->StopThread();
 	}
 	else if(msg.exist && !m_connector->m_isExist)
 	{
 		ModuleStateMessage msMsg(this, ipc__module_state__descriptor);
 		msMsg.GetMessage()->exist = false;
 		msMsg.GetMessage()->id = (char*)m_connector->GetId().c_str();
// 		m_connector->onIPCSignal(msMsg);
 
 		if (msMsg.GetMessage()->exist && m_connector->m_rand > msMsg.GetMessage()->rndval
 			|| !msMsg.GetMessage()->exist)
 		{
// 			LOG_INFO("Module exists: m_id-%s, m_module-%s\n",
// 				 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 			m_connector->StopThread();
 		}
 		else if(msMsg.GetMessage()->exist && m_connector->m_rand == msMsg.GetMessage()->rndval)
 		{
// 			LOG_WARNING("Random values is equal: m_id-%s, m_module-%s\n",
// 				    m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 		}
 		else
 		{
 			m_connector->OnConnected();
 		}
 	}
}

void IPCHandler::onMessage(const _Ipc__AddIPCObject& msg)
{
// 	AddIPCObjectMessage aoMsg(this, msg);
// 	m_connector->onSignal(aoMsg);
// 
 	IPCObjectName ipcName(*msg.ipc_name);
 	m_connector->OnAddIPCObject(ipcName.GetModuleNameString());
}

void IPCHandler::onMessage(const _Ipc__RemoveIPCObject& msg)
{
// 	RemoveIPCObjectMessage roMsg(this, msg);
// 	m_connector->onSignal(roMsg);
 }

void IPCHandler::onMessage(const _Ipc__IPCMessage& msg)
{
 	if(!m_connector->m_bConnected)
 	{
 		return;
 	}
 	
 	bool isTarget = (msg.n_ipc_path == 0);
 
// 	IPCMessage newMsg(msg);
// 	newMsg.clear_ipc_path();
 	if(!isTarget)
 	{
// 		IPCObjectName newPath(msg.ipc_path(0));
// 		if(newPath == m_connector->m_moduleName)
// 		{
// 			for(int i = 1; i < msg.ipc_path_size(); i++)
// 			{
// 				*newMsg.add_ipc_path() = msg.ipc_path(i);
// 			}
// 		}
// 		*newMsg.add_ipc_sender() = m_connector->GetIPCName();
// 		isTarget = (newPath == m_connector->m_moduleName && !newMsg.ipc_path_size());
 	}
 
 	if (isTarget &&
 	    !m_connector->onData(msg.message_name, (char*)msg.message.data, (int)msg.message.len))
 	{
// 		IPCProtoMessage protoMsg(this, newMsg);
// 		m_connector->onSignal(protoMsg);
 	}
// 	else if(newMsg.ipc_path_size())
// 	{
// 		IPCMessageSignal sigMsg(newMsg);
// 		m_connector->onSignal(sigMsg);
// 		IPCProtoMessage protoMsg(this, sigMsg);
// 		m_connector->onIPCSignal(protoMsg);
// 	}
}

void IPCHandler::onMessage(const _Ipc__IPCObjectList& msg)
{
 	for(int i = 0; i < msg.n_ipc_object; i++)
 	{
// 		AddIPCObjectMessage aoMsg(this, msg.ipc_object(i));
// 		m_connector->onSignal(aoMsg);
// 		
// 		IPCObjectName ipcName(msg.ipc_object(i).ipc_name());
// 		m_connector->OnAddIPCObject(ipcName.GetModuleNameString());
 	}
}

void IPCHandler::onMessage(const _Ipc__ChangeIPCName& msg)
{
// 	UpdateIPCObjectMessage uioMsg(this);
// 	*uioMsg.mutable_ipc_new_name() = msg.ipc_name();
// 	*uioMsg.mutable_ipc_old_name() = IPCObjectName::GetIPCName(m_connector->m_id);
// 	m_connector->onSignal(uioMsg);
// 
// 	if(m_connector->m_isCoordinator)
// 	{
// 		m_connector->onIPCSignal(uioMsg);
// 	}
// 
// 	IPCObjectName ipcName(msg.ipc_name());
// 	m_connector->m_id = ipcName.GetModuleNameString();
// 	
// 	LOG_INFO("Change connector id: m_id-%s, m_module-%s\n", m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
}

void IPCHandler::onMessage(const _Ipc__UpdateIPCObject& msg)
{
// 	UpdateIPCObjectMessage uioMsg(this, msg);
// 	m_connector->onSignal(uioMsg);
// 
// 	IPCObjectName ipcNameOld(msg.ipc_old_name());
// 	IPCObjectName ipcNameNew(msg.ipc_new_name());
// 	m_connector->OnUpdateIPCObject(ipcNameOld.GetModuleNameString(), ipcNameNew.GetModuleNameString());
}

void IPCHandler::onMessage(const _Ipc__Ping& msg)
{
}

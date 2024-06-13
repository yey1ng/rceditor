#include "node.h"
#include "prototype.h"
#include "nodemanager.h"

RCEDITOR_NAMESPACE_BEGIN

PinInfoList PinIDList::getPinInfoList(NodeManager* i_pNodeManager) noexcept
{
	PinInfoList l_PinInfoList{};
	for (auto l_PinID : m_PinIDList)
	{
		auto l_PinInfo = *i_pNodeManager->pinMap()[l_PinID];
		l_PinInfo.linkIDSet().clear(); // clear LinkIDSet
		if (l_PinInfo.getIsInputOutput())
			l_PinInfo.setPinTypeValue(PinType::InputOutput);
		l_PinInfoList.pinInfoList().push_back(l_PinInfo);
	}
	return l_PinInfoList;
}

std::string PinIDList::getIndexListStr() noexcept
{
	std::string l_IndexListStr = "-1";
	for (U32 i = 0; i < m_PinIDList.size(); ++i)
	{
		l_IndexListStr += '\0' + std::to_string(i);
	}
	return l_IndexListStr;
}

PinID PinIDList::addPinInfo(PinInfo& i_PinInfo, NodeManager* i_pNodeManager, S32 i_PinIndex) noexcept
{
	auto l_PinID = i_pNodeManager->getPinID();
	auto l_pPinInfo = std::make_shared<PinInfo>(i_PinInfo);
	l_pPinInfo->setPinID(l_PinID);

	if (i_PinIndex >= m_PinIDList.size() || i_PinIndex < 0)
		m_PinIDList.emplace_back(l_PinID);
	else
		m_PinIDList.insert(m_PinIDList.begin() + i_PinIndex, l_PinID);

	i_pNodeManager->pinMap()[l_PinID] = l_pPinInfo;

	return l_PinID;
}

std::string PinIDList::delPinInfo(U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	if (m_PinIDList.size() > i_PinIndex)
	{
		auto l_PinID = m_PinIDList[i_PinIndex];
		auto l_pPinInfo = i_pNodeManager->pinInfo(l_PinID);
		i_pNodeManager->delInfoByLinkIDSet(l_pPinInfo->linkIDSet());
		m_PinIDList.erase(m_PinIDList.begin() + i_PinIndex);
		i_pNodeManager->pinMap().erase(l_PinID);
		return l_pPinInfo->getName();
	}

	return "";
}

void PinIDList::delAllPinInfo(NodeManager* i_pNodeManager) noexcept
{
	for (auto l_PinID : m_PinIDList)
	{
		i_pNodeManager->pinMap().erase(l_PinID);
	}
}

NodeInfo::NodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID) noexcept
{
	m_ParentNodeID = i_ParentNodeID;
	m_NodeID = i_NodeID;
}

void NodeInfo::delNodeInfo(NodeManager* i_pNodeManager) noexcept
{
	if (m_NodeType == NodeType::Composite || isTaskNode())
	{
		auto l_pSubRCInfo = i_pNodeManager->getSubRCInfo(m_NodeID);
		if (l_pSubRCInfo)
		{
			for (auto l_SubNodeID : l_pSubRCInfo->m_NodeIDList)
			{
				auto l_pSubNodeInfo = i_pNodeManager->nodeInfo(l_SubNodeID);
				if (l_pSubNodeInfo)
					l_pSubNodeInfo->delNodeInfo(i_pNodeManager);
			}
		}

		i_pNodeManager->subRCInfoMap().erase(m_NodeID);
	}

	if (isTaskNode())
	{
		i_pNodeManager->delTaskTagInfo(m_NodeID);
	}

	i_pNodeManager->delInfoByLinkIDSet(m_LinkIDSet);

	m_TemplateInputOutputInputPinIDList.delAllPinInfo(i_pNodeManager);
	m_TemplateInputOutputOutputPinIDList.delAllPinInfo(i_pNodeManager);
	m_TemplateInputPinIDList.delAllPinInfo(i_pNodeManager);
	m_TemplateOutputPinIDList.delAllPinInfo(i_pNodeManager);
	m_InputOutputInputPinIDList.delAllPinInfo(i_pNodeManager);
	m_InputOutputOutputPinIDList.delAllPinInfo(i_pNodeManager);
	m_InputPinIDList.delAllPinInfo(i_pNodeManager);
	m_OutputPinIDList.delAllPinInfo(i_pNodeManager);
}

PinIDList& NodeInfo::getPinIDList(Bool i_IsTemplate, Bool i_IsInputOutput, PinType i_PinType) noexcept
{
	if (i_IsTemplate)
	{
		if (i_IsInputOutput)
		{
			if (i_PinType == PinType::Input)
				return m_TemplateInputOutputInputPinIDList;
			else
				return m_TemplateInputOutputOutputPinIDList;
		}
		else
		{
			if (i_PinType == PinType::Input)
				return m_TemplateInputPinIDList;
			else
				return m_TemplateOutputPinIDList;
		}
	}
	else
	{
		if (i_IsInputOutput)
		{
			if (i_PinType == PinType::Input)
				return m_InputOutputInputPinIDList;
			else
				return m_InputOutputOutputPinIDList;
		}
		else
		{
			if (i_PinType == PinType::Input)
				return m_InputPinIDList;
			else
				return m_OutputPinIDList;
		}
	}
}

void NodeInfo::addPinInfo(PinInfo& i_PinInfo, NodeManager* i_pNodeManager, S32 i_PinIndex) noexcept
{
	PinID l_InputPinID{};
	PinID l_OutputPinID{};
	i_PinInfo.setNodeID(m_NodeID);
	if (i_PinInfo.getIsTemplate())
	{
		if (i_PinInfo.getIsInputOutput())
		{
			if (i_PinInfo.getPinType() == PinType::Input)
				l_InputPinID = m_TemplateInputOutputInputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
			else if(i_PinInfo.getPinType() == PinType::Output)
				l_OutputPinID = m_TemplateInputOutputOutputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
			else
			{
				i_PinInfo.setPinTypeValue(PinType::Input);
				auto l_PinAccessTypeValue = i_PinInfo.getPinAccessTypeValue();
				i_PinInfo.setPinAccessTypeValue(PinAccessType::Writable);
				l_InputPinID = m_TemplateInputOutputInputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
				i_PinInfo.setPinTypeValue(PinType::Output);
				i_PinInfo.setPinAccessTypeValue(l_PinAccessTypeValue);
				l_OutputPinID = m_TemplateInputOutputOutputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
				i_PinInfo.setPinTypeValue(PinType::InputOutput);
			}
		}
		else
		{
			if (i_PinInfo.getPinType() == PinType::Input)
				l_InputPinID = m_TemplateInputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
			else
				l_OutputPinID = m_TemplateOutputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
		}
	}
	else
	{
		if (i_PinInfo.getIsInputOutput())
		{
			if (i_PinInfo.getPinType() == PinType::Input)
				l_InputPinID = m_InputOutputInputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
			else if (i_PinInfo.getPinType() == PinType::Output)
				l_OutputPinID = m_InputOutputOutputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
			else
			{
				i_PinInfo.setPinTypeValue(PinType::Input);
				auto l_PinAccessTypeValue = i_PinInfo.getPinAccessTypeValue();
				i_PinInfo.setPinAccessTypeValue(PinAccessType::Writable);
				l_InputPinID = m_InputOutputInputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
				i_PinInfo.setPinTypeValue(PinType::Output);
				i_PinInfo.setPinAccessTypeValue(l_PinAccessTypeValue);
				l_OutputPinID = m_InputOutputOutputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
				i_PinInfo.setPinTypeValue(PinType::InputOutput);
			}
		}
		else
		{
			if (i_PinInfo.getPinType() == PinType::Input)
				l_InputPinID = m_InputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
			else
				l_OutputPinID = m_OutputPinIDList.addPinInfo(i_PinInfo, i_pNodeManager, i_PinIndex);
		}
	}

	m_PinKeyMap[i_PinInfo.getName()] = { l_InputPinID, l_OutputPinID };
}

void NodeInfo::delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	std::string l_PinName{};
	if(i_PinType == PinType::Input)
		l_PinName = m_InputPinIDList.delPinInfo(i_PinIndex, i_pNodeManager);
	else if (i_PinType == PinType::Output)
		l_PinName = m_OutputPinIDList.delPinInfo(i_PinIndex, i_pNodeManager);
	else
	{
		l_PinName = m_InputOutputInputPinIDList.delPinInfo(i_PinIndex, i_pNodeManager);
		m_InputOutputOutputPinIDList.delPinInfo(i_PinIndex, i_pNodeManager);
	}

	delPinKey(l_PinName);
}

void NodeInfo::addParam(const ParamInfo& i_ParamInfo, Bool i_IsTemplate, S32 i_ParamIndex) noexcept
{
	addParamKey(i_ParamInfo.m_Name);

	ParamInfoList* l_pParamInfoList{};
	if (i_IsTemplate)
		l_pParamInfoList = &m_TemplateParamList;
	else
		l_pParamInfoList = &m_ParamList;
		
	if (i_ParamIndex >= m_ParamList.paramInfoList().size() || i_ParamIndex < 0)
		l_pParamInfoList->paramInfoList().emplace_back(i_ParamInfo);
	else
		l_pParamInfoList->paramInfoList().insert(m_ParamList.paramInfoList().begin() + i_ParamIndex, i_ParamInfo);
}

void NodeInfo::delParam(U32 i_ParamIndex) noexcept
{
	auto l_It = m_ParamList.paramInfoList().begin() + i_ParamIndex;
	delParamKey(l_It->getName());

	if (m_ParamList.paramInfoList().size() > i_ParamIndex)
		m_ParamList.paramInfoList().erase(l_It);
}

bool NodeInfo::canChangePinAccessType(PinType i_PinType) noexcept
{
	if (i_PinType == PinType::InputOutput || i_PinType == PinType::Output)
		return true;
	else
		return false;
}

std::string NodeInfo::getIndexListStr(PinType i_PinType) noexcept
{
	if (i_PinType == PinType::Input)
		return m_InputPinIDList.getIndexListStr();
	else if (i_PinType == PinType::Output)
		return m_OutputPinIDList.getIndexListStr();
	else
		return m_InputOutputInputPinIDList.getIndexListStr();
}

void NodeInfo::setIsHighLight(Bool i_IsHighlight, NodeManager* i_pNodeManager) noexcept
{
	if (i_IsHighlight)
	{
		if (m_ParentNodeID != 0)
		{
			auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
			if (l_pParentNodeInfo)
				l_pParentNodeInfo->setIsHighLight(true, i_pNodeManager);
		}
	}

	m_IsHighlight = i_IsHighlight;
}

void NodeInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	auto l_XmlEditorInfo = i_XmlElement->FirstChildElement(g_XmlEditorInfoStr.data());

	m_NodeName = l_XmlEditorInfo->FindAttribute(g_XmlNodeNameStr.data())->Value();
	m_Comment = l_XmlEditorInfo->FindAttribute(g_XmlCommentStr.data())->Value();

	PinInfoList l_InputOutputPinInfoList{};
	PinInfoList l_InputPinInfoList{};
	PinInfoList l_OutputPinInfoList{};
	ParamInfoList l_TemplateParamInfoList{};
	std::map<std::string, ParamInfo> l_TemplateParamInfoMap{};
	ParamInfoList l_ParamInfoList{};
	l_InputOutputPinInfoList.loadData<false, PinType::InputOutput>(i_XmlElement);
	l_InputPinInfoList.loadData<false, PinType::Input>(i_XmlElement);
	l_OutputPinInfoList.loadData<false, PinType::Output>(i_XmlElement);
	l_TemplateParamInfoList.loadTemplateData(i_XmlElement);
	l_ParamInfoList.loadData(i_XmlElement);
	//m_TemplateParamList.paramInfoList().clear();
	for (auto l_ParamInfo : l_TemplateParamInfoList.paramInfoList())
		l_TemplateParamInfoMap[l_ParamInfo.getName()] = l_ParamInfo;


	for (auto& l_PinInfo : l_InputOutputPinInfoList.pinInfoList())
		addPinInfo(l_PinInfo, i_pNodeManager);
	for (auto& l_PinInfo : l_InputPinInfoList.pinInfoList())
		addPinInfo(l_PinInfo, i_pNodeManager);
	for (auto& l_PinInfo : l_OutputPinInfoList.pinInfoList())
		addPinInfo(l_PinInfo, i_pNodeManager);
	for (const auto& i_ParamInfo : l_ParamInfoList.paramInfoList())
		addParam(i_ParamInfo);
	for (auto& l_ParamInfo : m_TemplateParamList.paramInfoList())
	{
		l_ParamInfo.setData(l_TemplateParamInfoMap[l_ParamInfo.getName()].getData());
		l_ParamInfo.setUseRef(l_TemplateParamInfoMap[l_ParamInfo.getName()].getUseRef());
		l_ParamInfo.setRefKey(l_TemplateParamInfoMap[l_ParamInfo.getName()].getRefKey());
	}
}

void NodeInfo::saveData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	auto savePinInfo = [i_XmlElement, i_pNodeManager](PinIDList i_PinIDList, std::string_view i_XmlKey) {
		tinyxml2::XMLElement* l_XmlPinInfoList = i_XmlElement->InsertNewChildElement(i_XmlKey.data());
		for (auto& l_PinID : i_PinIDList.pinIDList())
		{
			auto l_pPinInfo = i_pNodeManager->pinInfo(l_PinID);
			if (l_pPinInfo)
				l_pPinInfo->saveData(l_XmlPinInfoList);
		}
	};

	auto saveParamInfo = [i_XmlElement, this]() {
		tinyxml2::XMLElement* l_XmlTemplateParamInfoList = i_XmlElement->InsertNewChildElement(g_XmlTemplateParamListStr.data());
		for (auto& l_ParamInfo : m_TemplateParamList.paramInfoList())
		{
			l_ParamInfo.saveTemplateData(l_XmlTemplateParamInfoList);
		}
		tinyxml2::XMLElement* l_XmlParamInfoList = i_XmlElement->InsertNewChildElement(g_XmlParamListStr.data());
		for (auto& l_ParamInfo : m_ParamList.paramInfoList())
		{
			l_ParamInfo.saveData(l_XmlParamInfoList);
		}
	};

	i_XmlElement->SetAttribute(g_XmlNodeIDStr.data(), m_NodeID);
	i_XmlElement->SetAttribute(g_XmlParentIDStr.data(), m_ParentNodeID);
	i_XmlElement->SetAttribute(g_XmlTemplateNameStr.data(), m_TemplateName.c_str());

	//Save Graph
	tinyxml2::XMLElement* l_XmlEditorInfo = i_XmlElement->InsertNewChildElement(g_XmlEditorInfoStr.data());
	l_XmlEditorInfo->SetAttribute(g_XmlNodeNameStr.data(), m_NodeName.c_str());
	l_XmlEditorInfo->SetAttribute(g_XmlCommentStr.data(), m_Comment.c_str());

	savePinInfo(m_InputOutputOutputPinIDList, g_XmlInputOutputListStr);
	savePinInfo(m_InputPinIDList, g_XmlInputListStr);
	savePinInfo(m_OutputPinIDList, g_XmlOutputListStr);
	saveParamInfo();
}

std::shared_ptr<NodePrototype> NodeInfo::getNodePrototype(std::string_view i_NewNodeName, NodeManager* i_pNodeManager, NodeID i_RelativeNodeID) noexcept
{
	std::shared_ptr<NodePrototype> l_pNodePrototype{};
	if (m_NodeType == NodeType::GPU || m_NodeType == NodeType::CPU)
	{
		auto l_pTempNodePrototype = std::make_shared<FavoriteNodePrototype>();
		l_pNodePrototype = std::dynamic_pointer_cast<NodePrototype>(l_pTempNodePrototype);
	}
	else if (m_NodeType == NodeType::Start || m_NodeType == NodeType::End)
	{
		auto l_pTempNodePrototype = std::make_shared<FavoriteChildNodePrototype>();
		l_pNodePrototype = std::dynamic_pointer_cast<NodePrototype>(l_pTempNodePrototype);
	}
	else if (m_NodeType == NodeType::Composite)
	{
		auto l_pTempNodePrototype = std::make_shared<FavoriteCompositeNodePrototype>();
		l_pNodePrototype = std::dynamic_pointer_cast<NodePrototype>(l_pTempNodePrototype);
	}

	auto l_pNodeInfo = i_pNodeManager->nodeInfo(m_NodeID);
	l_pNodePrototype->initData(l_pNodeInfo, i_NewNodeName, i_pNodeManager, i_RelativeNodeID);

	return l_pNodePrototype;
}

void ChildNodeInfo::addPinInfo2(PinInfo& i_pPinInfo, NodeManager* i_pNodeManager, S32 i_PinIndex) noexcept
{
	NodeInfo::addPinInfo(i_pPinInfo, i_pNodeManager, i_PinIndex);

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return;

	if (m_NodeType == NodeType::Start)
	{
		i_pPinInfo.setPinTypeValue(PinType::Input);
	}
	else if (m_NodeType == NodeType::End)
	{
		i_pPinInfo.setPinTypeValue(PinType::Output);
	}
	l_pParentNodeInfo->addPinInfo(i_pPinInfo, i_pNodeManager, i_PinIndex);
}

void ChildNodeInfo::delPinInfo2(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::delPinInfo(i_PinType, i_PinIndex, i_pNodeManager);

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return;

	if (m_NodeType == NodeType::Start)
		l_pParentNodeInfo->delPinInfo(PinType::Input, i_PinIndex, i_pNodeManager);
	else if (m_NodeType == NodeType::End)
		l_pParentNodeInfo->delPinInfo(PinType::Output, i_PinIndex, i_pNodeManager);
}

void ChildNodeInfo::addParam2(const ParamInfo& i_ParamInfo, NodeManager* i_pNodeManager, Bool i_IsTemplate, S32 i_ParamIndex) noexcept
{
	NodeInfo::addParam(i_ParamInfo, i_IsTemplate, i_ParamIndex);

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return;
	l_pParentNodeInfo->addParam(i_ParamInfo, i_IsTemplate, i_ParamIndex);
}

void ChildNodeInfo::delParam2(U32 i_ParamIndex, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::delParam(i_ParamIndex);

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return;
	l_pParentNodeInfo->delParam(i_ParamIndex);
}

void ChildNodeInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	m_NodeName = i_XmlElement->FindAttribute(g_XmlNodeNameStr.data())->Value();
	m_Comment = i_XmlElement->FindAttribute(g_XmlCommentStr.data())->Value();
}

void ChildNodeInfo::saveData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	i_XmlElement->SetAttribute(g_XmlNodeIDStr.data(), m_NodeID);
	i_XmlElement->SetAttribute(g_XmlNodeNameStr.data(), m_NodeName.c_str());
	i_XmlElement->SetAttribute(g_XmlTemplateNameStr.data(), m_TemplateName.c_str());
	i_XmlElement->SetAttribute(g_XmlCommentStr.data(), m_Comment.c_str());
}

void CompositeNodeInfo::addPinInfo2(PinInfo& i_pPinInfo, NodeManager* i_pNodeManager, S32 i_PinIndex) noexcept
{
	NodeInfo::addPinInfo(i_pPinInfo, i_pNodeManager, i_PinIndex);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);

	if (l_pStartNodeInfo && i_pPinInfo.getPinType() == PinType::Input)
	{
		i_pPinInfo.setPinTypeValue(PinType::Output);
		l_pStartNodeInfo->addPinInfo(i_pPinInfo, i_pNodeManager, i_PinIndex);
		i_pPinInfo.setPinTypeValue(PinType::Input);
	}
	if (l_pEndNodeInfo && i_pPinInfo.getPinType() == PinType::Output)
	{
		i_pPinInfo.setPinTypeValue(PinType::Input);
		l_pEndNodeInfo->addPinInfo(i_pPinInfo, i_pNodeManager, i_PinIndex);
		i_pPinInfo.setPinTypeValue(PinType::Output);
	}
}

void CompositeNodeInfo::delPinInfo2(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::delPinInfo(i_PinType, i_PinIndex, i_pNodeManager);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (l_pStartNodeInfo && i_PinType == PinType::Input)
		l_pStartNodeInfo->delPinInfo(PinType::Output, i_PinIndex, i_pNodeManager);
	if (l_pEndNodeInfo && i_PinType == PinType::Output)
		l_pEndNodeInfo->delPinInfo(PinType::Input, i_PinIndex, i_pNodeManager);
}

void CompositeNodeInfo::addParam2(const ParamInfo& i_ParamInfo, NodeManager* i_pNodeManager, Bool i_IsTemplate, S32 i_ParamIndex) noexcept
{
	NodeInfo::addParam(i_ParamInfo, i_IsTemplate, i_ParamIndex);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (!(l_pStartNodeInfo && l_pEndNodeInfo)) return;
	l_pStartNodeInfo->addParam(i_ParamInfo, i_IsTemplate, i_ParamIndex);
	l_pEndNodeInfo->addParam(i_ParamInfo, i_IsTemplate, i_ParamIndex);
}

void CompositeNodeInfo::delParam2(U32 i_ParamIndex, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::delParam(i_ParamIndex);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (!(l_pStartNodeInfo && l_pEndNodeInfo)) return;
	l_pStartNodeInfo->delParam(i_ParamIndex);
	l_pEndNodeInfo->delParam(i_ParamIndex);
}

void CompositeNodeInfo::createChildNode(NodeManager* i_pNodeManager, NodeID i_StartNode, NodeID i_EndNode) noexcept
{
	if (i_StartNode == 0)
		i_StartNode = i_pNodeManager->getNodeID();
	if (i_EndNode == 0)
		i_EndNode = i_pNodeManager->getNodeID();

	auto l_pStartChildNodeInfo = i_pNodeManager->nodePrototypeMap()["Start"]->instanceNodeInfo(i_pNodeManager, m_NodeID, i_StartNode);
	auto l_pEndChildNodeInfo = i_pNodeManager->nodePrototypeMap()["End"]->instanceNodeInfo(i_pNodeManager, m_NodeID, i_EndNode);
	if (!(l_pStartChildNodeInfo && l_pEndChildNodeInfo)) return;

	m_StartChildNodeID = i_StartNode;
	m_EndChildNodeID = i_EndNode;
}

void CompositeNodeInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	auto l_XmlEditorInfo = i_XmlElement->FirstChildElement(g_XmlEditorInfoStr.data());
	if (!l_XmlEditorInfo) return;
	auto l_XmlStartNode = l_XmlEditorInfo->FirstChildElement(g_XmlStartNodeStr.data());
	auto l_XmlEndNode = l_XmlEditorInfo->FirstChildElement(g_XmlEndNodeStr.data());
	if (!(l_XmlStartNode && l_XmlEndNode)) return;
	NodeID l_StartNodeID = l_XmlStartNode->FindAttribute(g_XmlNodeIDStr.data())->Int64Value();
	NodeID l_EndNodeID = l_XmlEndNode->FindAttribute(g_XmlNodeIDStr.data())->Int64Value();

	auto l_pStartChildNodeInfo = i_pNodeManager->nodePrototypeMap()["Start"]->instanceNodeInfo(i_pNodeManager, m_NodeID, l_StartNodeID);
	auto l_pEndChildNodeInfo = i_pNodeManager->nodePrototypeMap()["End"]->instanceNodeInfo(i_pNodeManager, m_NodeID, l_EndNodeID);
	if (!(l_pStartChildNodeInfo && l_pEndChildNodeInfo)) return;
	m_StartChildNodeID = l_StartNodeID;
	m_EndChildNodeID = l_EndNodeID;

	l_pStartChildNodeInfo->loadData(l_XmlStartNode, i_pNodeManager);
	l_pEndChildNodeInfo->loadData(l_XmlEndNode, i_pNodeManager);

	//NodeInfo::loadData(i_XmlElement, i_pNodeManager);
	//auto l_XmlEditorInfo = i_XmlElement->FirstChildElement(g_XmlEditorInfoStr.data());

	m_NodeName = l_XmlEditorInfo->FindAttribute(g_XmlNodeNameStr.data())->Value();
	m_Comment = l_XmlEditorInfo->FindAttribute(g_XmlCommentStr.data())->Value();

	PinInfoList l_InputOutputPinInfoList{};
	PinInfoList l_InputPinInfoList{};
	PinInfoList l_OutputPinInfoList{};
	ParamInfoList l_TemplateParamInfoList{};
	ParamInfoList l_ParamInfoList{};
	l_InputOutputPinInfoList.loadData<false, PinType::InputOutput>(i_XmlElement);
	l_InputPinInfoList.loadData<false, PinType::Input>(i_XmlElement);
	l_OutputPinInfoList.loadData<false, PinType::Output>(i_XmlElement);
	//l_TemplateParamInfoList.loadTemplateData(i_XmlElement);
	l_ParamInfoList.loadData(i_XmlElement);
	m_TemplateParamList.paramInfoList().clear();

	for (auto& l_PinInfo : l_InputOutputPinInfoList.pinInfoList())
		addPinInfo2(l_PinInfo, i_pNodeManager);
	for (auto& l_PinInfo : l_InputPinInfoList.pinInfoList())
		addPinInfo2(l_PinInfo, i_pNodeManager);
	for (auto& l_PinInfo : l_OutputPinInfoList.pinInfoList())
		addPinInfo2(l_PinInfo, i_pNodeManager);
	//for (const auto& i_ParamInfo : l_TemplateParamInfoList.paramInfoList())
	//	addParam2(i_ParamInfo, i_pNodeManager, true);
	for (const auto& i_ParamInfo : l_ParamInfoList.paramInfoList())
		addParam2(i_ParamInfo, i_pNodeManager);
}

void CompositeNodeInfo::saveData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::saveData(i_XmlElement, i_pNodeManager);
	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (!l_pStartNodeInfo && !l_pEndNodeInfo) return;

	auto l_XmlEditorInfo = i_XmlElement->FirstChildElement(g_XmlEditorInfoStr.data());
	if (l_XmlEditorInfo)
	{
		auto l_XmlStartNode = l_XmlEditorInfo->InsertNewChildElement(g_XmlStartNodeStr.data());
		auto l_XmlEndNode = l_XmlEditorInfo->InsertNewChildElement(g_XmlEndNodeStr.data());
		l_pStartNodeInfo->saveData(l_XmlStartNode, i_pNodeManager);
		l_pEndNodeInfo->saveData(l_XmlEndNode, i_pNodeManager);
	}
}

void TaskNodeInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	CompositeNodeInfo::loadData(i_XmlElement, i_pNodeManager);
	m_QueueFamilyType.setEnumValue(i_XmlElement->FindAttribute(g_XmlQueueFamilyStr.data())->Value());
}

void TaskNodeInfo::saveData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	CompositeNodeInfo::saveData(i_XmlElement, i_pNodeManager);
	i_XmlElement->SetAttribute(g_XmlQueueFamilyStr.data(), m_QueueFamilyType.enumValueStr().data());
}

RCEDITOR_NAMESPACE_END
#include "node.h"
#include "paraminfo.h"
#include "prototype.h"
#include "nodemanager.h"
#include <imgui_node_editor/imgui_node_editor.h>


RCEDITOR_NAMESPACE_BEGIN

std::string_view getXmlKeyByPinType(PinType i_PinType, Bool i_IsInputOutput = false)
{
	if (i_IsInputOutput || i_PinType == PinType::InputOutput)
		return g_XmlInputOutputListStr;

	if (i_PinType == PinType::Input)
		return g_XmlInputListStr;
	else if (i_PinType == PinType::Output)
		return g_XmlOutputListStr;

	return g_XmlErrorKey;
}

PinInfo::PinInfo(const NodeID i_NodeID, const PinID i_PinID) noexcept
{
	m_NodeID = i_NodeID;
	m_PinID = i_PinID;
}

void PinInfo::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	if (m_IsInputOutput)
	{
		m_InputMetaData = i_XmlElement->FindAttribute(g_XmlInputMetaDataStr.data())->Value();
		m_OutputMetaData = i_XmlElement->FindAttribute(g_XmlOutputMetaDataStr.data())->Value();
	}
	else 
	{
		if (m_PinType == PinType::Input)
		{
			m_InputMetaData = i_XmlElement->FindAttribute(g_XmlMetaDataStr.data())->Value();
		}
		else if (m_PinType == PinType::Output)
		{
			m_OutputMetaData = i_XmlElement->FindAttribute(g_XmlMetaDataStr.data())->Value();
		}
	
	}
	m_Name = i_XmlElement->FindAttribute(g_XmlNameStr.data())->Value();
	m_DataType.setEnumValue(i_XmlElement->FindAttribute(g_XmlDataTypeStr.data())->Value());
	m_PinAccessType.setEnumValue(i_XmlElement->FindAttribute(g_XmlAccessTypeStr.data())->Value());
}

tinyxml2::XMLElement* PinInfo::saveData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	std::string l_XmlPinStr{};
	if (m_IsInputOutput)
		l_XmlPinStr = g_XmlInputOutputStr;
	else
	{
		if (m_PinType == PinType::Input)
			l_XmlPinStr = g_XmlInputStr;
		else if (m_PinType == PinType::Output)
			l_XmlPinStr = g_XmlOutputStr;
	}

	tinyxml2::XMLElement* l_XmlElement = i_XmlElement->InsertNewChildElement(l_XmlPinStr.data());
	l_XmlElement->SetAttribute(g_XmlNameStr.data(), m_Name.c_str());
	if (m_IsInputOutput)
	{
		l_XmlElement->SetAttribute(g_XmlInputMetaDataStr.data(), m_InputMetaData.c_str());
		l_XmlElement->SetAttribute(g_XmlOutputMetaDataStr.data(), m_OutputMetaData.c_str());
	}
	else
	{
		if (m_PinType == PinType::Input)
		{
			l_XmlElement->SetAttribute(g_XmlMetaDataStr.data(), m_InputMetaData.c_str());
		}
		else if (m_PinType == PinType::Output)
		{
			l_XmlElement->SetAttribute(g_XmlMetaDataStr.data(), m_OutputMetaData.c_str());
		}
	}
	l_XmlElement->SetAttribute(g_XmlDataTypeStr.data(), m_DataType.enumValueStr().c_str());
	l_XmlElement->SetAttribute(g_XmlAccessTypeStr.data(), m_PinAccessType.enumValueStr().c_str());

	return l_XmlElement;
}

template<Bool IsTemplate, PinType PinListType>
void PinInfoList::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	tinyxml2::XMLElement* l_XmlPinList = i_XmlElement->FirstChildElement(getXmlKeyByPinType(PinListType).data());
	if (!l_XmlPinList) return;
	for (tinyxml2::XMLElement* l_XmlElementPin = l_XmlPinList->FirstChildElement(/*g_XmlPinStr.data()*/); l_XmlElementPin != nullptr; l_XmlElementPin = l_XmlElementPin->NextSiblingElement(/*g_XmlPinStr.data()*/))
	{
		m_PinInfoList.push_back({});
		auto& l_PinInfo = *(m_PinInfoList.end() - 1);
		l_PinInfo.setIsTemplate(IsTemplate);
		l_PinInfo.setIsInputOutput(PinListType == PinType::InputOutput);
		l_PinInfo.setPinTypeValue(PinListType);
		l_PinInfo.loadData(l_XmlElementPin);
	}
}

template<Bool IsTemplate, PinType PinListType>
void PinInfoList::saveData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	tinyxml2::XMLElement* l_XmlPinList = i_XmlElement->InsertNewChildElement(getXmlKeyByPinType(PinListType).data());
	if (!l_XmlPinList) return;

	for (auto& l_PinInfo : m_PinInfoList)
	{
		l_PinInfo.saveData(l_XmlPinList);
	}
}

void ParamInfoList::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	auto l_XmlParamInfoList = i_XmlElement->FirstChildElement(g_XmlParamListStr.data());
	if (!l_XmlParamInfoList) return;

	for (auto l_XmlParamInfo = l_XmlParamInfoList->FirstChildElement(g_XmlParamStr.data()); l_XmlParamInfo != nullptr; l_XmlParamInfo = l_XmlParamInfo->NextSiblingElement(g_XmlParamStr.data()))
	{
		m_ParamInfoList.push_back({});
		auto l_LastParamInfo = m_ParamInfoList.end() - 1;
		l_LastParamInfo->loadData(l_XmlParamInfo);
	}
}

void ParamInfoList::loadTemplateData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	auto l_XmlParamInfoList = i_XmlElement->FirstChildElement(g_XmlTemplateParamListStr.data());
	if (!l_XmlParamInfoList) return;

	for (auto l_XmlParamInfo = l_XmlParamInfoList->FirstChildElement(g_XmlParamStr.data()); l_XmlParamInfo != nullptr; l_XmlParamInfo = l_XmlParamInfo->NextSiblingElement(g_XmlParamStr.data()))
	{
		m_ParamInfoList.push_back({});
		auto l_LastParamInfo = m_ParamInfoList.end() - 1;
		l_LastParamInfo->loadData(l_XmlParamInfo);
	}
}

void ParamInfoList::saveData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	auto l_XmlParamInfoList = i_XmlElement->InsertNewChildElement(g_XmlParamListStr.data());
	if (!l_XmlParamInfoList) return;

	for (auto& l_ParamInfo : m_ParamInfoList)
	{
		l_ParamInfo.saveData(l_XmlParamInfoList);
	}
}

void ParamInfoList::saveTemplateData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	auto l_XmlParamInfoList = i_XmlElement->InsertNewChildElement(g_XmlTemplateParamListStr.data());
	if (!l_XmlParamInfoList) return;

	for (auto& l_ParamInfo : m_ParamInfoList)
	{
		l_ParamInfo.saveTemplateData(l_XmlParamInfoList);
	}
}

std::string ParamInfoList::getIndexListStr() noexcept
{
	std::string l_IndexListStr = "-1";
	for (U32 i = 0; i < m_ParamInfoList.size(); ++i)
	{
		l_IndexListStr += '\0' + std::to_string(i);
	}
	return l_IndexListStr;
}

std::shared_ptr<NodePrototype> NodePrototype::createNodePrototype(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	std::shared_ptr<NodePrototype> l_pNodePrototype{};
	if (!i_XmlElement->FindAttribute(g_XmlTemplateNameStr.data()))
	{
		auto l_pTemplateNodePrototype = TemplateNodePrototype::createTemplatePrototype();
		l_pNodePrototype = std::dynamic_pointer_cast<NodePrototype>(l_pTemplateNodePrototype);
	}
	else
	{
		auto l_pFavoritePrototype = FavoriteNodePrototype::createFavoritePrototype(i_XmlElement);
		l_pNodePrototype = std::dynamic_pointer_cast<NodePrototype>(l_pFavoritePrototype);

		EnumType l_NodeType = { EnumName::NodeType, static_cast<S64>(NodeType::GPU) };
		l_NodeType.setEnumValue(i_XmlElement->FindAttribute(g_XmlNodeTypeStr.data())->Value());
	}

	return l_pNodePrototype;
}

void NodePrototype::initData(std::shared_ptr<NodeInfo> i_pNodeInfo, std::string_view i_NewNodeName, NodeManager* i_pNodeManager, NodeID& i_RelativeNodeID) noexcept
{
	m_NodeName = i_NewNodeName.data();
	m_TemplateName = i_pNodeInfo->getTemplateName();
	m_Comment = i_pNodeInfo->getComment();
	m_NodeType = i_pNodeInfo->getNodeType();
}

void NodePrototype::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	if (i_XmlElement->FindAttribute(g_XmlNodeNameStr.data()))
		m_NodeName = i_XmlElement->FindAttribute(g_XmlNodeNameStr.data())->Value();
	if (i_XmlElement->FindAttribute(g_XmlTemplateNameStr.data()))
		m_TemplateName = i_XmlElement->FindAttribute(g_XmlTemplateNameStr.data())->Value();
	m_NodeType.setEnumValue(i_XmlElement->FindAttribute(g_XmlNodeTypeStr.data())->Value());

	auto l_XmlEditorInfo = i_XmlElement->FirstChildElement(g_XmlEditorInfoStr.data());
	if(l_XmlEditorInfo)
		m_Comment = l_XmlEditorInfo->FindAttribute(g_XmlCommentStr.data())->Value();
}

void NodePrototype::saveData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	i_XmlElement->SetAttribute(g_XmlNodeNameStr.data(), m_NodeName.data());
	i_XmlElement->SetAttribute(g_XmlTemplateNameStr.data(), m_TemplateName.c_str());
	i_XmlElement->SetAttribute(g_XmlNodeTypeStr.data(), m_NodeType.enumValueStr().c_str());

	auto l_XmlEditorInfo = i_XmlElement->InsertNewChildElement(g_XmlEditorInfoStr.data());
	if (l_XmlEditorInfo)
		l_XmlEditorInfo->SetAttribute(g_XmlCommentStr.data(), m_Comment.c_str());
}

std::shared_ptr<NodeInfo> NodePrototype::instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_NodeID) noexcept
{
	if (i_NodeID == 0)
		i_NodeID = i_pNodeManager->getNodeID();

	std::shared_ptr<NodeInfo> l_pNodeInfo{};
	if (m_NodeType == NodeType::GPU || m_NodeType == NodeType::CPU || m_NodeType == NodeType::PS || m_NodeType == NodeType::CS)
	{
		auto l_pTempNodeInfo = std::make_shared<NormalNodeInfo>(NormalNodeInfo(i_NodeID, i_ParentNodeID));
		l_pNodeInfo = std::dynamic_pointer_cast<NodeInfo>(l_pTempNodeInfo);
	}
	else if (m_NodeType == NodeType::Start || m_NodeType == NodeType::End)
	{
		auto l_pTempNodeInfo = std::make_shared<ChildNodeInfo>(ChildNodeInfo(i_NodeID, i_ParentNodeID));
		l_pNodeInfo = std::dynamic_pointer_cast<NodeInfo>(l_pTempNodeInfo);
	}
	else if (m_NodeType == NodeType::Composite)
	{
		auto l_pTempNodeInfo = std::make_shared<CompositeNodeInfo>(CompositeNodeInfo(i_NodeID, i_ParentNodeID));
		l_pNodeInfo = std::dynamic_pointer_cast<NodeInfo>(l_pTempNodeInfo);
	}
	else if (m_NodeType == NodeType::Data)
	{
		auto l_pTempNodeInfo = std::make_shared<DataNodeInfo>(DataNodeInfo(i_NodeID, i_ParentNodeID));
		l_pNodeInfo = std::dynamic_pointer_cast<NodeInfo>(l_pTempNodeInfo);
	}
	else if (isTaskNode())
	{
		auto l_pTempNodeInfo = std::make_shared<TaskNodeInfo>(TaskNodeInfo(i_NodeID, i_ParentNodeID));
		l_pNodeInfo = std::dynamic_pointer_cast<NodeInfo>(l_pTempNodeInfo);
	}

	//Set Task Node ID
	if (i_ParentNodeID != 0)
	{
		auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(i_ParentNodeID);
		if (!l_pParentNodeInfo) return nullptr;
		if (l_pParentNodeInfo->isTaskNode())
			l_pNodeInfo->setTaskNodeID(i_ParentNodeID);
		else
			l_pNodeInfo->setTaskNodeID(l_pParentNodeInfo->getTaskNodeID());
	}

	i_pNodeManager->nodeMap()[i_NodeID] = l_pNodeInfo;
	if (i_NodeID >= i_pNodeManager->curNodeID())
		i_pNodeManager->loadNodeID(i_NodeID + 1);

	if (m_NodeType == NodeType::Composite || isTaskNode())
		i_pNodeManager->addSubRCInfo(i_NodeID);

	std::shared_ptr<SubRCInfo> l_pSubRCInfo = i_pNodeManager->getSubRCInfo(i_ParentNodeID);
	if (l_pSubRCInfo)
		l_pSubRCInfo->m_NodeIDList.insert(l_pNodeInfo->getNodeID());

	return l_pNodeInfo;
}

TemplateNodePrototype::TemplateNodePrototype()
{
}

std::shared_ptr<TemplateNodePrototype> TemplateNodePrototype::createTemplatePrototype() noexcept
{
	return std::make_shared<TemplateNodePrototype>();
}

void TemplateNodePrototype::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	NodePrototype::loadData(i_XmlElement);

	if (i_XmlElement->FindAttribute(g_XmlNodeTagStr.data()))
	{
		m_NodeTag.setEnumValue(i_XmlElement->FindAttribute(g_XmlNodeTagStr.data())->Value());
	}

	if (m_NodeTag & NodeTag::Block)
	{
		m_QueueFamilyType.m_Name = EnumName::TaskBlock;
	}
	else if (m_NodeTag & NodeTag::Host)
	{
		m_QueueFamilyType.m_Name = EnumName::TaskThread;
	}
	else if (m_NodeTag & NodeTag::Vulkan)
	{
		m_QueueFamilyType.m_Name = EnumName::TaskQueueFamily;
	}

	if (i_XmlElement->FindAttribute(g_XmlQueueFamilyStr.data()))
	{
		m_QueueFamilyType.setEnumValue(i_XmlElement->FindAttribute(g_XmlQueueFamilyStr.data())->Value());
	}

	m_TemplateName = m_NodeName;
	m_ClassName = i_XmlElement->FindAttribute(g_XmlClassNameStr.data())->Value();
	m_IsDynamicInputOutputPin = i_XmlElement->FindAttribute(g_XmlIsDynamicInputOutputStr.data())->BoolValue();
	m_IsDynamicInputPin = i_XmlElement->FindAttribute(g_XmlIsDynamicInputStr.data())->BoolValue();
	m_IsDynamicOutputPin = i_XmlElement->FindAttribute(g_XmlIsDynamicOutputStr.data())->BoolValue();
	m_IsDynamicParam = i_XmlElement->FindAttribute(g_XmlIsDynamicParamStr.data())->BoolValue();

	m_TemplateInputOutputPinInfoList.loadData<true, PinType::InputOutput>(i_XmlElement);
	m_TemplateInputPinInfoList.loadData<true, PinType::Input>(i_XmlElement);
	m_TemplateOutputPinInfoList.loadData<true, PinType::Output>(i_XmlElement);
	m_TemplateParamList.loadData(i_XmlElement);
}

std::shared_ptr<NodeInfo> TemplateNodePrototype::instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_NodeID) noexcept
{
	auto l_pNodeInfo = NodePrototype::instanceNodeInfo(i_pNodeManager, i_ParentNodeID, i_NodeID);

	l_pNodeInfo->setNodeName(m_TemplateName);
	l_pNodeInfo->setTemplateName(m_TemplateName);
	l_pNodeInfo->setClassName(m_ClassName);
	l_pNodeInfo->setNodeType(getNodeType());
	l_pNodeInfo->setNodeTag(getNodeTag());

	l_pNodeInfo->setIsDynamicInputOutputPin(getIsDynamicInputOutputPin()); 
	l_pNodeInfo->setIsDynamicInputPin(getIsDynamicInputPin());
	l_pNodeInfo->setIsDynamicOutputPin(getIsDynamicOutputPin());
	l_pNodeInfo->setIsDynamicParam(getIsDynamicParam());

	for (auto& l_PinInfo : m_TemplateInputOutputPinInfoList.pinInfoList())
		l_pNodeInfo->addPinInfo(l_PinInfo, i_pNodeManager);
	for (auto& l_PinInfo : m_TemplateInputPinInfoList.pinInfoList())
		l_pNodeInfo->addPinInfo(l_PinInfo, i_pNodeManager);
	for (auto& l_PinInfo : m_TemplateOutputPinInfoList.pinInfoList())
		l_pNodeInfo->addPinInfo(l_PinInfo, i_pNodeManager);
	for (auto& l_ParamInfo : m_TemplateParamList.paramInfoList())
		l_pNodeInfo->addParam(l_ParamInfo, true);

	if (std::dynamic_pointer_cast<TaskNodeInfo>(l_pNodeInfo))
	{
		auto l_pTaskNodeInfo = std::dynamic_pointer_cast<TaskNodeInfo>(l_pNodeInfo);
		l_pTaskNodeInfo->setQueueFamilyType(m_QueueFamilyType);
	}

	return l_pNodeInfo;
}

std::shared_ptr<FavoriteNodePrototype> FavoriteNodePrototype::createFavoritePrototype(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	std::shared_ptr<FavoriteNodePrototype> l_pFavoritePrototype{};
	EnumType l_NodeType = { EnumName::NodeType, static_cast<S64>(NodeType::GPU) };
	l_NodeType.setEnumValue(i_XmlElement->FindAttribute(g_XmlNodeTypeStr.data())->Value());

	if (l_NodeType == NodeType::CPU || l_NodeType == NodeType::GPU)
	{
		auto l_pFavoriteNormalNodePrototype = std::make_shared<FavoriteNormalNodePrototype>();
		l_pFavoritePrototype = std::dynamic_pointer_cast<FavoriteNodePrototype>(l_pFavoriteNormalNodePrototype);
	}
	else if (l_NodeType == NodeType::Start || l_NodeType == NodeType::End)
	{
		auto l_pFavoriteChildNodePrototype = std::make_shared<FavoriteChildNodePrototype>();
		l_pFavoritePrototype = std::dynamic_pointer_cast<FavoriteNodePrototype>(l_pFavoriteChildNodePrototype);
	}
	else if (l_NodeType == NodeType::Composite)
	{
		auto l_pFavoriteCompositeNodePrototype = std::make_shared<FavoriteCompositeNodePrototype>();
		l_pFavoritePrototype = std::dynamic_pointer_cast<FavoriteNodePrototype>(l_pFavoriteCompositeNodePrototype);
	}

	return l_pFavoritePrototype;
}

void FavoriteNodePrototype::initData(std::shared_ptr<NodeInfo> i_pNodeInfo, std::string_view i_NewNodeName, NodeManager* i_pNodeManager, NodeID& i_RelativeNodeID) noexcept
{
	NodePrototype::initData(i_pNodeInfo, i_NewNodeName, i_pNodeManager, i_RelativeNodeID);
	m_RelativeNodeID = i_RelativeNodeID;
	i_RelativeNodeID++;
	m_InputOutputPinInfoList = i_pNodeInfo->inputOutputOutputPinIDList().getPinInfoList(i_pNodeManager);
	m_InputPinInfoList = i_pNodeInfo->inputPinIDList().getPinInfoList(i_pNodeManager);
	m_OutputPinInfoList = i_pNodeInfo->outputPinIDList().getPinInfoList(i_pNodeManager);
	for (const auto& l_ParamInfo : i_pNodeInfo->templateParamList().paramInfoList())
	{
		m_TemplateParamList.paramInfoList().push_back(l_ParamInfo);
	}
	for (const auto& l_ParamInfo : i_pNodeInfo->paramList().paramInfoList())
	{
		m_ParamList.paramInfoList().push_back(l_ParamInfo);
	}

	m_Position = ax::NodeEditor::GetNodePosition(i_pNodeInfo->getNodeID());
}

void FavoriteNodePrototype::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	NodePrototype::loadData(i_XmlElement);
	m_TemplateName = i_XmlElement->FindAttribute(g_XmlTemplateNameStr.data())->Value();
	if(i_XmlElement->FindAttribute(g_XmlRelativeIDStr.data()))
		m_RelativeNodeID = i_XmlElement->FindAttribute(g_XmlRelativeIDStr.data())->Int64Value();

	if (i_XmlElement->FindAttribute(g_XmlLocationXStr.data()) && i_XmlElement->FindAttribute(g_XmlLocationYStr.data()))
	{
		m_Position.x = i_XmlElement->FindAttribute(g_XmlLocationXStr.data())->IntValue();
		m_Position.y = i_XmlElement->FindAttribute(g_XmlLocationYStr.data())->IntValue();
	}

	m_InputOutputPinInfoList.loadData<false, PinType::InputOutput>(i_XmlElement);
	m_InputPinInfoList.loadData<false, PinType::Input>(i_XmlElement);
	m_OutputPinInfoList.loadData<false, PinType::Output>(i_XmlElement);
	m_TemplateParamList.loadTemplateData(i_XmlElement);
	m_ParamList.loadData(i_XmlElement);
}

void FavoriteNodePrototype::saveData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	if (m_RelativeNodeID != 0)
	{
		i_XmlElement->SetAttribute(g_XmlRelativeIDStr.data(), m_RelativeNodeID);
	}
	NodePrototype::saveData(i_XmlElement);

	if (m_RelativeNodeID != 0)
	{
		i_XmlElement->SetAttribute(g_XmlLocationXStr.data(), m_Position.x);
		i_XmlElement->SetAttribute(g_XmlLocationYStr.data(), m_Position.y);
	}

	m_InputOutputPinInfoList.saveData<false, PinType::InputOutput>(i_XmlElement);
	m_InputPinInfoList.saveData<false, PinType::Input>(i_XmlElement);
	m_OutputPinInfoList.saveData<false, PinType::Output>(i_XmlElement);
	m_TemplateParamList.saveTemplateData(i_XmlElement);
	m_ParamList.saveData(i_XmlElement);
}

std::shared_ptr<NodeInfo> FavoriteNodePrototype::instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_NodeID) noexcept
{
	auto l_pTemplatePrototype = i_pNodeManager->nodePrototypeMap()[m_TemplateName];
	if (!l_pTemplatePrototype) return nullptr;

	auto l_pNodeInfo = l_pTemplatePrototype->instanceNodeInfo(i_pNodeManager, i_ParentNodeID, i_NodeID + m_RelativeNodeID);
	if(m_RelativeNodeID != 0)
		ax::NodeEditor::SetNodePosition(l_pNodeInfo->getNodeID(), m_Position);
	l_pNodeInfo->templateParamList().paramInfoList().clear();

	for (auto& l_PinInfo : m_InputOutputPinInfoList.pinInfoList())
		l_pNodeInfo->addPinInfo(l_PinInfo, i_pNodeManager);
	for (auto& l_PinInfo : m_InputPinInfoList.pinInfoList())
		l_pNodeInfo->addPinInfo(l_PinInfo, i_pNodeManager);
	for (auto& l_PinInfo : m_OutputPinInfoList.pinInfoList())
		l_pNodeInfo->addPinInfo(l_PinInfo, i_pNodeManager);
	for (auto& l_ParamInfo : m_TemplateParamList.paramInfoList())
		l_pNodeInfo->addParam(l_ParamInfo, true);
	for (auto& l_ParamInfo : m_ParamList.paramInfoList())
		l_pNodeInfo->addParam(l_ParamInfo);

	return l_pNodeInfo;
}

void FavoriteChildNodePrototype::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	if (!i_XmlElement) return;
	m_RelativeNodeID = i_XmlElement->FindAttribute(g_XmlRelativeIDStr.data())->Int64Value();
	m_NodeName = i_XmlElement->FindAttribute(g_XmlNodeNameStr.data())->Value();
	m_TemplateName = i_XmlElement->FindAttribute(g_XmlTemplateNameStr.data())->Value();
	m_Comment = i_XmlElement->FindAttribute(g_XmlCommentStr.data())->Value();

	if (i_XmlElement->FindAttribute(g_XmlLocationXStr.data()) && i_XmlElement->FindAttribute(g_XmlLocationYStr.data()))
	{
		m_Position.x = i_XmlElement->FindAttribute(g_XmlLocationXStr.data())->IntValue();
		m_Position.y = i_XmlElement->FindAttribute(g_XmlLocationYStr.data())->IntValue();
	}
}

void FavoriteChildNodePrototype::saveData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	i_XmlElement->SetAttribute(g_XmlRelativeIDStr.data(), m_RelativeNodeID);
	i_XmlElement->SetAttribute(g_XmlNodeNameStr.data(), m_NodeName.c_str());
	i_XmlElement->SetAttribute(g_XmlTemplateNameStr.data(), m_TemplateName.c_str());
	i_XmlElement->SetAttribute(g_XmlCommentStr.data(), m_Comment.c_str());
	if (m_RelativeNodeID != 0)
	{
		i_XmlElement->SetAttribute(g_XmlLocationXStr.data(), m_Position.x);
		i_XmlElement->SetAttribute(g_XmlLocationYStr.data(), m_Position.y);
	}
}

std::shared_ptr<NodeInfo> FavoriteChildNodePrototype::instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_NodeID) noexcept
{
	return FavoriteNodePrototype::instanceNodeInfo(i_pNodeManager, i_ParentNodeID, i_NodeID);
}

FavoriteCompositeNodePrototype::FavoriteCompositeNodePrototype() : FavoriteNodePrototype()
{
	m_pStartChildPrototype = std::make_shared<FavoriteChildNodePrototype>();
	m_pEndChildPrototype = std::make_shared<FavoriteChildNodePrototype>();
}

void FavoriteCompositeNodePrototype::initData(std::shared_ptr<NodeInfo> i_pNodeInfo, std::string_view i_NewNodeName, NodeManager* i_pNodeManager, NodeID& i_RelativeNodeID) noexcept
{
	FavoriteNodePrototype::initData(i_pNodeInfo, i_NewNodeName, i_pNodeManager, i_RelativeNodeID);

	auto l_pStartNodeID = i_pNodeInfo->getAssociationNodeID(PinType::Input);
	auto l_pEndNodeID = i_pNodeInfo->getAssociationNodeID(PinType::Output);
	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(l_pStartNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(l_pEndNodeID);
	if (!(l_pStartNodeInfo && l_pEndNodeInfo)) return;

	std::map<NodeID, NodeID> l_NodeIDRelativeMap{};

	l_NodeIDRelativeMap[l_pStartNodeID] = i_RelativeNodeID;
	m_pStartChildPrototype->initData(l_pStartNodeInfo, l_pStartNodeInfo->getNodeName(), i_pNodeManager, i_RelativeNodeID);
	l_NodeIDRelativeMap[l_pEndNodeID] = i_RelativeNodeID;
	m_pEndChildPrototype->initData(l_pEndNodeInfo, l_pEndNodeInfo->getNodeName(), i_pNodeManager, i_RelativeNodeID);

	auto l_SubRCInfo = i_pNodeManager->getSubRCInfo(i_pNodeInfo->getNodeID());
	m_pNodePrototypeList.clear();
	m_pNodePrototypeList.reserve(l_SubRCInfo->m_NodeIDList.size());
	//Init Sub Node
	for (auto l_NodeID : l_SubRCInfo->m_NodeIDList)
	{
		auto l_pNodeInfo = i_pNodeManager->nodeInfo(l_NodeID);
		if (!l_pNodeInfo) continue;

		if (l_pNodeInfo->getNodeType() != NodeType::Start && l_pNodeInfo->getNodeType() != NodeType::End)
		{
			l_NodeIDRelativeMap[l_NodeID] = i_RelativeNodeID;
			auto l_pSubNodePrototype = l_pNodeInfo->getNodePrototype(l_pNodeInfo->getNodeName(), i_pNodeManager, i_RelativeNodeID);
			if (l_pSubNodePrototype)
				m_pNodePrototypeList.push_back(l_pSubNodePrototype);
		}

	}

	//Init Link Info
	m_LinkInfoList.clear();
	m_LinkInfoList.resize(l_SubRCInfo->m_LinkIDList.size());
	U32 l_Index = 0;
	for (auto l_LinkID : l_SubRCInfo->m_LinkIDList)
	{
		auto l_pLinkInfo = i_pNodeManager->linkInfo(l_LinkID);
		if (l_pLinkInfo)
		{
			m_LinkInfoList[l_Index].m_OutputRelativeNodeID = l_NodeIDRelativeMap.at(l_pLinkInfo->m_OutputNodeID);
			m_LinkInfoList[l_Index].m_InputRelativeNodeID = l_NodeIDRelativeMap.at(l_pLinkInfo->m_InputNodeID);
			auto l_pOutputPinInfo = i_pNodeManager->pinInfo(l_pLinkInfo->m_OutputPinID);
			auto l_pInputPinInfo = i_pNodeManager->pinInfo(l_pLinkInfo->m_InputPinID);
			if (l_pOutputPinInfo && l_pInputPinInfo)
			{
				m_LinkInfoList[l_Index].m_OutputPinName = l_pOutputPinInfo->getName();
				m_LinkInfoList[l_Index].m_InputPinName = l_pInputPinInfo->getName();
			}
			l_Index++;
		}
	}
}

void FavoriteCompositeNodePrototype::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	FavoriteNodePrototype::loadData(i_XmlElement);

	auto l_XmlEditorInfo = i_XmlElement->FirstChildElement(g_XmlEditorInfoStr.data());
	auto l_XmlStartNode = l_XmlEditorInfo->FirstChildElement(g_XmlStartNodeStr.data());
	auto l_XmlEndNode = l_XmlEditorInfo->FirstChildElement(g_XmlEndNodeStr.data());
	m_pStartChildPrototype->loadData(l_XmlStartNode);
	m_pEndChildPrototype->loadData(l_XmlEndNode);
	//init ChildNode PinInfoList
	for (auto l_PinInfo : m_InputPinInfoList.pinInfoList())
	{
		l_PinInfo.setPinTypeValue(PinType::Output);
		m_pStartChildPrototype->outputPinInfoList().pinInfoList().push_back(l_PinInfo);
	}

	for (auto l_PinInfo : m_OutputPinInfoList.pinInfoList())
	{
		l_PinInfo.setPinTypeValue(PinType::Input);
		m_pEndChildPrototype->inputPinInfoList().pinInfoList().push_back(l_PinInfo);
	}

	// Load Node List
	auto l_XmlNodeList = i_XmlElement->FirstChildElement(g_XmlNodeListStr.data());
	if (l_XmlNodeList)
	{
		for (auto l_XmlSubNode = l_XmlNodeList->FirstChildElement(g_XmlFavoriteNodeStr.data()); l_XmlSubNode != nullptr; l_XmlSubNode = l_XmlSubNode->NextSiblingElement(g_XmlFavoriteNodeStr.data()))
		{
			EnumType l_NodeType = { EnumName::NodeType, static_cast<S64>(NodeType::GPU) };
			l_NodeType.setEnumValue(l_XmlSubNode->FindAttribute("NodeType")->Value());

			auto l_pPrototype = createFavoritePrototype(l_XmlSubNode);
			if(!l_pPrototype) continue;

			l_pPrototype->loadData(l_XmlSubNode);
			m_pNodePrototypeList.push_back(l_pPrototype);
		}
	}

	// Load Link List
	auto l_XmlNodeLinkListStr = i_XmlElement->FirstChildElement(g_XmlNodeLinkListStr.data());
	if (l_XmlNodeLinkListStr)
	{
		for (auto l_XmlSubLink = l_XmlNodeLinkListStr->FirstChildElement(g_XmlLinkStr.data()); l_XmlSubLink != nullptr; l_XmlSubLink = l_XmlSubLink->NextSiblingElement(g_XmlLinkStr.data()))
		{
			m_LinkInfoList.push_back({});
			auto l_FavoriteLinkInfo = m_LinkInfoList.end() - 1;
			l_FavoriteLinkInfo->loadData(l_XmlSubLink);
		}
	}
}

void FavoriteCompositeNodePrototype::saveData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	FavoriteNodePrototype::saveData(i_XmlElement);

	auto l_XmlEditorInfo = i_XmlElement->FirstChildElement(g_XmlEditorInfoStr.data());
	if(!l_XmlEditorInfo) return;
	auto l_XmlStartNode = l_XmlEditorInfo->InsertNewChildElement(g_XmlStartNodeStr.data());
	auto l_XmlEndNode = l_XmlEditorInfo->InsertNewChildElement(g_XmlEndNodeStr.data());
	if (l_XmlStartNode && l_XmlEndNode)
	{
		m_pStartChildPrototype->saveData(l_XmlStartNode);
		m_pEndChildPrototype->saveData(l_XmlEndNode);
	}

	//Save Node Data
	auto l_XmlNodeList = i_XmlElement->InsertNewChildElement(g_XmlNodeListStr.data());
	for(auto l_pSubPrototypeInfo : m_pNodePrototypeList)
	{
		if (l_pSubPrototypeInfo->getNodeType() != NodeType::Start && l_pSubPrototypeInfo->getNodeType() != NodeType::End)
		{
			auto l_XmlSubNode = l_XmlNodeList->InsertNewChildElement(g_XmlFavoriteNodeStr.data());
			l_pSubPrototypeInfo->saveData(l_XmlSubNode);
		}
	}

	//Save Link Data
	auto l_XmlNodeLinkList = i_XmlElement->InsertNewChildElement(g_XmlNodeLinkListStr.data());
	for (auto l_FavoriteLinkInfo : m_LinkInfoList)
	{
		l_FavoriteLinkInfo.saveData(l_XmlNodeLinkList);
	}
}

std::shared_ptr<NodeInfo> FavoriteCompositeNodePrototype::instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_BaseNodeID) noexcept
{
	auto l_pNodeInfo = FavoriteNodePrototype::instanceNodeInfo(i_pNodeManager, i_ParentNodeID, i_BaseNodeID);
	if (i_BaseNodeID == 0)
		i_BaseNodeID = l_pNodeInfo->getNodeID();

	auto l_pCompositeNodeInfo = std::dynamic_pointer_cast<CompositeNodeInfo>(l_pNodeInfo);
	if (!l_pCompositeNodeInfo) return nullptr;

	auto l_pStartNodeInfo = m_pStartChildPrototype->instanceNodeInfo(i_pNodeManager, l_pNodeInfo->getNodeID(), i_BaseNodeID);
	auto l_pEndNodeInfo = m_pEndChildPrototype->instanceNodeInfo(i_pNodeManager, l_pNodeInfo->getNodeID(), i_BaseNodeID);

	if (l_pStartNodeInfo && l_pEndNodeInfo)
	{
		l_pCompositeNodeInfo->setStartChildNodeID(l_pStartNodeInfo->getNodeID());
		l_pCompositeNodeInfo->setEndChildNodeID(l_pEndNodeInfo->getNodeID());
	}

	//Create Node Info
	for (auto l_pNodePrototype : m_pNodePrototypeList)
	{
		l_pNodePrototype->instanceNodeInfo(i_pNodeManager, l_pNodeInfo->getNodeID(), i_BaseNodeID);
	}

	//Create Link Info
	for (auto l_PrototypeLinkInfo : m_LinkInfoList)
	{
		l_PrototypeLinkInfo.instanceData(i_pNodeManager, i_BaseNodeID);
	}

	return l_pNodeInfo;
}

void FavoriteCompositeNodePrototype::PrototypeLinkInfo::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	auto l_XmlOutputInfo = i_XmlElement->FirstChildElement(g_XmlLinkProducerStr.data());
	auto l_XmlInputInfo = i_XmlElement->FirstChildElement(g_XmlLinkConsumerStr.data());
	if (l_XmlOutputInfo && l_XmlInputInfo)
	{
		m_OutputRelativeNodeID = l_XmlOutputInfo->FindAttribute(g_XmlNodeIDStr.data())->Int64Value();
		m_OutputPinName = l_XmlOutputInfo->FindAttribute(g_XmlPinNameStr.data())->Value();
		m_InputRelativeNodeID = l_XmlInputInfo->FindAttribute(g_XmlNodeIDStr.data())->Int64Value();
		m_InputPinName = l_XmlInputInfo->FindAttribute(g_XmlPinNameStr.data())->Value();
	}
}
void FavoriteCompositeNodePrototype::PrototypeLinkInfo::saveData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	auto l_XmlLinkInfo = i_XmlElement->InsertNewChildElement(g_XmlLinkStr.data());
	if (!l_XmlLinkInfo) return;
	auto l_XmlOutputInfo = l_XmlLinkInfo->InsertNewChildElement(g_XmlLinkProducerStr.data());
	auto l_XmlInputInfo = l_XmlLinkInfo->InsertNewChildElement(g_XmlLinkConsumerStr.data());
	if (!(l_XmlOutputInfo && l_XmlInputInfo)) return;

	l_XmlOutputInfo->SetAttribute(g_XmlNodeIDStr.data(), m_OutputRelativeNodeID);
	l_XmlOutputInfo->SetAttribute(g_XmlPinNameStr.data(), m_OutputPinName.c_str());
	l_XmlInputInfo->SetAttribute(g_XmlNodeIDStr.data(), m_InputRelativeNodeID);
	l_XmlInputInfo->SetAttribute(g_XmlPinNameStr.data(), m_InputPinName.c_str());
}

void FavoriteCompositeNodePrototype::PrototypeLinkInfo::instanceData(NodeManager* i_pNodeManager, NodeID i_BaseNodeID) noexcept
{
	auto l_pOutputNodeInfo = i_pNodeManager->nodeInfo(m_OutputRelativeNodeID + i_BaseNodeID);
	auto l_pInputNodeInfo = i_pNodeManager->nodeInfo(m_InputRelativeNodeID + i_BaseNodeID);
	if (l_pOutputNodeInfo && l_pInputNodeInfo)
	{
		auto [l_OutputNodeInputPinID, l_OutputNodeOutputPinID] = l_pOutputNodeInfo->getPinIDByKey(m_OutputPinName);
		auto [l_InputNodeInputPinID, l_InputNodeOutputPinID] = l_pInputNodeInfo->getPinIDByKey(m_InputPinName);

		i_pNodeManager->addLinkInfo(l_OutputNodeOutputPinID, l_InputNodeInputPinID);
	}
}

RCEDITOR_NAMESPACE_END
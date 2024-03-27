#include <base/public/nodemanager.h>

RCEDITOR_NAMESPACE_BEGIN

void ParamInfo::initData() noexcept {
	switch (static_cast<ParamDataType>(m_DataType.m_Value))
	{
	case ParamDataType::Int: { m_Data.m_Value = S32{}; } break;
	case ParamDataType::Bool: { m_Data.m_Value = Bool{}; } break;
	case ParamDataType::RCFormat: { EnumType l_EnumTypeData{}; l_EnumTypeData.setEnumName("RCFormat"); m_Data.m_Value = l_EnumTypeData; } break;
	case ParamDataType::Float: { m_Data.m_Value = F32{}; } break;
	case ParamDataType::String: { m_Data.m_Value = std::string(); } break;
	case ParamDataType::WString: { m_Data.m_Value = std::wstring(); } break;
	case ParamDataType::Vector2: { m_Data.m_Value = static_cast<Vector2f>(Vector2f::Zero()); } break;
	case ParamDataType::Vector3: { m_Data.m_Value = static_cast<Vector3f>(Vector3f::Zero()); } break;
	case ParamDataType::Vector4: { m_Data.m_Value = static_cast<Vector4f>(Vector4f::Zero()); } break;
	case ParamDataType::Matrix2: { m_Data.m_Value = static_cast<Matrix2f>(Matrix2f::Identity()); } break;
	case ParamDataType::Matrix3: { m_Data.m_Value = static_cast<Matrix3f>(Matrix3f::Identity()); } break;
	case ParamDataType::Matrix4: { m_Data.m_Value = static_cast<Matrix4f>(Matrix4f::Identity()); } break;
	default:;
	}
}

void ParamInfo::setDataType(ParamDataType i_DataType) noexcept {
	m_DataType = i_DataType;
	initData();
}

void ParamInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	m_Name = i_XmlElement->FindAttribute("Name")->Value();
	m_DataType.setEnumValue(i_XmlElement->FindAttribute("DataType")->Value());
	m_UseRef = i_XmlElement->FindAttribute("UseRef")->BoolValue();
	m_RefKey = i_XmlElement->FindAttribute("RefKey")->Value();
	m_Data.setData(m_DataType, i_XmlElement->FindAttribute("Data")->Value());
}

tinyxml2::XMLElement* ParamInfo::saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, rceditor::NodeManager* i_pNodeManager) noexcept
{
	tinyxml2::XMLElement* l_XmlParamElement = i_XmlDocument->NewElement("Param");
	l_XmlParamElement->SetAttribute("Name", m_Name.c_str());
	l_XmlParamElement->SetAttribute("DataType", m_DataType.enumValueStr().c_str());
	l_XmlParamElement->SetAttribute("Data", m_Data.getData(m_DataType).c_str());
	l_XmlParamElement->SetAttribute("UseRef", m_UseRef);
	l_XmlParamElement->SetAttribute("RefKey", m_RefKey.c_str());
	i_ParentXmlElement->InsertEndChild(l_XmlParamElement);

	return l_XmlParamElement;
}

PinInfo::PinInfo(const NodeID i_NodeID, const PinID i_PinID, const PinInfo& i_Info) noexcept {
	*this = i_Info;
	m_NodeID = i_NodeID;
	m_PinID = i_PinID;
}

PinInfo::PinInfo(const NodeID i_NodeID, const PinID i_PinID, Bool i_IsTemplate, Bool i_IsConnetion, PinType i_PinType) noexcept
{
	m_NodeID = i_NodeID;
	m_PinID = i_PinID;
	m_IsTemplate = i_IsTemplate;
	m_IsInputOutput = i_IsConnetion;
	m_PinType = i_PinType;
}

void PinInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	if (m_PinType == PinType::InputOutput)
	{
		m_InputMetaData = i_XmlElement->FindAttribute("InputMetaData")->Value();
		m_OutputMetaData = i_XmlElement->FindAttribute("OutputMetaData")->Value();
	}
	else if (m_PinType == PinType::Input)
	{
		m_InputMetaData = i_XmlElement->FindAttribute("MetaData")->Value();
	}
	else if (m_PinType == PinType::Output)
	{
		m_OutputMetaData = i_XmlElement->FindAttribute("MetaData")->Value();
	}
	m_Name = i_XmlElement->FindAttribute("Name")->Value();
	m_DataType.setEnumValue(i_XmlElement->FindAttribute("DataType")->Value());
	m_PinAccessType.setEnumValue(i_XmlElement->FindAttribute("AccessType")->Value());
}

tinyxml2::XMLElement* PinInfo::saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager) noexcept
{
	tinyxml2::XMLElement* l_XmlElement = i_XmlDocument->NewElement("Pin");
	l_XmlElement->SetAttribute("Name", m_Name.c_str());
	if (m_PinType == PinType::InputOutput)
	{
		l_XmlElement->SetAttribute("InputMetaData", m_InputMetaData.c_str());
		l_XmlElement->SetAttribute("OutputMetaData", m_OutputMetaData.c_str());
	}
	else if (m_PinType == PinType::Input)
	{
		l_XmlElement->SetAttribute("MetaData", m_InputMetaData.c_str());
	}
	else if (m_PinType == PinType::Output)
	{
		l_XmlElement->SetAttribute("MetaData", m_OutputMetaData.c_str());
	}
	l_XmlElement->SetAttribute("DataType", m_DataType.enumValueStr().c_str());
	l_XmlElement->SetAttribute("AccessType", m_PinAccessType.enumValueStr().c_str());
	i_ParentXmlElement->InsertEndChild(l_XmlElement);

	return l_XmlElement;
}

void BaseNodeInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{

}

tinyxml2::XMLElement* BaseNodeInfo::saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager) noexcept
{
	return nullptr;
}

void TemplateNodeInfo::loadData(tinyxml2::XMLElement* i_XmlElementRoot) noexcept
{
	{
		m_NodeName = i_XmlElementRoot->FindAttribute("NodeName")->Value();
		m_TemplateNodeName = m_NodeName;
		m_ClassName = i_XmlElementRoot->FindAttribute("ClassName")->Value();
		m_IsDynamicInputOutputPin = i_XmlElementRoot->FindAttribute("IsDynamicInputOutput")->BoolValue();
		m_IsDynamicInputPin = i_XmlElementRoot->FindAttribute("IsDynamicInput")->BoolValue();
		m_IsDynamicOutputPin = i_XmlElementRoot->FindAttribute("IsDynamicOutput")->BoolValue();
		m_IsDynamicParam = i_XmlElementRoot->FindAttribute("IsDynamicParam")->BoolValue();
	}

	tinyxml2::XMLElement* l_XmlElementNodeGraph = i_XmlElementRoot->FirstChildElement("NodeGraph");
	{
		m_NodeType.setEnumValue(l_XmlElementNodeGraph->FindAttribute("NodeType")->Value());
		m_Comment = l_XmlElementNodeGraph->FindAttribute("Comment")->Value();
	}

	tinyxml2::XMLElement* l_XmlElementInputOutputPinList = i_XmlElementRoot->FirstChildElement("InputOutput");
	if (l_XmlElementInputOutputPinList)
	{
		for (tinyxml2::XMLElement* l_XmlElementPin = l_XmlElementInputOutputPinList->FirstChildElement("Pin"); l_XmlElementPin != nullptr; l_XmlElementPin = l_XmlElementPin->NextSiblingElement("Pin"))
		{
			m_TemplateInputOutputPinInfos.push_back({});
			auto& l_PinInfo = m_TemplateInputOutputPinInfos[m_TemplateInputOutputPinInfos.size() - 1];
			l_PinInfo.setIsTemplate(true);
			l_PinInfo.setIsInputOutput(true);
			l_PinInfo.setPinTypeValue(PinType::InputOutput);
			l_PinInfo.loadData(l_XmlElementPin);
		}
	}

	tinyxml2::XMLElement* l_XmlElementOutPinList = i_XmlElementRoot->FirstChildElement("Output");
	if (l_XmlElementOutPinList)
	{
		for (tinyxml2::XMLElement* l_XmlElementPin = l_XmlElementOutPinList->FirstChildElement("Pin"); l_XmlElementPin != nullptr; l_XmlElementPin = l_XmlElementPin->NextSiblingElement("Pin"))
		{
			m_TemplateOutputPinInfos.push_back({});
			auto& l_PinInfo = m_TemplateOutputPinInfos[m_TemplateOutputPinInfos.size() - 1];
			l_PinInfo.setIsTemplate(true);
			l_PinInfo.setIsInputOutput(false);
			l_PinInfo.setPinTypeValue(PinType::Output);
			l_PinInfo.loadData(l_XmlElementPin);
		}
	}

	tinyxml2::XMLElement* l_XmlElementInputPinList = i_XmlElementRoot->FirstChildElement("Input");
	if (l_XmlElementInputPinList)
	{
		for (tinyxml2::XMLElement* l_XmlElementPin = l_XmlElementInputPinList->FirstChildElement("Pin"); l_XmlElementPin != nullptr; l_XmlElementPin = l_XmlElementPin->NextSiblingElement("Pin"))
		{
			m_TemplateInputPinInfos.push_back({});
			auto& l_PinInfo = m_TemplateInputPinInfos[m_TemplateInputPinInfos.size() - 1];
			l_PinInfo.loadData(l_XmlElementPin);
			l_PinInfo.setIsTemplate(true);
			l_PinInfo.setIsInputOutput(false);
			l_PinInfo.setPinTypeValue(PinType::Input);
		}
	}

	tinyxml2::XMLElement* l_XmlElementParamList = i_XmlElementRoot->FirstChildElement("ParamList");
	if (l_XmlElementParamList)
	{
		for (tinyxml2::XMLElement* l_XmlElementParam = l_XmlElementParamList->FirstChildElement("Param"); l_XmlElementParam != nullptr; l_XmlElementParam = l_XmlElementParam->NextSiblingElement("Param"))
		{
			m_TemplateParams.push_back({});
			m_TemplateParams[m_TemplateParams.size() - 1].loadData(l_XmlElementParam);
		}
	}
}

std::shared_ptr<NodeInfo> TemplateNodeInfo::instanceNodeInfo(NodeID i_ParentNodeID, NodeManager* i_pNodeManager, NodeID i_NodeID, Bool i_IsNewChildNode) noexcept
{
	NodeID l_NodeID = 0;
	if (i_NodeID == 0)
		l_NodeID = i_pNodeManager->getNodeID();
	else
		l_NodeID = i_NodeID;

	std::shared_ptr<NodeInfo> l_pNodeInfo{};
	if (m_NodeType == NodeType::GPU || m_NodeType == NodeType::CPU)
	{
		auto l_pTempNodeInfo = std::make_shared<NormalNodeInfo>(NormalNodeInfo(l_NodeID, i_ParentNodeID));
		l_pNodeInfo = std::dynamic_pointer_cast<NodeInfo>(l_pTempNodeInfo);
		
	}
	else if (m_NodeType == NodeType::Start || m_NodeType == NodeType::End)
	{
		auto l_pTempNodeInfo = std::make_shared<ChildNodeInfo>(ChildNodeInfo(l_NodeID, i_ParentNodeID));
		l_pNodeInfo = std::dynamic_pointer_cast<NodeInfo>(l_pTempNodeInfo);
	}
	else if (m_NodeType == NodeType::Composite)
	{
		auto l_pTempNodeInfo = std::make_shared<CompositeNodeInfo>(CompositeNodeInfo(l_NodeID, i_ParentNodeID));
		l_pNodeInfo = std::dynamic_pointer_cast<NodeInfo>(l_pTempNodeInfo);
	}
	else if (m_NodeType == NodeType::Task)
	{
		auto l_pTempNodeInfo = std::make_shared<TaskNodeInfo>(TaskNodeInfo(l_NodeID, i_ParentNodeID));
		l_pNodeInfo = std::dynamic_pointer_cast<NodeInfo>(l_pTempNodeInfo);
	}

	i_pNodeManager->nodeMap()[l_NodeID] = l_pNodeInfo;
	l_pNodeInfo->initData(m_TemplateNodeName, i_pNodeManager, i_IsNewChildNode);

	return l_pNodeInfo;
}

void FavoriteNodeInfo::loadData(tinyxml2::XMLElement* i_XmlElementRoot) noexcept
{ 
	if (!i_XmlElementRoot->FindAttribute("TemplateName")) return;

	auto loadPinList = [&i_XmlElementRoot](PinType i_PinType, std::vector<PinInfo>& i_PinInfoList) {
		std::string l_XmlElementKey{};
		if (i_PinType == PinType::InputOutput)
			l_XmlElementKey = "InputOutput";
		else if (i_PinType == PinType::Input)
			l_XmlElementKey = "Input";
		else if (i_PinType == PinType::Output)
			l_XmlElementKey = "Output";
		else
			return;


		auto l_XmlElementPinList = i_XmlElementRoot->FirstChildElement(l_XmlElementKey.c_str());
		for (tinyxml2::XMLElement* l_XmlElementPin = l_XmlElementPinList->FirstChildElement("Pin"); l_XmlElementPin != nullptr; l_XmlElementPin = l_XmlElementPin->NextSiblingElement("Pin"))
		{
			PinInfo l_PinInfo{};
			l_PinInfo.setIsTemplate(false);
			l_PinInfo.setIsInputOutput(i_PinType == PinType::InputOutput);
			l_PinInfo.setPinTypeValue(i_PinType);
			l_PinInfo.loadData(l_XmlElementPin);
			i_PinInfoList.push_back(std::move(l_PinInfo));
		}
	};

	auto loadParamList = [&i_XmlElementRoot](std::string_view i_ElementKey, std::vector<ParamInfo> & i_ParamInfoList) {
		auto l_XmlElementParamList = i_XmlElementRoot->FirstChildElement(i_ElementKey.data());
		for (tinyxml2::XMLElement* l_XmlElementParam = l_XmlElementParamList->FirstChildElement("Param"); l_XmlElementParam != nullptr; l_XmlElementParam = l_XmlElementParam->NextSiblingElement("Param"))
		{
			ParamInfo l_ParamInfo{};
			l_ParamInfo.loadData(l_XmlElementParam);
			i_ParamInfoList.push_back(std::move(l_ParamInfo));
		}
	};

	m_NodeName = i_XmlElementRoot->FindAttribute("NodeName")->Value();
	m_TemplateNodeName = i_XmlElementRoot->FindAttribute("TemplateName")->Value();
	auto l_XmlElementGraph = i_XmlElementRoot->FirstChildElement("NodeGraph");
	if (l_XmlElementGraph)
		m_Comment = l_XmlElementGraph->FindAttribute("Comment")->Value();

	loadPinList(PinType::InputOutput, m_InputOutputPinInfos);
	loadPinList(PinType::Input, m_InputPinInfos);
	loadPinList(PinType::Output, m_OutputPinInfos);
	loadParamList("ParamList", m_Params);
}

std::shared_ptr<NodeInfo> FavoriteNodeInfo::instanceNodeInfo(NodeID i_ParentNodeID, NodeManager* i_pNodeManager, NodeID i_NodeID, Bool i_IsNewChildNode) noexcept
{
	NodeID l_NodeID = 0;
	if (i_NodeID == 0)
		l_NodeID = i_pNodeManager->getNodeID();
	else
		l_NodeID = i_NodeID;

	auto l_pTemplateNodeInfo = i_pNodeManager->templateNodeInfoMap()[m_TemplateNodeName];
	if (!l_pTemplateNodeInfo) return nullptr;
	auto l_pNodeInfo = l_pTemplateNodeInfo->instanceNodeInfo(i_ParentNodeID, i_pNodeManager, i_NodeID, i_IsNewChildNode);
	if (!l_pNodeInfo) return nullptr;

	l_pNodeInfo->setNodeName(m_NodeName);
	l_pNodeInfo->setComment(m_Comment);
	for (auto& l_PinInfo : m_InputOutputPinInfos)
		l_pNodeInfo->addPinInfo(l_PinInfo, -1, i_pNodeManager, false);
	for (auto& l_PinInfo : m_InputPinInfos)
		l_pNodeInfo->addPinInfo(l_PinInfo, -1, i_pNodeManager, false);
	for (auto& l_PinInfo :m_OutputPinInfos)
		l_pNodeInfo->addPinInfo(l_PinInfo, -1, i_pNodeManager, false);
	for (const auto& i_ParamInfo : m_Params)
		l_pNodeInfo->addParam(i_ParamInfo, -1);

	return l_pNodeInfo;
}

void PinIDList::instanceTemplatePinList(const std::shared_ptr<TemplateNodeInfo> i_pTemplateNodeInfo, NodeManager* i_pNodeManager) noexcept
{
	for (auto& l_PinInfo : i_pTemplateNodeInfo->templateInputOutputPinInfos())
	{
		addPinInfo(l_PinInfo, -1, i_pNodeManager);
	}
}

void PinIDList::copyPinIDListData(NodeManager* i_pNodeManager, const PinIDList& i_PinIDList) noexcept
{
	auto copyPinInfo = [i_pNodeManager](PinID i_SrcPinID, NodeID i_NodeID) -> PinID {
		auto l_PinID = i_pNodeManager->getPinID();
		auto l_pPinInfo = i_pNodeManager->pinInfo(i_SrcPinID);
		if (l_pPinInfo)
		{
			auto l_pCurPinInfo = std::make_shared<PinInfo>(*l_pPinInfo);
			l_pCurPinInfo->setNodeID(i_NodeID);
			i_pNodeManager->pinMap()[l_PinID] = l_pCurPinInfo;
		}
		return l_PinID;
	};

	for (auto l_OutputPinID : i_PinIDList.inputOutputOutputPinIDs())
	{
		auto l_InputPinID = l_OutputPinID + 1;
		m_InputOutputOutputPinIDs.push_back(copyPinInfo(l_OutputPinID, m_NodeID));
		m_InputOutputInputPinIDs.push_back(copyPinInfo(l_InputPinID, m_NodeID));
	}

	for (auto l_PinID : i_PinIDList.outputPinIDs())
	{
		m_OutputPinIDs.push_back(copyPinInfo(l_PinID, m_NodeID));
	}

	for (auto l_PinID : i_PinIDList.inputPinIDs())
	{
		m_InputPinIDs.push_back(copyPinInfo(l_PinID, m_NodeID));
	}
}

void PinIDList::deleteInfo(NodeManager* i_pNodeManager) noexcept
{
	for (auto l_PinID : m_InputOutputOutputPinIDs)
		i_pNodeManager->pinMap().erase(l_PinID);
	for (auto l_PinID : m_InputOutputInputPinIDs)
		i_pNodeManager->pinMap().erase(l_PinID);
	for (auto l_PinID : m_InputPinIDs)
		i_pNodeManager->pinMap().erase(l_PinID);
	for (auto l_PinID : m_OutputPinIDs)
		i_pNodeManager->pinMap().erase(l_PinID);
}

//std::vector<PinID>& PinIDList::getPinIDList(PinType i_PinType, Bool i_IsInputOutput) noexcept
//{
//	if (i_IsInputOutput)
//	{
//		if (i_PinType == PinType::Output)
//			return m_InputOutputOutputPinIDs;
//		else if (i_PinType == PinType::Output)
//			return m_InputOutputInputPinIDs;
//	}
//	else
//	{
//		if (i_PinType == PinType::Input)
//			return m_OutputPinIDs;
//		else if (i_PinType == PinType::Output)
//			return m_InputPinIDs;
//	}
//	return {};
//}
//
//PinID PinIDList::getPinID(PinType i_PinType, Bool i_IsInputOutput, U32 i_PinIndex) noexcept
//{
//	auto& l_PinIDList = getPinIDList(i_PinType, i_IsInputOutput);
//	if (l_PinIDList.size() > i_PinIndex)a
//		return l_PinIDList[i_PinIndex];
//	return 0;
//}

PinID PinIDList::addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept
{
	PinInfo l_PinInfo{};
	l_PinInfo.setNodeID(m_NodeID);
	l_PinInfo.setName(i_PinNameStr.data());
	l_PinInfo.setDataTypeValue(EditorDataType::Int);
	l_PinInfo.setPinAccessTypeValue(PinAccessType::Readable);

	auto createPinInfo = [&l_PinInfo, i_pNodeManager](PinType i_PinType, std::vector<PinID>& i_PinIDs)->PinID {

		PinID l_PinID = i_pNodeManager->getPinID();
		l_PinInfo.setPinTypeValue(i_PinType);
		std::shared_ptr<PinInfo> l_pPinInfo = std::make_shared<PinInfo>(l_PinInfo);
		i_pNodeManager->pinMap()[l_PinID] = l_pPinInfo;
		i_PinIDs.emplace_back(l_PinID);
		return l_PinID;
	};


	if (i_PinType == PinType::InputOutput) {
		l_PinInfo.setIsInputOutput(true);

		PinID l_PinID = createPinInfo(PinType::Output, m_InputOutputOutputPinIDs);

		l_PinInfo.setPinAccessTypeValue(PinAccessType::Writable);
		createPinInfo(PinType::Input, m_InputOutputInputPinIDs);
		return l_PinID;
	}
	else if (i_PinType == PinType::Output) {
		return createPinInfo(PinType::Output, m_OutputPinIDs);
	}
	else if (i_PinType == PinType::Input) {
		return createPinInfo(PinType::Input, m_InputPinIDs);
	}

	return 0;
}

PinID PinIDList::addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	auto createPinInfo = [&i_pPinInfo, i_PinIndex, i_pNodeManager](PinType i_PinType, std::vector<PinID>& i_PinIDs)->PinID {
		PinID l_PinID = i_pNodeManager->getPinID();
		std::shared_ptr<PinInfo> l_pPinInfo = std::make_shared<PinInfo>(i_pPinInfo);
		l_pPinInfo->setPinTypeValue(i_PinType);
		i_pNodeManager->pinMap()[l_PinID] = l_pPinInfo;

		if (i_PinIndex >= i_PinIDs.size() || i_PinIndex < 0)
			i_PinIDs.emplace_back(l_PinID);
		else
			i_PinIDs.insert(i_PinIDs.begin() + i_PinIndex, l_PinID);
		return l_PinID;
	};

	if (!i_pNodeManager->nodeInfo(m_NodeID)) return 0;

	i_pPinInfo.setNodeID(m_NodeID);
	if (i_pPinInfo.getPinTypeValue() == PinType::InputOutput) {

		PinID l_OutputPinID = createPinInfo(PinType::Output, m_InputOutputOutputPinIDs);

		i_pPinInfo.setPinAccessTypeValue(PinAccessType::Writable);
		PinID l_InputPinID = createPinInfo(PinType::Input, m_InputOutputInputPinIDs);

		i_pNodeManager->nodeInfo(m_NodeID)->addNameKey(i_pPinInfo.getName(), { l_OutputPinID, l_InputPinID });
		return l_OutputPinID;
	}
	else if (i_pPinInfo.getPinTypeValue() == PinType::Output) {
		PinID l_PinID = createPinInfo(PinType::Output, m_OutputPinIDs);
		i_pNodeManager->nodeInfo(m_NodeID)->addNameKey(i_pPinInfo.getName(), { l_PinID, l_PinID });
		return l_PinID;

	}
	else if (i_pPinInfo.getPinTypeValue() == PinType::Input) {
		PinID l_PinID = createPinInfo(PinType::Input, m_InputPinIDs);
		i_pNodeManager->nodeInfo(m_NodeID)->addNameKey(i_pPinInfo.getName(), { l_PinID, l_PinID });
		return l_PinID;
	}

	return 0;
}

void PinIDList::delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	auto destroyPinInfo = [i_PinIndex, i_pNodeManager, *this](std::vector<PinID>& i_PinIDs) {
		if (i_PinIDs.size() > i_PinIndex)
		{
			auto l_PinID = i_PinIDs[i_PinIndex];
			auto l_pPinInfo = i_pNodeManager->pinInfo(l_PinID);
			i_pNodeManager->delInfoByLinkIDSet(l_pPinInfo->linkIDSet());
			i_PinIDs.erase(i_PinIDs.begin() + i_PinIndex);
			i_pNodeManager->pinMap().erase(l_PinID);

			if (i_pNodeManager->nodeInfo(m_NodeID) && l_pPinInfo->getIsInputOutput() && l_pPinInfo->getPinType() == PinType::Output)
				i_pNodeManager->nodeInfo(m_NodeID)->delNameKey(l_pPinInfo->getName());
		}
	};

	if (i_PinType == PinType::InputOutput) {
		destroyPinInfo(m_InputOutputOutputPinIDs);
		destroyPinInfo(m_InputOutputInputPinIDs);
	}
	else if (i_PinType == PinType::Output) {
		destroyPinInfo(m_OutputPinIDs);
	}
	else if (i_PinType == PinType::Input) {
		destroyPinInfo(m_InputPinIDs);
	}
}

void PinIDList::swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev) noexcept
{
	auto swapPinID_ = [i_PinIndex, i_IsSwapPrev](std::vector<PinID> i_PinIDList) {
		auto l_PinListSize = i_PinIDList.size();

		if (l_PinListSize > i_PinIndex)
		{
			S32 l_OtherPinIndex = i_PinIndex;
			if (i_IsSwapPrev)
				l_OtherPinIndex--;
			else
				l_OtherPinIndex++;

			if (l_OtherPinIndex < 0)
				return;
			if (l_OtherPinIndex >= l_PinListSize)
				return;

			std::swap(*(i_PinIDList.begin() + i_PinIndex), *(i_PinIDList.begin() + l_OtherPinIndex));
		}
	};

	if (i_PinType == PinType::InputOutput)
	{
		swapPinID_(m_InputOutputInputPinIDs);
		swapPinID_(m_InputOutputInputPinIDs);
	}
	else if (i_PinType == PinType::Output)
		swapPinID_(m_OutputPinIDs);
	else if (i_PinType == PinType::Input)
		swapPinID_(m_InputPinIDs);
}

void PinIDList::changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange) noexcept
{
	if (i_IsChange == PinInfoChangeFlags_None)
		return;

	auto changePinInfo_ = [i_PinIndex, i_pPinInfo, i_pNodeManager, i_IsChange](std::vector<PinID>& i_PinIDs) {
		if (i_PinIDs.size() > i_PinIndex)
		{
			auto l_PinID = i_PinIDs[i_PinIndex];
			auto l_pPinInfo = i_pNodeManager->pinInfo(l_PinID);
			if (!l_pPinInfo) return;

			if (i_IsChange & PinInfoChangeFlags_NameChange) {
				l_pPinInfo->setName(i_pPinInfo->getName());
			}
			if (i_IsChange & PinInfoChangeFlags_MetaChange) {
				l_pPinInfo->setMetaData(i_pPinInfo->getMetaData());
			}
			if (i_IsChange & PinInfoChangeFlags_DataType) {
				l_pPinInfo->setDataTypeValue(i_pPinInfo->getDataTypeValue());
			}
			if (i_IsChange & PinInfoChangeFlags_PinAccessType) {
				l_pPinInfo->setPinAccessTypeValue(i_pPinInfo->getPinAccessTypeValue());
			}
		}
	};

	if (i_PinType == PinType::InputOutput)
	{
		changePinInfo_(m_InputOutputOutputPinIDs);
		changePinInfo_(m_InputOutputInputPinIDs);
	}
	else if(i_PinType == PinType::Output)
		changePinInfo_(m_OutputPinIDs);
	else if (i_PinType == PinType::Input)
		changePinInfo_(m_InputPinIDs);
}

std::string PinIDList::getIndexListStr(PinType i_PinType) noexcept
{
	std::string l_IndexListStr = "-1";
	auto getIndexListStr_ = [&l_IndexListStr](std::vector<PinID>& i_PinIDs) {
		for (U32 i = 0; i < i_PinIDs.size(); ++i)
		{
			l_IndexListStr += '\0' + std::to_string(i);
		}
	};

	if (i_PinType == PinType::InputOutput) {
		getIndexListStr_(m_InputOutputOutputPinIDs);
	}
	else if (i_PinType == PinType::Output) {
		getIndexListStr_(m_OutputPinIDs);
	}
	else if (i_PinType == PinType::Input) {
		getIndexListStr_(m_InputPinIDs);
	}

	return l_IndexListStr;
}

std::map<std::string, std::pair<PinID, PinID>> PinIDList::getPinKeyMap(NodeManager* i_pNodeManager) noexcept
{
	std::map<std::string, std::pair<PinID, PinID>> l_PinNameMap{};

	auto _getInputOutputPinKeyMap = [&i_pNodeManager, &l_PinNameMap](std::vector<PinID> i_OutputPinIDList, std::vector<PinID> i_InputPinIDList) {
		if (i_OutputPinIDList.size() != i_InputPinIDList.size()) return;
		for (auto i = 0; i < i_OutputPinIDList.size(); ++i) {
			auto l_OutputPinID = i_OutputPinIDList[i];
			auto l_InputPinID = i_InputPinIDList[i];

			auto l_pOutputPinInfo = i_pNodeManager->pinInfo(l_OutputPinID);
			auto l_pInputPinInfo = i_pNodeManager->pinInfo(l_InputPinID);
			if (l_pOutputPinInfo && l_pInputPinInfo)
				l_PinNameMap[l_pOutputPinInfo->getName()] = { l_OutputPinID, l_InputPinID };
		}
	};

	auto _getPinKeyMap = [&i_pNodeManager, &l_PinNameMap](std::vector<PinID> i_PinIDList) {
		for (auto l_PinID : i_PinIDList) {
			auto l_pPinInfo = i_pNodeManager->pinInfo(l_PinID);
			if (l_pPinInfo)
				l_PinNameMap[l_pPinInfo->getName()] = { l_PinID , l_PinID };
		}
	};

	_getInputOutputPinKeyMap(m_InputOutputOutputPinIDs, m_InputOutputInputPinIDs);
	_getPinKeyMap(m_InputPinIDs);
	_getPinKeyMap(m_OutputPinIDs);

	return l_PinNameMap;
}

void PinIDList::loadData(tinyxml2::XMLElement * i_XmlElement, NodeManager * i_pNodeManager) noexcept
{
	auto _loadData = [&i_XmlElement, i_pNodeManager, this](std::string_view i_XmlKey) {
		PinType l_PinType{};
		if (i_XmlKey == "InputOutput")
			l_PinType = PinType::InputOutput;
		else if(i_XmlKey == "Input")
			l_PinType = PinType::Input;
		else if (i_XmlKey == "Output")
			l_PinType = PinType::Output;

		auto l_pNodeInfo = i_pNodeManager->nodeInfo(m_NodeID);
		if (!l_pNodeInfo) return;

		tinyxml2::XMLElement* l_XmlElementPinList = i_XmlElement->FirstChildElement(i_XmlKey.data());
		for (auto l_XxmlElementPin = l_XmlElementPinList->FirstChildElement("Pin"); l_XxmlElementPin != nullptr; l_XxmlElementPin = l_XxmlElementPin->NextSiblingElement("Pin"))
		{
			PinInfo l_Info{};
			l_Info.setNodeID(m_NodeID);
			l_Info.setPinTypeValue(l_PinType);
			l_Info.loadData(l_XxmlElementPin, i_pNodeManager);

			if (i_XmlKey == "InputOutput") {
				l_Info.setIsInputOutput(true);
			}
			else {
				l_Info.setIsInputOutput(false);
			}
			l_pNodeInfo->addPinInfo(l_Info, -1, i_pNodeManager);
		}
	};

	_loadData("InputOutput");
	_loadData("Input");
	_loadData("Output");
}

tinyxml2::XMLElement* PinIDList::saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager) noexcept
{
	auto saveInputOutputPinInfo = [&i_XmlDocument, &i_ParentXmlElement, &i_pNodeManager](const std::vector<rceditor::PinID>& i_OutputPinIDList, const std::vector<rceditor::PinID>& i_InputPinIDList) -> void {
		if (i_OutputPinIDList.size() != i_InputPinIDList.size()) return;

		tinyxml2::XMLElement* l_XMLPinListElement = i_XmlDocument->NewElement("InputOutput");
		for (U32 l_Index = 0; l_Index < i_OutputPinIDList.size(); ++l_Index)
		{
			auto l_OutputPinID = i_OutputPinIDList[l_Index];
			auto l_InputPinID = i_InputPinIDList[l_Index];

			const auto& l_pOutputPinInfo = i_pNodeManager->pinInfo(l_OutputPinID);
			const auto& l_pInputPinInfo = i_pNodeManager->pinInfo(l_InputPinID);
			if (!l_pOutputPinInfo || !l_pInputPinInfo) continue;

			tinyxml2::XMLElement* l_XmlPinElement = i_XmlDocument->NewElement("Pin");
			l_XmlPinElement->SetAttribute("Name", l_pOutputPinInfo->m_Name.c_str());
			l_XmlPinElement->SetAttribute("InputMetaData", l_pInputPinInfo->m_InputMetaData.c_str());
			l_XmlPinElement->SetAttribute("OutputMetaData", l_pOutputPinInfo->m_OutputMetaData.c_str());
			l_XmlPinElement->SetAttribute("DataType", l_pOutputPinInfo->m_DataType.enumValueStr().c_str());
			l_XmlPinElement->SetAttribute("AccessType", l_pOutputPinInfo->m_PinAccessType.enumValueStr().c_str());
			l_XMLPinListElement->InsertEndChild(l_XmlPinElement);
		}

		i_ParentXmlElement->InsertEndChild(l_XMLPinListElement);
	};

	auto savePinInfo = [&i_XmlDocument, &i_ParentXmlElement, &i_pNodeManager](const std::string& i_ElementStr, const std::vector<rceditor::PinID>& i_PinIDList) -> void {
		tinyxml2::XMLElement* l_XMLPinListElement = i_XmlDocument->NewElement(i_ElementStr.c_str());
		for (const auto& l_PinID : i_PinIDList)
		{
			const auto& l_pPinInfo = i_pNodeManager->pinInfo(l_PinID);
			if (!l_pPinInfo) continue;

			tinyxml2::XMLElement* l_XmlPinElement = i_XmlDocument->NewElement("Pin");
			l_XmlPinElement->SetAttribute("Name", l_pPinInfo->m_Name.c_str());
			if (l_pPinInfo->getPinType() == PinType::Input)
				l_XmlPinElement->SetAttribute("MetaData", l_pPinInfo->m_InputMetaData.c_str());
			else
				l_XmlPinElement->SetAttribute("MetaData", l_pPinInfo->m_OutputMetaData.c_str());
			l_XmlPinElement->SetAttribute("DataType", l_pPinInfo->m_DataType.enumValueStr().c_str());
			l_XmlPinElement->SetAttribute("AccessType", l_pPinInfo->m_PinAccessType.enumValueStr().c_str());
			l_XMLPinListElement->InsertEndChild(l_XmlPinElement);
		}

		i_ParentXmlElement->InsertEndChild(l_XMLPinListElement);
	};


	saveInputOutputPinInfo(m_InputOutputOutputPinIDs, m_InputOutputInputPinIDs);
	savePinInfo("Input", m_InputPinIDs);
	savePinInfo("Output", m_OutputPinIDs);

	return i_ParentXmlElement;
}

NodeInfo::NodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID) noexcept
{
	setNodeID(i_NodeID);
	setParentNodeID(i_ParentNodeID);
}
 
NodeInfo::NodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager) noexcept : NodeInfo(i_NodeID, i_ParentNodeID)
{
	auto i_pTemplateNodeInfo = i_pNodeManager->templateNodeInfoMap().at(i_TemplateNodeKey);
	m_NodeName = i_pTemplateNodeInfo->getNodeName();
	m_TemplateNodeName = i_pTemplateNodeInfo->getTemplateNodeName();
	m_NodeType = i_pTemplateNodeInfo->getNodeType();
	m_ClassName = i_pTemplateNodeInfo->getClassName();

	m_IsDynamicInputOutputPin = i_pTemplateNodeInfo->getIsDynamicInputOutputPin();
	m_IsDynamicInputPin = i_pTemplateNodeInfo->getIsDynamicInputPin();
	m_IsDynamicOutputPin = i_pTemplateNodeInfo->getIsDynamicOutputPin();
	m_IsDynamicParam = i_pTemplateNodeInfo->getIsDynamicParam();

	for (auto& l_PinInfo : i_pTemplateNodeInfo->templateInputOutputPinInfos())
		addPinInfo(l_PinInfo, -1, i_pNodeManager);
	for (const auto& i_ParamInfo : i_pTemplateNodeInfo->templateParams())
		m_TemplateParams.push_back(i_ParamInfo);
}

void NodeInfo::deleteInfo(NodeManager* i_pManager) noexcept
{
	i_pManager->delInfoByLinkIDSet(m_LinkIDSet);
	m_TemplatePinIDList.deleteInfo(i_pManager);
	m_PinIDList.deleteInfo(i_pManager);
}

void NodeInfo::initData(std::string_view i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsInitChildNode) noexcept
{
	auto i_pTemplateNodeInfo = i_pNodeManager->templateNodeInfoMap().at(i_TemplateNodeKey.data());
	m_NodeName = i_pTemplateNodeInfo->getNodeName();
	m_TemplateNodeName = i_pTemplateNodeInfo->getTemplateNodeName();
	m_NodeType = i_pTemplateNodeInfo->getNodeType();
	m_ClassName = i_pTemplateNodeInfo->getClassName();

	m_IsDynamicInputOutputPin = i_pTemplateNodeInfo->getIsDynamicInputOutputPin();
	m_IsDynamicInputPin = i_pTemplateNodeInfo->getIsDynamicInputPin();
	m_IsDynamicOutputPin = i_pTemplateNodeInfo->getIsDynamicOutputPin();
	m_IsDynamicParam = i_pTemplateNodeInfo->getIsDynamicParam();

	for (auto& l_PinInfo : i_pTemplateNodeInfo->templateInputOutputPinInfos())
		addPinInfo(l_PinInfo, -1, i_pNodeManager, true);
	for (auto& l_PinInfo : i_pTemplateNodeInfo->templateInputPinInfos())
		addPinInfo(l_PinInfo, -1, i_pNodeManager, true);
	for (auto& l_PinInfo : i_pTemplateNodeInfo->templateOutputPinInfos())
		addPinInfo(l_PinInfo, -1, i_pNodeManager, true);
	for (const auto& i_ParamInfo : i_pTemplateNodeInfo->templateParams())
		m_TemplateParams.push_back(i_ParamInfo);
}

PinID NodeInfo::addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept
{
	return m_PinIDList.addPinInfo(i_PinType, i_PinNameStr, i_pNodeManager);
}

PinID NodeInfo::addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager, Bool i_IsTemplatePinIDList) noexcept
{
	if (!i_IsTemplatePinIDList)
		return m_PinIDList.addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
	else
		return m_TemplatePinIDList.addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
}

void NodeInfo::delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	return m_PinIDList.delPinInfo(i_PinType, i_PinIndex, i_pNodeManager);
}

void NodeInfo::swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev, NodeManager* i_pNodeManager) noexcept
{
	return m_PinIDList.swapPinID(i_PinType, i_PinIndex, i_IsSwapPrev);
}


void NodeInfo::changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange) noexcept
{
	//return m_PinIDList.changePinInfo(i_PinType, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);
}

void NodeInfo::addParam() noexcept
{
	m_Params.emplace_back();
}

void NodeInfo::addParam(const ParamInfo& i_ParamInfo, S32 i_ParamIndex) noexcept
{
	addNameKey(i_ParamInfo.m_Name, {});
	if (i_ParamIndex >= m_Params.size() || i_ParamIndex < 0)
		m_Params.emplace_back(std::move(i_ParamInfo));
	else
		m_Params.insert(m_Params.begin() + i_ParamIndex, std::move(i_ParamInfo));
}

void NodeInfo::delParam(U32 i_ParamIndex) noexcept
{
	auto l_It = m_Params.begin() + i_ParamIndex;
	delNameKey(l_It->m_Name);
	if (m_Params.size() > i_ParamIndex)
		m_Params.erase(l_It);
}

bool NodeInfo::canChangePinAccessType(PinType i_PinType) noexcept
{
	if (i_PinType == PinType::InputOutput || i_PinType == PinType::Output)
		return true;
	else
		return false;
}

void NodeInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept 
{
	auto l_XmlElementNodeGraph = i_XmlElement->FirstChildElement("NodeGraph");

	m_NodeName = l_XmlElementNodeGraph->FindAttribute("NodeName")->Value();
	m_Comment = l_XmlElementNodeGraph->FindAttribute("Comment")->Value();
	m_PinIDList.loadData(i_XmlElement, i_pNodeManager);
	//m_PinKeyMap = m_PinIDList.getPinKeyMap(i_pNodeManager);
	
	auto l_XmlElementNodeParamList = i_XmlElement->FirstChildElement("ParamList");
	for (auto l_XmlElementParam = l_XmlElementNodeParamList->FirstChildElement("Param"); l_XmlElementParam != nullptr; l_XmlElementParam = l_XmlElementParam->NextSiblingElement("Param"))
	{
		ParamInfo i_ParamInfo{};
		i_ParamInfo.loadData(l_XmlElementParam, i_pNodeManager);
		m_ParamKeySet.insert(i_ParamInfo.m_Name);
		m_Params.push_back(std::move(i_ParamInfo));
	}
}

tinyxml2::XMLElement* NodeInfo::saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager) noexcept
{
	tinyxml2::XMLElement* l_XmlNode = i_XmlDocument->NewElement("Node");
	l_XmlNode->SetAttribute("NodeID", m_NodeID);
	l_XmlNode->SetAttribute("ParentNodeID", m_ParentNodeID);
	l_XmlNode->SetAttribute("TemplateName", m_TemplateNodeName.c_str());

	//Save Graph
	tinyxml2::XMLElement* l_XmlNodeGraph = i_XmlDocument->NewElement("NodeGraph");
	l_XmlNodeGraph->SetAttribute("NodeName", m_NodeName.c_str());
	l_XmlNodeGraph->SetAttribute("Comment", m_Comment.c_str());
	l_XmlNode->InsertEndChild(l_XmlNodeGraph);

	//Save Pin List
	m_PinIDList.saveData(i_XmlDocument, l_XmlNode, i_pNodeManager);

	//Save Param Info
	tinyxml2::XMLElement* l_XmlParamList = i_XmlDocument->NewElement("ParamList");
	for (auto l_ParamInfo : m_Params)
	{
		l_ParamInfo.saveData(i_XmlDocument, l_XmlParamList);
	}
	l_XmlNode->InsertEndChild(l_XmlParamList);

	i_ParentXmlElement->InsertEndChild(l_XmlNode);
	return l_XmlNodeGraph;
}

void NodeInfo::saveFavoriteData(std::string_view i_FilePath, NodeManager* i_pNodeManager) noexcept
{
	std::string l_FilePath = i_FilePath.data();
	S32 l_PosLeft = l_FilePath.rfind("\\", i_FilePath.length());
	S32 l_PosRight = l_FilePath.rfind(".xml", i_FilePath.length());
	std::string l_FilenameStr = l_FilePath.substr(l_PosLeft + 1, l_PosRight - l_PosLeft - 1);

	auto copyPinList = [&](const std::vector<PinID>& i_SrcPinIDList, std::vector<PinInfo>& i_DstPinInfoList, PinType i_PinType) {
		for (auto l_PinID : i_SrcPinIDList)
		{
			std::shared_ptr<PinInfo> l_pPinInfo = i_pNodeManager->pinInfo(l_PinID);
			assert(l_pPinInfo);

			PinInfo l_CurPinInfo = *l_pPinInfo;
			l_CurPinInfo.m_NodeID = {};
			l_CurPinInfo.m_PinID = {};
			l_CurPinInfo.setPinTypeValue(i_PinType);
			i_DstPinInfoList.push_back(std::move(l_CurPinInfo));
		}
	};

	//Dir Key
	std::string l_DirStr = l_FilePath.substr(0, l_PosLeft);
	S32 l_PosDir = l_DirStr.rfind("\\", l_DirStr.length());
	std::string l_PathKeyStr = l_DirStr.substr(l_PosDir + 1, l_DirStr.length() - l_PosDir) + "##" + l_DirStr.substr(0, l_PosDir);

	//Get FavoriteNodeInfo
	{
		FavoriteNodeInfo l_NewFavoriteNodeInfo{};

		l_NewFavoriteNodeInfo.setNodeName(l_FilenameStr);
		l_NewFavoriteNodeInfo.setTemplateNodeName(m_TemplateNodeName);
		l_NewFavoriteNodeInfo.setComment(m_Comment);

		copyPinList(m_PinIDList.inputOutputOutputPinIDs(), l_NewFavoriteNodeInfo.inputOutputPinInfos(), PinType::InputOutput);
		copyPinList(m_PinIDList.inputPinIDs(), l_NewFavoriteNodeInfo.inputPinInfos(), PinType::Input);
		copyPinList(m_PinIDList.outputPinIDs(), l_NewFavoriteNodeInfo.outputPinInfos(), PinType::Output);

		std::vector<ParamInfo> l_ParamInfoList{};
		for(const auto& l_Params : m_Params)
		{
			l_ParamInfoList.push_back(l_Params);
		}
		l_NewFavoriteNodeInfo.params() = std::move(l_ParamInfoList);

		i_pNodeManager->templateNodeFileMap()[l_PathKeyStr].emplace_back(TemplateFileType::Template, l_FilenameStr);

		auto l_pFavoriteNodeInfo = std::make_shared<FavoriteNodeInfo>(l_NewFavoriteNodeInfo);
		i_pNodeManager->registerTemplateNode(std::dynamic_pointer_cast<TemplateNodeInfo>(l_pFavoriteNodeInfo));
	}

	//Save FavoriteNodeInfo
	{
		tinyxml2::XMLDocument l_XMLDocument{};
		tinyxml2::XMLElement* l_XmlNode = l_XMLDocument.NewElement("Node");
		l_XmlNode->SetAttribute("NodeName", l_FilenameStr.c_str());
		l_XmlNode->SetAttribute("TemplateName", m_TemplateNodeName.c_str());

		tinyxml2::XMLElement* l_XmlNodeGraph = l_XMLDocument.NewElement("NodeGraph");
		l_XmlNodeGraph->SetAttribute("Comment", m_Comment.c_str());
		l_XmlNode->InsertEndChild(l_XmlNodeGraph);

		//Save Pin List
		m_PinIDList.saveData(&l_XMLDocument, l_XmlNode, i_pNodeManager);

		//Save Param Info
		tinyxml2::XMLElement* l_XmlParamList = l_XMLDocument.NewElement("ParamList");
		for (auto l_ParamInfo : m_Params)
		{
			l_ParamInfo.saveData(&l_XMLDocument, l_XmlParamList);
		}
		l_XmlNode->InsertEndChild(l_XmlParamList);
	
		l_XMLDocument.InsertEndChild(l_XmlNode);

		l_XMLDocument.SaveFile(l_FilePath.c_str());
	}
}

NormalNodeInfo::NormalNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID) noexcept : NodeInfo(i_NodeID, i_ParentNodeID)
{

}

NormalNodeInfo::NormalNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager) noexcept : NodeInfo(i_NodeID, i_ParentNodeID, i_TemplateNodeKey, i_pNodeManager)
{

}

ChildNodeInfo::ChildNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID) noexcept : NodeInfo(i_NodeID, i_ParentNodeID)
{
	if (m_NodeType == NodeType::Start)
		m_IsDynamicOutputPin = true;
	else if (m_NodeType == NodeType::End)
		m_IsDynamicInputPin = true;
}

ChildNodeInfo::ChildNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager) noexcept : NodeInfo(i_NodeID, i_ParentNodeID, i_TemplateNodeKey, i_pNodeManager)
{
	if (m_NodeType == NodeType::Start)
		m_IsDynamicOutputPin = true;
	else if (m_NodeType == NodeType::End)
		m_IsDynamicInputPin = true;
}

void ChildNodeInfo::initPinIDList(const std::vector<PinID>& i_PinIDList, NodeManager* i_pNodeManager) noexcept
{
	std::vector<PinID> l_CurNodePinIDList = m_PinIDList.outputPinIDs();
	PinType l_CurPinType = PinType::Output;

	if (m_NodeType == NodeType::End)
	{
		l_CurNodePinIDList = m_PinIDList.inputPinIDs();
		l_CurPinType = PinType::Input;
	}
	else
		return;

	auto copyStartNodePinInfo = [i_pNodeManager, &l_CurNodePinIDList, &l_CurPinType](PinID i_SrcPinID, NodeID i_NodeID) {
		auto l_PinID = i_pNodeManager->getPinID();
		auto l_pPinInfo = i_pNodeManager->pinInfo(i_SrcPinID);
		if (l_pPinInfo)
		{
			auto l_pCurPinInfo = std::make_shared<PinInfo>(*l_pPinInfo);
			l_pCurPinInfo->setNodeID(i_NodeID);
			l_pCurPinInfo->setPinTypeValue(l_CurPinType);
			i_pNodeManager->pinMap()[l_PinID] = l_pCurPinInfo;
			
			l_CurNodePinIDList.push_back(i_SrcPinID);
		}
	};

	for (auto l_PinID : i_PinIDList)
	{
		copyStartNodePinInfo(l_PinID, m_NodeID);
	}
}

PinID ChildNodeInfo::addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept
{
	PinID l_PinID = NodeInfo::addPinInfo(i_PinType, i_PinNameStr, i_pNodeManager);

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return 0;
	if (m_NodeType == NodeType::Start)
		l_pParentNodeInfo->pinIDList().addPinInfo(PinType::Input, i_PinNameStr, i_pNodeManager);
	else if (m_NodeType == NodeType::End)
		l_pParentNodeInfo->pinIDList().addPinInfo(PinType::Output, i_PinNameStr, i_pNodeManager);

	return l_PinID;
}

PinID ChildNodeInfo::addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager, Bool i_IsTemplatePinIDList) noexcept
{
	PinID l_PinID = NodeInfo::addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return 0;

	auto l_PinIDList = l_pParentNodeInfo->pinIDList();
	if(i_IsTemplatePinIDList)
		l_PinIDList = l_pParentNodeInfo->templatePinIDList();
	if (m_NodeType == NodeType::Start)
	{
		i_pPinInfo.setPinTypeValue(PinType::Input);
		l_PinIDList.addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
	}
	else if (m_NodeType == NodeType::End)
	{
		i_pPinInfo.setPinTypeValue(PinType::Output);
		l_PinIDList.addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
	}

	return l_PinID;
}

void ChildNodeInfo::delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::delPinInfo(i_PinType, i_PinIndex, i_pNodeManager);

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return;
	if (m_NodeType == NodeType::Start)
		l_pParentNodeInfo->pinIDList().delPinInfo(PinType::Input, i_PinIndex, i_pNodeManager);
	else if (m_NodeType == NodeType::End)
		l_pParentNodeInfo->pinIDList().delPinInfo(PinType::Output, i_PinIndex, i_pNodeManager);
}

void ChildNodeInfo::swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::swapPinID(i_PinType, i_PinIndex, i_IsSwapPrev, i_pNodeManager);

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return;
	if (m_NodeType == NodeType::Start)
		l_pParentNodeInfo->pinIDList().swapPinID(PinType::Input, i_PinIndex, i_IsSwapPrev);
	else if (m_NodeType == NodeType::End)
		l_pParentNodeInfo->pinIDList().swapPinID(PinType::Output, i_PinIndex, i_IsSwapPrev);
}

void ChildNodeInfo::changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange) noexcept
{
	if (i_IsChange == PinInfoChangeFlags_None) return;

	NodeInfo::changePinInfo(i_PinType, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return;
	if (m_NodeType == NodeType::Start)
		l_pParentNodeInfo->pinIDList().changePinInfo(PinType::Input, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);
	else if (m_NodeType == NodeType::End)
		l_pParentNodeInfo->pinIDList().changePinInfo(PinType::Output, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);
}

bool ChildNodeInfo::canChangePinAccessType(PinType i_PinType) noexcept {
	return true; 
};

void ChildNodeInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	m_NodeName = i_XmlElement->FindAttribute("NodeName")->Value();
	m_Comment = i_XmlElement->FindAttribute("Comment")->Value();

	auto l_pParentNodeInfo = i_pNodeManager->nodeInfo(m_ParentNodeID);
	if (!l_pParentNodeInfo) return;

	//if (m_NodeType == NodeType::Start)
	//{
	//	for (auto l_PinID : l_pParentNodeInfo->pinIDList().inputPinIDs())
	//	{
	//		auto l_pPinInfo = i_pNodeManager->pinInfo(l_PinID);
	//		if (l_pPinInfo)
	//		{
	//			auto l_PinInfo = *l_pPinInfo;
	//			l_PinInfo.setPinTypeValue(PinType::Output);
	//			addPinInfo(l_PinInfo, -1, i_pNodeManager);
	//		}
	//	}
	//}
	//else if (m_NodeType == NodeType::End)
	//{
	//
	//	for (auto l_PinID : l_pParentNodeInfo->pinIDList().outputPinIDs())
	//	{
	//		auto l_pPinInfo = i_pNodeManager->pinInfo(l_PinID);
	//		if (l_pPinInfo)
	//		{
	//			auto l_PinInfo = *l_pPinInfo;
	//			l_PinInfo.setPinTypeValue(PinType::Input);
	//			addPinInfo(l_PinInfo, -1, i_pNodeManager);
	//		}
	//	}
	//}

	//m_PinKeyMap = m_PinIDList.getPinKeyMap(i_pNodeManager);
}

tinyxml2::XMLElement* ChildNodeInfo::saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager) noexcept
{
	tinyxml2::XMLElement* l_XmlNode{};
	if(m_NodeType == NodeType::Start)
		l_XmlNode = i_XmlDocument->NewElement("StartNode");
	else if (m_NodeType == NodeType::End)
		l_XmlNode = i_XmlDocument->NewElement("EndNode");
	
	if (!l_XmlNode) return l_XmlNode;
	l_XmlNode->SetAttribute("NodeID", m_NodeID);
	l_XmlNode->SetAttribute("NodeName", m_NodeName.c_str());
	l_XmlNode->SetAttribute("TemplateName", m_TemplateNodeName.c_str());
	l_XmlNode->SetAttribute("Comment", m_Comment.c_str());
	i_ParentXmlElement->InsertEndChild(l_XmlNode);

	return l_XmlNode;
}

CompositeNodeInfo::CompositeNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID) noexcept : NodeInfo(i_NodeID, i_ParentNodeID)
{
	m_IsDynamicInputOutputPin = false;
	m_IsDynamicParam = false;
}

CompositeNodeInfo::CompositeNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsNewChildNode) noexcept : NodeInfo(i_NodeID, i_ParentNodeID, i_TemplateNodeKey, i_pNodeManager)
{
	m_IsDynamicInputOutputPin = false;
	m_IsDynamicParam = false;

	if (i_IsNewChildNode)
	{
		auto l_pStartChildNodeInfo = i_pNodeManager->instanceNodeInfo("Start", i_NodeID);
		m_StartChildNodeID = l_pStartChildNodeInfo->getNodeID();

		auto l_pEndChildNodeInfo = i_pNodeManager->instanceNodeInfo("End", i_NodeID);
		m_EndChildNodeID = l_pEndChildNodeInfo->getNodeID();
	}
}

void CompositeNodeInfo::initData(std::string_view i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsInitChildNode) noexcept
{
	NodeInfo::initData(i_TemplateNodeKey, i_pNodeManager, i_IsInitChildNode);
	m_IsDynamicInputOutputPin = false;
	m_IsDynamicParam = false;

	if (i_IsInitChildNode)
	{
		auto l_pStartChildNodeInfo = i_pNodeManager->instanceNodeInfo("Start", m_NodeID);
		m_StartChildNodeID = l_pStartChildNodeInfo->getNodeID();

		auto l_pEndChildNodeInfo = i_pNodeManager->instanceNodeInfo("End", m_NodeID);
		m_EndChildNodeID = l_pEndChildNodeInfo->getNodeID();
	}
}

PinID CompositeNodeInfo::addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept
{
	PinID l_PinID = NodeInfo::addPinInfo(i_PinType, i_PinNameStr, i_pNodeManager);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (l_pStartNodeInfo && i_PinType == PinType::Input)
		l_pStartNodeInfo->pinIDList().addPinInfo(PinType::Output, i_PinNameStr, i_pNodeManager);
	if (l_pEndNodeInfo && i_PinType == PinType::Output)
		l_pEndNodeInfo->pinIDList().addPinInfo(PinType::Input, i_PinNameStr, i_pNodeManager);

	return l_PinID;
}

PinID CompositeNodeInfo::addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager, Bool i_IsTemplatePinIDList) noexcept
{
	PinID l_PinID = NodeInfo::addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager, i_IsTemplatePinIDList);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);

	if (l_pStartNodeInfo && i_pPinInfo.getPinType() == PinType::Input)
	{
		i_pPinInfo.setPinTypeValue(PinType::Output);
		if (!i_IsTemplatePinIDList)
			l_pStartNodeInfo->pinIDList().addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
		else
			l_pStartNodeInfo->templatePinIDList().addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
		i_pPinInfo.setPinTypeValue(PinType::Input);
	}
	if (l_pEndNodeInfo && i_pPinInfo.getPinType() == PinType::Output)
	{
		i_pPinInfo.setPinTypeValue(PinType::Input);
		if (!i_IsTemplatePinIDList)
			l_pEndNodeInfo->pinIDList().addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
		else
			l_pEndNodeInfo->templatePinIDList().addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
		i_pPinInfo.setPinTypeValue(PinType::Output);
	}

	return l_PinID;
}

void CompositeNodeInfo::delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::delPinInfo(i_PinType, i_PinIndex, i_pNodeManager);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (l_pStartNodeInfo && i_PinType == PinType::Input)
		l_pStartNodeInfo->pinIDList().delPinInfo(PinType::Output, i_PinIndex, i_pNodeManager);
	if (l_pEndNodeInfo && i_PinType == PinType::Output)
		l_pEndNodeInfo->pinIDList().delPinInfo(PinType::Input, i_PinIndex, i_pNodeManager);
}

void CompositeNodeInfo::swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::swapPinID(i_PinType, i_PinIndex, i_IsSwapPrev, i_pNodeManager);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (l_pStartNodeInfo && i_PinType == PinType::Input)
		l_pStartNodeInfo->pinIDList().swapPinID(PinType::Output, i_PinIndex, i_IsSwapPrev);
	if (l_pEndNodeInfo && i_PinType == PinType::Output)
		l_pEndNodeInfo->pinIDList().swapPinID(PinType::Input, i_PinIndex, i_IsSwapPrev);
}

void CompositeNodeInfo::changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange) noexcept
{
	NodeInfo::changePinInfo(i_PinType, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (l_pStartNodeInfo && i_PinType == PinType::Input)
		l_pStartNodeInfo->pinIDList().changePinInfo(PinType::Output, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);
	if (l_pEndNodeInfo && i_PinType == PinType::Output)
		l_pEndNodeInfo->pinIDList().changePinInfo(PinType::Input, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);
}

bool CompositeNodeInfo::canChangePinAccessType(PinType i_PinType) noexcept {
	return true;
};

void CompositeNodeInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	auto l_XmlNodeGraph = i_XmlElement->FirstChildElement("NodeGraph");
	if (!l_XmlNodeGraph) return;
	auto l_XmlStartNode = l_XmlNodeGraph->FirstChildElement("StartNode");
	auto l_XmlEndNode = l_XmlNodeGraph->FirstChildElement("EndNode");
	if (!(l_XmlStartNode && l_XmlEndNode)) return;
	NodeID l_StartNodeID = l_XmlStartNode->FindAttribute("NodeID")->Int64Value();
	NodeID l_EndNodeID = l_XmlEndNode->FindAttribute("NodeID")->Int64Value();

	auto l_pStartChildNodeInfo = i_pNodeManager->instanceNodeInfo("Start", m_NodeID, l_StartNodeID);
	auto l_pEndChildNodeInfo = i_pNodeManager->instanceNodeInfo("End", m_NodeID, l_EndNodeID);
	if (!(l_pStartChildNodeInfo && l_pEndChildNodeInfo)) return;
	m_StartChildNodeID = l_StartNodeID;
	m_EndChildNodeID = l_EndNodeID;

	NodeInfo::loadData(i_XmlElement, i_pNodeManager);
	l_pStartChildNodeInfo->loadData(l_XmlStartNode, i_pNodeManager);
	l_pEndChildNodeInfo->loadData(l_XmlEndNode, i_pNodeManager);
}

tinyxml2::XMLElement* CompositeNodeInfo::saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager) noexcept
{
	tinyxml2::XMLElement* l_XmlGraphNode = NodeInfo::saveData(i_XmlDocument, i_ParentXmlElement, i_pNodeManager);
	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (!l_pStartNodeInfo && !l_pEndNodeInfo) return nullptr;

	l_pStartNodeInfo->saveData(i_XmlDocument, l_XmlGraphNode, i_pNodeManager);
	l_pEndNodeInfo->saveData(i_XmlDocument, l_XmlGraphNode, i_pNodeManager);

	return l_XmlGraphNode;
}

TaskNodeInfo::TaskNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID) noexcept : NodeInfo(i_NodeID, i_ParentNodeID)
{
	m_IsDynamicInputOutputPin = false;
}

TaskNodeInfo::TaskNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsNewChildNode) noexcept : NodeInfo(i_NodeID, i_ParentNodeID, i_TemplateNodeKey, i_pNodeManager)
{
	m_IsDynamicInputOutputPin = false;

	m_Params = std::move(m_TemplateParams);
	m_TemplateParams.clear();

	if (i_IsNewChildNode)
	{
		auto l_pStartChildNodeInfo = i_pNodeManager->instanceNodeInfo("Start", i_NodeID);
		m_StartChildNodeID = l_pStartChildNodeInfo->getNodeID();

		auto l_pEndChildNodeInfo = i_pNodeManager->instanceNodeInfo("End", i_NodeID);
		m_EndChildNodeID = l_pEndChildNodeInfo->getNodeID();
	}
}

void TaskNodeInfo::initData(std::string_view i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsInitChildNode) noexcept
{
	NodeInfo::initData(i_TemplateNodeKey, i_pNodeManager, i_IsInitChildNode);
	m_IsDynamicInputOutputPin = false;
	m_IsDynamicParam = false;

	if (i_IsInitChildNode)
	{
		auto l_pStartChildNodeInfo = i_pNodeManager->instanceNodeInfo("Start", m_NodeID);
		m_StartChildNodeID = l_pStartChildNodeInfo->getNodeID();

		auto l_pEndChildNodeInfo = i_pNodeManager->instanceNodeInfo("End", m_NodeID);
		m_EndChildNodeID = l_pEndChildNodeInfo->getNodeID();
	}
}

PinID TaskNodeInfo::addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept
{
	PinID l_PinID = NodeInfo::addPinInfo(i_PinType, i_PinNameStr, i_pNodeManager);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (l_pStartNodeInfo && i_PinType == PinType::Input)
		l_pStartNodeInfo->pinIDList().addPinInfo(PinType::Output, i_PinNameStr, i_pNodeManager);
	if (l_pEndNodeInfo && i_PinType == PinType::Output)
		l_pEndNodeInfo->pinIDList().addPinInfo(PinType::Input, i_PinNameStr, i_pNodeManager);

	return l_PinID;
}

PinID TaskNodeInfo::addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager, Bool i_IsTemplatePinIDList) noexcept
{
	PinID l_PinID = NodeInfo::addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager, i_IsTemplatePinIDList);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);

	if (l_pStartNodeInfo && i_pPinInfo.getPinType() == PinType::Input)
	{
		i_pPinInfo.setPinTypeValue(PinType::Output);
		if (!i_IsTemplatePinIDList)
			l_pStartNodeInfo->pinIDList().addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
		else
			l_pStartNodeInfo->templatePinIDList().addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
		i_pPinInfo.setPinTypeValue(PinType::Input);
	}
	if (l_pEndNodeInfo && i_pPinInfo.getPinType() == PinType::Output)
	{
		i_pPinInfo.setPinTypeValue(PinType::Input);
		if (!i_IsTemplatePinIDList)
			l_pEndNodeInfo->pinIDList().addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
		else
			l_pEndNodeInfo->templatePinIDList().addPinInfo(i_pPinInfo, i_PinIndex, i_pNodeManager);
		i_pPinInfo.setPinTypeValue(PinType::Output);
	}

	return l_PinID;
}

void TaskNodeInfo::delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::delPinInfo(i_PinType, i_PinIndex, i_pNodeManager);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (l_pStartNodeInfo && i_PinType == PinType::Input)
		l_pStartNodeInfo->pinIDList().delPinInfo(PinType::Output, i_PinIndex, i_pNodeManager);
	if (l_pEndNodeInfo && i_PinType == PinType::Output)
		l_pEndNodeInfo->pinIDList().delPinInfo(PinType::Input, i_PinIndex, i_pNodeManager);
}

void TaskNodeInfo::swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev, NodeManager* i_pNodeManager) noexcept
{
	NodeInfo::swapPinID(i_PinType, i_PinIndex, i_IsSwapPrev, i_pNodeManager);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (l_pStartNodeInfo && i_PinType == PinType::Input)
		l_pStartNodeInfo->pinIDList().swapPinID(PinType::Output, i_PinIndex, i_IsSwapPrev);
	if (l_pEndNodeInfo && i_PinType == PinType::Output)
		l_pEndNodeInfo->pinIDList().swapPinID(PinType::Input, i_PinIndex, i_IsSwapPrev);
}

void TaskNodeInfo::changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange) noexcept
{
	NodeInfo::changePinInfo(i_PinType, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);

	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (l_pStartNodeInfo && i_PinType == PinType::Input)
		l_pStartNodeInfo->pinIDList().changePinInfo(PinType::Output, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);
	if (l_pEndNodeInfo && i_PinType == PinType::Output)
		l_pEndNodeInfo->pinIDList().changePinInfo(PinType::Input, i_PinIndex, i_pNodeManager, i_pPinInfo, i_IsChange);
}

bool TaskNodeInfo::canChangePinAccessType(PinType i_PinType) noexcept {
	return true;
};

void TaskNodeInfo::loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager) noexcept
{
	auto l_XmlNodeGraph = i_XmlElement->FirstChildElement("NodeGraph");
	if (!l_XmlNodeGraph) return;
	auto l_XmlStartNode = l_XmlNodeGraph->FirstChildElement("StartNode");
	auto l_XmlEndNode = l_XmlNodeGraph->FirstChildElement("EndNode");
	if (!(l_XmlStartNode && l_XmlEndNode)) return;
	NodeID l_StartNodeID = l_XmlStartNode->FindAttribute("NodeID")->Int64Value();
	NodeID l_EndNodeID = l_XmlEndNode->FindAttribute("NodeID")->Int64Value();

	auto l_pStartChildNodeInfo = i_pNodeManager->instanceNodeInfo("Start", m_NodeID, l_StartNodeID);
	auto l_pEndChildNodeInfo = i_pNodeManager->instanceNodeInfo("End", m_NodeID, l_EndNodeID);
	if (!(l_pStartChildNodeInfo && l_pEndChildNodeInfo)) return;
	m_StartChildNodeID = l_StartNodeID;
	m_EndChildNodeID = l_EndNodeID;

	NodeInfo::loadData(i_XmlElement, i_pNodeManager);

	l_pStartChildNodeInfo->loadData(l_XmlStartNode, i_pNodeManager);
	l_pEndChildNodeInfo->loadData(l_XmlEndNode, i_pNodeManager);
}

tinyxml2::XMLElement* TaskNodeInfo::saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager) noexcept
{
	tinyxml2::XMLElement* l_XmlGraphNode = NodeInfo::saveData(i_XmlDocument, i_ParentXmlElement, i_pNodeManager);
	auto l_pStartNodeInfo = i_pNodeManager->nodeInfo(m_StartChildNodeID);
	auto l_pEndNodeInfo = i_pNodeManager->nodeInfo(m_EndChildNodeID);
	if (!l_pStartNodeInfo && !l_pEndNodeInfo) return nullptr;

	l_pStartNodeInfo->saveData(i_XmlDocument, l_XmlGraphNode, i_pNodeManager);
	l_pEndNodeInfo->saveData(i_XmlDocument, l_XmlGraphNode, i_pNodeManager);

	return l_XmlGraphNode;
}

NodeManager::NodeManager() noexcept
{
	initData();

	m_RCTemplatePath = "G:\\rceditor\\rctemplate";
	registerTemplateNodes(m_RCTemplatePath, m_RCTemplatePath.string().c_str());
}

NodeManager::~NodeManager() noexcept
{
}

void NodeManager::initData() noexcept
{
	m_pCurStackInfo = std::make_shared<StackInfo>();
	m_pStackInfoMap[m_CurParentNodeID] = m_pCurStackInfo;
	m_pCurTaskTagInfo = std::make_shared<TaskTagInfo>();
	m_pTaskTagInfoMap[m_CurParentNodeID] = m_pCurTaskTagInfo;
}

void NodeManager::clearRunningData() noexcept
{
	m_pNodeInfoMap.clear();
	m_pLinkInfoMap.clear();
	m_pPinInfoMap.clear();
	m_pCurStackInfo = nullptr;
	m_pStackInfoMap.clear();
	m_pCurTaskTagInfo = nullptr;
	m_pTaskTagInfoMap.clear();

	m_TagNodeChangeNodeID = 0;
	m_IsTagNodeChange = false;
	m_CurParentNodeID = 0;
	m_CurTagNodeID = 0;
	initData();
}

void NodeManager::addStackInfo(NodeID i_NodeID) noexcept
{
	m_pStackInfoMap[i_NodeID] = std::make_shared<StackInfo>();
}

void NodeManager::delStackInfo(NodeID i_NodeID) noexcept
{
	auto l_pNodeInfo = nodeInfo(i_NodeID);
	auto l_pStackInfo = getStackInfo(i_NodeID);
	if (l_pNodeInfo && l_pStackInfo)
	{
		for (auto l_TempNodeID : l_pStackInfo->m_NodeIDList)
		{
			delNodeInfo(l_TempNodeID, false);
		}

		m_pStackInfoMap.erase(i_NodeID);
	}
}

void NodeManager::addTaskTagInfo(NodeID i_NodeID) noexcept
{
	auto l_It = m_pTaskTagInfoMap.find(i_NodeID);
	if (l_It == m_pTaskTagInfoMap.end()) {
		auto l_pPreTaskTagInfo = m_pTaskTagInfoMap[m_CurTagNodeID];
		m_CurTagNodeID = i_NodeID;
		m_CurParentNodeID = i_NodeID;
		m_pCurTaskTagInfo = std::make_shared<TaskTagInfo>(*l_pPreTaskTagInfo);
		m_pTaskTagInfoMap[i_NodeID] = m_pCurTaskTagInfo;
		m_pCurStackInfo = m_pStackInfoMap[i_NodeID];
	}
	else {
		selTaskTagInfo(i_NodeID);
		m_pCurTaskTagInfo->m_IsShow = true;
	}
	m_TagNodeChangeNodeID = i_NodeID;
	m_IsTagNodeChange = true;
}

void NodeManager::delTaskTagInfo(NodeID i_NodeID) noexcept
{
	m_pCurTaskTagInfo->m_IsShow = false;
	if (m_CurTagNodeID == i_NodeID)
	{
		m_CurParentNodeID = 0;
		m_CurTagNodeID = 0;
		m_IsTagNodeChange = true;
		m_TagNodeChangeNodeID = 0;
		m_pCurTaskTagInfo = m_pTaskTagInfoMap[m_CurTagNodeID];
		m_pCurStackInfo = m_pStackInfoMap[m_CurTagNodeID];
	}

	//m_pTaskTagInfoMap.erase(i_NodeID);
}

void NodeManager::selTaskTagInfo(NodeID i_NodeID) noexcept
{
	if (m_CurTagNodeID != i_NodeID && m_pTaskTagInfoMap.find(i_NodeID) != m_pTaskTagInfoMap.end())
	{
		m_IsTagNodeChange = true;
		//m_CurParentNodeID = i_NodeID;
		m_CurTagNodeID = i_NodeID;
		m_pCurTaskTagInfo = m_pTaskTagInfoMap[i_NodeID];
		if (!m_pCurTaskTagInfo->m_NodeIDStack.empty())
			m_CurParentNodeID = m_pCurTaskTagInfo->m_NodeIDStack.top();
		else
			m_CurParentNodeID = i_NodeID;
		m_pCurStackInfo = m_pStackInfoMap[m_CurParentNodeID];
	}
}

void NodeManager::pushCurParentNodeID(NodeID i_NodeID) noexcept
{
	m_CurParentNodeID = i_NodeID;
	m_pCurTaskTagInfo->m_NodeIDStack.push(m_CurParentNodeID);
	m_pCurStackInfo = getStackInfo(m_CurParentNodeID);
}

NodeID NodeManager::popCurParentNodeID() noexcept {
	if (m_pCurTaskTagInfo->m_NodeIDStack.empty())
	{
		return m_CurParentNodeID;
	}
	m_pCurTaskTagInfo->m_NodeIDStack.pop();
	if (m_pCurTaskTagInfo->m_NodeIDStack.empty())
		m_CurParentNodeID = m_CurTagNodeID;
	else
		m_CurParentNodeID = m_pCurTaskTagInfo->m_NodeIDStack.top();

	m_pCurStackInfo = getStackInfo(m_CurParentNodeID);
	return m_CurParentNodeID;
}

void NodeManager::getCurStackStr(NodeID i_NodeID, std::string& i_StackTitleStr) noexcept
{
	if (m_CurTagNodeID == 0) {
		i_StackTitleStr = g_RootNodeStackStr;
		return;
	}
	else {
		auto l_pTaskNodeInfo = nodeInfo(m_CurTagNodeID);
		auto l_pNodeInfo = nodeInfo(i_NodeID);
		if (l_pTaskNodeInfo && l_pNodeInfo)
		{
			if (l_pNodeInfo->getNodeType() != NodeType::Task) {
				i_StackTitleStr = "->" + l_pNodeInfo->nodeName() + i_StackTitleStr;
				getCurStackStr(l_pNodeInfo->getParentNodeID(), i_StackTitleStr);
			}
			else {
				i_StackTitleStr = l_pNodeInfo->nodeName() + i_StackTitleStr;
			}
		}
	}
}

//void NodeManager::loadRCTemplate(const std::filesystem::directory_entry& i_FilePath, NodeInfo& i_NodeInfo) noexcept
//{
//	tinyxml2::XMLDocument l_XMLDocument{};
//	tinyxml2::XMLError l_Error = l_XMLDocument.LoadFile(i_FilePath.path().string().c_str());
//	assert(l_Error == tinyxml2::XML_SUCCESS);
//
//	tinyxml2::XMLElement* l_ElementNode = l_XMLDocument.RootElement();
//	for (const tinyxml2::XMLAttribute* l_Attribute = l_ElementNode->FirstAttribute(); l_Attribute != nullptr; l_Attribute = l_Attribute->Next())
//	{
//		//S32 l_Index = 0;
//		//l_Attribute->QueryIntValue(&l_Index);
//		i_NodeInfo.setData(l_Attribute->Name(), l_Attribute->Value());
//	}
//
//	tinyxml2::XMLElement* l_ElementNodeGraph = l_ElementNode->FirstChildElement("NodeGraph");
//	for (const tinyxml2::XMLAttribute* l_Attribute = l_ElementNodeGraph->FirstAttribute(); l_Attribute != nullptr; l_Attribute = l_Attribute->Next())
//	{
//		i_NodeInfo.setData(l_Attribute->Name(), l_Attribute->Value());
//	}
//
//	auto loadPinElement = [&](const std::string& i_ElementKey) {
//		tinyxml2::XMLElement* l_ElementPinList = l_ElementNode->FirstChildElement(i_ElementKey.c_str());
//		if (!l_ElementPinList) return;
//		for (tinyxml2::XMLElement* l_XMLPin = l_ElementPinList->FirstChildElement("Pin"); l_XMLPin != nullptr; l_XMLPin = l_XMLPin->NextSiblingElement("Pin"))
//		{
//			PinInfo l_PinInfo{};
//			for (const tinyxml2::XMLAttribute* l_Attribute = l_XMLPin->FirstAttribute(); l_Attribute != nullptr; l_Attribute = l_Attribute->Next())
//			{
//				l_PinInfo.setData(l_Attribute->Name(), l_Attribute->Value());
//			}
//			l_PinInfo.m_IsTemplate = true;
//
//			if (i_ElementKey == "InputOutputPinList")
//			{
//				l_PinInfo.m_IsInputOutput = true;
//
//				TemplatePinID l_OutputPinID = getTemplatePinID();
//				l_PinInfo.m_PinType = PinType::Output;
//				m_pTemplatePinInfoMap[l_OutputPinID] = std::make_shared<PinInfo>(l_PinInfo);
//				i_NodeInfo.m_InputOutputOutputPinIDs.push_back(l_OutputPinID);
//
//				TemplatePinID l_InputPinID = getTemplatePinID();
//				l_PinInfo.m_PinType = PinType::Input;
//				l_PinInfo.m_PinAccessType = PinAccessType::Writable;
//				m_pTemplatePinInfoMap[l_InputPinID] = std::make_shared<PinInfo>(l_PinInfo);
//				i_NodeInfo.m_InputOutputInputPinIDs.push_back(l_InputPinID);
//			}
//			else if (i_ElementKey == "Input")
//			{
//				l_PinInfo.m_PinType = PinType::Input;
//
//				TemplatePinID l_PinID = getTemplatePinID();
//				m_pTemplatePinInfoMap[l_PinID] = std::make_shared<PinInfo>(l_PinInfo);
//				i_NodeInfo.m_InputPinIDs.push_back(l_PinID);
//			}
//			else if (i_ElementKey == "Output")
//			{
//				l_PinInfo.m_PinType = PinType::Output;
//
//				TemplatePinID l_PinID = getTemplatePinID();
//				m_pTemplatePinInfoMap[l_PinID] = std::make_shared<PinInfo>(l_PinInfo);
//				i_NodeInfo.m_OutputPinIDs.push_back(l_PinID);
//			}
//		}
//	};
//
//	auto loadParamElement = [&](const std::string& i_ElementKey) {
//		tinyxml2::XMLElement* l_ElementPinList = l_ElementNode->FirstChildElement(i_ElementKey.c_str());
//		if (!l_ElementPinList) return;
//		for (tinyxml2::XMLElement* l_XMLPin = l_ElementPinList->FirstChildElement("Param"); l_XMLPin != nullptr; l_XMLPin = l_XMLPin->NextSiblingElement("Param"))
//		{
//			EditorParamInfo l_ParamInfo{};
//			for (const tinyxml2::XMLAttribute* l_Attribute = l_XMLPin->FirstAttribute(); l_Attribute != nullptr; l_Attribute = l_Attribute->Next())
//			{
//				S32 l_Index = 0;
//				l_Attribute->QueryIntValue(&l_Index);
//				l_ParamInfo.setData(l_Attribute->Name(), l_Attribute->Value());
//			}
//
//			i_NodeInfo.m_Params.push_back(l_ParamInfo);
//		}
//	};
//
//	loadPinElement("InputOutputPinList");
//	loadPinElement("Input");
//	loadPinElement("Output");
//
//	loadParamElement("ParamList");
//
//	return;
//}
//
//static std::vector<std::string> RC_TEMPLATE_NODE_KEY_LIST {"NodeName", "TemplateName", "ClassName", "IsDynamicInputOutput", "IsDynamicInput", "IsDynamicOutput", "IsDynamicParam"};
//static std::vector<std::string> RC_TEMPLATE_NODE_GRAPH_KEY_LIST{ "NodeType", "Comment" };
//static std::vector<std::string> RC_TEMPLATE_PIN_KEY_LIST{ "Name", "MetaData", "PinAccessType", "DataType"};
//
//void NodeManager::saveRCTemplate(const std::string& i_FilePath, NodeID i_NodeID) noexcept
//{
//	std::shared_ptr<NodeInfo> l_pNodeInfo = nodeInfo(i_NodeID);
//	assert(l_pNodeInfo);
//
//	S32 l_PosLeft = i_FilePath.rfind("\\", i_FilePath.length());
//	S32 l_PosRight = i_FilePath.rfind(".xml", i_FilePath.length());
//	std::string l_FilenameStr = i_FilePath.substr(l_PosLeft + 1, l_PosRight - l_PosLeft - 1);
//
//	//Save Template Node Info
//	{
//		auto copyInputOutputPinList = [&](std::vector<PinID>& i_DstInputPinList, std::vector<PinID>& i_DstOutputPinList, const std::vector<PinID>& i_SrcPinIDList) {
//			for (auto l_PinID : i_SrcPinIDList)
//			{
//				std::shared_ptr<PinInfo> l_pPinInfo = pinInfo(l_PinID);
//				assert(l_pPinInfo);
//
//				PinInfo l_CurPinInfo = *l_pPinInfo;
//				l_CurPinInfo.m_NodeID = {};
//
//				PinID l_InputPinID = getPinID();
//				l_CurPinInfo.m_PinType = PinType::Input;
//				m_pPinInfoMap[l_InputPinID] = std::make_shared<PinInfo>(l_CurPinInfo);
//				i_DstInputPinList.push_back(l_InputPinID);
//
//				PinID l_OutputPinID = getPinID();
//				l_CurPinInfo.m_PinType = PinType::Output;
//				m_pPinInfoMap[l_OutputPinID] = std::make_shared<PinInfo>(l_CurPinInfo);
//				i_DstOutputPinList.push_back(l_OutputPinID);
//			}
//			};
//
//		auto copyPinList = [&](std::vector<PinID>& i_DstPinList, const std::vector<PinID>& i_SrcPinIDList) {
//			for (auto l_PinID : i_SrcPinIDList)
//			{
//				std::shared_ptr<PinInfo> l_pPinInfo = pinInfo(l_PinID);
//				assert(l_pPinInfo);
//
//				PinInfo l_CurPinInfo = *l_pPinInfo;
//				l_CurPinInfo.m_NodeID = {};
//
//				PinID l_PinID = getPinID();
//				m_pPinInfoMap[l_PinID] = std::make_shared<PinInfo>(l_CurPinInfo);
//				i_DstPinList.push_back(l_PinID);
//			}
//			};
//
//		// add New Template
//		NodeInfo l_NewTemplateNodeInfo = *l_pNodeInfo;
//		l_NewTemplateNodeInfo.m_Name = l_FilenameStr;
//		l_NewTemplateNodeInfo.m_GUID = "";
//		l_NewTemplateNodeInfo.m_TemplateName = "";
//
//		copyInputOutputPinList(l_NewTemplateNodeInfo.m_InputOutputOutputPinIDs, l_NewTemplateNodeInfo.m_InputOutputInputPinIDs, l_pNodeInfo->m_TemplateInputOutputOutputPinIDs);
//		copyInputOutputPinList(l_NewTemplateNodeInfo.m_InputOutputOutputPinIDs, l_NewTemplateNodeInfo.m_InputOutputInputPinIDs, l_pNodeInfo->m_InputOutputOutputPinIDs);
//
//		copyPinList(l_NewTemplateNodeInfo.m_OutputPinIDs, l_pNodeInfo->m_TemplateOutputPinIDs);
//		copyPinList(l_NewTemplateNodeInfo.m_OutputPinIDs, l_pNodeInfo->m_OutputPinIDs);
//
//		copyPinList(l_NewTemplateNodeInfo.m_InputPinIDs, l_pNodeInfo->m_TemplateInputPinIDs);
//		copyPinList(l_NewTemplateNodeInfo.m_InputPinIDs, l_pNodeInfo->m_InputPinIDs);
//
//		//Dir Key
//		std::string l_DirStr = i_FilePath.substr(0, l_PosLeft);
//		S32 l_PosDir = l_DirStr.rfind("\\", l_DirStr.length());
//		std::string l_PathKeyStr = l_DirStr.substr(l_PosDir + 1, l_DirStr.length() - l_PosDir) + "##" + l_DirStr.substr(0, l_PosDir);
//		m_pTemplateNodeFileMap[l_PathKeyStr].emplace_back(TemplateFileType::Template, l_FilenameStr);
//		registerNode(std::move(l_NewTemplateNodeInfo));
//	}
//
//	//Save Template Node XML
//	auto l_pTemplateNodeInfo = m_pRegisteredNodeInfoMap[l_FilenameStr];
//	assert(l_pTemplateNodeInfo);
//
//	tinyxml2::XMLDocument l_XMLDocument{};
//	tinyxml2::XMLElement* l_ElementNode = l_XMLDocument.NewElement("Node");
//	{
//		for (const auto& l_Key : RC_TEMPLATE_NODE_KEY_LIST)
//		{
//			l_ElementNode->SetAttribute(l_Key.c_str(), l_pTemplateNodeInfo->getData(l_Key).c_str());
//		}
//		l_XMLDocument.InsertEndChild(l_ElementNode);
//	}
//
//	{
//		tinyxml2::XMLElement* l_ElementNodeGraph = l_XMLDocument.NewElement("NodeGraph");
//		for (const auto& l_Key : RC_TEMPLATE_NODE_GRAPH_KEY_LIST)
//		{
//			l_ElementNodeGraph->SetAttribute(l_Key.c_str(), l_pTemplateNodeInfo->getData(l_Key).c_str());
//		}
//		l_ElementNode->InsertEndChild(l_ElementNodeGraph);
//	}
//
//	auto savePinElement = [&](tinyxml2::XMLElement* i_XMLElement, std::vector<PinID>& i_PinIDList) {
//		for (const auto& l_PinID : i_PinIDList) {
//			auto l_pPinInfo = pinInfo(l_PinID);
//			assert(l_pPinInfo);
//
//			tinyxml2::XMLElement* l_ElementPin = l_XMLDocument.NewElement("Pin");
//
//			for (const auto& l_Key : RC_TEMPLATE_PIN_KEY_LIST)
//			{
//				l_ElementPin->SetAttribute(l_Key.c_str(), l_pPinInfo->getData(l_Key).c_str());
//			}
//			i_XMLElement->InsertEndChild(l_ElementPin);
//		}
//	};
//
//	{
//		tinyxml2::XMLElement* l_ElementInputOutputPinList = l_XMLDocument.NewElement("InputOutputPinList");
//		savePinElement(l_ElementInputOutputPinList, l_pTemplateNodeInfo->m_InputOutputOutputPinIDs);
//		l_ElementNode->InsertEndChild(l_ElementInputOutputPinList);
//	}
//
//	{
//		tinyxml2::XMLElement* l_ElementInputPinList = l_XMLDocument.NewElement("Input");
//		savePinElement(l_ElementInputPinList, l_pTemplateNodeInfo->m_InputPinIDs);
//		l_ElementNode->InsertEndChild(l_ElementInputPinList);
//	}
//
//	{
//		tinyxml2::XMLElement* l_ElementOutputPinList = l_XMLDocument.NewElement("Output");
//		savePinElement(l_ElementOutputPinList, l_pTemplateNodeInfo->m_OutputPinIDs);
//		l_ElementNode->InsertEndChild(l_ElementOutputPinList);
//	}
//
//	l_XMLDocument.SaveFile(i_FilePath.c_str());
//
//	return;
//}

void NodeManager::registerTemplateNodes(std::filesystem::path i_Path, const std::string& i_MapKey) noexcept
{
	tinyxml2::XMLDocument l_XmlDocument{};
	std::filesystem::directory_iterator l_DirIter(i_Path);

	for (const auto& l_Entry : l_DirIter)
	{
		if (l_Entry.is_regular_file())
		{
			tinyxml2::XMLDocument l_XMLDocument{};
			tinyxml2::XMLError l_Error = l_XMLDocument.LoadFile(l_Entry.path().string().c_str());
			assert(l_Error == tinyxml2::XML_SUCCESS);

			std::shared_ptr<TemplateNodeInfo> l_pTemplateNodeInfo{};
			tinyxml2::XMLElement* l_XmlElementNode = l_XMLDocument.RootElement();
			if (!l_XmlElementNode->FindAttribute("TemplateName")) {
				l_pTemplateNodeInfo = std::make_shared<TemplateNodeInfo>();
			}
			else {
				std::shared_ptr<FavoriteNodeInfo> l_pFavoriteNodeInfo = std::make_shared<FavoriteNodeInfo>();
				l_pTemplateNodeInfo = std::dynamic_pointer_cast<FavoriteNodeInfo>(l_pFavoriteNodeInfo);
			}

			l_pTemplateNodeInfo->loadData(l_XmlElementNode);

			m_pTemplateNodeFileMap[i_MapKey].emplace_back(TemplateFileType::Template, l_pTemplateNodeInfo->getNodeName());
			registerTemplateNode(l_pTemplateNodeInfo);
		}
		else if (l_Entry.is_directory())
		{
			const auto& l_CurDirPathStr = l_Entry.path().string();
			S32 l_Pos = l_CurDirPathStr.rfind("\\", l_CurDirPathStr.length());
			std::string l_CurDirParentPathStr = l_CurDirPathStr.substr(0, l_Pos);
			std::string l_CurDirStr = l_CurDirPathStr.substr(l_Pos+1, l_Entry.path().string().length());
			std::string l_PathKeyStr = l_CurDirStr + "##" + l_CurDirParentPathStr;

			m_pTemplateNodeFileMap[i_MapKey].emplace_back(TemplateFileType::Dir, l_PathKeyStr);
			registerTemplateNodes(l_Entry.path().string().c_str(), l_PathKeyStr);
		}
	}
}

void NodeManager::registerTemplateNode(std::shared_ptr<TemplateNodeInfo> i_pTemplateNodeInfo) noexcept
{
	if (m_pTemplateNodeInfoMap.find(i_pTemplateNodeInfo->getNodeName()) == m_pTemplateNodeInfoMap.end())
	{
		m_pTemplateNodeInfoMap.emplace(i_pTemplateNodeInfo->getNodeName(), i_pTemplateNodeInfo);
	}
	else
	{
		assert(false);
	}
}

void NodeManager::delInfoByLinkIDSet(const std::set<LinkID>& i_LinkIDSet) noexcept
{
	auto l_It = i_LinkIDSet.begin();
	while (l_It != i_LinkIDSet.end())
	{
		auto l_LinkID = *l_It;
		std::shared_ptr<LinkInfo> l_pLinkInfo = m_pLinkInfoMap[l_LinkID];
		assert(l_pLinkInfo);

		std::shared_ptr<NodeInfo> l_pStartNodeInfo = m_pNodeInfoMap[l_pLinkInfo->m_OutputNodeID];
		std::shared_ptr<NodeInfo> l_pEndNodeInfo = m_pNodeInfoMap[l_pLinkInfo->m_InputNodeID];
		std::shared_ptr<PinInfo> l_pStartPinInfo = m_pPinInfoMap[l_pLinkInfo->m_OutputPinID];
		std::shared_ptr<PinInfo> l_pEndPinInfo = m_pPinInfoMap[l_pLinkInfo->m_InputPinID];
		assert(l_pStartNodeInfo && l_pEndNodeInfo && l_pStartPinInfo && l_pEndPinInfo);

		l_It++;
		if (l_pStartNodeInfo->linkIDSet().count(l_LinkID) > 0)
			l_pStartNodeInfo->linkIDSet().erase(l_LinkID);
		if (l_pEndNodeInfo->linkIDSet().count(l_LinkID) > 0)
			l_pEndNodeInfo->linkIDSet().erase(l_LinkID);
		if (l_pStartPinInfo->linkIDSet().count(l_LinkID) > 0)
			l_pStartPinInfo->linkIDSet().erase(l_LinkID);
		if (l_pEndPinInfo->linkIDSet().count(l_LinkID) > 0)
			l_pEndPinInfo->linkIDSet().erase(l_LinkID);

		m_pLinkInfoMap.erase(l_LinkID);
		m_pCurStackInfo->m_LinkIDList.erase(l_LinkID);
	}
}

std::shared_ptr<NodeInfo> NodeManager::instanceNodeInfo(std::string_view i_TemplateNodeName, NodeID i_ParentNodeID, NodeID i_NodeID, Bool i_IsNewChildNode) noexcept
{
	std::shared_ptr<TemplateNodeInfo> l_pTemplateNodeInfo = m_pTemplateNodeInfoMap[std::string(i_TemplateNodeName)];
	assert(l_pTemplateNodeInfo != nullptr);

	auto l_NodeID = i_NodeID;
	if(l_NodeID == 0)
		l_NodeID = getNodeID();
	if(l_pTemplateNodeInfo->getNodeType() == NodeType::Composite || l_pTemplateNodeInfo->getNodeType() == NodeType::Task)
		addStackInfo(l_NodeID);

	std::shared_ptr<NodeInfo> l_pNodeInfo = l_pTemplateNodeInfo->instanceNodeInfo(i_ParentNodeID, this, l_NodeID, i_IsNewChildNode);
	assert(l_pNodeInfo != nullptr);
	std::shared_ptr<StackInfo> l_pStackInfo = getStackInfo(i_ParentNodeID);
	if(l_pStackInfo)
		l_pStackInfo->m_NodeIDList.insert(l_pNodeInfo->getNodeID());

	return l_pNodeInfo;
}

Bool NodeManager::delNodeInfo(NodeID i_NodeID, Bool i_IsDelNodeIDSet) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_pNodeInfoMap[i_NodeID];
	if (!l_pNodeInfo) return false;
	std::shared_ptr<StackInfo> l_pStackInfo = getStackInfo(l_pNodeInfo->getParentNodeID());
	assert(l_pStackInfo != nullptr);

	if (l_pNodeInfo->getNodeType() == NodeType::Composite)
		delStackInfo(i_NodeID);

	l_pNodeInfo->deleteInfo(this);
	m_pNodeInfoMap.erase(i_NodeID);
	if(i_IsDelNodeIDSet)
		l_pStackInfo->m_NodeIDList.erase(i_NodeID);

	return true;
}

LinkID NodeManager::addLinkInfo(PinID i_StartPinID, PinID i_EndPinID) noexcept
{
	std::shared_ptr<PinInfo> l_pStartPinInfo = m_pPinInfoMap[i_StartPinID];
	std::shared_ptr<PinInfo> l_pEndPinInfo = m_pPinInfoMap[i_EndPinID];
	if (!l_pStartPinInfo || !l_pEndPinInfo) return false;

	std::shared_ptr<NodeInfo> l_pStartNodeInfo = m_pNodeInfoMap[l_pStartPinInfo->getNodeID()];
	std::shared_ptr<NodeInfo> l_pEndNodeInfo = m_pNodeInfoMap[l_pEndPinInfo->getNodeID()];
	if (!l_pStartNodeInfo || !l_pEndNodeInfo) return false;

	//const auto& l_EditorStyle = ax::NodeEditor::GetStyle();
	//const ImColor l_SingleLinkColor = l_EditorStyle.Colors[ax::NodeEditor::StyleColor_SingleLink];
	//const ImColor l_MultipleLinkColor = l_EditorStyle.Colors[ax::NodeEditor::StyleColor_MultipleLink];
	const ImColor l_SingleLinkColor = ImColor(237, 34, 36, 255);
	const ImColor l_MultipleLinkColor = ImColor(118, 185, 0, 255);
	ImColor l_Color = l_MultipleLinkColor;
	if (l_pStartPinInfo->getPinAccessType() == PinAccessType::Writable || l_pEndPinInfo->getPinAccessType() == PinAccessType::Writable)
		l_Color = l_SingleLinkColor;

	if (l_pEndPinInfo->linkIDSet().size() > 0)
	{
		auto l_EndPinLinkInfo = linkInfo(*l_pEndPinInfo->linkIDSet().begin());
		
		if (l_EndPinLinkInfo->m_InputPinID == i_EndPinID) {
			return *l_pEndPinInfo->linkIDSet().begin();
		}
		else {
			delInfoByLinkIDSet(l_pEndPinInfo->linkIDSet());
		}
	}

	LinkID l_LinkID = getLinkID();
	std::shared_ptr<LinkInfo> l_LinkInfo = std::make_shared<LinkInfo>(LinkInfo{ l_pStartPinInfo->getNodeID(), i_StartPinID, l_pEndPinInfo->getNodeID(), i_EndPinID, l_Color });
	m_pLinkInfoMap[l_LinkID] = l_LinkInfo;

	////只加载当前层link
	//if (l_pStartNodeInfo->getParentNodeID() == m_CurParentNodeID)
	//	m_pCurStackInfo->m_LinkIDList.insert(l_LinkID);
	// Load时需要加载所有层的link
	auto l_ParentNodeID = l_pStartNodeInfo->getParentNodeID();
	getStackInfo(l_ParentNodeID)->m_LinkIDList.insert(l_LinkID);

	l_pStartPinInfo->linkIDSet().emplace(l_LinkID);
	l_pEndPinInfo->linkIDSet().emplace(l_LinkID);
	l_pStartNodeInfo->linkIDSet().emplace(l_LinkID);
	l_pEndNodeInfo->linkIDSet().emplace(l_LinkID);

	return l_LinkID;
}

Bool NodeManager::canAddLink(PinID i_CurSelPinID, PinID i_OtherPinID) noexcept
{
	if (i_CurSelPinID == i_OtherPinID)
		return false;

	std::shared_ptr<PinInfo> l_pStartPinInfo = m_pPinInfoMap[i_CurSelPinID];
	std::shared_ptr<PinInfo> l_pEndPinInfo = m_pPinInfoMap[i_OtherPinID];

	assert(l_pStartPinInfo && l_pEndPinInfo);

	if (l_pStartPinInfo->getPinType() == PinType::Input)
	{
		std::swap(l_pStartPinInfo, l_pEndPinInfo);
	}

	if ((!l_pEndPinInfo->getMetaData().empty() && (l_pStartPinInfo->getMetaData() != l_pEndPinInfo->getMetaData())) || (l_pStartPinInfo->getNodeID() == l_pEndPinInfo->getNodeID())
		|| (l_pStartPinInfo->getPinType() == l_pEndPinInfo->getPinType()) || (l_pStartPinInfo->getDataType() != l_pEndPinInfo->getDataType()) || (l_pStartPinInfo->getPinAccessType() != l_pEndPinInfo->getPinAccessType()))
		return false;

	return true;
}

Bool NodeManager::delLinkInfo(LinkID i_LinkID) noexcept
{
	std::shared_ptr<LinkInfo> l_pLinkInfo = m_pLinkInfoMap[i_LinkID];
	assert(l_pLinkInfo);

	std::shared_ptr<NodeInfo> l_pStartNodeInfo = m_pNodeInfoMap[l_pLinkInfo->m_OutputNodeID];
	std::shared_ptr<NodeInfo> l_pEndNodeInfo = m_pNodeInfoMap[l_pLinkInfo->m_InputNodeID];
	std::shared_ptr<PinInfo> l_pStartPinInfo = m_pPinInfoMap[l_pLinkInfo->m_OutputPinID];
	std::shared_ptr<PinInfo> l_pEndPinInfo = m_pPinInfoMap[l_pLinkInfo->m_InputPinID];
	assert(l_pStartNodeInfo && l_pEndNodeInfo && l_pStartPinInfo && l_pEndPinInfo);

	if (l_pStartNodeInfo->linkIDSet().count(i_LinkID) > 0)
		l_pStartNodeInfo->linkIDSet().erase(i_LinkID);
	if (l_pEndNodeInfo->linkIDSet().count(i_LinkID) > 0)
		l_pEndNodeInfo->linkIDSet().erase(i_LinkID);
	if (l_pStartPinInfo->linkIDSet().count(i_LinkID) > 0)
		l_pStartPinInfo->linkIDSet().erase(i_LinkID);
	if (l_pEndPinInfo->linkIDSet().count(i_LinkID) > 0)
		l_pEndPinInfo->linkIDSet().erase(i_LinkID);

	m_pLinkInfoMap.erase(i_LinkID);
	m_pCurStackInfo->m_LinkIDList.erase(i_LinkID);

	return true;
}

void NodeManager::getLinkInfoToFlodMap(std::vector<RealLinkInfoHead>& i_RealLinkInfoHeadList, std::map<std::string, LinkInfoToFlod>& i_LinkInfoToFlodMap) noexcept
{
	for (auto l_It : m_pLinkInfoMap)
	{
		const auto& l_pOutputNodeInfo = nodeInfo(l_It.second->m_OutputNodeID);
		const auto& l_pInputNodeInfo = nodeInfo(l_It.second->m_InputNodeID);
		const auto& l_pOutputPinInfo = pinInfo(l_It.second->m_OutputPinID);
		const auto& l_pInputPinInfo = pinInfo(l_It.second->m_InputPinID);
		if (!(l_pOutputNodeInfo && l_pInputNodeInfo && l_pOutputPinInfo && l_pInputPinInfo)) return;

		// is EndPoint
		Bool l_IsEndPoint = false;
		if (l_pInputNodeInfo->getAssociationNodeID() == l_pInputNodeInfo->getNodeID())
			l_IsEndPoint = true;
		else {
			const auto& l_pAssociationNodeInfo = nodeInfo(l_pInputNodeInfo->getAssociationNodeID());
			if (!l_pAssociationNodeInfo) continue;
			auto [l_OutputID, l_InputID] = l_pAssociationNodeInfo->getNameKey(l_pInputPinInfo->getName());
			std::shared_ptr<PinInfo> l_pAssociationPinInfo{};
			if (l_pAssociationNodeInfo->getNodeType() == NodeType::Start) {
				l_pAssociationPinInfo = pinInfo(l_OutputID);
			}
			else if (l_pAssociationNodeInfo->getNodeType() == NodeType::Composite || l_pAssociationNodeInfo->getNodeType() == NodeType::Task) {
				l_pAssociationPinInfo = pinInfo(l_InputID);
			}
			
			if(l_pAssociationPinInfo->linkIDSet().empty())
				l_IsEndPoint = true;
		}

		if (l_IsEndPoint) {
			RealLinkInfoHead l_RealLinkInfoHead{};
			l_RealLinkInfoHead.m_FlodKey = std::to_string(l_pOutputNodeInfo->getNodeID()) + l_pOutputPinInfo->getName();
			l_RealLinkInfoHead.m_LinkInfo = *l_It.second;
			i_RealLinkInfoHeadList.push_back(std::move(l_RealLinkInfoHead));
		}
		else {
			LinkInfoToFlod l_LinkInfoToFlod{};
			l_LinkInfoToFlod.m_LinkInfo = *l_It.second;
			std::string l_FlodKey = std::to_string(l_pInputNodeInfo->getAssociationNodeID()) + l_pInputPinInfo->getName();
			i_LinkInfoToFlodMap[l_FlodKey] = l_LinkInfoToFlod;
		}
	}
}

void NodeManager::flodLink(std::string_view i_FlodKey, RealLinkInfo& i_RealLinkInfo, std::map<std::string, LinkInfoToFlod>& i_LinkInfoToFlodMap) noexcept
{
	auto l_It = i_LinkInfoToFlodMap.find(i_FlodKey.data());
	if (l_It != i_LinkInfoToFlodMap.end())
	{
		if(!l_It->second.m_IsUse)
			l_It->second.m_IsUse = true;

		const auto& l_pOutputNodeInfo = nodeInfo(l_It->second.m_LinkInfo.m_OutputNodeID);
		const auto& l_pInputNodeInfo = nodeInfo(l_It->second.m_LinkInfo.m_InputNodeID);
		const auto& l_pOutputPinInfo = pinInfo(l_It->second.m_LinkInfo.m_OutputPinID);
		const auto& l_pInputPinInfo = pinInfo(l_It->second.m_LinkInfo.m_InputPinID);
		if (!(l_pOutputNodeInfo && l_pInputNodeInfo && l_pOutputPinInfo && l_pInputPinInfo)) return;

		if (l_pOutputNodeInfo->getAssociationNodeID() != l_pOutputNodeInfo->getNodeID())
		{
			std::string l_NextFlodKey = std::to_string(l_pOutputPinInfo->getNodeID()) + l_pOutputPinInfo->getName();
			flodLink(l_NextFlodKey, i_RealLinkInfo, i_LinkInfoToFlodMap);
		}
		i_RealLinkInfo.m_LinkInfoList.push_back(l_It->second.m_LinkInfo);
	}
}

std::vector<RealLinkInfo> NodeManager::getFlodLinkInfo() noexcept
{
	std::vector<RealLinkInfo> l_RealLinkInfoList{};
	std::vector<RealLinkInfoHead> l_RealLinkInfoHeadList{};
	std::map<std::string, LinkInfoToFlod> l_LinkInfoToFlodMap{};
	getLinkInfoToFlodMap(l_RealLinkInfoHeadList, l_LinkInfoToFlodMap);

	for (auto l_It : l_RealLinkInfoHeadList)
	{
		RealLinkInfo l_RealLinkInfo{};
		flodLink(l_It.m_FlodKey, l_RealLinkInfo, l_LinkInfoToFlodMap);
		l_RealLinkInfo.m_LinkInfoList.push_back(l_It.m_LinkInfo);
		l_RealLinkInfoList.emplace_back(std::move(l_RealLinkInfo));
	}

	return l_RealLinkInfoList;
}


RCEDITOR_NAMESPACE_END
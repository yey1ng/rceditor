#include <base/public/nodemanager.h>

RCEDITOR_NAMESPACE_BEGIN

NodeManager::NodeManager() noexcept
{
	m_pRegisteredNodeInfoMap.clear();
	m_pNodeInfoMap.clear();
	m_pLinkInfoMap.clear();
	m_pCurNodeIDSet.clear();
	m_pCurLinkIDSet.clear();

	m_RCTemplatePath = "G:\\rceditor\\rctemplate";

	registerNodes(m_RCTemplatePath, m_RCTemplatePath.string().c_str());
}

NodeManager::~NodeManager() noexcept
{
}


void NodeManager::clearRunningData() noexcept
{
	m_pNodeInfoMap.clear();
	m_pLinkInfoMap.clear();
	m_pPinInfoMap.clear();
	m_pCurNodeIDSet.clear();
	m_pCurLinkIDSet.clear();

	m_CurParentNodeID = 0;
	while (!m_CurParentNodeIDStack.empty()) {
		m_CurParentNodeIDStack.pop();
	}
}

void NodeManager::loadRCTemplate(const std::filesystem::directory_entry& i_FilePath, NodeInfo& i_NodeInfo) noexcept
{
	tinyxml2::XMLDocument l_XMLDocument{};
	tinyxml2::XMLError l_Error = l_XMLDocument.LoadFile(i_FilePath.path().string().c_str());
	assert(l_Error == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* l_ElementNode = l_XMLDocument.RootElement();
	for (const tinyxml2::XMLAttribute* l_Attribute = l_ElementNode->FirstAttribute(); l_Attribute != nullptr; l_Attribute = l_Attribute->Next())
	{
		//S32 l_Index = 0;
		//l_Attribute->QueryIntValue(&l_Index);
		i_NodeInfo.setData(l_Attribute->Name(), l_Attribute->Value());
	}

	tinyxml2::XMLElement* l_ElementNodeGraph = l_ElementNode->FirstChildElement("NodeGraph");
	for (const tinyxml2::XMLAttribute* l_Attribute = l_ElementNodeGraph->FirstAttribute(); l_Attribute != nullptr; l_Attribute = l_Attribute->Next())
	{
		i_NodeInfo.setData(l_Attribute->Name(), l_Attribute->Value());
	}

	auto loadPinElement = [&](const std::string& i_ElementKey) {
		tinyxml2::XMLElement* l_ElementPinList = l_ElementNode->FirstChildElement(i_ElementKey.c_str());
		if (!l_ElementPinList) return;
		for (tinyxml2::XMLElement* l_XMLPin = l_ElementPinList->FirstChildElement("Pin"); l_XMLPin != nullptr; l_XMLPin = l_XMLPin->NextSiblingElement("Pin"))
		{
			PinInfo l_PinInfo{};
			for (const tinyxml2::XMLAttribute* l_Attribute = l_XMLPin->FirstAttribute(); l_Attribute != nullptr; l_Attribute = l_Attribute->Next())
			{
				l_PinInfo.setData(l_Attribute->Name(), l_Attribute->Value());
			}
			l_PinInfo.m_IsTemplate = true;

			if (i_ElementKey == "ConnectionPinList")
			{
				l_PinInfo.m_IsConnection = true;

				TemplatePinID l_OutputPinID = getTemplatePinID();
				l_PinInfo.m_PinType = PinType::Output;
				m_pTemplatePinInfoMap[l_OutputPinID] = std::make_shared<PinInfo>(l_PinInfo);
				i_NodeInfo.m_ConnectionOutputPinIDs.push_back(l_OutputPinID);

				TemplatePinID l_InputPinID = getTemplatePinID();
				l_PinInfo.m_PinType = PinType::Input;
				l_PinInfo.m_PinAccessType = PinAccessType::Writable;
				m_pTemplatePinInfoMap[l_InputPinID] = std::make_shared<PinInfo>(l_PinInfo);
				i_NodeInfo.m_ConnectionInputPinIDs.push_back(l_InputPinID);
			}
			else if (i_ElementKey == "InputPinList")
			{
				l_PinInfo.m_PinType = PinType::Input;

				TemplatePinID l_PinID = getTemplatePinID();
				m_pTemplatePinInfoMap[l_PinID] = std::make_shared<PinInfo>(l_PinInfo);
				i_NodeInfo.m_InputPinIDs.push_back(l_PinID);
			}
			else if (i_ElementKey == "OutputPinList")
			{
				l_PinInfo.m_PinType = PinType::Output;

				TemplatePinID l_PinID = getTemplatePinID();
				m_pTemplatePinInfoMap[l_PinID] = std::make_shared<PinInfo>(l_PinInfo);
				i_NodeInfo.m_OutputPinIDs.push_back(l_PinID);
			}
		}
	};

	auto loadParamElement = [&](const std::string& i_ElementKey) {
		tinyxml2::XMLElement* l_ElementPinList = l_ElementNode->FirstChildElement(i_ElementKey.c_str());
		if (!l_ElementPinList) return;
		for (tinyxml2::XMLElement* l_XMLPin = l_ElementPinList->FirstChildElement("Param"); l_XMLPin != nullptr; l_XMLPin = l_XMLPin->NextSiblingElement("Param"))
		{
			EditorParamInfo l_ParamInfo{};
			for (const tinyxml2::XMLAttribute* l_Attribute = l_XMLPin->FirstAttribute(); l_Attribute != nullptr; l_Attribute = l_Attribute->Next())
			{
				S32 l_Index = 0;
				l_Attribute->QueryIntValue(&l_Index);
				l_ParamInfo.setData(l_Attribute->Name(), l_Attribute->Value());
			}

			i_NodeInfo.m_Params.push_back(l_ParamInfo);
		}
	};

	loadPinElement("ConnectionPinList");
	loadPinElement("InputPinList");
	loadPinElement("OutputPinList");

	loadParamElement("ParamList");

	return;
}

static std::vector<std::string> RC_TEMPLATE_NODE_KEY_LIST {"NodeName", "TemplateName", "ClassName", "IsDynamicConnectionPin", "IsDynamicInputPin", "IsDynamicOutputPin", "IsDynamicParam"};
static std::vector<std::string> RC_TEMPLATE_NODE_GRAPH_KEY_LIST{ "NodeType", "Comment" };
static std::vector<std::string> RC_TEMPLATE_PIN_KEY_LIST{ "Name", "MetaData", "PinAccessType", "DataType"};

void NodeManager::saveRCTemplate(const std::string& i_FilePath, NodeID i_NodeID) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = nodeInfo(i_NodeID);
	assert(l_pNodeInfo);

	S32 l_PosLeft = i_FilePath.rfind("\\", i_FilePath.length());
	S32 l_PosRight = i_FilePath.rfind(".xml", i_FilePath.length());
	std::string l_FilenameStr = i_FilePath.substr(l_PosLeft + 1, l_PosRight - l_PosLeft - 1);

	//Save Template Node Info
	{
		auto copyConnectionPinList = [&](std::vector<PinID>& i_DstInputPinList, std::vector<PinID>& i_DstOutputPinList, const std::vector<PinID>& i_SrcPinIDList) {
			for (auto l_PinID : i_SrcPinIDList)
			{
				std::shared_ptr<PinInfo> l_pPinInfo = pinInfo(l_PinID);
				assert(l_pPinInfo);

				PinInfo l_CurPinInfo = *l_pPinInfo;
				l_CurPinInfo.m_NodeID = {};

				PinID l_InputPinID = getPinID();
				l_CurPinInfo.m_PinType = PinType::Input;
				m_pPinInfoMap[l_InputPinID] = std::make_shared<PinInfo>(l_CurPinInfo);
				i_DstInputPinList.push_back(l_InputPinID);

				PinID l_OutputPinID = getPinID();
				l_CurPinInfo.m_PinType = PinType::Output;
				m_pPinInfoMap[l_OutputPinID] = std::make_shared<PinInfo>(l_CurPinInfo);
				i_DstOutputPinList.push_back(l_OutputPinID);
			}
			};

		auto copyPinList = [&](std::vector<PinID>& i_DstPinList, const std::vector<PinID>& i_SrcPinIDList) {
			for (auto l_PinID : i_SrcPinIDList)
			{
				std::shared_ptr<PinInfo> l_pPinInfo = pinInfo(l_PinID);
				assert(l_pPinInfo);

				PinInfo l_CurPinInfo = *l_pPinInfo;
				l_CurPinInfo.m_NodeID = {};

				PinID l_PinID = getPinID();
				m_pPinInfoMap[l_PinID] = std::make_shared<PinInfo>(l_CurPinInfo);
				i_DstPinList.push_back(l_PinID);
			}
			};

		// add New Template
		NodeInfo l_NewTemplateNodeInfo = *l_pNodeInfo;
		l_NewTemplateNodeInfo.m_Name = l_FilenameStr;
		l_NewTemplateNodeInfo.m_GUID = "";
		l_NewTemplateNodeInfo.m_TemplateName = "";

		copyConnectionPinList(l_NewTemplateNodeInfo.m_ConnectionOutputPinIDs, l_NewTemplateNodeInfo.m_ConnectionInputPinIDs, l_pNodeInfo->m_TemplateConnectionOutputPinIDs);
		copyConnectionPinList(l_NewTemplateNodeInfo.m_ConnectionOutputPinIDs, l_NewTemplateNodeInfo.m_ConnectionInputPinIDs, l_pNodeInfo->m_ConnectionOutputPinIDs);

		copyPinList(l_NewTemplateNodeInfo.m_OutputPinIDs, l_pNodeInfo->m_TemplateOutputPinIDs);
		copyPinList(l_NewTemplateNodeInfo.m_OutputPinIDs, l_pNodeInfo->m_OutputPinIDs);

		copyPinList(l_NewTemplateNodeInfo.m_InputPinIDs, l_pNodeInfo->m_TemplateInputPinIDs);
		copyPinList(l_NewTemplateNodeInfo.m_InputPinIDs, l_pNodeInfo->m_InputPinIDs);

		//Dir Key
		std::string l_DirStr = i_FilePath.substr(0, l_PosLeft);
		S32 l_PosDir = l_DirStr.rfind("\\", l_DirStr.length());
		std::string l_PathKeyStr = l_DirStr.substr(l_PosDir + 1, l_DirStr.length() - l_PosDir) + "##" + l_DirStr.substr(0, l_PosDir);
		m_pTemplateNodeFileMap[l_PathKeyStr].emplace_back(TemplateFileType::Template, l_FilenameStr);
		registerNode(std::move(l_NewTemplateNodeInfo));
	}

	//Save Template Node XML
	auto l_pTemplateNodeInfo = m_pRegisteredNodeInfoMap[l_FilenameStr];
	assert(l_pTemplateNodeInfo);

	tinyxml2::XMLDocument l_XMLDocument{};
	tinyxml2::XMLElement* l_ElementNode = l_XMLDocument.NewElement("Node");
	{
		for (const auto& l_Key : RC_TEMPLATE_NODE_KEY_LIST)
		{
			l_ElementNode->SetAttribute(l_Key.c_str(), l_pTemplateNodeInfo->getData(l_Key).c_str());
		}
		l_XMLDocument.InsertEndChild(l_ElementNode);
	}

	{
		tinyxml2::XMLElement* l_ElementNodeGraph = l_XMLDocument.NewElement("NodeGraph");
		for (const auto& l_Key : RC_TEMPLATE_NODE_GRAPH_KEY_LIST)
		{
			l_ElementNodeGraph->SetAttribute(l_Key.c_str(), l_pTemplateNodeInfo->getData(l_Key).c_str());
		}
		l_ElementNode->InsertEndChild(l_ElementNodeGraph);
	}

	auto savePinElement = [&](tinyxml2::XMLElement* i_XMLElement, std::vector<PinID>& i_PinIDList) {
		for (const auto& l_PinID : i_PinIDList) {
			auto l_pPinInfo = pinInfo(l_PinID);
			assert(l_pPinInfo);

			tinyxml2::XMLElement* l_ElementPin = l_XMLDocument.NewElement("Pin");

			for (const auto& l_Key : RC_TEMPLATE_PIN_KEY_LIST)
			{
				l_ElementPin->SetAttribute(l_Key.c_str(), l_pPinInfo->getData(l_Key).c_str());
			}
			i_XMLElement->InsertEndChild(l_ElementPin);
		}
	};

	{
		tinyxml2::XMLElement* l_ElementConnectionPinList = l_XMLDocument.NewElement("ConnectionPinList");
		savePinElement(l_ElementConnectionPinList, l_pTemplateNodeInfo->m_ConnectionOutputPinIDs);
		l_ElementNode->InsertEndChild(l_ElementConnectionPinList);
	}

	{
		tinyxml2::XMLElement* l_ElementInputPinList = l_XMLDocument.NewElement("InputPinList");
		savePinElement(l_ElementInputPinList, l_pTemplateNodeInfo->m_InputPinIDs);
		l_ElementNode->InsertEndChild(l_ElementInputPinList);
	}

	{
		tinyxml2::XMLElement* l_ElementOutputPinList = l_XMLDocument.NewElement("OutputPinList");
		savePinElement(l_ElementOutputPinList, l_pTemplateNodeInfo->m_OutputPinIDs);
		l_ElementNode->InsertEndChild(l_ElementOutputPinList);
	}

	l_XMLDocument.SaveFile(i_FilePath.c_str());

	return;
}

void NodeManager::registerNodes(std::filesystem::path i_Path, const std::string& i_MapKey) noexcept
{
	tinyxml2::XMLDocument l_XmlDocument{};
	std::filesystem::directory_iterator l_DirIter(i_Path);

	for (const auto& l_Entry : l_DirIter)
	{
		if (l_Entry.is_regular_file())
		{
			NodeInfo l_NodeInfo{};
			loadRCTemplate(l_Entry, l_NodeInfo);

			m_pTemplateNodeFileMap[i_MapKey].emplace_back(TemplateFileType::Template, l_NodeInfo.m_Name);
			registerNode(std::move(l_NodeInfo));
		}
		else if (l_Entry.is_directory())
		{
			const auto& l_CurDirPathStr = l_Entry.path().string();
			S32 l_Pos = l_CurDirPathStr.rfind("\\", l_CurDirPathStr.length());
			std::string l_CurDirParentPathStr = l_CurDirPathStr.substr(0, l_Pos);
			std::string l_CurDirStr = l_CurDirPathStr.substr(l_Pos+1, l_Entry.path().string().length());
			std::string l_PathKeyStr = l_CurDirStr + "##" + l_CurDirParentPathStr;

			m_pTemplateNodeFileMap[i_MapKey].emplace_back(TemplateFileType::Dir, l_PathKeyStr);
			registerNodes(l_Entry.path().string().c_str(), l_PathKeyStr);
		}
	}
}

void NodeManager::registerNode(NodeInfo&& i_Info) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = std::make_shared<NodeInfo>(std::move(i_Info));
	if (m_pRegisteredNodeInfoMap.find(l_pNodeInfo->m_Name) == m_pRegisteredNodeInfoMap.end())
	{
		m_pRegisteredNodeInfoMap.emplace(l_pNodeInfo->m_Name, l_pNodeInfo);
	}
	else
	{
		assert(false);
	}
}

void NodeManager::delByLinkIDSet(const std::set<LinkID>& i_LinkIDSet) noexcept
{
	auto l_It = i_LinkIDSet.begin();
	while(l_It != i_LinkIDSet.end())
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
		if (l_pStartNodeInfo->m_LinkIDSet.count(l_LinkID) > 0)
			l_pStartNodeInfo->m_LinkIDSet.erase(l_LinkID);
		if (l_pEndNodeInfo->m_LinkIDSet.count(l_LinkID) > 0)
			l_pEndNodeInfo->m_LinkIDSet.erase(l_LinkID);
		if (l_pStartPinInfo->m_LinkIDSet.count(l_LinkID) > 0)
			l_pStartPinInfo->m_LinkIDSet.erase(l_LinkID);
		if (l_pEndPinInfo->m_LinkIDSet.count(l_LinkID) > 0)
			l_pEndPinInfo->m_LinkIDSet.erase(l_LinkID);

		m_pLinkInfoMap.erase(l_LinkID);
		m_pCurLinkIDSet.erase(l_LinkID);
	}
}

NodeID NodeManager::addNodeInfo(const std::string& i_Name, NodeID i_CurParentNodeID) noexcept
{
	NodeID l_NodeID = 0;
	std::shared_ptr<NodeInfo> l_pRegisteredNodeInfo = m_pRegisteredNodeInfoMap[i_Name];
	assert(l_pRegisteredNodeInfo != nullptr);
	std::shared_ptr<NodeInfo> l_pNodeInfo = std::make_shared<NodeInfo>(*l_pRegisteredNodeInfo);
	assert(l_pNodeInfo != nullptr);
	
	l_NodeID = getNodeID();

	copyConnectionPinInfos<true>(l_NodeID, l_pRegisteredNodeInfo->m_ConnectionOutputPinIDs, l_pNodeInfo->m_TemplateConnectionOutputPinIDs, l_pNodeInfo->m_TemplateConnectionInputPinIDs);
	copyPinInfos<true>(l_NodeID, l_pRegisteredNodeInfo->m_InputPinIDs, l_pNodeInfo->m_TemplateInputPinIDs);
	copyPinInfos<true>(l_NodeID, l_pRegisteredNodeInfo->m_OutputPinIDs, l_pNodeInfo->m_TemplateOutputPinIDs);

	copyParams(l_pRegisteredNodeInfo->m_Params, l_pNodeInfo->m_TemplateParams);

	l_pNodeInfo->m_TemplateName = l_pRegisteredNodeInfo->m_Name;
	if (i_CurParentNodeID == 0)
		l_pNodeInfo->m_ParentNodeID = m_CurParentNodeID;
	else
		l_pNodeInfo->m_ParentNodeID = i_CurParentNodeID;

	m_pNodeInfoMap[l_NodeID] = l_pNodeInfo;

	//直
	if(l_pNodeInfo->m_ParentNodeID == m_CurParentNodeID)
		m_pCurNodeIDSet.emplace(l_NodeID);
	return l_NodeID;
}

NodeID NodeManager::copyNodeInfo(NodeID i_NodeID) noexcept
{
	NodeID l_NodeID = 0;
	std::shared_ptr<NodeInfo> l_pSrcNodeInfo = m_pNodeInfoMap[i_NodeID];
	assert(l_pSrcNodeInfo);
	std::shared_ptr<NodeInfo> l_pNodeInfo = std::make_shared<NodeInfo>(*l_pSrcNodeInfo);
	assert(l_pNodeInfo);

	l_NodeID = getNodeID();

	copyConnectionPinInfos<false>(l_NodeID, l_pSrcNodeInfo->m_TemplateConnectionInputPinIDs, l_pNodeInfo->m_TemplateConnectionOutputPinIDs, l_pNodeInfo->m_TemplateConnectionInputPinIDs);
	copyPinInfos<false>(l_NodeID, l_pSrcNodeInfo->m_TemplateInputPinIDs, l_pNodeInfo->m_TemplateInputPinIDs);
	copyPinInfos<false>(l_NodeID, l_pSrcNodeInfo->m_TemplateOutputPinIDs, l_pNodeInfo->m_TemplateOutputPinIDs);
	copyParams(l_pSrcNodeInfo->m_TemplateParams, l_pNodeInfo->m_TemplateParams);

	copyConnectionPinInfos<false>(l_NodeID, l_pSrcNodeInfo->m_ConnectionInputPinIDs, l_pNodeInfo->m_ConnectionOutputPinIDs, l_pNodeInfo->m_ConnectionInputPinIDs);
	copyPinInfos<false>(l_NodeID, l_pSrcNodeInfo->m_InputPinIDs, l_pNodeInfo->m_InputPinIDs);
	copyPinInfos<false>(l_NodeID, l_pSrcNodeInfo->m_OutputPinIDs, l_pNodeInfo->m_OutputPinIDs);
	copyParams(l_pSrcNodeInfo->m_Params, l_pNodeInfo->m_Params);

	l_pNodeInfo->m_ParentNodeID = m_CurParentNodeID;

	return l_NodeID;
}

Bool NodeManager::delNodeInfo(NodeID i_NodeID) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_pNodeInfoMap[i_NodeID];
	if (!l_pNodeInfo) return false;

	delByLinkIDSet(l_pNodeInfo->m_LinkIDSet);

	for (auto l_PinID : l_pNodeInfo->m_TemplateConnectionOutputPinIDs)
		m_pPinInfoMap.erase(l_PinID);
	for (auto l_PinID : l_pNodeInfo->m_TemplateConnectionInputPinIDs)
		m_pPinInfoMap.erase(l_PinID);
	for (auto l_PinID : l_pNodeInfo->m_TemplateInputPinIDs)
		m_pPinInfoMap.erase(l_PinID);
	for (auto l_PinID : l_pNodeInfo->m_TemplateOutputPinIDs)
		m_pPinInfoMap.erase(l_PinID);

	for (auto l_PinID : l_pNodeInfo->m_ConnectionOutputPinIDs)
		m_pPinInfoMap.erase(l_PinID);
	for (auto l_PinID : l_pNodeInfo->m_ConnectionInputPinIDs)
		m_pPinInfoMap.erase(l_PinID);
	for (auto l_PinID : l_pNodeInfo->m_InputPinIDs)
		m_pPinInfoMap.erase(l_PinID);
	for (auto l_PinID : l_pNodeInfo->m_OutputPinIDs)
		m_pPinInfoMap.erase(l_PinID);

	m_pNodeInfoMap.erase(i_NodeID);
	m_pCurNodeIDSet.erase(i_NodeID);

	return true;
}

std::pair<NodeID, std::shared_ptr<NodeInfo>> NodeManager::findNodeByGUID(const std::string& i_GUID) noexcept
{
	for (auto& l_pNodeInfo : m_pNodeInfoMap)
	{
		if (l_pNodeInfo.second->m_GUID == i_GUID)
			return l_pNodeInfo;
	}

	return {0, nullptr};
}

LinkID NodeManager::addLinkInfo(PinID i_StartPinID, PinID i_EndPinID, NodeID i_CurParentNodeID) noexcept
{
	std::shared_ptr<PinInfo> l_pStartPinInfo = m_pPinInfoMap[i_StartPinID];
	std::shared_ptr<PinInfo> l_pEndPinInfo = m_pPinInfoMap[i_EndPinID];
	if (!l_pStartPinInfo || !l_pEndPinInfo) return false;

	std::shared_ptr<NodeInfo> l_pStartNodeInfo = m_pNodeInfoMap[l_pStartPinInfo->m_NodeID];
	std::shared_ptr<NodeInfo> l_pEndNodeInfo = m_pNodeInfoMap[l_pEndPinInfo->m_NodeID];
	if (!l_pStartNodeInfo || !l_pEndNodeInfo) return false;

	//const auto& l_EditorStyle = ax::NodeEditor::GetStyle();
	//const ImColor l_SingleLinkColor = l_EditorStyle.Colors[ax::NodeEditor::StyleColor_SingleLink];
	//const ImColor l_MultipleLinkColor = l_EditorStyle.Colors[ax::NodeEditor::StyleColor_MultipleLink];
	const ImColor l_SingleLinkColor = ImColor(237, 34, 36, 255);
	const ImColor l_MultipleLinkColor = ImColor(118, 185, 0, 255);
	ImColor l_Color = l_MultipleLinkColor;
	if (l_pStartPinInfo->m_PinAccessType == PinAccessType::Writable || l_pEndPinInfo->m_PinAccessType == PinAccessType::Writable)
		l_Color = l_SingleLinkColor;

	if (l_pEndPinInfo->m_LinkIDSet.size() > 0)
		delByLinkIDSet(l_pEndPinInfo->m_LinkIDSet);

	LinkID l_LinkID = getLinkID();
	std::shared_ptr<LinkInfo> l_LinkInfo = std::make_shared<LinkInfo>(LinkInfo{ l_pStartPinInfo->m_NodeID, i_StartPinID, l_pEndPinInfo->m_NodeID, i_EndPinID, l_Color });
	m_pLinkInfoMap[l_LinkID] = l_LinkInfo;

	//只加载当前层link
	if (i_CurParentNodeID == 0)
		i_CurParentNodeID = m_CurParentNodeID;
	if(l_pStartNodeInfo->m_ParentNodeID == i_CurParentNodeID)
		m_pCurLinkIDSet.emplace(l_LinkID);

	l_pStartPinInfo->m_LinkIDSet.emplace(l_LinkID);
	l_pEndPinInfo->m_LinkIDSet.emplace(l_LinkID);
	l_pStartNodeInfo->m_LinkIDSet.emplace(l_LinkID);
	l_pEndNodeInfo->m_LinkIDSet.emplace(l_LinkID);

	return l_LinkID;
}

Bool NodeManager::canAddLink(PinID i_CurSelPinID, PinID i_OtherPinID) noexcept
{
	if (i_CurSelPinID == i_OtherPinID)
		return false;

	std::shared_ptr<PinInfo> l_pStartPinInfo = m_pPinInfoMap[i_CurSelPinID];
	std::shared_ptr<PinInfo> l_pEndPinInfo = m_pPinInfoMap[i_OtherPinID];

	assert(l_pStartPinInfo && l_pEndPinInfo);

	if (l_pStartPinInfo->m_PinType == PinType::Input)
	{
		std::swap(l_pStartPinInfo, l_pEndPinInfo);
	}

	if ((!l_pEndPinInfo->m_MetaData.empty() && (l_pStartPinInfo->m_MetaData != l_pEndPinInfo->m_MetaData)) || (l_pStartPinInfo->m_NodeID == l_pEndPinInfo->m_NodeID) 
		|| (l_pStartPinInfo->m_PinType == l_pEndPinInfo->m_PinType) || (l_pStartPinInfo->m_DataType != l_pEndPinInfo->m_DataType) || (l_pStartPinInfo->m_PinAccessType != l_pEndPinInfo->m_PinAccessType))
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

	if (l_pStartNodeInfo->m_LinkIDSet.count(i_LinkID) > 0)
		l_pStartNodeInfo->m_LinkIDSet.erase(i_LinkID);
	if (l_pEndNodeInfo->m_LinkIDSet.count(i_LinkID) > 0)
		l_pEndNodeInfo->m_LinkIDSet.erase(i_LinkID);
	if (l_pStartPinInfo->m_LinkIDSet.count(i_LinkID) > 0)
		l_pStartPinInfo->m_LinkIDSet.erase(i_LinkID);
	if (l_pEndPinInfo->m_LinkIDSet.count(i_LinkID) > 0)
		l_pEndPinInfo->m_LinkIDSet.erase(i_LinkID);

	m_pLinkInfoMap.erase(i_LinkID);
	m_pCurLinkIDSet.erase(i_LinkID);

	return true;
}

void NodeManager::copyParams(const std::vector<EditorParamInfo>& i_SrcParams, std::vector<EditorParamInfo>& o_DstParams) noexcept
{
	for (const auto& i_ParamInfo : i_SrcParams)
	{
		o_DstParams.emplace_back(i_ParamInfo.m_Name, i_ParamInfo.m_DataType, i_ParamInfo.m_Data, i_ParamInfo.m_UseRef, i_ParamInfo.m_RefKey);
	}
}

Bool NodeManager::updateCurInfo() noexcept
{
	m_pCurNodeIDSet.clear();
	m_pCurLinkIDSet.clear();

	U32 l_CurNodeIndex = 0;
	for (auto& l_pNode : m_pNodeInfoMap)
	{
		if (l_pNode.second->m_ParentNodeID == m_CurParentNodeID)
			m_pCurNodeIDSet.emplace(l_pNode.first);
	}

	for (auto& l_pLink : m_pLinkInfoMap)
	{
		if ((m_pCurNodeIDSet.count(l_pLink.second->m_OutputNodeID) > 0) || (m_pCurNodeIDSet.count(l_pLink.second->m_InputNodeID) > 0))
			m_pCurLinkIDSet.emplace(l_pLink.first);
	}

	return true;
}

RCEDITOR_NAMESPACE_END
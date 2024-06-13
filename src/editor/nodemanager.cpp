#include "nodemanager.h"

RCEDITOR_NAMESPACE_BEGIN



NodeManager::NodeManager() noexcept
{
	initData();
}

NodeManager::~NodeManager() noexcept
{
}

void NodeManager::initData() noexcept
{
	m_pCurSubRCInfo = std::make_shared<SubRCInfo>();
	m_pSubRCInfoMap[m_CurParentNodeID] = m_pCurSubRCInfo;
	m_pCurTaskTagInfo = std::make_shared<TaskTagInfo>();
	m_pTaskTagInfoMap[m_CurParentNodeID] = m_pCurTaskTagInfo;
}

void NodeManager::cleatData() noexcept
{
	m_NodePrototypeFileMap.clear();
	m_pNodePrototypeMap.clear();
	clearRunningData();
}

void NodeManager::loadPrototype(std::string i_Path) noexcept
{
	//m_RCTemplatePath = "G:\\rceditor\\rctemplate";
	if (!i_Path.empty())
	{
		cleatData();
		registerNodePrototypes(i_Path, i_Path);
	}
}

void NodeManager::clearRunningData() noexcept
{
	m_pNodeInfoMap.clear();
	m_pLinkInfoMap.clear();
	m_pPinInfoMap.clear();
	m_pCurSubRCInfo = nullptr;
	m_pSubRCInfoMap.clear();
	m_pCurTaskTagInfo = nullptr;
	m_pTaskTagInfoMap.clear();

	m_TagNodeChangeNodeID = 0;
	m_IsTagNodeChange = false;
	m_CurParentNodeID = 0;
	m_CurTagNodeID = 0;
	initData();
}

void NodeManager::addSubRCInfo(NodeID i_NodeID) noexcept
{
	m_pSubRCInfoMap[i_NodeID] = std::make_shared<SubRCInfo>();
}

void NodeManager::delSubRCInfo(NodeID i_NodeID) noexcept
{
	auto l_pNodeInfo = nodeInfo(i_NodeID);
	auto l_pSubRCInfo = getSubRCInfo(i_NodeID);
	if (l_pNodeInfo && l_pSubRCInfo)
	{
		for (auto l_TempNodeID : l_pSubRCInfo->m_NodeIDList)
		{
			delNodeInfo(l_TempNodeID, false);
		}

		m_pSubRCInfoMap.erase(i_NodeID);
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
		m_pCurSubRCInfo = m_pSubRCInfoMap[i_NodeID];
	}
	else {
		selTaskTagInfo(i_NodeID);
		m_pCurTaskTagInfo->m_IsShow = true;
	}
	m_TagNodeChangeNodeID = i_NodeID;
	m_IsTagNodeChange = true;
}

void NodeManager::closeTaskTagInfo(NodeID i_NodeID) noexcept
{
	m_pCurTaskTagInfo->m_IsShow = false;
	if (m_CurTagNodeID == i_NodeID)
	{
		m_CurParentNodeID = 0;
		m_CurTagNodeID = 0;
		m_IsTagNodeChange = true;
		m_TagNodeChangeNodeID = 0;
		m_pCurTaskTagInfo = m_pTaskTagInfoMap[m_CurTagNodeID];
		m_pCurSubRCInfo = m_pSubRCInfoMap[m_CurTagNodeID];
	}
}

void NodeManager::delTaskTagInfo(NodeID i_NodeID) noexcept
{
	m_pTaskTagInfoMap.erase(i_NodeID);
}

void NodeManager::selTaskTagInfo(NodeID i_NodeID) noexcept
{
	if (m_CurTagNodeID != i_NodeID && m_pTaskTagInfoMap.find(i_NodeID) != m_pTaskTagInfoMap.end())
	{
		m_IsTagNodeChange = true;
		m_CurTagNodeID = i_NodeID;
		m_pCurTaskTagInfo = m_pTaskTagInfoMap[i_NodeID];
		if (!m_pCurTaskTagInfo->m_NodeIDStack.empty())
			m_CurParentNodeID = m_pCurTaskTagInfo->m_NodeIDStack.top();
		else
			m_CurParentNodeID = i_NodeID;
		m_pCurSubRCInfo = m_pSubRCInfoMap[m_CurParentNodeID];
	}
}

void NodeManager::pushCurParentNodeID(NodeID i_NodeID) noexcept
{
	m_CurParentNodeID = i_NodeID;
	m_pCurTaskTagInfo->m_NodeIDStack.push(m_CurParentNodeID);
	m_pCurSubRCInfo = getSubRCInfo(m_CurParentNodeID);
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

	m_pCurSubRCInfo = getSubRCInfo(m_CurParentNodeID);
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
			if (!l_pNodeInfo->isTaskNode()) {
				i_StackTitleStr = "->" + l_pNodeInfo->nodeName() + i_StackTitleStr;
				getCurStackStr(l_pNodeInfo->getParentNodeID(), i_StackTitleStr);
			}
			else {
				i_StackTitleStr = l_pNodeInfo->nodeName() + i_StackTitleStr;
			}
		}
	}
}

void NodeManager::registerNodePrototypes(std::filesystem::path i_Path, const std::string& i_MapKey) noexcept
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

			std::shared_ptr<NodePrototype> l_pNodePrototype{};
			tinyxml2::XMLElement* l_XmlElementNode = l_XMLDocument.RootElement();

			if (!l_XmlElementNode->FindAttribute(g_XmlTemplateNameStr.data())) {
				auto l_pTemplateNodePrototype = std::make_shared<TemplateNodePrototype>();
				l_pNodePrototype = std::dynamic_pointer_cast<NodePrototype>(l_pTemplateNodePrototype);
			}
			else {
				EnumType l_NodeType = { EnumName::NodeType, static_cast<S64>(NodeType::GPU) };
				l_NodeType.setEnumValue(l_XmlElementNode->FindAttribute(g_XmlNodeTypeStr.data())->Value());

				if (l_NodeType == NodeType::CPU || l_NodeType == NodeType::GPU)
				{
					std::shared_ptr<FavoriteNormalNodePrototype> l_pFavoriteNodeInfo = std::make_shared<FavoriteNormalNodePrototype>();
					l_pNodePrototype = std::dynamic_pointer_cast<NodePrototype>(l_pFavoriteNodeInfo);
				}
				else if (l_NodeType == NodeType::Composite)
				{
					std::shared_ptr<FavoriteCompositeNodePrototype> l_pFavoriteNodeInfo = std::make_shared<FavoriteCompositeNodePrototype>();
					l_pNodePrototype = std::dynamic_pointer_cast<NodePrototype>(l_pFavoriteNodeInfo);
				}
				else continue;
			}

			l_pNodePrototype->loadData(l_XmlElementNode);

			m_NodePrototypeFileMap[i_MapKey].emplace_back(PrototypeFileType::Prototype, l_pNodePrototype->getNodeName());
			registerNodePrototype(l_pNodePrototype);
		}
		else if (l_Entry.is_directory())
		{
			const auto& l_CurDirPathStr = l_Entry.path().string();
			S32 l_Pos = l_CurDirPathStr.rfind("\\", l_CurDirPathStr.length());
			std::string l_CurDirParentPathStr = l_CurDirPathStr.substr(0, l_Pos);
			std::string l_CurDirStr = l_CurDirPathStr.substr(l_Pos+1, l_Entry.path().string().length());
			std::string l_PathKeyStr = l_CurDirStr + "##" + l_CurDirParentPathStr;

			m_NodePrototypeFileMap[i_MapKey].emplace_back(PrototypeFileType::Dir, l_PathKeyStr);
			m_NodePrototypeFileMap[l_PathKeyStr] = {};
			registerNodePrototypes(l_Entry.path().string().c_str(), l_PathKeyStr);
		}
	}
}

void NodeManager::registerNodePrototype(std::shared_ptr<NodePrototype> i_pTemplateNodeInfo) noexcept
{
	if (m_pNodePrototypeMap.find(i_pTemplateNodeInfo->getNodeName()) == m_pNodePrototypeMap.end())
	{
		m_pNodePrototypeMap.emplace(i_pTemplateNodeInfo->getNodeName(), i_pTemplateNodeInfo);
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
		m_pCurSubRCInfo->m_LinkIDList.erase(l_LinkID);
	}
}

std::shared_ptr<NodeInfo> NodeManager::instanceNodeInfo(std::string_view i_NodePrototypeName, NodeID i_ParentNodeID, NodeID i_NodeID) noexcept
{
	std::shared_ptr<NodePrototype> l_pNodePrototype = m_pNodePrototypeMap[std::string(i_NodePrototypeName)];
	assert(l_pNodePrototype != nullptr);

	return l_pNodePrototype->instanceNodeInfo(this, i_ParentNodeID, i_NodeID);
}

Bool NodeManager::delNodeInfo(NodeID i_NodeID, Bool i_IsDelNodeIDSet) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_pNodeInfoMap[i_NodeID];
	if (!l_pNodeInfo) return false;
	std::shared_ptr<SubRCInfo> l_pSubRCInfo = getSubRCInfo(l_pNodeInfo->getParentNodeID());
	assert(l_pSubRCInfo != nullptr);

	if (l_pNodeInfo->getNodeType() == NodeType::Composite)
		delSubRCInfo(i_NodeID);

	l_pNodeInfo->delNodeInfo(this);
	m_pNodeInfoMap.erase(i_NodeID);
	if(i_IsDelNodeIDSet)
		l_pSubRCInfo->m_NodeIDList.erase(i_NodeID);

	return true;
}

std::vector<std::shared_ptr<NodeInfo>> NodeManager::searchNodeList(std::string_view i_NodeKey, SearchType i_SearchType) noexcept
{
	std::vector<std::shared_ptr<NodeInfo>> l_pFindNodeInfoList{};
	if (i_NodeKey.empty()) return l_pFindNodeInfoList;

	auto _serachNode = [&i_NodeKey, &i_SearchType, &l_pFindNodeInfoList](std::shared_ptr<NodeInfo> i_pNodeInfo){
		std::string l_LowerNodeKey = toLower(i_NodeKey.data());
		if (!i_pNodeInfo) return;

		if (i_SearchType == SearchType::Node)
		{
			auto l_LowerName = toLower(i_pNodeInfo->getNodeName());
			if (l_LowerName.find(l_LowerNodeKey) != std::string::npos)
				l_pFindNodeInfoList.push_back(i_pNodeInfo);
		}
		else if (i_SearchType == SearchType::Template)
		{
			auto l_LowerName = toLower(i_pNodeInfo->getTemplateName());
			if (l_LowerName.find(l_LowerNodeKey) != std::string::npos)
				l_pFindNodeInfoList.push_back(i_pNodeInfo);
		}
	};

	for (auto l_It : m_pNodeInfoMap)
		_serachNode(l_It.second);

	return l_pFindNodeInfoList;
}

void NodeManager::highLightSearchNode(std::string_view i_NodeKey, SearchType i_SearchType) noexcept
{
	for (auto l_It : m_pNodeInfoMap)
	{
		l_It.second->setIsHighLight(false, this);
	}

	auto l_SearchNodeList = searchNodeList(i_NodeKey, i_SearchType);
	for (auto l_pNodeInfo : l_SearchNodeList)
	{
		l_pNodeInfo->setIsHighLight(true, this);
	}
}

LinkID NodeManager::addLinkInfo(PinID i_StartPinID, PinID i_EndPinID) noexcept
{
	std::shared_ptr<PinInfo> l_pStartPinInfo = m_pPinInfoMap[i_StartPinID];
	std::shared_ptr<PinInfo> l_pEndPinInfo = m_pPinInfoMap[i_EndPinID];
	if (!l_pStartPinInfo || !l_pEndPinInfo) return false;

	std::shared_ptr<NodeInfo> l_pStartNodeInfo = m_pNodeInfoMap[l_pStartPinInfo->getNodeID()];
	std::shared_ptr<NodeInfo> l_pEndNodeInfo = m_pNodeInfoMap[l_pEndPinInfo->getNodeID()];
	if (!l_pStartNodeInfo || !l_pEndNodeInfo) return false;

	const ImColor l_SingleLinkColor = ImColor(237, 34, 36, 255);
	const ImColor l_MultipleLinkColor = ImColor(118, 185, 0, 255);
	ImColor l_Color = l_MultipleLinkColor;
	if (l_pStartPinInfo->getPinAccessType() == PinAccessType::Writable || l_pEndPinInfo->getPinAccessType() == PinAccessType::Writable)
		l_Color = l_SingleLinkColor;

	if (l_pEndPinInfo->linkIDSet().size() > 0)
	{
		auto l_EndPinLinkInfo = linkInfo(*l_pEndPinInfo->linkIDSet().begin());
		
		if (l_EndPinLinkInfo->m_InputPinID == i_EndPinID && l_EndPinLinkInfo->m_OutputPinID == i_StartPinID) {
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
	//	m_pCurSubRCInfo->m_LinkIDList.insert(l_LinkID);
	// Load时需要加载所有层的link
	auto l_ParentNodeID = l_pStartNodeInfo->getParentNodeID();
	getSubRCInfo(l_ParentNodeID)->m_LinkIDList.insert(l_LinkID);

	l_pStartPinInfo->linkIDSet().emplace(l_LinkID);
	l_pEndPinInfo->linkIDSet().emplace(l_LinkID);
	l_pStartNodeInfo->linkIDSet().emplace(l_LinkID);
	l_pEndNodeInfo->linkIDSet().emplace(l_LinkID);

	return l_LinkID;
}

LinkType NodeManager::canAddLink(PinID i_CurSelPinID, PinID i_OtherPinID) noexcept
{
	if (i_CurSelPinID == i_OtherPinID)
		return LinkType::SamePin;

	std::shared_ptr<PinInfo> l_pStartPinInfo = m_pPinInfoMap[i_CurSelPinID];
	std::shared_ptr<PinInfo> l_pEndPinInfo = m_pPinInfoMap[i_OtherPinID];
	assert(l_pStartPinInfo && l_pEndPinInfo);
	if (l_pStartPinInfo->getNodeID() == l_pEndPinInfo->getNodeID())
		return LinkType::SameNode;
	if (l_pStartPinInfo->getPinType() == l_pEndPinInfo->getPinType())
		return LinkType::PinTypeDifferent;

	if (l_pStartPinInfo->getPinType() == PinType::Input)
	{
		std::swap(l_pStartPinInfo, l_pEndPinInfo);
	}
	if (!l_pEndPinInfo->getMetaData().empty() && (l_pStartPinInfo->getMetaData() != l_pEndPinInfo->getMetaData()))
		return LinkType::MetaDataDifferent;

	auto l_pStartNodeInfo = nodeInfo(l_pStartPinInfo->getNodeID());
	auto l_pEndNodeInfo = nodeInfo(l_pEndPinInfo->getNodeID());
	assert(l_pStartNodeInfo && l_pEndNodeInfo);

	if(l_pStartPinInfo->getNodeID() == l_pEndPinInfo->getNodeID())
		return LinkType::SameNode;
	if(l_pStartPinInfo->getDataType() != l_pEndPinInfo->getDataType())
		return LinkType::DataTypeDifferent;
	if (l_pStartNodeInfo->getNodeType() != NodeType::Start && l_pEndNodeInfo->getNodeType() != NodeType::End)
	{
		if(l_pStartPinInfo->getPinType() !=  PinAccessType::Writable && l_pStartPinInfo->getPinType() != l_pEndPinInfo->getPinType())
			return LinkType::PinAccessTypeError;
	}
	if (l_pStartNodeInfo->getNodeType() == NodeType::Start && l_pEndNodeInfo->getNodeType() == NodeType::End && l_pStartPinInfo->getPinAccessType() == PinAccessType::Readable && l_pEndPinInfo->getPinAccessType() == PinAccessType::Writable)
	{
		return LinkType::TaskPinAccessTypeError;
	}

	return LinkType::Success;
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
	m_pCurSubRCInfo->m_LinkIDList.erase(i_LinkID);

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
			if(l_pAssociationNodeInfo->isTaskNode())
				l_IsEndPoint = true;
			else
			{
				auto [l_InputID, l_OutputID] = l_pAssociationNodeInfo->getPinIDByKey(l_pInputPinInfo->getName());
				std::shared_ptr<PinInfo> l_pAssociationPinInfo{};
				if (l_pAssociationNodeInfo->getNodeType() == NodeType::Start) {
					l_pAssociationPinInfo = pinInfo(l_OutputID);
				}
				else if (l_pAssociationNodeInfo->getNodeType() == NodeType::Composite) {
					l_pAssociationPinInfo = pinInfo(l_OutputID);
				}

				if (l_pAssociationPinInfo->linkIDSet().empty())
					l_IsEndPoint = true;
			}
		}

		if (l_IsEndPoint) {
			RealLinkInfoHead l_RealLinkInfoHead{};
			l_RealLinkInfoHead.m_LinkID = l_It.first;
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
		if (!l_It->second.m_IsUse)
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

std::map<LinkID, RealLinkInfo> NodeManager::getFlodLinkInfo() noexcept
{
	std::map<LinkID, RealLinkInfo>l_RealLinkInfoMap{};
	std::vector<RealLinkInfoHead> l_RealLinkInfoHeadList{};
	std::map<std::string, LinkInfoToFlod> l_LinkInfoToFlodMap{};
	getLinkInfoToFlodMap(l_RealLinkInfoHeadList, l_LinkInfoToFlodMap);

	for (auto l_It : l_RealLinkInfoHeadList)
	{
		RealLinkInfo l_RealLinkInfo{};
		flodLink(l_It.m_FlodKey, l_RealLinkInfo, l_LinkInfoToFlodMap);
		l_RealLinkInfo.m_LinkInfoList.push_back(l_It.m_LinkInfo);
		l_RealLinkInfoMap[l_It.m_LinkID] = std::move(l_RealLinkInfo);
	}

	return l_RealLinkInfoMap;
}

RCEDITOR_NAMESPACE_END
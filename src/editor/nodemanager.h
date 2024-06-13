#pragma once

#include <set>
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <base/public/type.h>
#include "node.h"
#include "paraminfo.h"
#include "prototype.h"

RCEDITOR_NAMESPACE_BEGIN

enum class PrototypeFileType
{
	Dir = 0,
	Prototype = 1,
};

struct FileDetails
{
	PrototypeFileType m_Type = PrototypeFileType::Dir;
	std::string m_FilenameStr{};

	FileDetails() = default;
	FileDetails(PrototypeFileType i_Type, std::string i_FilenameStr){
		m_Type = i_Type;
		m_FilenameStr = i_FilenameStr;
	}
};

struct LinkInfo
{
	NodeID m_OutputNodeID = 0;
	LinkID m_OutputPinID = 0;
	NodeID m_InputNodeID = 0;
	LinkID m_InputPinID = 0;

	ImColor m_Color = ImColor(0.0f);
};

struct SubRCInfo
{
	std::set<NodeID> m_NodeIDList{};
	std::set<LinkID> m_LinkIDList{};
};

struct TaskTagInfo {
	Bool m_IsShow = true;
	U32 m_TabItemFlags = 0;
	F32 m_Zoom{};
	struct {
		F32 x;
		F32 y;
	} m_ViewScroll;
	struct {
		F32 xMin;
		F32 yMin;
		F32 xMax;
		F32 yMax;
	} m_VisibleRect;
	std::stack<NodeID> m_NodeIDStack;
};

struct LinkInfoToFlod
{
	Bool m_IsUse = false;
	LinkInfo m_LinkInfo{};
};

struct RealLinkInfoHead
{
	LinkID m_LinkID{};
	std::string m_FlodKey;
	LinkInfo m_LinkInfo{};
};

struct RealLinkInfo
{
	std::vector<LinkInfo> m_LinkInfoList{};
};

class NodeManager
{
public:
	NodeManager() noexcept;
	~NodeManager() noexcept;

public:
	void initData() noexcept;
	void cleatData() noexcept;
	void loadPrototype(std::string i_Path) noexcept;
	void clearRunningData() noexcept;

	void addSubRCInfo(NodeID i_NodeID) noexcept;
	void delSubRCInfo(NodeID i_NodeID) noexcept;

	void addTaskTagInfo(NodeID i_NodeID) noexcept;
	void closeTaskTagInfo(NodeID i_NodeID) noexcept;
	void delTaskTagInfo(NodeID i_NodeID) noexcept;
	void selTaskTagInfo(NodeID i_NodeID) noexcept;

	void pushCurParentNodeID(NodeID i_NodeID) noexcept;
	NodeID popCurParentNodeID() noexcept;
	void getCurStackStr(NodeID i_NodeID, std::string& i_StackTitleStr) noexcept;

public:
	void registerNodePrototypes(std::filesystem::path i_Path, const std::string& i_MapKey) noexcept;
	void registerNodePrototype(std::shared_ptr<NodePrototype> i_pNodePrototype) noexcept;

	std::shared_ptr<NodeInfo> instanceNodeInfo(std::string_view i_NodePrototypeName, NodeID i_ParentNodeID = 0, NodeID i_NodeID = 0) noexcept;

	void delInfoByLinkIDSet(const std::set<LinkID>& i_LinkIDSet) noexcept;
	Bool delNodeInfo(NodeID i_NodeID, Bool i_IsDelNodeIDSet = true) noexcept;
	std::vector<std::shared_ptr<NodeInfo>> searchNodeList(std::string_view i_NodeKey, SearchType i_SearchType) noexcept;
	void highLightSearchNode(std::string_view i_NodeKey, SearchType i_SearchType) noexcept;

	LinkID addLinkInfo(PinID i_StartPinID, PinID i_EndPinID) noexcept;
	LinkType canAddLink(PinID i_CurSelPinID, PinID i_OtherPinID) noexcept;
	Bool delLinkInfo(LinkID i_LinkID) noexcept;

	void getLinkInfoToFlodMap(std::vector<RealLinkInfoHead>& i_RealLinkInfoHeadList, std::map<std::string, LinkInfoToFlod>& i_LinkInfoToFlodMap) noexcept;
	void flodLink(std::string_view i_FlodKey, RealLinkInfo& i_LinkInfoList, std::map<std::string, LinkInfoToFlod>& i_LinkInfoToFlodMap) noexcept;
	std::map<LinkID, RealLinkInfo> getFlodLinkInfo() noexcept;

public:
	void clearID() noexcept { m_NextNodeID = NODE_ID_OFFSET; m_NextLinkID = LINK_ID_OFFSET; m_NextPinID = PIN_ID_OFFSET; }
	void loadNodeID(NodeID i_NodeID) noexcept { m_NextNodeID = i_NodeID; }
	NodeID curNodeID() noexcept { return m_NextNodeID; }
	NodeID getNodeID() noexcept { return m_NextNodeID++; }
	LinkID getLinkID() noexcept { return m_NextLinkID++; }
	PinID getPinID() noexcept { return m_NextPinID++; }
	TemplatePinID getTemplatePinID() noexcept { return m_NextTemplate++; }

public:
	const std::map<std::string, std::vector<FileDetails>>& nodePrototypeFileMap() const noexcept { return m_NodePrototypeFileMap; };
	std::map<std::string, std::vector<FileDetails>>& nodePrototypeFileMap() noexcept { return m_NodePrototypeFileMap; };

	const std::map<std::string, std::shared_ptr<NodePrototype>>& nodePrototypeMap() const noexcept { return m_pNodePrototypeMap; };
	std::map<std::string, std::shared_ptr<NodePrototype>>& nodePrototypeMap() noexcept { return m_pNodePrototypeMap; };

	std::map<NodeID, std::shared_ptr<NodeInfo>>& nodeMap() noexcept { return m_pNodeInfoMap; };
	std::map<LinkID, std::shared_ptr<LinkInfo>>& linkMap() noexcept { return m_pLinkInfoMap; };
	std::map<PinID, std::shared_ptr<PinInfo>>& pinMap() noexcept { return m_pPinInfoMap; };

	std::shared_ptr<NodeInfo> nodeInfo(NodeID i_ID) noexcept { auto l_It = m_pNodeInfoMap.find(i_ID); if (l_It != m_pNodeInfoMap.end()) return l_It->second; else return nullptr; }
	std::shared_ptr<LinkInfo> linkInfo(LinkID i_ID) noexcept { auto l_It = m_pLinkInfoMap.find(i_ID); if (l_It != m_pLinkInfoMap.end()) return l_It->second; else return nullptr; }
	std::shared_ptr<PinInfo> pinInfo(PinID i_ID) noexcept { auto l_It = m_pPinInfoMap.find(i_ID); if (l_It != m_pPinInfoMap.end()) return l_It->second; else return nullptr; }
	bool nodeOnUse(NodeID i_ID) noexcept { const auto l_pNodeInfo = nodeInfo(i_ID); if (l_pNodeInfo) return l_pNodeInfo->linkIDSet().size() > 0; return false; }

	const NodeID& curParentNodeID() const noexcept { return m_CurParentNodeID; }
	NodeID& curParentNodeID() noexcept { return m_CurParentNodeID; }

	std::shared_ptr<SubRCInfo> curSubRCInfo() noexcept { return m_pCurSubRCInfo; }
	const std::shared_ptr<SubRCInfo> curSubRCInfo() const noexcept { return m_pCurSubRCInfo; }

	std::map<NodeID, std::shared_ptr<SubRCInfo>>& subRCInfoMap() noexcept { return m_pSubRCInfoMap; }
	const std::map<NodeID, std::shared_ptr<SubRCInfo>>& subRCInfoMap() const noexcept { return m_pSubRCInfoMap; }

	std::shared_ptr<SubRCInfo> getSubRCInfo(NodeID i_NodeID) noexcept { return m_pSubRCInfoMap.at(i_NodeID); }
	const std::shared_ptr<SubRCInfo> getSubRCInfo(NodeID i_NodeID) const noexcept { return m_pSubRCInfoMap.at(i_NodeID); }
	
	NodeID getTagNodeChangeNodeID() noexcept { return m_TagNodeChangeNodeID; }
	void setTagNodeChangeNodeID(NodeID i_TagNodeChangeIndex) noexcept { m_TagNodeChangeNodeID = i_TagNodeChangeIndex; }

	Bool isTagNodeChange() const noexcept { return m_IsTagNodeChange; }
	void setTagNodeChange(Bool i_IsChange) noexcept { m_IsTagNodeChange = i_IsChange; }

	const NodeID& curTagNodeID() const noexcept { return m_CurTagNodeID; }
	NodeID& curTagNodeID() noexcept { return m_CurTagNodeID; }

	std::shared_ptr<TaskTagInfo> curTaskTagInfo() noexcept { return m_pCurTaskTagInfo; }
	const std::shared_ptr<TaskTagInfo> curTaskTagInfo() const noexcept { return m_pCurTaskTagInfo; }

	std::map<NodeID, std::shared_ptr<TaskTagInfo>>& taskTagInfoMap() noexcept { return m_pTaskTagInfoMap; }
	const std::map<NodeID, std::shared_ptr<TaskTagInfo>>& taskTagInfoMap() const noexcept { return m_pTaskTagInfoMap; }

	bool hasTaskTagInfo(NodeID i_NodeID) noexcept { if (m_pTaskTagInfoMap.find(i_NodeID) != m_pTaskTagInfoMap.end()) return true; return false; }

	std::shared_ptr<TaskTagInfo> getTaskTagInfo(NodeID i_NodeID) noexcept { return m_pTaskTagInfoMap.at(i_NodeID); }
	const std::shared_ptr<TaskTagInfo> getTaskTagInfo(NodeID i_NodeID) const noexcept { return m_pTaskTagInfoMap.at(i_NodeID); }

	////////////////////////////////////////

private:
	std::map<std::string, std::vector<FileDetails>> m_NodePrototypeFileMap{};
	std::map<std::string, std::shared_ptr<NodePrototype>> m_pNodePrototypeMap{};

	std::map<NodeID, std::shared_ptr<NodeInfo>> m_pNodeInfoMap{};
	std::map<LinkID, std::shared_ptr<LinkInfo>> m_pLinkInfoMap{};
	std::map<PinID, std::shared_ptr<PinInfo>> m_pPinInfoMap{};

	NodeID m_CurParentNodeID = 0;
	std::shared_ptr<SubRCInfo> m_pCurSubRCInfo{};
	std::map<NodeID, std::shared_ptr<SubRCInfo>> m_pSubRCInfoMap{};

	NodeID m_TagNodeChangeNodeID = 0;
	Bool m_IsTagNodeChange = false;
	NodeID m_CurTagNodeID = 0;
	std::shared_ptr<TaskTagInfo> m_pCurTaskTagInfo{};
	std::map<NodeID, std::shared_ptr<TaskTagInfo>> m_pTaskTagInfoMap{};

	NodeID m_NextNodeID = NODE_ID_OFFSET + 1;
	LinkID m_NextLinkID = LINK_ID_OFFSET + 1;
	PinID m_NextPinID = PIN_ID_OFFSET + 1;
	TemplatePinID m_NextTemplate = TEMPLATE_PIN_ID_OFFSET + 1;
};

RCEDITOR_NAMESPACE_END
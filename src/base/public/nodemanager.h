#pragma once

#include <set>
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <base/public/base.h>

RCEDITOR_NAMESPACE_BEGIN

enum class TemplateFileType
{
	Dir = 0,
	Template = 1,
};

struct FileDetails
{
	TemplateFileType m_Type = TemplateFileType::Dir;
	std::string m_FilenameStr{};

	FileDetails() = default;
	FileDetails(TemplateFileType i_Type, std::string i_FilenameStr){
		m_Type = i_Type;
		m_FilenameStr = i_FilenameStr;
	}
};

class NodeManager
{
public:
	//void buildFrom(const std::map<U64, NodeInfo>& i_NodeInfo, const std::map<U64, LinkInfo>& i_LinkInfo) noexcept;

	//const std::set<DString>& internalNodeNames() const noexcept { return m_InternalNodeNames; };

	//void initInternal() noexcept;

	//void nodeAddInputPin(Node& i_Node, const PinInfo& i_Info) noexcept;
	//void nodeAddOutputPin(Node& i_Node, const PinInfo& i_Info) noexcept;

	//std::string getNewNameWithCount(const DString& i_Name) noexcept;
	//std::string ModifyNameWithCount(const std::string& i_OldName, const std::string& i_Name) noexcept;

	//////////////////////////////////////////

	void clearID() noexcept { m_NextNodeID = NODE_ID_OFFSET; m_NextLinkID = LINK_ID_OFFSET; m_NextPinID = PIN_ID_OFFSET; }
	NodeID getNodeID() noexcept { return m_NextNodeID++; };
	LinkID getLinkID() noexcept { return m_NextLinkID++; }
	PinID getPinID() noexcept { return m_NextPinID++; }
	TemplatePinID getTemplatePinID() noexcept{ return m_NextTemplate++; }

public:
	NodeManager() noexcept;
	~NodeManager() noexcept;

	void clearRunningData() noexcept;

	const std::filesystem::path& templatePath() const noexcept { return m_RCTemplatePath; }
	const std::map<std::string, std::vector<FileDetails>>& templateNodeFileMap() const noexcept { return m_pTemplateNodeFileMap; };
	const std::map<std::string, std::shared_ptr<NodeInfo>>& registeredNodes() const noexcept { return m_pRegisteredNodeInfoMap; };
	std::map<NodeID, std::shared_ptr<NodeInfo>>& nodeMap() noexcept { return m_pNodeInfoMap; };
	std::map<LinkID, std::shared_ptr<LinkInfo>>& linkMap() noexcept { return m_pLinkInfoMap; };
	std::map<PinID, std::shared_ptr<PinInfo>>& pinMap() noexcept { return m_pPinInfoMap; };
	std::map<TemplatePinID, std::shared_ptr<PinInfo>>& templatePinMap() noexcept { return m_pTemplatePinInfoMap; };
	std::set<NodeID>& curNodeSet() noexcept { return m_pCurNodeIDSet; };
	std::set<LinkID>& curLinkSet() noexcept { return m_pCurLinkIDSet; };

	std::shared_ptr<NodeInfo> nodeInfo(NodeID i_ID) noexcept { auto l_It = m_pNodeInfoMap.find(i_ID); if (l_It != m_pNodeInfoMap.end()) return l_It->second; else return nullptr; }
	std::shared_ptr<LinkInfo> linkInfo(LinkID i_ID) noexcept { auto l_It = m_pLinkInfoMap.find(i_ID); if (l_It != m_pLinkInfoMap.end()) return l_It->second; else return nullptr; }
	std::shared_ptr<PinInfo> pinInfo(PinID i_ID) noexcept { auto l_It = m_pPinInfoMap.find(i_ID); if (l_It != m_pPinInfoMap.end()) return l_It->second; else return nullptr; }
	std::shared_ptr<PinInfo> templatePinInfo(TemplatePinID i_ID) noexcept { auto l_It = m_pTemplatePinInfoMap.find(i_ID); if (l_It != m_pTemplatePinInfoMap.end()) return l_It->second; else return nullptr; }
	bool nodeOnUse(NodeID i_ID) noexcept { const auto l_pNodeInfo = nodeInfo(i_ID); if (l_pNodeInfo) return l_pNodeInfo->m_LinkIDSet.size() > 0; return false; }

	const NodeID& curParentNodeID() const noexcept { return m_CurParentNodeID; }
	NodeID& curParentNodeID() noexcept { return m_CurParentNodeID; }
	const std::stack<NodeID>& curParentNodeIDStack() const noexcept { return m_CurParentNodeIDStack; }
	std::stack<NodeID>& curParentNodeIDStack() noexcept{ return m_CurParentNodeIDStack; }

	void loadRCTemplate(const std::filesystem::directory_entry& i_FilePath, NodeInfo& i_NodeInfo) noexcept;
	void saveRCTemplate(const std::string& i_FilePath, NodeID i_NodeID) noexcept;
	void registerNodes(std::filesystem::path i_Path, const std::string& i_MapKey) noexcept;
	void registerNode(NodeInfo&& i_Info) noexcept;

	void delByLinkIDSet(const std::set<LinkID>& i_LinkIDSet) noexcept;

	NodeID addNodeInfo(const std::string& i_Name, NodeID i_CurParentNodeID = 0) noexcept;
	NodeID copyNodeInfo(NodeID i_NodeID) noexcept;
	Bool delNodeInfo(NodeID i_NodeID) noexcept;
	std::pair<NodeID, std::shared_ptr<NodeInfo>> findNodeByGUID(const std::string& i_GUID) noexcept;

	LinkID addLinkInfo(PinID i_StartPinID, PinID i_EndPinID, NodeID i_CurParentNodeID = 0) noexcept;
	Bool canAddLink(PinID i_CurSelPinID, PinID i_OtherPinID) noexcept;
	Bool delLinkInfo(LinkID i_LinkID) noexcept;

	template<Bool IsFromTemplate>
	void copyPinInfos(NodeID i_NodeID, const std::vector<PinID>& l_SrcPinIDs, std::vector<PinID>& o_DstPinIDs) noexcept
	{
		for (auto l_SrcPinID : l_SrcPinIDs)
		{
			std::shared_ptr<PinInfo> l_pSrcPinInfo{};
			if (IsFromTemplate) {
				l_pSrcPinInfo = templatePinInfo(l_SrcPinID);
			}
			else {
				l_pSrcPinInfo = pinInfo(l_SrcPinID);
			}
			assert(l_pSrcPinInfo);

			PinID l_DstPinID = getPinID();
			std::shared_ptr<PinInfo> l_pDstPinInfo = std::make_shared<PinInfo>(*l_pSrcPinInfo);
			l_pDstPinInfo->m_NodeID = i_NodeID;
			m_pPinInfoMap[l_DstPinID] = l_pDstPinInfo;
			o_DstPinIDs.emplace_back(l_DstPinID);
		}
	}
	
	template<Bool IsFromTemplate>
	void copyConnectionPinInfos(NodeID i_NodeID, const std::vector<PinID>& l_SrcPinIDs, std::vector<PinID>& o_DstOutputPinIDs, std::vector<PinID>& o_DstInputPinIDs) noexcept
	{
		for (auto l_SrcPinID : l_SrcPinIDs)
		{
			std::shared_ptr<PinInfo> l_pSrcOutputPinInfo{};
			std::shared_ptr<PinInfo> l_pSrcInputPinInfo{};

			if (IsFromTemplate) {
				l_pSrcOutputPinInfo = templatePinInfo(l_SrcPinID);
				l_pSrcInputPinInfo = templatePinInfo(l_SrcPinID + 1);
			}
			else {
				l_pSrcOutputPinInfo = pinInfo(l_SrcPinID);
				l_pSrcInputPinInfo = pinInfo(l_SrcPinID + 1);
			}
			assert(l_pSrcInputPinInfo && l_pSrcOutputPinInfo);

			PinID l_DstOutputPinID = getPinID();
			std::shared_ptr<PinInfo> l_pDstOutputPinInfo = std::make_shared<PinInfo>(*l_pSrcOutputPinInfo);
			PinID l_DstInputPinID = getPinID();
			std::shared_ptr<PinInfo> l_pDstInputPinInfo = std::make_shared<PinInfo>(*l_pSrcInputPinInfo);

			l_pDstOutputPinInfo->m_NodeID = i_NodeID;
			l_pDstInputPinInfo->m_NodeID = i_NodeID;
			m_pPinInfoMap[l_DstOutputPinID] = l_pDstOutputPinInfo;
			m_pPinInfoMap[l_DstInputPinID] = l_pDstInputPinInfo;

			o_DstOutputPinIDs.emplace_back(l_DstOutputPinID);
			o_DstInputPinIDs.emplace_back(l_DstInputPinID);
		}
	}

	void copyParams(const std::vector<EditorParamInfo>& i_SrcParams, std::vector<EditorParamInfo>& o_DstParams) noexcept;

	template<PinType T>
	PinID addPinInfo(NodeID i_NodeID, const std::string& i_PinNameStr) noexcept
	{
		PinInfo l_PinInfo{};
		l_PinInfo.m_NodeID = i_NodeID;
		l_PinInfo.m_Name = i_PinNameStr;
		l_PinInfo.m_DataType = EditorAllDataType::Int;
		l_PinInfo.m_PinType = T;
		l_PinInfo.m_PinAccessType = PinAccessType::Readable;

		if (m_pNodeInfoMap[i_NodeID])
		{
			if (T == PinType::Connection)
			{
				l_PinInfo.m_PinAccessType = PinAccessType::Writable;
				l_PinInfo.m_IsConnection = true;

				PinID l_OutputPinID = getPinID();
				l_PinInfo.m_PinType = PinType::Output;
				std::shared_ptr<PinInfo> l_pOutputPinInfo = std::make_shared<PinInfo>(l_PinInfo);
				m_pPinInfoMap[l_OutputPinID] = l_pOutputPinInfo;
				m_pNodeInfoMap[i_NodeID]->m_ConnectionOutputPinIDs.emplace_back(l_OutputPinID);

				PinID l_InputPinID = getPinID();
				l_PinInfo.m_PinType = PinType::Input;
				std::shared_ptr<PinInfo> l_pInputPinInfo = std::make_shared<PinInfo>(l_PinInfo);
				m_pPinInfoMap[l_InputPinID] = l_pInputPinInfo;
				m_pNodeInfoMap[i_NodeID]->m_ConnectionInputPinIDs.emplace_back(l_InputPinID);

				return l_InputPinID;
			}
			else if (T == PinType::Input)
			{
				PinID l_PinID = getPinID();
				std::shared_ptr<PinInfo> l_pPinInfo = std::make_shared<PinInfo>(l_PinInfo);
				m_pPinInfoMap[l_PinID] = l_pPinInfo;
				m_pNodeInfoMap[i_NodeID]->m_InputPinIDs.emplace_back(l_PinID);

				return l_PinID;
			}
			else if (T == PinType::Output)
			{
				PinID l_PinID = getPinID();
				std::shared_ptr<PinInfo> l_pPinInfo = std::make_shared<PinInfo>(l_PinInfo);
				m_pPinInfoMap[l_PinID] = l_pPinInfo;
				m_pNodeInfoMap[i_NodeID]->m_OutputPinIDs.emplace_back(l_PinID);

				return l_PinID;
			}
		}

		return 0;
	}

	template <PinType T>
	void delPinInfo(NodeID i_NodeID, U32 i_PinIndex) noexcept
	{
		std::shared_ptr<NodeInfo> l_pNodeInfo = m_pNodeInfoMap[i_NodeID];
		assert(l_pNodeInfo);

		PinID l_PinID{};
		if (PinType::Connection == T)
		{
			{
				l_PinID = l_pNodeInfo->m_ConnectionOutputPinIDs[i_PinIndex];
				std::shared_ptr<PinInfo> l_pPinInfo = pinInfo(l_PinID);
				assert(l_pPinInfo);

				delByLinkIDSet(l_pPinInfo->m_LinkIDSet);

				if (pinInfo(l_PinID))
					m_pPinInfoMap.erase(l_PinID);

				l_pNodeInfo->m_ConnectionOutputPinIDs.erase(l_pNodeInfo->m_ConnectionOutputPinIDs.begin() + i_PinIndex);
			}

			{
				l_PinID = l_pNodeInfo->m_ConnectionInputPinIDs[i_PinIndex];
				std::shared_ptr<PinInfo> l_pPinInfo = pinInfo(l_PinID);
				assert(l_pPinInfo);

				delByLinkIDSet(l_pPinInfo->m_LinkIDSet);

				if (pinInfo(l_PinID))
					m_pPinInfoMap.erase(l_PinID);

				l_pNodeInfo->m_ConnectionInputPinIDs.erase(l_pNodeInfo->m_ConnectionInputPinIDs.begin() + i_PinIndex);
			}
		}
		else if (PinType::Input == T)
		{
			l_PinID = l_pNodeInfo->m_InputPinIDs[i_PinIndex];
			std::shared_ptr<PinInfo> l_pPinInfo = pinInfo(l_PinID);
			assert(l_pPinInfo);

			delByLinkIDSet(l_pPinInfo->m_LinkIDSet);

			if (pinInfo(l_PinID))
				m_pPinInfoMap.erase(l_PinID);

			l_pNodeInfo->m_InputPinIDs.erase(l_pNodeInfo->m_InputPinIDs.begin() + i_PinIndex);
		}
		else if (PinType::Output == T)
		{
			l_PinID = l_pNodeInfo->m_OutputPinIDs[i_PinIndex];
			std::shared_ptr<PinInfo> l_pPinInfo = pinInfo(l_PinID);
			assert(l_pPinInfo);

			delByLinkIDSet(l_pPinInfo->m_LinkIDSet);

			if (pinInfo(l_PinID))
				m_pPinInfoMap.erase(l_PinID);

			l_pNodeInfo->m_OutputPinIDs.erase(l_pNodeInfo->m_OutputPinIDs.begin() + i_PinIndex);
		}

		return;
	}

	Bool updateCurInfo() noexcept;
	////////////////////////////////////////

private:
	std::map<std::string, std::vector<FileDetails>> m_pTemplateNodeFileMap{};
	std::map<std::string, std::shared_ptr<NodeInfo>> m_pRegisteredNodeInfoMap{};
	std::map<TemplatePinID, std::shared_ptr<PinInfo>> m_pTemplatePinInfoMap{};

	std::map<NodeID, std::shared_ptr<NodeInfo>> m_pNodeInfoMap{};
	std::map<LinkID, std::shared_ptr<LinkInfo>> m_pLinkInfoMap{};
	std::map<PinID, std::shared_ptr<PinInfo>> m_pPinInfoMap{};
	std::set<NodeID> m_pCurNodeIDSet{};
	std::set<LinkID> m_pCurLinkIDSet{};

	NodeID m_CurParentNodeID = 0;
	std::stack<NodeID> m_CurParentNodeIDStack;

	NodeID m_NextNodeID = NODE_ID_OFFSET + 1;
	LinkID m_NextLinkID = LINK_ID_OFFSET + 1;
	PinID m_NextPinID = PIN_ID_OFFSET + 1;
	TemplatePinID m_NextTemplate = TEMPLATE_PIN_ID_OFFSET + 1;

	std::filesystem::path m_RCTemplatePath{};
};

RCEDITOR_NAMESPACE_END
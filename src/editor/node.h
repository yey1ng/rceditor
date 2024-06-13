#pragma once

#include <base/public/type.h>
#include "prototype.h"

RCEDITOR_NAMESPACE_BEGIN

class PinInfo;
class PinInfoList;
class ParamInfo;
class ParamInfoList;
class NodePrototype;
class NodeManager;

class PinIDList
{
public:
	PinInfoList getPinInfoList(NodeManager* i_pNodeManager) noexcept;
	std::string getIndexListStr() noexcept;

	PinID addPinInfo(PinInfo& i_PinInfo, NodeManager* i_pNodeManager, S32 i_PinIndex = -1) noexcept;
	std::string delPinInfo(U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;
	void delAllPinInfo(NodeManager* i_pNodeManager) noexcept;

public:
	const std::vector<PinID> pinIDList() const noexcept { return m_PinIDList; }
	std::vector<PinID> pinIDList() noexcept { return m_PinIDList; }

private:
	std::vector<PinID> m_PinIDList{};
};

class NodeManager;
class NodeInfo
{
public:
	NodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID) noexcept;
	virtual ~NodeInfo() noexcept = default;

	void delNodeInfo(NodeManager* i_pNodeManager) noexcept;

	PinIDList& getPinIDList(Bool i_IsTemplate, Bool i_IsInputOutput, PinType i_PinType) noexcept;
	virtual void addPinInfo(PinInfo& i_pPinInfo, NodeManager* i_pNodeManager, S32 i_PinIndex = -1) noexcept;
	virtual void delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;

	virtual void addPinInfo2(PinInfo& i_pPinInfo, NodeManager* i_pNodeManager, S32 i_PinIndex = -1) noexcept { addPinInfo(i_pPinInfo, i_pNodeManager, i_PinIndex); }
	virtual void delPinInfo2(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept { delPinInfo(i_PinType, i_PinIndex, i_pNodeManager); }

	virtual void addParam(const ParamInfo& i_ParamInfo, Bool i_IsTemplate = false, S32 i_ParamIndex = -1) noexcept;
	virtual void delParam(U32 i_ParamIndex) noexcept;

	virtual void addParam2(const ParamInfo& i_ParamInfo, NodeManager* i_pNodeManager, Bool i_IsTemplate = false, S32 i_ParamIndex = -1) noexcept { addParam(i_ParamInfo, i_IsTemplate, i_ParamIndex); }
	virtual void delParam2(U32 i_ParamIndex, NodeManager* i_pNodeManager) noexcept { delParam(i_ParamIndex); }

	virtual bool canChangePinAccessType(PinType i_PinType) noexcept;
	std::string getIndexListStr(PinType i_PinType) noexcept;

	virtual void createChildNode(NodeManager* i_pNodeManager, NodeID i_StartNode = 0, NodeID i_EndNode = 0) noexcept {};
	virtual NodeID getAssociationNodeID(PinType i_PinType = PinType::Input) noexcept { return m_NodeID; }

public:
	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual void saveData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual std::shared_ptr<NodePrototype> getNodePrototype(std::string_view i_NewNodeName, NodeManager* i_pNodeManager, NodeID i_RelativeNodeID = 0) noexcept;

public:
	Bool isHighLight() const noexcept { return m_IsHighlight; }
	void setIsHighLight(Bool i_IsHighlight, NodeManager* i_pNodeManager) noexcept;

	ImColor getColor(NodeType i_Type) noexcept
	{
		auto l_Color = ImColor(255, 255, 255);

		if (isHighLight())
		{
			return ImColor(255, 255, 255);
		}
		else
		{
			switch (i_Type)
			{
			case NodeType::CPU:
				return ImColor(43, 162, 69);
			case NodeType::GPU:
				return ImColor(123, 104, 238);
			case NodeType::PS:
				return ImColor(123, 104, 238);
			case NodeType::CS:
				return ImColor(123, 104, 238);
			case NodeType::Start:
				return ImColor(12, 12, 89);
			case NodeType::End:
				return ImColor(12, 84, 12);
			case NodeType::Composite:
				return ImColor(85, 85, 12);
			case NodeType::Task:
				return ImColor(85, 85, 12);
			case NodeType::Data:
				return ImColor(123, 104, 238);
			//case NodeType::HostTask:
			//	return ImColor(85, 85, 12);
			//case NodeType::VulkanTask:
			//	return ImColor(85, 85, 12);
			default:
				l_Color = ImColor(255, 0, 0);
				break;
			}

			assert(false);
		}

		return l_Color;
	}

	ImColor	color() noexcept { return getColor(static_cast<NodeType>(m_NodeType.m_Value)); }

	void setNodeID(NodeID i_NodeID) noexcept { m_NodeID = i_NodeID; }
	NodeID getNodeID() const noexcept { return m_NodeID; }

	void setParentNodeID(const NodeID& i_ParentNodeID) noexcept { m_ParentNodeID = i_ParentNodeID; }
	const NodeID& getParentNodeID() const noexcept { return m_ParentNodeID; }

	void setTaskNodeID(const NodeID& i_TaskNodeID) noexcept { m_TaskNodeID = i_TaskNodeID; }
	const NodeID& getTaskNodeID() const noexcept { return m_TaskNodeID; }

	std::string& nodeName() noexcept { return m_NodeName; }
	void setNodeName(const std::string& i_NodeName) noexcept { m_NodeName = i_NodeName; }
	const std::string& getNodeName() const noexcept { return m_NodeName; }

	void setTemplateName(const std::string& i_TemplateName) noexcept { m_TemplateName = i_TemplateName; }
	const std::string& getTemplateName() const noexcept { return m_TemplateName; }

	void setClassName(const std::string& i_ClassName) noexcept { m_ClassName = i_ClassName; }
	const std::string& getClassName() const noexcept { return m_ClassName; }

	void setComment(const std::string& i_Comment) noexcept { m_Comment = i_Comment; }
	std::string& getComment() noexcept { return m_Comment; }
	const std::string& getComment() const noexcept { return m_Comment; }

	void setNodeType(NodeType i_NodeType) noexcept { m_NodeType = i_NodeType; }
	NodeType getNodeType() const noexcept { return static_cast<NodeType>(m_NodeType.m_Value); }

	Bool isTaskNode() noexcept { if (m_NodeType == NodeType::Task) return true; return false; }

	void setNodeTag(NodeTag i_NodeTag) noexcept { m_NodeTag = i_NodeTag; }
	NodeTag getNodeTag() const noexcept { return static_cast<NodeTag>(m_NodeTag.m_Value); }

	void setIsDynamicInputOutputPin(Bool isDynamic) noexcept { m_IsDynamicInputOutputPin = isDynamic; }
	Bool getIsDynamicInputOutputPin() const noexcept { return m_IsDynamicInputOutputPin; }

	void setIsDynamicInputPin(Bool isDynamic) noexcept { m_IsDynamicInputPin = isDynamic; }
	Bool getIsDynamicInputPin() const noexcept { return m_IsDynamicInputPin; }

	void setIsDynamicOutputPin(Bool isDynamic) noexcept { m_IsDynamicOutputPin = isDynamic; }
	Bool getIsDynamicOutputPin() const noexcept { return m_IsDynamicOutputPin; }

	void setIsDynamicParam(Bool isDynamic) noexcept { m_IsDynamicParam = isDynamic; }
	Bool getIsDynamicParam() const noexcept { return m_IsDynamicParam; }

	const auto& templateInputOutputInputPinIDList() const noexcept { return m_TemplateInputOutputInputPinIDList; }
	auto& templateInputOutputInputPinIDList() noexcept { return m_TemplateInputOutputInputPinIDList; }

	const auto& templateInputOutputOutputPinIDList() const noexcept { return m_TemplateInputOutputOutputPinIDList; }
	auto& templateInputOutputOutputPinIDList() noexcept { return m_TemplateInputOutputOutputPinIDList; }

	const auto& templateInputPinIDList() const noexcept { return m_TemplateInputPinIDList; }
	auto& templateInputPinIDList() noexcept { return m_TemplateInputPinIDList; }

	const auto& templateOutputPinIDList() const noexcept { return m_TemplateOutputPinIDList; }
	auto& templateOutputPinIDList() noexcept { return m_TemplateOutputPinIDList; }

	const auto& templateParamList() const noexcept { return m_TemplateParamList; }
	auto& templateParamList() noexcept { return m_TemplateParamList; }

	const auto& inputOutputInputPinIDList() const noexcept { return m_InputOutputInputPinIDList; }
	auto& inputOutputInputPinIDList() noexcept { return m_InputOutputInputPinIDList; }

	const auto& inputOutputOutputPinIDList() const noexcept { return m_InputOutputOutputPinIDList; }
	auto& inputOutputOutputPinIDList() noexcept { return m_InputOutputOutputPinIDList; }

	const auto& inputPinIDList() const noexcept { return m_InputPinIDList; }
	auto& inputPinIDList() noexcept { return m_InputPinIDList; }

	const auto& outputPinIDList() const noexcept { return m_OutputPinIDList; }
	auto& outputPinIDList() noexcept { return m_OutputPinIDList; }

	const auto& paramList() const noexcept { return m_ParamList; }
	auto& paramList() noexcept { return m_ParamList; }

	const auto& linkIDSet() const noexcept { return m_LinkIDSet; }
	auto& linkIDSet() noexcept { return m_LinkIDSet; }

	const auto& pinKeyMap() const noexcept { return m_PinKeyMap; }
	auto& pinKeyMap() noexcept { return m_PinKeyMap; }

	// InputPinID, OutputPinID
	std::pair<PinID, PinID> getPinIDByKey(std::string_view i_Key) noexcept { if (hasPinKey(i_Key)) return m_PinKeyMap[i_Key.data()]; else return { 0, 0 }; }
	bool hasPinKey(std::string_view i_Key) noexcept { return m_PinKeyMap.find(i_Key.data()) != m_PinKeyMap.end(); }
	void addPinKey(std::string_view i_Key, std::pair<PinID, PinID> i_PinIDPair) noexcept { m_PinKeyMap[i_Key.data()] = i_PinIDPair; }
	void delPinKey(std::string_view i_Key) noexcept { m_PinKeyMap.erase(i_Key.data()); }

	bool hasParamKey(std::string_view i_Key) noexcept { return m_ParamKeySet.find(i_Key.data()) != m_ParamKeySet.end(); }
	void addParamKey(std::string_view i_Key) noexcept { m_ParamKeySet.insert(i_Key.data()); }
	void delParamKey(std::string_view i_Key) noexcept { m_ParamKeySet.erase(i_Key.data()); }

protected:
	NodeID m_NodeID{};
	NodeID m_ParentNodeID{};
	NodeID m_TaskNodeID{};
	std::string	m_NodeName = "";
	std::string m_TemplateName = "";
	std::string m_ClassName{};
	std::string	m_Comment{};

	EnumType m_NodeType = { EnumName::NodeType, static_cast<S64>(NodeType::GPU) };
	EnumType m_NodeTag = { EnumName::NodeTag, static_cast<S64>(NodeTag::NoTag) };

	Bool m_IsDynamicInputOutputPin = false;
	Bool m_IsDynamicInputPin = false;
	Bool m_IsDynamicOutputPin = false;
	Bool m_IsDynamicParam = false;

	PinIDList m_TemplateInputOutputInputPinIDList{};
	PinIDList m_TemplateInputOutputOutputPinIDList{};
	PinIDList m_TemplateInputPinIDList{};
	PinIDList m_TemplateOutputPinIDList{};
	ParamInfoList m_TemplateParamList{};

	PinIDList m_InputOutputInputPinIDList{};
	PinIDList m_InputOutputOutputPinIDList{};
	PinIDList m_InputPinIDList{};
	PinIDList m_OutputPinIDList{};
	ParamInfoList m_ParamList{};

	std::set<LinkID> m_LinkIDSet{};

	std::map<std::string, std::pair<PinID, PinID>> m_PinKeyMap{};
	std::set<std::string> m_ParamKeySet{};

	Bool m_IsHighlight = false;
};

class NormalNodeInfo : public NodeInfo
{
public:
	NormalNodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept : NodeInfo(i_NodeID, i_ParentNodeID) {};
	virtual ~NormalNodeInfo() noexcept = default;
};

class ChildNodeInfo : public NodeInfo
{
public:
	ChildNodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept : NodeInfo(i_NodeID, i_ParentNodeID) {};
	virtual ~ChildNodeInfo() noexcept = default;

	virtual void addPinInfo2(PinInfo& i_pPinInfo, NodeManager* i_pNodeManager, S32 i_PinIndex = -1) noexcept;
	virtual void delPinInfo2(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;

	virtual void addParam2(const ParamInfo& i_ParamInfo, NodeManager* i_pNodeManager, Bool i_IsTemplate = false, S32 i_ParamIndex = -1) noexcept;
	virtual void delParam2(U32 i_ParamIndex, NodeManager* i_pNodeManager) noexcept;

	virtual bool canChangePinAccessType(PinType i_PinType) noexcept { return true; };
	virtual NodeID getAssociationNodeID(PinType i_PinType = PinType::Input) noexcept { return m_ParentNodeID; }

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual void saveData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
};

class CompositeNodeInfo : public NodeInfo
{
public:
	CompositeNodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept : NodeInfo(i_NodeID, i_ParentNodeID) {};
	virtual ~CompositeNodeInfo() noexcept = default;

	virtual void addPinInfo2(PinInfo& i_pPinInfo, NodeManager* i_pNodeManager, S32 i_PinIndex = -1) noexcept;
	virtual void delPinInfo2(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;

	virtual void addParam2(const ParamInfo& i_ParamInfo, NodeManager* i_pNodeManager, Bool i_IsTemplate = false, S32 i_ParamIndex = -1) noexcept;
	virtual void delParam2(U32 i_ParamIndex, NodeManager* i_pNodeManager) noexcept;

	virtual void createChildNode(NodeManager* i_pNodeManager, NodeID i_StartNode = 0, NodeID i_EndNode = 0) noexcept;

	virtual bool canChangePinAccessType(PinType i_PinType) noexcept { return true; };
	virtual NodeID getAssociationNodeID(PinType i_PinType = PinType::Input) noexcept { if (i_PinType == PinType::Input) return m_StartChildNodeID; else return m_EndChildNodeID; }

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual void saveData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;

public:
	void setStartChildNodeID(NodeID i_StartChildNodeID) noexcept { m_StartChildNodeID = i_StartChildNodeID; }
	void setEndChildNodeID(NodeID i_EndChildNodeID) noexcept { m_EndChildNodeID = i_EndChildNodeID; }

	const NodeID& getStartChildNodeID() const noexcept { return m_StartChildNodeID; }
	const NodeID& getEndChildNodeID() const noexcept { return m_EndChildNodeID; }

protected:
	NodeID m_StartChildNodeID{};
	NodeID m_EndChildNodeID{};
};

class TaskNodeInfo : public CompositeNodeInfo
{
public:
	TaskNodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept : CompositeNodeInfo(i_NodeID, i_ParentNodeID) {};
	virtual ~TaskNodeInfo() noexcept = default;

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual void saveData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
public:
	void setQueueFamilyType(EnumType i_QueueFamilyType) noexcept { m_QueueFamilyType = i_QueueFamilyType; }
	EnumType getQueueFamilyType() noexcept { return m_QueueFamilyType; }

protected:
	EnumType m_QueueFamilyType = { EnumName::TaskQueueFamily, static_cast<S64>(TaskQueueFamily::Graphic1) };
};

class DataNodeInfo : public NodeInfo
{
public:
	DataNodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept : NodeInfo(i_NodeID, i_ParentNodeID) {};
	virtual ~DataNodeInfo() noexcept = default;

};

RCEDITOR_NAMESPACE_END


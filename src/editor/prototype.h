#pragma once

#include <base/public/type.h>

RCEDITOR_NAMESPACE_BEGIN

class ParamInfo;
class NodeInfo;
class NodeManager;

struct PinInfo
{
	PinInfo() noexcept = default;
	PinInfo(const NodeID i_NodeID, const PinID i_PinID) noexcept;

	void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	tinyxml2::XMLElement* saveData(tinyxml2::XMLElement* i_XmlElement) noexcept;

public:
	static ImColor getPinColor(PinAccessType i_Type) noexcept
	{
		switch (i_Type)
		{
		case PinAccessType::Readable: return ImColor(118, 185, 0, 255);
		case PinAccessType::Writable: return ImColor(237, 34, 36, 255);
		}

		assert(false);
		return ImColor(255, 255, 255);
	}
	ImColor	color() noexcept { return getPinColor(static_cast<PinAccessType>(m_PinAccessType.m_Value)); }

	void setNodeID(NodeID i_NodeID) noexcept { m_NodeID = i_NodeID; }
	NodeID getNodeID() const noexcept { return m_NodeID; }

	void setPinID(PinID i_PinID) noexcept { m_PinID = i_PinID; }
	PinID getPinID() const noexcept { return m_PinID; }

	void setName(const std::string& i_Name) noexcept { m_Name = i_Name; }
	const std::string& getName() const noexcept { return m_Name; }

	void setMetaData(const std::string& i_MetaData) noexcept { if (m_PinType == PinType::Input) m_InputMetaData = i_MetaData; else if (m_PinType == PinType::Output) m_OutputMetaData = i_MetaData; }
	std::string getMetaData() const noexcept { std::string l_MetaDataStr = ""; if (m_PinType == PinType::Input) l_MetaDataStr = m_InputMetaData; else if (m_PinType == PinType::Output) l_MetaDataStr = m_OutputMetaData; return l_MetaDataStr; }

	std::string getInputMetaData() noexcept { return m_InputMetaData; }
	std::string getOutputMetaData() noexcept { return m_OutputMetaData; }

	void insertLinkID(LinkID i_LinkID) noexcept { m_LinkIDSet.insert(i_LinkID); }
	void erase(LinkID i_LinkID) noexcept { m_LinkIDSet.erase(i_LinkID); }

	const std::set<LinkID>& linkIDSet() const noexcept { return m_LinkIDSet; }
	std::set<LinkID>& linkIDSet() noexcept { return m_LinkIDSet; }

	void setIsTemplate(bool i_IsTemplate) noexcept { m_IsTemplate = i_IsTemplate; }
	bool getIsTemplate() const noexcept { return m_IsTemplate; }

	void setIsInputOutput(bool i_IsInputOutput) noexcept { m_IsInputOutput = i_IsInputOutput; }
	bool getIsInputOutput() const noexcept { return m_IsInputOutput; }

	EnumType getDataType() const noexcept { return m_DataType; }
	void setDataTypeValue(EditorDataType i_DataType) noexcept { m_DataType = i_DataType; }
	EditorDataType getDataTypeValue() const noexcept { return static_cast<EditorDataType>(m_DataType.m_Value); }

	EnumType getPinType() const noexcept { return m_PinType; }
	void setPinTypeValue(PinType i_PinType) noexcept { m_PinType = i_PinType; }
	PinType getPinTypeValue() const noexcept { return static_cast<PinType>(m_PinType.m_Value); }

	EnumType getPinAccessType() const noexcept { return m_PinAccessType; }
	void setPinAccessTypeValue(PinAccessType i_PinAccessType) noexcept { m_PinAccessType = i_PinAccessType; }
	PinAccessType getPinAccessTypeValue() const noexcept { return static_cast<PinAccessType>(m_PinAccessType.m_Value); }

public:
	NodeID				m_NodeID = 0;
	PinID				m_PinID = 0;
	std::string			m_Name = "";
	std::string			m_InputMetaData = "";
	std::string			m_OutputMetaData = "";
	std::set<LinkID>    m_LinkIDSet{};

	Bool				m_IsTemplate = false;
	Bool				m_IsInputOutput = false;
	EnumType			m_DataType = { EnumName::EditorDataType, static_cast<S64>(EditorDataType::Int) };
	EnumType			m_PinType = { EnumName::PinType, static_cast<S64>(PinType::Input) };
	EnumType			m_PinAccessType = { EnumName::PinAccessType, static_cast<S64>(PinAccessType::Readable) };
};

class PinIDList;
class PinInfoList
{
public:
	template<Bool IsTemplate, PinType PinListType>
	void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	template<Bool IsTemplate, PinType PinListType>
	void saveData(tinyxml2::XMLElement* i_XmlElement) noexcept;

public:
	const std::vector<PinInfo>& pinInfoList() const noexcept { return m_PinInfoList; }
	std::vector<PinInfo>& pinInfoList() noexcept { return m_PinInfoList; }

private:
	std::vector<PinInfo> m_PinInfoList{};
};

class VkBufferInfo
{
	EnumType getBufferUsage() const noexcept { return m_BufferUsage; }
	void setBufferUsageValue(VkBufferUsage i_PinAccessType) noexcept { m_BufferUsage = i_PinAccessType; }
	VkBufferUsage getBufferUsageValue() const noexcept { return static_cast<VkBufferUsage>(m_BufferUsage.m_Value); }

	auto getFormatStr() const noexcept { return m_FormatStr; };
	void setFormatStr(std::string i_Str) noexcept { m_FormatStr = i_Str; }

	auto& bufferStruct() noexcept { return m_BufferStruct; }
	const auto& bufferStruct() const noexcept { return m_BufferStruct; }

private:
	EnumType m_BufferUsage = { EnumName::VkBufferUsage, static_cast<S64>(VkBufferUsage::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) };
	std::string m_FormatStr{};
	std::map<std::string, std::string> m_BufferStruct;
};

class VkImageInfo
{
	EnumType getImageUsage() const noexcept { return m_ImageUsage; }
	void setImageUsageValue(VkImageUsage i_PinAccessType) noexcept { m_ImageUsage = i_PinAccessType; }
	VkImageUsage getImageUsageValue() const noexcept { return static_cast<VkImageUsage>(m_ImageUsage.m_Value); }

	auto getFormatStr() const noexcept { return m_FormatStr; };
	void setFormatStr(std::string i_Str) noexcept { m_FormatStr = i_Str; }

private:
	EnumType m_ImageUsage = { EnumName::VkImageUsage, static_cast<S64>(VkImageUsage::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) };
	std::string m_FormatStr{};
};

class ParamInfoList
{
public:
	void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	void loadTemplateData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	void saveData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	void saveTemplateData(tinyxml2::XMLElement* i_XmlElement) noexcept;

	std::string getIndexListStr() noexcept;

public:
	const std::vector<ParamInfo>& paramInfoList() const noexcept { return m_ParamInfoList; }
	std::vector<ParamInfo>& paramInfoList() noexcept { return m_ParamInfoList; }

private:
	std::vector<ParamInfo> m_ParamInfoList;
};

class NodePrototype
{
public:
	NodePrototype() = default;
	virtual ~NodePrototype() = default;
	static std::shared_ptr<NodePrototype> createNodePrototype(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual void initData(std::shared_ptr<NodeInfo> i_pNodeInfo, std::string_view i_NewNodeName, NodeManager* i_pNodeManager, NodeID& i_RelativeNodeID) noexcept;

public:
	virtual void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual void saveData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual std::shared_ptr<NodeInfo> instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_NodeID = 0) noexcept;

public:
	void setNodeName(const std::string& i_NodeName) noexcept { m_NodeName = i_NodeName; }
	const std::string& getNodeName() const noexcept { return m_NodeName; }
	std::string& getNodeName() noexcept { return m_NodeName; }

	void setTemplateName(const std::string& i_TemplateName) noexcept { m_TemplateName = i_TemplateName; }
	const std::string& getTemplateName() const noexcept { return m_TemplateName; }

	void setComment(const std::string& i_Comment) noexcept { m_Comment = i_Comment; }
	std::string& getComment() noexcept { return m_Comment; }

	void setNodeType(NodeType i_NodeType) noexcept { m_NodeType = i_NodeType; }
	NodeType getNodeType() const noexcept { return static_cast<NodeType>(m_NodeType.m_Value); }
	Bool isTaskNode() noexcept { if (m_NodeType == NodeType::Task) return true; return false; }

protected:
	std::string	m_NodeName{};
	std::string m_TemplateName{};
	std::string m_Comment{};
	EnumType m_NodeType = { EnumName::NodeType, static_cast<S64>(NodeType::GPU) };
};

class TemplateNodePrototype : public NodePrototype
{
public:
	TemplateNodePrototype();
	virtual ~TemplateNodePrototype() = default;
	static std::shared_ptr<TemplateNodePrototype> createTemplatePrototype() noexcept;

public:
	virtual void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual std::shared_ptr<NodeInfo> instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_NodeID = 0) noexcept;

	virtual void test() noexcept {};

public:
	void setClassName(std::string_view i_ClassName) noexcept { m_ClassName = i_ClassName; }
	std::string getClassName() const noexcept { return m_ClassName; }

	//void setNodeTag(NodeTag i_NodeTag) noexcept { m_NodeTag = i_NodeTag; }
	NodeTag getNodeTag() const noexcept { return static_cast<NodeTag>(m_NodeTag.m_Value); }

	void setIsDynamicInputOutputPin(Bool isDynamic) noexcept { m_IsDynamicInputOutputPin = isDynamic; }
	Bool getIsDynamicInputOutputPin() const noexcept { return m_IsDynamicInputOutputPin; }

	void setIsDynamicInputPin(Bool isDynamic) noexcept { m_IsDynamicInputPin = isDynamic; }
	Bool getIsDynamicInputPin() const noexcept { return m_IsDynamicInputPin; }

	void setIsDynamicOutputPin(Bool isDynamic) noexcept { m_IsDynamicOutputPin = isDynamic; }
	Bool getIsDynamicOutputPin() const noexcept { return m_IsDynamicOutputPin; }

	void setIsDynamicParam(Bool isDynamic) noexcept { m_IsDynamicParam = isDynamic; }
	Bool getIsDynamicParam() const noexcept { return m_IsDynamicParam; }

	const auto& templateInputOutputPinInfoList() const noexcept { return m_TemplateInputOutputPinInfoList; }
	auto& templateInputOutputPinInfoList() noexcept { return m_TemplateInputOutputPinInfoList; }

	const auto& templateInputPinInfoList() const noexcept { return m_TemplateInputPinInfoList; }
	auto& templateInputPinInfoList() noexcept { return m_TemplateInputPinInfoList; }

	const auto& templateOutputPinInfoList() const noexcept { return m_TemplateOutputPinInfoList; }
	auto& templateOutputPinInfoList() noexcept { return m_TemplateOutputPinInfoList; }

	const auto& templateParamList() const noexcept { return m_TemplateParamList; }
	auto& templateParamList() noexcept { return m_TemplateParamList; }

protected:
	std::string m_ClassName{};
	EnumType m_NodeTag = { EnumName::NodeTag, static_cast<S64>(NodeTag::NoTag) };

	EnumType m_QueueFamilyType = { EnumName::TaskQueueFamily, static_cast<S64>(TaskQueueFamily::Graphic1) };
	Bool m_IsDynamicInputOutputPin = false;
	Bool m_IsDynamicInputPin = false;
	Bool m_IsDynamicOutputPin = false;
	Bool m_IsDynamicParam = false;

	PinInfoList m_TemplateInputOutputPinInfoList{};
	PinInfoList m_TemplateInputPinInfoList{};
	PinInfoList m_TemplateOutputPinInfoList{};
	ParamInfoList m_TemplateParamList{};
};

class FavoriteNodePrototype : public NodePrototype
{
public:
	FavoriteNodePrototype() = default;
	virtual ~FavoriteNodePrototype() = default;
	static std::shared_ptr<FavoriteNodePrototype> createFavoritePrototype(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual void initData(std::shared_ptr<NodeInfo> i_pNodeInfo, std::string_view i_NewNodeName, NodeManager* i_pNodeManager, NodeID& i_RelativeNodeID) noexcept;

public:
	virtual void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual void saveData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual std::shared_ptr<NodeInfo> instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_NodeID) noexcept;

	const auto& inputOutputPinInfoList() const noexcept { return m_InputOutputPinInfoList; }
	auto& inputOutputPinInfoList() noexcept { return m_InputOutputPinInfoList; }

	const auto& inputPinInfoList() const noexcept { return m_InputPinInfoList; }
	auto& inputPinInfoList() noexcept { return m_InputPinInfoList; }

	const auto& outputPinInfoList() const noexcept { return m_OutputPinInfoList; }
	auto& outputPinInfoList() noexcept { return m_OutputPinInfoList; }

	const auto& templateParamList() const noexcept { return m_TemplateParamList; }
	auto& templateParamList() noexcept { return m_TemplateParamList; }

	const auto& paramList() const noexcept { return m_ParamList; }
	auto& paramList() noexcept { return m_ParamList; }

protected:
	NodeID m_RelativeNodeID{};
	ImVec2 m_Position{};

	PinInfoList m_InputOutputPinInfoList{};
	PinInfoList m_InputPinInfoList{};
	PinInfoList m_OutputPinInfoList{};
	ParamInfoList m_TemplateParamList{};
	ParamInfoList m_ParamList{};
};

class FavoriteNormalNodePrototype : public FavoriteNodePrototype
{
public:
	FavoriteNormalNodePrototype() = default;
	virtual ~FavoriteNormalNodePrototype() = default;
};

class FavoriteChildNodePrototype : public FavoriteNodePrototype
{
public:
	FavoriteChildNodePrototype() = default;
	virtual ~FavoriteChildNodePrototype() = default;

public:
	virtual void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual void saveData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual std::shared_ptr<NodeInfo> instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_NodeID) noexcept;
};

class FavoriteCompositeNodePrototype : public FavoriteNodePrototype
{
public:
	FavoriteCompositeNodePrototype();
	virtual ~FavoriteCompositeNodePrototype() = default;
	virtual void initData(std::shared_ptr<NodeInfo> i_pNodeInfo, std::string_view i_NewNodeName, NodeManager* i_pNodeManager, NodeID& i_RelativeNodeID) noexcept;

public:
	virtual void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual void saveData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	virtual std::shared_ptr<NodeInfo> instanceNodeInfo(NodeManager* i_pNodeManager, NodeID i_ParentNodeID, NodeID i_BaseNodeID) noexcept;

protected:
	struct PrototypeLinkInfo
	{
		NodeID m_OutputRelativeNodeID{};
		std::string m_OutputPinName{};
		NodeID m_InputRelativeNodeID{};
		std::string m_InputPinName{};

		void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
		void saveData(tinyxml2::XMLElement* i_XmlElement) noexcept;
		void instanceData(NodeManager* i_pNodeManager, NodeID i_BaseNodeID) noexcept;
	};

	std::shared_ptr<FavoriteChildNodePrototype> m_pStartChildPrototype{};
	std::shared_ptr<FavoriteChildNodePrototype> m_pEndChildPrototype{};
		 
	std::vector<std::shared_ptr<NodePrototype>> m_pNodePrototypeList{};
	std::vector<PrototypeLinkInfo> m_LinkInfoList{};
};



RCEDITOR_NAMESPACE_END
#pragma once

#include <set>
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <base/public/type.h>

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

class NodeManager;
class BaseData
{
public:
	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept = 0;
	virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept = 0;
};

struct ParamData
{
	std::variant<std::monostate, S32, Bool, EnumType, F32, std::string, std::wstring,
		Vector2f, Vector3f, Vector4f, Matrix2f, Matrix3f, Matrix4f> m_Value{ 0 };

	template <typename T>
	static inline std::string eigenMatrix2Str(const T& i_Data, Eigen::IOFormat l_Format = Eigen::IOFormat(Eigen::FullPrecision, 4, ",", ",", "", "", "", ""))
	{
		std::stringstream l_StrStream{};
		l_StrStream << i_Data.format(l_Format);
		return l_StrStream.str();
	}

	template <typename T>
	static inline T str2EigenMatrix(const std::string& i_Str, Eigen::IOFormat l_Format = Eigen::IOFormat(Eigen::FullPrecision, 4, ",", "\n", "[", "]", "[", "]"))
	{
		std::stringstream l_StrStream(i_Str);
		std::string l_Line{};
		T l_Data{};

		U32 l_Index = 0;
		while (std::getline(l_StrStream, l_Line, ',')) {
			/*l_MatrixData.push_back(std::stof(l_Line))*/
			l_Data(l_Index) = std::stof(l_Line);
			l_Index++;
		}
		return l_Data;
	}

	inline std::string getData(EnumType i_DataType) const noexcept
	{
		switch (static_cast<ParamDataType>(i_DataType.m_Value))
		{
		case ParamDataType::Int: return std::to_string(std::get<S32>(m_Value));
		case ParamDataType::Bool: return std::to_string(std::get<Bool>(m_Value));
		case ParamDataType::RCFormat:
		{
			std::string l_Str{};
			EnumType l_EnumTypeData = std::get<EnumType>(m_Value);
			l_Str = l_EnumTypeData.enumValueStr();
			return l_Str;
		}
		case ParamDataType::TaskQueueFamily:
		{
			std::string l_Str{};
			EnumType l_EnumTypeData = std::get<EnumType>(m_Value);
			l_Str = l_EnumTypeData.enumValueStr();
			return l_Str;
		}
		case ParamDataType::Float: return std::to_string(std::get<F32>(m_Value));
		case ParamDataType::String: return std::get<std::string>(m_Value);
		case ParamDataType::WString: return wstring2string(std::get<std::wstring>(m_Value));
		case ParamDataType::Vector2: return eigenMatrix2Str(std::get<Vector2f>(m_Value));
		case ParamDataType::Vector3: return eigenMatrix2Str(std::get<Vector3f>(m_Value));
		case ParamDataType::Vector4: return eigenMatrix2Str(std::get<Vector4f>(m_Value));
		case ParamDataType::Matrix2: return eigenMatrix2Str(std::get<Matrix2f>(m_Value));
		case ParamDataType::Matrix3: return eigenMatrix2Str(std::get<Matrix3f>(m_Value));
		case ParamDataType::Matrix4: return eigenMatrix2Str(std::get<Matrix4f>(m_Value));
		default: return std::string{};
		}
	}

	inline ParamData setData(EnumType i_DataType, const std::string& i_ParamDataStr)
	{
		ParamData l_ParamData{};
		switch (static_cast<ParamDataType>(i_DataType.m_Value))
		{
		case ParamDataType::Int: m_Value = stoi(i_ParamDataStr); break;
		case ParamDataType::Bool: m_Value = static_cast<Bool>(stoi(i_ParamDataStr)); break;
		case ParamDataType::RCFormat: { EnumType l_EnumTypeData{}; l_EnumTypeData.setEnumName("RCFormat"); l_EnumTypeData.setEnumValue(i_ParamDataStr); m_Value = l_EnumTypeData; } break;
		case ParamDataType::TaskQueueFamily: { EnumType l_EnumTypeData{}; l_EnumTypeData.setEnumName("TaskQueueFamily"); l_EnumTypeData.setEnumValue(i_ParamDataStr); m_Value = l_EnumTypeData; } break;
		case ParamDataType::Float: m_Value = stof(i_ParamDataStr); break;
		case ParamDataType::String: m_Value = i_ParamDataStr; break;
		case ParamDataType::WString: m_Value = string2wstring(i_ParamDataStr); break;
		case ParamDataType::Vector2: m_Value = str2EigenMatrix<Vector2f>(i_ParamDataStr); break;
		case ParamDataType::Vector3: m_Value = str2EigenMatrix<Vector3f>(i_ParamDataStr); break;
		case ParamDataType::Vector4: m_Value = str2EigenMatrix<Vector4f>(i_ParamDataStr); break;
		case ParamDataType::Matrix2: m_Value = str2EigenMatrix<Matrix2f>(i_ParamDataStr); break;
		case ParamDataType::Matrix3: m_Value = str2EigenMatrix<Matrix3f>(i_ParamDataStr); break;
		case ParamDataType::Matrix4: m_Value = str2EigenMatrix<Matrix4f>(i_ParamDataStr); break;
		default:;
		}
		return l_ParamData;
	}

	inline ParamData initData(EnumType i_DataType)
	{
		ParamData l_ParamData{};
		switch (static_cast<ParamDataType>(i_DataType.m_Value))
		{
		case ParamDataType::Int: m_Value = int{}; break;
		case ParamDataType::Bool: m_Value = Bool{}; break;
		case ParamDataType::RCFormat: { EnumType l_EnumTypeData{}; l_EnumTypeData.setEnumName("RCFormat"); l_EnumTypeData.setEnumValue(0); m_Value = l_EnumTypeData; } break;
		case ParamDataType::TaskQueueFamily: { EnumType l_EnumTypeData{}; l_EnumTypeData.setEnumName("TaskQueueFamily"); l_EnumTypeData.setEnumValue(0); m_Value = l_EnumTypeData; } break;
		case ParamDataType::Float: m_Value = F32{}; break;
		case ParamDataType::String: m_Value = std::string{}; break;
		case ParamDataType::WString: m_Value = std::wstring{}; break;
		case ParamDataType::Vector2: m_Value = static_cast<Vector2f>(Vector2f::Zero()); break;
		case ParamDataType::Vector3: m_Value = static_cast<Vector3f>(Vector3f::Zero()); break;
		case ParamDataType::Vector4: m_Value = static_cast<Vector4f>(Vector4f::Zero()); break;
		case ParamDataType::Matrix2: m_Value = static_cast<Matrix2f>(Matrix2f::Identity()); break;
		case ParamDataType::Matrix3: m_Value = static_cast<Matrix3f>(Matrix3f::Identity()); break;
		case ParamDataType::Matrix4: m_Value = static_cast<Matrix4f>(Matrix4f::Identity()); break;
		default:;
		}
		return l_ParamData;
	}
};

struct ParamInfo : virtual public BaseData
{
public:
	ParamInfo() noexcept = default;

	void initData() noexcept;
	void setDataType(ParamDataType i_DataType) noexcept;

public:
	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;

public:
	std::string			m_Name = "TempParam";
	EnumType			m_DataType = { EnumName::ParamDataType, static_cast<S64>(EditorDataType::Int) };
	ParamData			m_Data{};
	Bool				m_UseRef = false;
	std::string			m_RefKey = "";
};

struct PinInfo : virtual public BaseData
{
	PinInfo() noexcept = default;
	PinInfo(const NodeID i_NodeID, const PinID i_PinID, const PinInfo& i_Info) noexcept;
	PinInfo(const NodeID i_NodeID, const PinID i_PinID, Bool i_IsTemplate, Bool i_IsConnetion, PinType i_PinType) noexcept;

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;

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

	void setName(const std::string& i_Name) noexcept { m_Name = i_Name; }
	const std::string& getName() const noexcept { return m_Name; }

	void setMetaData(const std::string& i_MetaData) noexcept { if (m_PinType == PinType::Input) m_InputMetaData = i_MetaData; else if (m_PinType == PinType::Output) m_OutputMetaData = i_MetaData; }
	std::string getMetaData() const noexcept { std::string l_MetaDataStr = ""; if (m_PinType == PinType::Input) l_MetaDataStr = m_InputMetaData; else if (m_PinType == PinType::Output) l_MetaDataStr = m_OutputMetaData; return l_MetaDataStr; }

	std::string getInputMetaData() noexcept { return m_InputMetaData; }
	std::string getOutputMetaData() noexcept { return m_OutputMetaData; }

	void setNodeID(NodeID i_NodeID) noexcept { m_NodeID = i_NodeID; }
	NodeID getNodeID() const noexcept { return m_NodeID; }

	void setPinID(PinID i_PinID) noexcept { m_PinID = i_PinID; }
	PinID getPinID() const noexcept { return m_PinID; }

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
	std::string			m_Name = "";
	std::string			m_InputMetaData = "";
	std::string			m_OutputMetaData = "";
	NodeID				m_NodeID = 0;
	PinID				m_PinID = 0;
	std::set<LinkID>    m_LinkIDSet{};

	Bool				m_IsTemplate = false;
	Bool				m_IsInputOutput = false;
	EnumType			m_DataType = { EnumName::EditorDataType, static_cast<S64>(EditorDataType::Int) };
	EnumType			m_PinType = { EnumName::PinType, static_cast<S64>(PinType::Input) };
	EnumType			m_PinAccessType = { EnumName::PinAccessType, static_cast<S64>(PinAccessType::Readable) };
};

class BaseNodeInfo : virtual public BaseData
{
public:
	BaseNodeInfo() noexcept = default;
	virtual ~BaseNodeInfo() noexcept = default;

	std::string& nodeName() noexcept { return m_NodeName; }
	void setNodeName(const std::string& i_NodeName) noexcept { m_NodeName = i_NodeName; }
	const std::string& getNodeName() const noexcept { return m_NodeName; }

	void setTemplateNodeName(const std::string& i_TemplateNodeName) noexcept { m_TemplateNodeName = i_TemplateNodeName; }
	const std::string& getTemplateNodeName() const noexcept { return m_TemplateNodeName; }

	void setNodeType(NodeType i_NodeType) noexcept { m_NodeType = i_NodeType; }
	NodeType getNodeType() const noexcept { return static_cast<NodeType>(m_NodeType.m_Value); }

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;

protected:
	std::string	m_NodeName = "";
	std::string m_TemplateNodeName = "";
	EnumType m_NodeType = { EnumName::NodeType, static_cast<S64>(NodeType::GPU) };
};

class NodeInfo;
class TemplateNodeInfo : public BaseNodeInfo
{
public:
	TemplateNodeInfo() noexcept = default;
	virtual ~TemplateNodeInfo() noexcept = default;

	virtual void loadData(tinyxml2::XMLElement* i_XmlElementRoot) noexcept;
	virtual std::shared_ptr<NodeInfo> instanceNodeInfo(NodeID i_ParentNodeID, NodeManager* i_pNodeManager, NodeID i_NodeID = 0, Bool i_IsNewChildNode = true) noexcept;

public:
	void setClassName(const std::string& className) noexcept { m_ClassName = className; }
	const std::string& getClassName() const noexcept { return m_ClassName; }

	void setComment(const std::string& comment) noexcept { m_Comment = comment; }
	const std::string& getComment() const noexcept { return m_Comment; }

	void setIsDynamicInputOutputPin(Bool isDynamic) noexcept { m_IsDynamicInputOutputPin = isDynamic; }
	Bool getIsDynamicInputOutputPin() const noexcept { return m_IsDynamicInputOutputPin; }

	void setIsDynamicInputPin(Bool isDynamic) noexcept { m_IsDynamicInputPin = isDynamic; }
	Bool getIsDynamicInputPin() const noexcept { return m_IsDynamicInputPin; }

	void setIsDynamicOutputPin(Bool isDynamic) noexcept { m_IsDynamicOutputPin = isDynamic; }
	Bool getIsDynamicOutputPin() const noexcept { return m_IsDynamicOutputPin; }

	void setIsDynamicParam(Bool isDynamic) noexcept { m_IsDynamicParam = isDynamic; }
	Bool getIsDynamicParam() const noexcept { return m_IsDynamicParam; }

	const auto& templateInputOutputPinInfos() const noexcept { return m_TemplateInputOutputPinInfos; }
	auto& templateInputOutputPinInfos() noexcept { return m_TemplateInputOutputPinInfos; }

	const auto& templateOutputPinInfos() const noexcept { return m_TemplateOutputPinInfos; }
	auto& templateOutputPinInfos() noexcept { return m_TemplateOutputPinInfos; }

	const auto& templateInputPinInfos() const noexcept { return m_TemplateInputPinInfos; }
	auto& templateInputPinInfos() noexcept { return m_TemplateInputPinInfos; }

	const auto& templateParams() const noexcept { return m_TemplateParams; }
	auto& templateParams() noexcept { return m_TemplateParams; }

protected:
	std::string m_ClassName{};
	std::string	m_Comment{};

	Bool m_IsDynamicInputOutputPin = false;
	Bool m_IsDynamicInputPin = false;
	Bool m_IsDynamicOutputPin = false;
	Bool m_IsDynamicParam = false;

	std::vector<PinInfo> m_TemplateInputOutputPinInfos{};
	std::vector<PinInfo> m_TemplateOutputPinInfos{};
	std::vector<PinInfo> m_TemplateInputPinInfos{};
	std::vector<ParamInfo> m_TemplateParams{};
};

class NodeInfo;
class FavoriteNodeInfo : public TemplateNodeInfo
{
public:
	FavoriteNodeInfo() noexcept = default;
	virtual ~FavoriteNodeInfo() noexcept = default;

	virtual void loadData(tinyxml2::XMLElement* i_XmlElementRoot) noexcept;
	virtual std::shared_ptr<NodeInfo> instanceNodeInfo(NodeID i_ParentNodeID, NodeManager* i_pNodeManager, NodeID i_NodeID = 0, Bool i_IsNewChildNode = true) noexcept;

public:
	const auto& inputOutputPinInfos() const noexcept { return m_InputOutputPinInfos; }
	auto& inputOutputPinInfos() noexcept { return m_InputOutputPinInfos; }

	const auto& outputPinInfos() const noexcept { return m_OutputPinInfos; }
	auto& outputPinInfos() noexcept { return m_OutputPinInfos; }

	const auto& inputPinInfos() const noexcept { return m_InputPinInfos; }
	auto& inputPinInfos() noexcept { return m_InputPinInfos; }

	const auto& params() const noexcept { return m_Params; }
	auto& params() noexcept { return m_Params; }
protected:

	std::vector<PinInfo> m_InputOutputPinInfos{};
	std::vector<PinInfo> m_OutputPinInfos{};
	std::vector<PinInfo> m_InputPinInfos{};
	std::vector<ParamInfo> m_Params{};
};

class PinIDList : virtual public BaseData
{
public:
	PinIDList() noexcept = default;
	virtual ~PinIDList() noexcept {};

	void instanceTemplatePinList(const std::shared_ptr<TemplateNodeInfo> i_pTemplateNodeInfo, NodeManager* i_pNodeManager) noexcept;
	void copyPinIDListData(NodeManager* i_pNodeManager, const PinIDList& i_PinIDList) noexcept;

	void deleteInfo(NodeManager* i_pNodeManager) noexcept;

	//std::vector<PinID>& getPinIDList(PinType i_PinType, Bool i_IsInputOutput) noexcept;
	//PinID getPinID(PinType i_PinType, Bool i_IsInputOutput, U32 i_PinIndex) noexcept;

	PinID addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept;
	PinID addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;
	void delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;
	void swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev) noexcept;
	virtual void changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange = PinInfoChangeFlags_None) noexcept;

	std::string getIndexListStr(PinType i_PinType) noexcept;
	std::map<std::string, std::pair<PinID, PinID>> getPinKeyMap(NodeManager* i_pNodeManager) noexcept;

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;

public:
	void setNodeID(NodeID i_NodeID) noexcept { m_NodeID = i_NodeID; }
	NodeID getNodeID() const noexcept { return m_NodeID; }

	const auto& inputOutputOutputPinIDs() const noexcept { return m_InputOutputOutputPinIDs; }
	auto& inputOutputOutputPinIDs() noexcept { return m_InputOutputOutputPinIDs; }

	const auto& inputOutputInputPinIDs() const noexcept { return m_InputOutputInputPinIDs; }
	auto& inputOutputInputPinIDs() noexcept { return m_InputOutputInputPinIDs; }

	const auto& outputPinIDs() const noexcept { return m_OutputPinIDs; }
	auto& outputPinIDs() noexcept { return m_OutputPinIDs; }

	const auto& inputPinIDs() const noexcept { return m_InputPinIDs; }
	auto& inputPinIDs() noexcept { return m_InputPinIDs; }
private:
	NodeID m_NodeID{};
	std::vector<PinID> m_InputOutputOutputPinIDs{};
	std::vector<PinID> m_InputOutputInputPinIDs{};
	std::vector<PinID> m_OutputPinIDs{};
	std::vector<PinID> m_InputPinIDs{};
};

class NodeInfo : public BaseNodeInfo
{
public:
	NodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept;
	NodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager) noexcept;
	virtual ~NodeInfo() noexcept = default;

	virtual void initData(std::string_view i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsInitChildNode = true) noexcept;

	void deleteInfo(NodeManager* i_pManager) noexcept;

	virtual PinID addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept;
	virtual PinID addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager, Bool i_IsTemplatePinIDList = false) noexcept;
	virtual void delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;
	virtual void swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev, NodeManager* i_pNodeManager) noexcept;
	virtual void changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange = PinInfoChangeFlags_None) noexcept;
	virtual void addParam() noexcept;
	virtual void addParam(const ParamInfo& i_ParamInfo, S32 i_ParamIndex) noexcept;
	virtual void delParam(U32 i_ParamIndex) noexcept;

	virtual bool canChangePinAccessType(PinType i_PinType) noexcept;

	ImColor getColor(NodeType i_Type) noexcept
	{
		auto l_Color = ImColor(255, 255, 255);
		switch (i_Type)
		{
		case NodeType::CPU:
			l_Color = ImColor(43, 162, 69);
			return l_Color;
		case NodeType::GPU:
			l_Color = ImColor(123, 104, 238);
			return l_Color;
		case NodeType::Start:
			l_Color = ImColor(12, 12, 89);
			return l_Color;
		case NodeType::End:
			l_Color = ImColor(12, 84, 12);
			return l_Color;
		case NodeType::Composite:
			l_Color = ImColor(85, 85, 12);
		case NodeType::Task:
			l_Color = ImColor(85, 85, 12);
			return l_Color;
		default:
			l_Color = ImColor(255, 0, 0);
			break;
		}

		assert(false);
		return l_Color;
	}

	ImColor	color() noexcept { return getColor(static_cast<NodeType>(m_NodeType.m_Value)); }

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual void saveFavoriteData(std::string_view i_FilePath, NodeManager* i_pNodeManager) noexcept;

public:
	void setNodeID(NodeID i_NodeID) noexcept { m_NodeID = i_NodeID; m_TemplatePinIDList.setNodeID(i_NodeID); m_PinIDList.setNodeID(i_NodeID); }
	NodeID getNodeID() const noexcept { return m_NodeID; }

	void setParentNodeID(const NodeID& parentNodeID) noexcept { m_ParentNodeID = parentNodeID; }
	const NodeID& getParentNodeID() const noexcept { return m_ParentNodeID; }

	void setClassName(const std::string& className) noexcept { m_ClassName = className; }
	const std::string& getClassName() const noexcept { return m_ClassName; }

	std::string comment() noexcept { return m_Comment; }
	void setComment(const std::string& comment) noexcept { m_Comment = comment; }
	const std::string& getComment() const noexcept { return m_Comment; }

	void setIsDynamicInputOutputPin(Bool isDynamic) noexcept { m_IsDynamicInputOutputPin = isDynamic; }
	Bool getIsDynamicInputOutputPin() const noexcept { return m_IsDynamicInputOutputPin; }

	void setIsDynamicInputPin(Bool isDynamic) noexcept { m_IsDynamicInputPin = isDynamic; }
	Bool getIsDynamicInputPin() const noexcept { return m_IsDynamicInputPin; }

	void setIsDynamicOutputPin(Bool isDynamic) noexcept { m_IsDynamicOutputPin = isDynamic; }
	Bool getIsDynamicOutputPin() const noexcept { return m_IsDynamicOutputPin; }

	void setIsDynamicParam(Bool isDynamic) noexcept { m_IsDynamicParam = isDynamic; }
	Bool getIsDynamicParam() const noexcept { return m_IsDynamicParam; }

	const auto& linkIDSet() const noexcept { return m_LinkIDSet; }
	auto& linkIDSet() noexcept { return m_LinkIDSet; }

	const auto& templatePinIDList() const noexcept { return m_TemplatePinIDList; }
	auto& templatePinIDList() noexcept { return m_TemplatePinIDList; }

	const auto& pinIDList() const noexcept { return m_PinIDList; }
	auto& pinIDList() noexcept { return m_PinIDList; }

	const auto& templateParams() const noexcept { return m_TemplateParams; }
	auto& templateParams() noexcept { return m_TemplateParams; }

	const auto& params() const noexcept { return m_Params; }
	auto& params() noexcept { return m_Params; }

	std::pair<PinID, PinID> getNameKey(std::string_view i_KeyName) noexcept { if (hasNameKey(i_KeyName)) return m_PinKeyMap[i_KeyName.data()]; else return { 0, 0 }; }
	bool hasNameKey(std::string_view i_KeyName) noexcept { return m_PinKeyMap.find(i_KeyName.data()) != m_PinKeyMap.end(); }
	void addNameKey(std::string_view i_KeyName, std::pair<PinID, PinID> i_PinIDPair) noexcept { m_PinKeyMap[i_KeyName.data()] = i_PinIDPair; }
	void delNameKey(std::string_view i_KeyName) noexcept { m_PinKeyMap.erase(i_KeyName.data()); }

	virtual NodeID getAssociationNodeID(PinType i_PinType = PinType::Input) noexcept { return m_NodeID; }

protected:
	NodeID m_NodeID{};
	NodeID m_ParentNodeID{};
	std::string m_ClassName{};
	std::string	m_Comment{};

	Bool m_IsShowTemplateInfo = false;

	Bool m_IsDynamicInputOutputPin = false;
	Bool m_IsDynamicInputPin = false;
	Bool m_IsDynamicOutputPin = false;
	Bool m_IsDynamicParam = false;

	PinIDList m_TemplatePinIDList{};
	PinIDList m_PinIDList{};

	std::vector<ParamInfo> m_TemplateParams{};
	std::vector<ParamInfo> m_Params{};

	std::set<LinkID> m_LinkIDSet{};

	std::map<std::string, std::pair<PinID, PinID>> m_PinKeyMap{};
	std::set<std::string> m_ParamKeySet{};
};

class NormalNodeInfo : public NodeInfo
{
public:
	NormalNodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept;
	NormalNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager) noexcept;
	virtual ~NormalNodeInfo() noexcept = default;

	//virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept {};
	//virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept { return nullptr; };
};

class ChildNodeInfo : public NodeInfo
{
public:
	ChildNodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept;
	ChildNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager) noexcept;
	virtual ~ChildNodeInfo() noexcept = default;

	void initPinIDList(const std::vector<PinID>& i_PinIDList, NodeManager* i_pNodeManager) noexcept;

	virtual PinID addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept;
	virtual PinID addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager, Bool i_IsTemplatePinIDList = false) noexcept;
	virtual void delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;
	virtual void swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev, NodeManager* i_pNodeManager) noexcept;
	virtual void changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange = PinInfoChangeFlags_None) noexcept;

	virtual bool canChangePinAccessType(PinType i_PinType) noexcept;
	virtual NodeID getAssociationNodeID(PinType i_PinType = PinType::Input) noexcept { return m_ParentNodeID; }

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
};

class CompositeNodeInfo : public NodeInfo
{
public:
	CompositeNodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept;
	CompositeNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsNewChildNode = true) noexcept;
	virtual ~CompositeNodeInfo() noexcept = default;
	virtual void initData(std::string_view i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsInitChildNode = true) noexcept;

	virtual PinID addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept;
	virtual PinID addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager, Bool i_IsTemplatePinIDList = false) noexcept;
	virtual void delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual void swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev, NodeManager* i_pNodeManager) noexcept;
	virtual void changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange = PinInfoChangeFlags_None) noexcept;

	virtual bool canChangePinAccessType(PinType i_PinType) noexcept;
	virtual NodeID getAssociationNodeID(PinType i_PinType = PinType::Input) noexcept { if (i_PinType == PinType::Input) return m_StartChildNodeID; else return m_EndChildNodeID; }

	const NodeID& getStartChildNodeID() const noexcept { return m_StartChildNodeID; }
	const NodeID& getEndChildNodeID() const noexcept { return m_EndChildNodeID; }

private:
	NodeID m_StartChildNodeID{};
	NodeID m_EndChildNodeID{};
};

class TaskNodeInfo : public NodeInfo
{
public:
	TaskNodeInfo(NodeID i_NodeID = 0, NodeID i_ParentNodeID = 0) noexcept;
	TaskNodeInfo(NodeID i_NodeID, NodeID i_ParentNodeID, const std::string& i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsNewChildNode = true) noexcept;
	virtual ~TaskNodeInfo() noexcept = default;
	virtual void initData(std::string_view i_TemplateNodeKey, NodeManager* i_pNodeManager, Bool i_IsInitChildNode = true) noexcept;

	virtual PinID addPinInfo(PinType i_PinType, std::string_view i_PinNameStr, NodeManager* i_pNodeManager) noexcept;
	virtual PinID addPinInfo(PinInfo& i_pPinInfo, S32 i_PinIndex, NodeManager* i_pNodeManager, Bool i_IsTemplatePinIDList = false) noexcept;
	virtual void delPinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager) noexcept;

	virtual void loadData(tinyxml2::XMLElement* i_XmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual tinyxml2::XMLElement* saveData(tinyxml2::XMLDocument* i_XmlDocument, tinyxml2::XMLElement* i_ParentXmlElement, NodeManager* i_pNodeManager = nullptr) noexcept;
	virtual void swapPinID(PinType i_PinType, U32 i_PinIndex, Bool i_IsSwapPrev, NodeManager* i_pNodeManager) noexcept;
	virtual void changePinInfo(PinType i_PinType, U32 i_PinIndex, NodeManager* i_pNodeManager, const std::shared_ptr<PinInfo> i_pPinInfo, PinInfoChangeFlags i_IsChange = PinInfoChangeFlags_None) noexcept;

	virtual bool canChangePinAccessType(PinType i_PinType) noexcept;

	const NodeID& getStartChildNodeID() const noexcept { return m_StartChildNodeID; }
	const NodeID& getEndChildNodeID() const noexcept { return m_EndChildNodeID; }
	
	void setQueueType(S64 i_QueueType) noexcept { m_QueueType = i_QueueType; }
	S64 getQueueType() noexcept { return m_QueueType.m_Value; }

	void setQueueIndex(U32 i_QueueIndex) noexcept { m_QueueIndex = i_QueueIndex; }
	U32 getQueueIndex() noexcept { return m_QueueIndex; }

private:
	NodeID m_StartChildNodeID{};
	NodeID m_EndChildNodeID{};
	EnumType m_QueueType{ EnumName::TaskQueueFamily, static_cast<S64>(TaskQueueFamily::Graphic) };
	U32 m_QueueIndex{};
};

struct LinkInfo
{
	NodeID m_OutputNodeID = 0;
	LinkID m_OutputPinID = 0;
	NodeID m_InputNodeID = 0;
	LinkID m_InputPinID = 0;

	ImColor m_Color = ImColor(0.0f);
};

struct StackInfo
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
	//void buildFrom(const std::map<U64, NodeInfo>& i_NodeInfo, const std::map<U64, LinkInfo>& i_LinkInfo) noexcept;

	//const std::set<DString>& internalNodeNames() const noexcept { return m_InternalNodeNames; };

	//void initInternal() noexcept;

	//void nodeAddInputPin(Node& i_Node, const PinInfo& i_Info) noexcept;
	//void nodeAddOutputPin(Node& i_Node, const PinInfo& i_Info) noexcept;

	//std::string getNewNameWithCount(const DString& i_Name) noexcept;
	//std::string ModifyNameWithCount(const std::string& i_OldName, const std::string& i_Name) noexcept;

	//////////////////////////////////////////

	void clearID() noexcept { m_NextNodeID = NODE_ID_OFFSET; m_NextLinkID = LINK_ID_OFFSET; m_NextPinID = PIN_ID_OFFSET; }
	void loadNodeID(NodeID i_NodeID) noexcept { m_NextNodeID = i_NodeID; }
	NodeID getNodeID() noexcept { return m_NextNodeID++; };
	LinkID getLinkID() noexcept { return m_NextLinkID++; }
	PinID getPinID() noexcept { return m_NextPinID++; }
	TemplatePinID getTemplatePinID() noexcept{ return m_NextTemplate++; }

public:
	NodeManager() noexcept;
	~NodeManager() noexcept;

	//void loadRCTemplate(const std::filesystem::directory_entry& i_FilePath, NodeInfo& i_NodeInfo) noexcept;
	//void saveRCTemplate(const std::string& i_FilePath, NodeID i_NodeID) noexcept;
	void registerTemplateNodes(std::filesystem::path i_Path, const std::string& i_MapKey) noexcept;
	void registerTemplateNode(std::shared_ptr<TemplateNodeInfo> i_pTemplateNodeInfo) noexcept;

	void delInfoByLinkIDSet(const std::set<LinkID>& i_LinkIDSet) noexcept;
	std::shared_ptr<NodeInfo> instanceNodeInfo(std::string_view i_TemplateNodeName, NodeID i_ParentNodeID = 0, NodeID i_NodeID = 0, Bool i_IsNewChildNode = true) noexcept;
	Bool delNodeInfo(NodeID i_NodeID, Bool i_IsDelNodeIDSet = true) noexcept;

	LinkID addLinkInfo(PinID i_StartPinID, PinID i_EndPinID) noexcept;
	Bool canAddLink(PinID i_CurSelPinID, PinID i_OtherPinID) noexcept;
	Bool delLinkInfo(LinkID i_LinkID) noexcept;

	void getLinkInfoToFlodMap(std::vector<RealLinkInfoHead>& i_RealLinkInfoHeadList, std::map<std::string, LinkInfoToFlod>& i_LinkInfoToFlodMap) noexcept;
	void flodLink(std::string_view i_FlodKey, RealLinkInfo& i_LinkInfoList, std::map<std::string, LinkInfoToFlod>& i_LinkInfoToFlodMap) noexcept;
	std::vector<RealLinkInfo> getFlodLinkInfo() noexcept;

public:
	const std::filesystem::path& templatePath() const noexcept { return m_RCTemplatePath; }
	const std::map<std::string, std::vector<FileDetails>>& templateNodeFileMap() const noexcept { return m_pTemplateNodeFileMap; };
	std::map<std::string, std::vector<FileDetails>>& templateNodeFileMap() noexcept { return m_pTemplateNodeFileMap; };
	const std::map<std::string, std::shared_ptr<TemplateNodeInfo>>& templateNodeInfoMap() const noexcept { return m_pTemplateNodeInfoMap; };
	std::map<std::string, std::shared_ptr<TemplateNodeInfo>>& templateNodeInfoMap() noexcept { return m_pTemplateNodeInfoMap; };
	std::map<NodeID, std::shared_ptr<NodeInfo>>& nodeMap() noexcept { return m_pNodeInfoMap; };
	std::map<LinkID, std::shared_ptr<LinkInfo>>& linkMap() noexcept { return m_pLinkInfoMap; };
	std::map<PinID, std::shared_ptr<PinInfo>>& pinMap() noexcept { return m_pPinInfoMap; };

	std::shared_ptr<NodeInfo> nodeInfo(NodeID i_ID) noexcept { auto l_It = m_pNodeInfoMap.find(i_ID); if (l_It != m_pNodeInfoMap.end()) return l_It->second; else return nullptr; }
	std::shared_ptr<LinkInfo> linkInfo(LinkID i_ID) noexcept { auto l_It = m_pLinkInfoMap.find(i_ID); if (l_It != m_pLinkInfoMap.end()) return l_It->second; else return nullptr; }
	std::shared_ptr<PinInfo> pinInfo(PinID i_ID) noexcept { auto l_It = m_pPinInfoMap.find(i_ID); if (l_It != m_pPinInfoMap.end()) return l_It->second; else return nullptr; }
	bool nodeOnUse(NodeID i_ID) noexcept { const auto l_pNodeInfo = nodeInfo(i_ID); if (l_pNodeInfo) return l_pNodeInfo->linkIDSet().size() > 0; return false; }

	void initData() noexcept;
	void clearRunningData() noexcept;

	void addStackInfo(NodeID i_NodeID) noexcept;
	void delStackInfo(NodeID i_NodeID) noexcept;

	void addTaskTagInfo(NodeID i_NodeID) noexcept;
	void delTaskTagInfo(NodeID i_NodeID) noexcept;
	void selTaskTagInfo(NodeID i_NodeID) noexcept;

	void pushCurParentNodeID(NodeID i_NodeID) noexcept;
	NodeID popCurParentNodeID() noexcept;
	void getCurStackStr(NodeID i_NodeID, std::string& i_StackTitleStr) noexcept;

	const NodeID& curParentNodeID() const noexcept { return m_CurParentNodeID; }
	NodeID& curParentNodeID() noexcept { return m_CurParentNodeID; }

	std::shared_ptr<StackInfo> curStackInfo() noexcept { return m_pCurStackInfo; }
	const std::shared_ptr<StackInfo> curStackInfo() const noexcept { return m_pCurStackInfo; }

	std::shared_ptr<StackInfo> getStackInfo(NodeID i_NodeID) noexcept { return m_pStackInfoMap.at(i_NodeID); }
	const std::shared_ptr<StackInfo> getStackInfo(NodeID i_NodeID) const noexcept { return m_pStackInfoMap.at(i_NodeID); }
	
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
	std::map<std::string, std::vector<FileDetails>> m_pTemplateNodeFileMap{};
	std::map<std::string, std::shared_ptr<TemplateNodeInfo>> m_pTemplateNodeInfoMap{};

	std::map<NodeID, std::shared_ptr<NodeInfo>> m_pNodeInfoMap{};
	std::map<LinkID, std::shared_ptr<LinkInfo>> m_pLinkInfoMap{};
	std::map<PinID, std::shared_ptr<PinInfo>> m_pPinInfoMap{};

	NodeID m_CurParentNodeID = 0;
	std::shared_ptr<StackInfo> m_pCurStackInfo{};
	std::map<NodeID, std::shared_ptr<StackInfo>> m_pStackInfoMap{};
	NodeID m_TagNodeChangeNodeID = 0;
	Bool m_IsTagNodeChange = false;
	NodeID m_CurTagNodeID = 0;
	std::shared_ptr<TaskTagInfo> m_pCurTaskTagInfo{};
	std::map<NodeID, std::shared_ptr<TaskTagInfo>> m_pTaskTagInfoMap{};

	NodeID m_NextNodeID = NODE_ID_OFFSET + 1;
	LinkID m_NextLinkID = LINK_ID_OFFSET + 1;
	PinID m_NextPinID = PIN_ID_OFFSET + 1;
	TemplatePinID m_NextTemplate = TEMPLATE_PIN_ID_OFFSET + 1;

	std::filesystem::path m_RCTemplatePath{};
};

RCEDITOR_NAMESPACE_END
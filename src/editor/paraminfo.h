#pragma once
#include "node.h"

RCEDITOR_NAMESPACE_BEGIN

template <typename T>
std::string eigenMatrix2Str(const T& i_Data, Eigen::IOFormat l_Format = Eigen::IOFormat(Eigen::FullPrecision, 4, ",", ",", "", "", "", ""))
{
	std::stringstream l_StrStream{};
	l_StrStream << i_Data.format(l_Format);
	return l_StrStream.str();
}

template <typename T>
T str2EigenMatrix(const std::string& i_Str, Eigen::IOFormat l_Format = Eigen::IOFormat(Eigen::FullPrecision, 4, ",", "\n", "[", "]", "[", "]"))
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

class ParamData
{
public:
	const auto type() const noexcept { return m_Type; }
	auto type() noexcept { return m_Type; }

	const auto value() const noexcept { return m_Value; }
	auto& value() noexcept { return m_Value; }

	std::string getDataStr() const noexcept;
	void setDataStr(const std::string& i_ParamDataStr) noexcept;
	void initData() noexcept;
	
	void setTypeStr(std::string_view i_DataType) noexcept { m_Type.setEnumValue(i_DataType.data()); }
	std::string getTypeStr() const noexcept { return m_Type.enumValueStr(); }

	void setType(ParamDataType i_DataType) noexcept { if (m_Type != i_DataType) { m_Type = i_DataType; initData(); } }
	ParamDataType getType() const noexcept { return static_cast<ParamDataType>(m_Type.m_Value); }

	std::string getEnumTypeStr() noexcept { return m_Type.enumAllTypeStr(); }

private:
	EnumType m_Type = { EnumName::ParamDataType, static_cast<S64>(EditorDataType::Int) };
	std::variant<std::monostate, S32, Bool, EnumType, F32, std::string, std::wstring,
		Vector2f, Vector3f, Vector4f, Matrix2f, Matrix3f, Matrix4f> m_Value{ 0 };
};

struct ParamInfo 
{
public:
	void setName(const std::string& i_Name) noexcept { m_Name = i_Name; }
	const std::string& getName() const noexcept { return m_Name; }

	std::string getDataStr() const noexcept { return m_Data.getDataStr(); };
	void setDataStr(const std::string& i_ParamDataStr) noexcept { m_Data.setDataStr(i_ParamDataStr); }

	void setData(const ParamData& i_Data) noexcept { m_Data = i_Data; }
	const ParamData& getData() const noexcept { return m_Data; }
	ParamData& getData() noexcept { return m_Data; }

	void setUseRef(bool i_UseRef) noexcept { m_UseRef = i_UseRef; }
	bool getUseRef() const noexcept { return m_UseRef; }

	void setRefKey(const std::string& i_RefKey) noexcept { m_RefKey = i_RefKey; }
	const std::string& getRefKey() const noexcept { return m_RefKey; }

	void setDataTypeStr(std::string_view i_DataTypeStr) noexcept { m_Data.setTypeStr(i_DataTypeStr); initWidgetType(); };
	std::string getDataTypeStr() const noexcept { return m_Data.getTypeStr(); }

	void setDataType(const ParamDataType& i_DataType) noexcept { m_Data.setType(i_DataType); initWidgetType(); }
	ParamDataType getDataType() const noexcept { return m_Data.getType(); }

	std::string getEnumTypeStr() noexcept { return m_Data.getEnumTypeStr(); }

	void setWidgetType(const EnumType& i_WidgetType) noexcept { m_WidgetType == i_WidgetType; }
	EnumType& getWidgetType()  noexcept { return m_WidgetType; }

	void setIntMin(S32 i_Data) { m_iMin = i_Data; }
	S32 getIntMin() noexcept { return m_iMin; }
	void setIntMax(S32 i_Data) { m_iMax = i_Data; }
	S32 getIntMax() noexcept { return m_iMax; }
	void setFloatMin(F32 i_Data) { m_fMin = i_Data; }
	F32 getFloatMin() noexcept { return m_fMin; }
	void setFloatMax(F32 i_Data) { m_fMax = i_Data; }
	F32 getFloatMax() noexcept { return m_fMax; }

	void initWidgetType() noexcept;
	Bool useSlider() noexcept;
	Bool useFileBrowser() noexcept;
	Bool useColorPick() noexcept;

public:
	void loadData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	void loadTemplateData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	tinyxml2::XMLElement* saveData(tinyxml2::XMLElement* i_XmlElement) noexcept;
	tinyxml2::XMLElement* saveTemplateData(tinyxml2::XMLElement* i_XmlElement) noexcept;
public:
	std::string			m_Name = "TempParam";
	ParamData			m_Data{};
	Bool				m_UseRef = false;
	std::string			m_RefKey = "";

	EnumType m_WidgetType = { EnumName::WidgetTypeInt, 0 };
	F32 m_fMin, m_fMax{};
	U32 m_iMin, m_iMax{};
};

RCEDITOR_NAMESPACE_END
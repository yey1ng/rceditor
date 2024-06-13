#include "paraminfo.h"

RCEDITOR_NAMESPACE_BEGIN

std::string ParamData::getDataStr() const noexcept
{
	switch (static_cast<ParamDataType>(m_Type.m_Value))
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
	case ParamDataType::VkBufferUsage:
	{
		std::string l_Str{};
		EnumType l_EnumTypeData = std::get<EnumType>(m_Value);
		l_Str = l_EnumTypeData.enumValueStr();
		return l_Str;
	}
	case ParamDataType::VkImageUsage:
	{
		std::string l_Str{};
		EnumType l_EnumTypeData = std::get<EnumType>(m_Value);
		l_Str = l_EnumTypeData.enumValueStr();
		return l_Str;
	}
	case ParamDataType::Float: return std::to_string(std::get<F32>(m_Value));
	case ParamDataType::String: return std::get<std::string>(m_Value);
	case ParamDataType::WString: return wstring2string(std::get<std::wstring>(m_Value));
	case ParamDataType::Vector2f: return eigenMatrix2Str(std::get<Vector2f>(m_Value));
	case ParamDataType::Vector3f: return eigenMatrix2Str(std::get<Vector3f>(m_Value));
	case ParamDataType::Vector4f: return eigenMatrix2Str(std::get<Vector4f>(m_Value));
	case ParamDataType::Matrix2f: return eigenMatrix2Str(std::get<Matrix2f>(m_Value));
	case ParamDataType::Matrix3f: return eigenMatrix2Str(std::get<Matrix3f>(m_Value));
	case ParamDataType::Matrix4f: return eigenMatrix2Str(std::get<Matrix4f>(m_Value));
	default: return std::string{};
	}
}
void ParamData::setDataStr(const std::string& i_ParamDataStr) noexcept
{
	switch (static_cast<ParamDataType>(m_Type.m_Value))
	{
	case ParamDataType::Int: m_Value = stoi(i_ParamDataStr); break;
	case ParamDataType::Bool: m_Value = static_cast<Bool>(stoi(i_ParamDataStr)); break;
	case ParamDataType::RCFormat: { EnumType l_EnumTypeData{}; l_EnumTypeData.setTypeName("RCFormat"); l_EnumTypeData.setEnumValue(i_ParamDataStr); m_Value = l_EnumTypeData; } break;
	case ParamDataType::VkBufferUsage: { EnumType l_EnumTypeData{}; l_EnumTypeData.setTypeName("VkBufferUsage"); l_EnumTypeData.setEnumValue(i_ParamDataStr); m_Value = l_EnumTypeData; } break;
	case ParamDataType::VkImageUsage: { EnumType l_EnumTypeData{}; l_EnumTypeData.setTypeName("VkImageUsage"); l_EnumTypeData.setEnumValue(i_ParamDataStr); m_Value = l_EnumTypeData; } break;
	case ParamDataType::Float: m_Value = stof(i_ParamDataStr); break;
	case ParamDataType::String: m_Value = i_ParamDataStr; break;
	case ParamDataType::WString: m_Value = string2wstring(i_ParamDataStr); break;
	case ParamDataType::Vector2f: m_Value = str2EigenMatrix<Vector2f>(i_ParamDataStr); break;
	case ParamDataType::Vector3f: m_Value = str2EigenMatrix<Vector3f>(i_ParamDataStr); break;
	case ParamDataType::Vector4f: m_Value = str2EigenMatrix<Vector4f>(i_ParamDataStr); break;
	case ParamDataType::Matrix2f: m_Value = str2EigenMatrix<Matrix2f>(i_ParamDataStr); break;
	case ParamDataType::Matrix3f: m_Value = str2EigenMatrix<Matrix3f>(i_ParamDataStr); break;
	case ParamDataType::Matrix4f: m_Value = str2EigenMatrix<Matrix4f>(i_ParamDataStr); break;
	default:;
	}
}

void ParamData::initData() noexcept
{
	switch (static_cast<ParamDataType>(m_Type.m_Value))
	{
	case ParamDataType::Int: m_Value = int{}; break;
	case ParamDataType::Bool: m_Value = Bool{}; break;
	case ParamDataType::RCFormat: { EnumType l_EnumTypeData{}; l_EnumTypeData.setTypeName("RCFormat"); l_EnumTypeData.m_Value = 0; m_Value = l_EnumTypeData; } break;
	case ParamDataType::VkBufferUsage: { EnumType l_EnumTypeData{}; l_EnumTypeData.setTypeName("VkBufferUsage"); l_EnumTypeData.m_Value = 0; m_Value = l_EnumTypeData; } break;
	case ParamDataType::VkImageUsage: { EnumType l_EnumTypeData{}; l_EnumTypeData.setTypeName("VkImageUsage"); l_EnumTypeData.m_Value = 0; m_Value = l_EnumTypeData; } break;
	case ParamDataType::Float: m_Value = F32{}; break;
	case ParamDataType::String: m_Value = std::string{}; break;
	case ParamDataType::WString: m_Value = std::wstring{}; break;
	case ParamDataType::Vector2f: m_Value = static_cast<Vector2f>(Vector2f::Zero()); break;
	case ParamDataType::Vector3f: m_Value = static_cast<Vector3f>(Vector3f::Zero()); break;
	case ParamDataType::Vector4f: m_Value = static_cast<Vector4f>(Vector4f::Zero()); break;
	case ParamDataType::Matrix2f: m_Value = static_cast<Matrix2f>(Matrix2f::Identity()); break;
	case ParamDataType::Matrix3f: m_Value = static_cast<Matrix3f>(Matrix3f::Identity()); break;
	case ParamDataType::Matrix4f: m_Value = static_cast<Matrix4f>(Matrix4f::Identity()); break;
	default:;
	}
}

void ParamInfo::loadData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	m_Name = i_XmlElement->FindAttribute("Name")->Value();
	m_UseRef = i_XmlElement->FindAttribute("UseRef")->BoolValue();
	m_RefKey = i_XmlElement->FindAttribute("RefKey")->Value();
	//setDataTypeStr(i_XmlElement->FindAttribute("DataType")->Value());
	//setDataStr(i_XmlElement->FindAttribute("Data")->Value());

	std::string l_Test1 = i_XmlElement->FindAttribute("DataType")->Value();
	std::string l_Test2 = i_XmlElement->FindAttribute("Data")->Value();

	setDataTypeStr(l_Test1);
	setDataStr(l_Test2);

	tinyxml2::XMLElement* l_XmlMetaData = i_XmlElement->FirstChildElement("MetaData");
	if (l_XmlMetaData)
	{
		m_WidgetType.setEnumValue(l_XmlMetaData->FindAttribute("WidgetType")->Value());
		if (m_WidgetType.m_Value != 0)
		{
			if (getDataType() == ParamDataType::Int)
			{
				m_iMin = l_XmlMetaData->FindAttribute("Min")->IntValue();
				m_iMax = l_XmlMetaData->FindAttribute("Max")->IntValue();
			}
			else if (getDataType() == ParamDataType::Float)
			{
				m_fMin = l_XmlMetaData->FindAttribute("Min")->FloatValue();
				m_fMax = l_XmlMetaData->FindAttribute("Max")->FloatValue();
			}
		}
	}
}

void ParamInfo::loadTemplateData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	m_Name = i_XmlElement->FindAttribute("Name")->Value();
	m_UseRef = i_XmlElement->FindAttribute("UseRef")->BoolValue();
	m_RefKey = i_XmlElement->FindAttribute("RefKey")->Value();
	m_Data.setTypeStr(i_XmlElement->FindAttribute("DataType")->Value());
	m_Data.setDataStr(i_XmlElement->FindAttribute("Data")->Value());
}

tinyxml2::XMLElement* ParamInfo::saveData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	tinyxml2::XMLElement* l_XmlParamElement = i_XmlElement->InsertNewChildElement(g_XmlParamStr.data());
	l_XmlParamElement->SetAttribute("Name", m_Name.c_str());
	l_XmlParamElement->SetAttribute("DataType", m_Data.getTypeStr().c_str());
	l_XmlParamElement->SetAttribute("Data", m_Data.getDataStr().c_str());
	l_XmlParamElement->SetAttribute("UseRef", m_UseRef);
	l_XmlParamElement->SetAttribute("RefKey", m_RefKey.c_str());

	if (m_WidgetType.m_Value > 0)
	{
		tinyxml2::XMLElement* l_XmlMetaData = l_XmlParamElement->InsertNewChildElement("MetaData");
		l_XmlMetaData->SetAttribute("WidgetType", m_WidgetType.enumValueStr().c_str());
		if (getDataType() == ParamDataType::Int)
		{
			l_XmlMetaData->SetAttribute("Min", m_iMin);
			l_XmlMetaData->SetAttribute("Max", m_iMax);
		}
		else if (getDataType() == ParamDataType::Float)
		{
			l_XmlMetaData->SetAttribute("Min", m_fMin);
			l_XmlMetaData->SetAttribute("Max", m_fMax);
		}
	}
	return l_XmlParamElement;
}

tinyxml2::XMLElement* ParamInfo::saveTemplateData(tinyxml2::XMLElement* i_XmlElement) noexcept
{
	tinyxml2::XMLElement* l_XmlParamElement = i_XmlElement->InsertNewChildElement(g_XmlParamStr.data());
	l_XmlParamElement->SetAttribute("Name", m_Name.c_str());
	l_XmlParamElement->SetAttribute("DataType", m_Data.getTypeStr().c_str());
	l_XmlParamElement->SetAttribute("Data", m_Data.getDataStr().c_str());
	l_XmlParamElement->SetAttribute("UseRef", m_UseRef);
	l_XmlParamElement->SetAttribute("RefKey", m_RefKey.c_str());

	return l_XmlParamElement;
}

void ParamInfo::initWidgetType() noexcept
{
	switch (getDataType())
	{
	case ParamDataType::Int:
		m_WidgetType = EnumType{ EnumName::WidgetTypeInt, 0 };
		break;
	case ParamDataType::Float:
		m_WidgetType = EnumType{ EnumName::WidgetTypeFloat, 0 };
		break;
	case ParamDataType::String:
		m_WidgetType = EnumType{ EnumName::WidgetTypeString, 0 };
		break;
	case ParamDataType::WString:
		m_WidgetType = EnumType{ EnumName::WidgetTypeWString, 0 };
		break;
	case ParamDataType::Vector3f:
		m_WidgetType = EnumType{ EnumName::WidgetTypeVector3f, 0 };
		break;
	case ParamDataType::Vector4f:
		m_WidgetType = EnumType{ EnumName::WidgetTypeVector4f, 0 };
		break;
	default:
		m_WidgetType.m_Value = 0;
		break;
	}
}

Bool ParamInfo::useSlider() noexcept
{
	if (getDataType() == ParamDataType::Int && m_WidgetType.m_Name == EnumName::WidgetTypeInt && static_cast<WidgetTypeInt>(m_WidgetType.m_Value) == WidgetTypeInt::Slider)
		return true;
	else if (getDataType() == ParamDataType::Float && m_WidgetType.m_Name == EnumName::WidgetTypeFloat && static_cast<WidgetTypeFloat>(m_WidgetType.m_Value) == WidgetTypeFloat::Slider)
		return true;

	return false;
}

Bool ParamInfo::useFileBrowser() noexcept
{
	if (getDataType() == ParamDataType::String && m_WidgetType.m_Name == EnumName::WidgetTypeString && (static_cast<WidgetTypeString>(m_WidgetType.m_Value) == WidgetTypeString::File || static_cast<WidgetTypeString>(m_WidgetType.m_Value) == WidgetTypeString::Dir))
		return true;
	else if (getDataType() == ParamDataType::WString && m_WidgetType.m_Name == EnumName::WidgetTypeWString && (static_cast<WidgetTypeWString>(m_WidgetType.m_Value) == WidgetTypeWString::File || static_cast<WidgetTypeWString>(m_WidgetType.m_Value) == WidgetTypeWString::Dir))
		return true;

	return false;
}

Bool ParamInfo::useColorPick() noexcept
{
	if (getDataType() == ParamDataType::Vector3f && m_WidgetType.m_Name == EnumName::WidgetTypeVector3f && static_cast<WidgetTypeVector3f>(m_WidgetType.m_Value) == WidgetTypeVector3f::ColorPick)
		return true;
	else if (getDataType() == ParamDataType::Vector4f && m_WidgetType.m_Name == EnumName::WidgetTypeVector4f && static_cast<WidgetTypeVector4f>(m_WidgetType.m_Value) == WidgetTypeVector4f::ColorPick)
		return true;

	return false;
}


RCEDITOR_NAMESPACE_END
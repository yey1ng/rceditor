#pragma once

#include "pugixml.hpp"
#include "tinyxml2.h"

#include <Objbase.h>
#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")

#include <set>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <Eigen/Core>
#include <imgui/imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS_IMPLEMENTED
#include <imgui.h>
#include <imgui_internal.h>

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 1024
#include <magic_enum/magic_enum.hpp>

#define RCEDITOR_NAMESPACE_BEGIN			namespace rceditor{
#define RCEDITOR_NAMESPACE_END				}
#define RCEDITOR_NS_BEGIN(name)				namespace name{
#define RCEDITOR_NS_END(name)				}

RCEDITOR_NAMESPACE_BEGIN

using U32 = uint32_t;
using U64 = uint64_t;
using S32 = int32_t;
using S64 = int64_t;
using F32 = float_t;
using Bool = bool;
using Flags = U32;
using Matrix2f = Eigen::Matrix<float, 2, 2, Eigen::RowMajor>;
using Matrix3f = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;
using Matrix4f = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;
using Vector2f = Eigen::Matrix<float, 1, 2, Eigen::RowMajor>;
using Vector3f = Eigen::Matrix<float, 1, 3, Eigen::RowMajor>;
using Vector4f = Eigen::Matrix<float, 1, 4, Eigen::RowMajor>;

using TemplatePinID = U64;
using PinID = U64;
using NodeID = U64;
using LinkID = U64;

constexpr U64 NODE_ID_OFFSET = 1e8;
constexpr U64 LINK_ID_OFFSET = 2e8;
constexpr U64 PIN_ID_OFFSET = 3e8;
constexpr U64 TEMPLATE_PIN_ID_OFFSET = 4e8;
 
enum class EditorAllDataType : S64
{
	//Unknown = 0,
	Int,
	Bool,
	Float,
	String,
	WString,
	Vector2,
	Vector3,
	Vector4,
	Matrix2,
	Matrix3,
	Matrix4,
	RCFormat,
	Buffer,
	Texture,
	Count
};

enum class EditorParamDataType : S64
{
	//Unknown = 0,
	Int,
	Bool,
	Float,
	String,
	WString,
	Vector2,
	Vector3,
	Vector4,
	Matrix2,
	Matrix3,
	Matrix4,
	RCFormat,
	Count
};

enum class PinType : S64
{
	//Unknown = 0,
	Input,
	Output,
	Connection,
	//ConnectionInput,
	//ConnectionOutput,
	Count,
};

enum class PinAccessType : S64
{
	//Unknown = 0,
	Readable,
	Writable,
	Count,
};

enum class NodeType : S64
{
	//Unknown = 0,
	CPU,
	GPU,
	TASK,
	Count,
};

enum class RCFormat : S64
{
	RC_FORMAT_UNKNOWN,                ///< Unknown format.
	RC_FORMAT_R32G32B32A32_TYPELESS,  ///< 4-channel RGBA format with each channel being a typeless 32-bit value.
	RC_FORMAT_R32G32B32A32_FLOAT,     ///< 4-channel RGBA format with each channel being a 32-bit IEEE 754 floating
	///  point value.
	RC_FORMAT_R32G32B32A32_UINT,      ///< 4-channel RGBA format with each channel being a 32-bit unsigned integer.
	RC_FORMAT_R32G32B32A32_SINT,      ///< 4-channel RGBA format with each channel being a 32-bit signed integer.
	RC_FORMAT_R32G32B32_TYPELESS,     ///< 3-channel RGB format with each channel being a typeless 32-bit value.
	RC_FORMAT_R32G32B32_FLOAT,        ///< 3-channel RGB format with each channel being a 32-bit IEEE 754 floating
	///  point value.
	RC_FORMAT_R32G32B32_UINT,         ///< 3-channel RGB format with each channel being a 32-bit unsigned integer.
	RC_FORMAT_R32G32B32_SINT,         ///< 3-channel RGB format with each channel being a 32-bit signed integer.

	RC_FORMAT_R16G16B16A16_TYPELESS,  ///< 4-channel RGBA format with each channel being a typeless 16-bit value.
	RC_FORMAT_R16G16B16A16_FLOAT,     ///< 4-channel RGBA format with each channel being a 16-bit floating point
	///  value.
	RC_FORMAT_R16G16B16A16_UNORM,     ///< 4-channel RGBA format with each channel being a normalized, 16-bit unsigned
	///  integer.
	RC_FORMAT_R16G16B16A16_UINT,      ///< 4-channel RGBA format with each channel being a 16-bit unsigned integer.
	RC_FORMAT_R16G16B16A16_SNORM,     ///< 4-channel RGBA format with each channel being a normalized, 16-bit signed
	///  integer.

	/// 4-channel RGBA format with each channel being a 16-bit signed integer.
	RC_FORMAT_R16G16B16A16_SINT,

	/// 2-channel RG format with each channel being a typeless 32-bit value.
	RC_FORMAT_R32G32_TYPELESS,

	/// 2-channel RG format with each channel being a 32-bit IEEE 754 floating point value.
	RC_FORMAT_R32G32_FLOAT,

	/// 2-channel RG format with each channel being a 32-bit unsigned integer.
	RC_FORMAT_R32G32_UINT,

	/// 2-channel RG format with each channel being a 32-bit signed integer.
	RC_FORMAT_R32G32_SINT,

	/// 2-channel RG format with the first channel being a typeless 32-bit value, the second channel a typeless 8-bit
	/// value and 24 unused bits at the end.
	RC_FORMAT_R32G8X24_TYPELESS,

	/// 2-channel RG format with the first channel being a 32-bit depth value, the second one a 8-bit unsigned integer
	/// value and 24 unused bits at the end.
	RC_FORMAT_D32_FLOAT_S8X24_UINT,

	/// Single channel R format with the channel being a typeless 32-bit IEEE 754 floating point value and additional
	/// sets of 8 and 24 unused bits afterwards.
	RC_FORMAT_R32_FLOAT_X8X24_TYPELESS,

	/// Single channel R format with 32 unused bits, the channel being an 8-bit unsigned integer value and 24 unused
	/// bits at the end.
	RC_FORMAT_X32_TYPELESS_G8X24_UINT,

	/// 4-channel RGBA format with the RGB channels being typeless 10-bit values and the A channel being a typeless
	/// 2-bit value.
	RC_FORMAT_R10G10B10A2_TYPELESS,

	/// 4-channel RGBA format with the RGB channels being 10-bit normalized, unsigned integer values and the A channel
	/// being a 2-bit normalized, unsigned integer value.
	RC_FORMAT_R10G10B10A2_UNORM,

	/// 4-channel RGBA format with the RGB channels being 10-bit unsigned integer values and the A channel being a 2-bit
	/// unsigned integer value.
	RC_FORMAT_R10G10B10A2_UINT,

	/// 3-channel RGB format with the RG channels being 11-bit floating point values and the B channel being a 10-bit
	/// floating point value.
	RC_FORMAT_R11G11B10_FLOAT,

	RC_FORMAT_R8G8B8A8_TYPELESS,    ///< 4-channel RGBA format with all channels being typeless 8-bit values.
	RC_FORMAT_R8G8B8A8_UNORM,       ///< 4-channel RGBA format with all channels being normalized 8-bit unsigned
	///  integers.
	RC_FORMAT_R8G8B8A8_UNORM_SRGB,  ///< 4-channel RGBA format with all channels being normalized 8-bit unsigned integer
	///  SRGB values.
	RC_FORMAT_R8G8B8A8_UINT,        ///< 4-channel RGBA format with all channels being 8-bit unsigned integers.
	RC_FORMAT_R8G8B8A8_SNORM,       ///< 4-channel RGBA format with all channels being normalized, 8-bit signed
	///  integers.
	RC_FORMAT_R8G8B8A8_SINT,        ///< 4-channel RGBA format with all channels being 8-bit signed integers.

	RC_FORMAT_R16G16_TYPELESS,  ///< 2-channel RG format with each channel being a typeless 16-bit value.
	RC_FORMAT_R16G16_FLOAT,  ///< 2-channel RG format with each channel being a 16-bit IEEE 754 floating point value.
	RC_FORMAT_R16G16_UNORM,  ///< 2-channel RG format with each channel being a normalized, 16-bit unsigned integer.
	RC_FORMAT_R16G16_UINT,   ///< 2-channel RG format with each channel being a 16-bit unsigned integer.
	RC_FORMAT_R16G16_SNORM,  ///< 2-channel RG format with each channel being a normalized, 16-bit signed integer
	///  value.
	RC_FORMAT_R16G16_SINT,   ///< 2-channel RG format with each channel being a 16-bit signed integer.

	RC_FORMAT_R32_TYPELESS,  ///< Single channel R format with the channel being a typeless 32-bit value.
	RC_FORMAT_D32_FLOAT,     ///< Single channel R format with the channel being a 32-bit IEEE 754 floating point depth
	///  value.
	RC_FORMAT_R32_FLOAT,     ///< Single channel R format with the channel being a 32-bit IEEE 754 floating point
	///  value.
	RC_FORMAT_R32_UINT,      ///< Single channel R format with the channel being a 32-bit unsigned integer.
	RC_FORMAT_R32_SINT,      ///< Single channel R format with the channel being a 32-bit signed integer.

	/// 2-channel RG format with the first channel being a typeless 24-bit value and the second one a typeless 8-bit
	/// value.
	RC_FORMAT_R24G8_TYPELESS,

	/// 2-channel RG format with the first channel being a normalized, 24-bit unsigned integer depth value and the
	/// second one an 8-bit unsigned integer stencil value.
	RC_FORMAT_D24_UNORM_S8_UINT,

	/// 2-channel RG format with the first channel being a normalized, 24-bit unsigned integer value and the second one
	/// a typeless 8-bit value.
	RC_FORMAT_R24_UNORM_X8_TYPELESS,

	/// Single channel R format with 24 unused bits with the channel being an 8-bit unsigned integer.
	RC_FORMAT_X24_TYPELESS_G8_UINT,

	RC_FORMAT_R8G8_TYPELESS,  ///< 2-channel RG format with each channel being a typeless 8-bit value.
	RC_FORMAT_R8G8_UNORM,     ///< 2-channel RG format with each channel being a normalized, 8-bit unsigned integer.
	RC_FORMAT_R8G8_UINT,      ///< 2-channel RG format with each channel being a 8-bit unsigned integer.
	RC_FORMAT_R8G8_SNORM,     ///< 2-channel RG format with each channel being a normalized, 8-bit signed integer.
	RC_FORMAT_R8G8_SINT,      ///< 2-channel RG format with each channel being a 8-bit signed integer.

	RC_FORMAT_R16_TYPELESS,  ///< Single channel R format with the channel being a typeless 16-bit value.
	RC_FORMAT_R16_FLOAT,     ///< Single channel R format with the channel being a 16-bit IEEE 754 floating point
	///  value.
	RC_FORMAT_D16_UNORM,     ///< Single channel R format with the channel being a 16-bit IEEE 754 floating point
	///  depth value.
	RC_FORMAT_R16_UNORM,     ///< Single channel R format with the channel being a 16-bit unsigned integer.
	RC_FORMAT_R16_UINT,      ///< Single channel R format with the channel being a 16-bit signed integer.
	RC_FORMAT_R16_SNORM,     ///< Single channel R format with the channel being a normalized, 16-bit signed integer.
	RC_FORMAT_R16_SINT,      ///< Single channel R format with the channel being a 16-bit signed integer.

	RC_FORMAT_R8_TYPELESS,  ///< Single channel R format with the channel being a typeless 8-bit value.
	RC_FORMAT_R8_UNORM,     ///< Single channel R format with the channel being a normalized, 8-bit unsigned integer.
	RC_FORMAT_R8_UINT,      ///< Single channel R format with the channel being a 8-bit signed integer.
	RC_FORMAT_R8_SNORM,     ///< Single channel R format with the channel being a normalized, 8-bit signed integer.
	RC_FORMAT_R8_SINT,      ///< Single channel R format with the channel being a 8-bit signed integer.
	RC_FORMAT_A8_UNORM,     ///< Single channel A format with the channel being a normalized, 8-bit unsigned integer.

	RC_FORMAT_R1_UNORM,  ///< Single channel R format with the channel being a 1-bit unsigned integer.

	/// 4-channel RGB format with the first three channels being a 9-bit mantissa. Together with the 5-bit exponent that
	/// is shared for all three channels they form three 9-bit mantissa + 5-bit exponent floating point value.
	RC_FORMAT_R9G9B9E5_SHAREDEXP,

	/// 4-channel RGB format with each channel being a normalized, 8-bit unsigned integer. Each block of 32 bits
	/// describes the RGB values for a pair of pixels that always share one R and B value but have separate G values.
	RC_FORMAT_R8G8_B8G8_UNORM,

	/// 4-channel RGB format with each channel being a normalized, 8-bit unsigned integer. Each block of 32 bits
	/// describes the RGB values for a pair of pixels that always share one R and B value but have separate G values.
	RC_FORMAT_G8R8_G8B8_UNORM,

	/// 4-channel block compressed format with the first channel being a typeless 5-bit value, the second one a
	/// typeless, 6-bit value, the third one a typeless, 5-bit value and the last one a typeless, 0-bit or 1-bit value.
	RC_FORMAT_BC1_TYPELESS,

	/// 4-channel block compressed format with the first channel being a normalized, 5-bit unsigned integer, the second
	/// one a normalized, 6-bit unsigned integer, the third one a normalized, 5-bit unsigned integer and the last one a
	/// normalized, 0-bit or 1-bit unsigned integer.
	RC_FORMAT_BC1_UNORM,

	/// 4-channel block compressed format with the first channel being a normalized, 5-bit unsigned integer SRGB value,
	/// the second one a normalized, 6-bit unsigned integer SRGB value, the third one a normalized, 5-bit unsigned
	/// integer SRGB value and the last one a normalized, 0-bit or 1-bit unsigned integer SRGB value.
	RC_FORMAT_BC1_UNORM_SRGB,

	/// 4-channel block compressed format with the first channel being a typeless 5-bit value, the second one a
	/// typeless, 6-bit value, the third one a typeless, 5-bit value and the last one a typeless, 4-bit value.
	RC_FORMAT_BC2_TYPELESS,

	/// 4-channel block compressed format with the first channel being a normalized, 5-bit unsigned integer, the second
	/// one a normalized, 6-bit unsigned integer, the third one a normalized, 5-bit unsigned integer and the last one a
	/// normalized, 4-bit unsigned integer.
	RC_FORMAT_BC2_UNORM,

	/// 4-channel block compressed format with the first channel being a normalized, 5-bit unsigned integer SRGB value,
	/// the second one a normalized, 6-bit unsigned integer SRGB value, the third one a normalized, 5-bit unsigned
	/// integer SRGB value and the last one a normalized, 4-bit unsigned integer SRGB value.
	RC_FORMAT_BC2_UNORM_SRGB,

	/// 4-channel block compressed format with the first channel being a typeless 5-bit value, the second one a
	/// typeless, 6-bit value, the third one a typeless, 5-bit value and the last one a typeless, 8-bit value.
	RC_FORMAT_BC3_TYPELESS,

	/// 4-channel block compressed format with the first channel being a normalized, 5-bit unsigned integer, the second
	/// one a normalized, 6-bit unsigned integer, the third one a normalized, 5-bit unsigned integer and the last one a
	/// normalized, 8-bit unsigned integer.
	RC_FORMAT_BC3_UNORM,

	/// 4-channel block compressed format with the first channel being a normalized, 5-bit unsigned integer SRGB value,
	/// the second one a normalized, 6-bit unsigned integer SRGB value, the third one a normalized, 5-bit unsigned
	/// integer SRGB value and the last one a normalized, 0-bit or 1-bit unsigned integer SRGB value.
	RC_FORMAT_BC3_UNORM_SRGB,

	/// Single channel block compressed format with the channel being a typeless 8-bit value.
	RC_FORMAT_BC4_TYPELESS,

	/// Single channel block compressed format with the channel being a normalized, 8-bit signed integer value.
	RC_FORMAT_BC4_UNORM,

	/// Single channel block compressed format with the channel being a normalized, 8-bit signed integer value.
	RC_FORMAT_BC4_SNORM,

	/// 2-channel block compressed format with each channel being a typeless 8-bit value.
	RC_FORMAT_BC5_TYPELESS,

	/// 2-channel block compressed format with each channel being a normalized, 8-bit unsigned integer value.
	RC_FORMAT_BC5_UNORM,

	/// 2-channel block compressed format with each channel being a normalized, 8-bit signed integer value.
	RC_FORMAT_BC5_SNORM,

	/// 3-channel BGR format with the first channel being a normalized, 5-bit unsigned integer, the second one a
	/// normalized, 6-bit unsigned integer and the third one a normalized, 5-bit unsigned integer.
	RC_FORMAT_B5G6R5_UNORM,

	/// 4-channel BGRA format with the first three channels being a normalized, 5-bit unsigned integer and the last one
	/// a normalized, 1-bit unsigned integer.
	RC_FORMAT_B5G5R5A1_UNORM,

	/// 4-channel BGRA format with each channel being a normalized, 8-bit unsigned integer.
	RC_FORMAT_B8G8R8A8_UNORM,

	/// 3-channel BGR format with each channel being a normalized, 8-bit unsigned integer value and 8 unused bits at the
	/// end.
	RC_FORMAT_B8G8R8X8_UNORM,

	/// 4-channel RGB 2.8-biased fixed-point format with the first three channels being a normalized, 10-bit
	/// unsigned integer and the last one a normalized 2-bit unsigned integer.
	RC_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,

	RC_FORMAT_B8G8R8A8_TYPELESS,    ///< 4-channel BGRA format with each channel being a typeless 8-bit value.
	RC_FORMAT_B8G8R8A8_UNORM_SRGB,  ///< 4-channel BGRA format with each channel being a normalized, 8-bit unsigned
	///  integer SRGB value.
	RC_FORMAT_B8G8R8X8_TYPELESS,    ///< 3-channel BGR format with each channel being a typeless 8-bit value and 8
	///  unused bits at the end.
	RC_FORMAT_B8G8R8X8_UNORM_SRGB,  ///< 3-channel BGR format with each channel being a normalized, 8-bit unsigned
	///  integer and 8 unused bits a the end.

	/// 3-channel block compressed HDR format with each channel being a typeless 16-bit value.
	RC_FORMAT_BC6H_TYPELESS,

	/// 3-channel block compressed HDR format with each channel being a 16-bit unsigned "half" floating point value.
	RC_FORMAT_BC6H_UF16,

	/// 3-channel block compressed HDR format with each channel being a 16-bit signed "half" floating point value.
	RC_FORMAT_BC6H_SF16,

	/// 3-channel or 4-channel block compressed format with the first three channels being a typeless, 4-7-bit value and
	/// the last one an optional, typeless 0-8-bit value.
	RC_FORMAT_BC7_TYPELESS,

	/// 3-channel or 4-channel block compressed format with the first three channels being an normalized, 4-7-bit
	/// unsigned integer and the last one an optional, normalized, 0-8-bit unsigned integer.
	RC_FORMAT_BC7_UNORM,

	/// 3-channel or 4-channel block compressed format with the first three channels being an normalized, 4-7-bit
	/// unsigned integer and the last one an optional, normalized, 0-8-bit unsigned integer .
	RC_FORMAT_BC7_UNORM_SRGB,

	RC_FORMAT_AYUV,            ///< 4-channel video resource format with each channel being a 8-bit value.
	RC_FORMAT_Y410,            ///< 4-channel video resource format with each of the first three channels being a
	///  10-bit value and the last one a 2-bit value.
	RC_FORMAT_Y416,            ///< 4-channel video resource format with each channel being a 16-bit value.
	RC_FORMAT_NV12,            ///< 2-channel video resource format with each channel being a 8-bit value.
	RC_FORMAT_P010,            ///< 2-channel video resource format with each channel being a 16-bit value.
	RC_FORMAT_P016,            ///< 2-channel video resource format with each channel being a 8-bit value.
	RC_FORMAT_420_OPAQUE,      ///< Video resource format with opaque layout.
	RC_FORMAT_YUY2,            ///< 4-channel video resource format with each channel being a 8-bit value.
	RC_FORMAT_Y210,            ///< 4-channel video resource format with each channel being a 16-bit value.
	RC_FORMAT_Y216,            ///< 4-channel video resource format with each channel being a 16-bit value.
	RC_FORMAT_NV11,            ///< 2-channel video resource format with each channel being a 8-bit value.
	RC_FORMAT_AI44,            ///< 4-bit palletized video resource format.
	RC_FORMAT_IA44,            ///< 4-bit palletized video resource format.
	RC_FORMAT_P8,              ///< RGB video resource format with 8-bit palletization.
	RC_FORMAT_A8P8,            ///< RGB video resource format with 8-bit palletization.
	RC_FORMAT_B4G4R4A4_UNORM,  ///< 4-channels BGRA format with each channel being a normalized 4-bit unsigned integer.

	RC_FORMAT_COUNT,  ///< Number of formats available in <c><i>RCFormat</i></c>.
};

enum class EnumName
{
	ENUM_NAME_UNKNOWN = 0,
	RCFormat = 1,
	NodeType = 2,
	PinType = 3,
	PinAccessType = 4,
	EditorAllDataType = 5,
	EditorParamDataType = 6,
};

struct EnumType
{
	void setEnumName(const std::string& i_Name) noexcept { m_Name = magic_enum::enum_cast<EnumName>(i_Name).value(); }
	EnumName getEnumName() const noexcept { return m_Name; }
	S64	enumValue() const noexcept { return m_Value; }

	template <typename T>
	T getEnum(const std::string& i_Str) const noexcept { return magic_enum::enum_cast<T>(i_Str);}

	std::string enumValueStr() const noexcept
	{
		std::string l_Str = "";
		switch (m_Name)
		{
		case EnumName::RCFormat:
			l_Str = magic_enum::enum_name((static_cast<RCFormat>(m_Value))).data();
			break;
		case EnumName::NodeType:
			l_Str = magic_enum::enum_name((static_cast<NodeType>(m_Value))).data();
			break;
		case EnumName::PinType:
			l_Str = magic_enum::enum_name((static_cast<PinType>(m_Value))).data();
			break;
		case EnumName::PinAccessType:
			l_Str = magic_enum::enum_name((static_cast<PinAccessType>(m_Value))).data();
			break;
		case EnumName::EditorAllDataType:
			l_Str = magic_enum::enum_name((static_cast<EditorAllDataType>(m_Value))).data();
			break;
		case EnumName::EditorParamDataType:
			l_Str = magic_enum::enum_name((static_cast<EditorParamDataType>(m_Value))).data();
			break;
		}

		return l_Str;
	}

	std::vector<std::string> enumValueStrVector() noexcept
	{
		std::vector<std::string> l_Result;
		switch (m_Name)
		{
		case EnumName::RCFormat:
		{
			U32 l_Count = static_cast<U32>(RCFormat::RC_FORMAT_COUNT);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<RCFormat>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::NodeType:
		{
			U32 l_Count = static_cast<U32>(NodeType::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<NodeType>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::PinType:
		{
			U32 l_Count = static_cast<U32>(PinType::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<PinType>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::PinAccessType:
		{
			U32 l_Count = static_cast<U32>(PinAccessType::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<PinAccessType>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::EditorAllDataType:
		{
			U32 l_Count = static_cast<U32>(EditorAllDataType::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<EditorAllDataType>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::EditorParamDataType:
		{
			U32 l_Count = static_cast<U32>(EditorParamDataType::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<EditorParamDataType>(i))).data());
			}
			return l_Result;
			break;
		}
		default:
			assert(false);
			break;
		}

		return l_Result;
	}

	std::string	enumAllTypeStr() noexcept
	{
		const std::vector<std::string> l_StrVec = enumValueStrVector();
		std::string l_Result{};
		for (auto l_Str : l_StrVec)
		{
			l_Result += l_Str;
			l_Result += '\0';
		}
		return l_Result;
	}

	std::string	enumNameStr() const noexcept
	{
		assert(m_Name != EnumName::ENUM_NAME_UNKNOWN);
		std::string l_Result{};
		l_Result = magic_enum::enum_name<EnumName>(m_Name);
		return l_Result;
	}

	void setEnumValue(const std::string& i_String)
	{
		switch (m_Name)
		{
		case EnumName::RCFormat:
		{
			auto l_Value = magic_enum::enum_cast<RCFormat>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::NodeType:
		{
			auto l_Value = magic_enum::enum_cast<NodeType>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::PinType:
		{
			auto l_Value = magic_enum::enum_cast<PinType>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::PinAccessType:
		{
			auto l_Value = magic_enum::enum_cast<PinAccessType>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::EditorAllDataType:
		{
			auto l_Value = magic_enum::enum_cast<EditorAllDataType>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::EditorParamDataType:
		{
			auto l_Value = magic_enum::enum_cast<EditorParamDataType>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		default:
			break;
		}
	}

	template <typename T>
	bool operator=(T i_Vaule)
	{
		return m_Value = static_cast<S64>(i_Vaule);
	}

	template <typename T>
	bool operator==(T i_Vaule) const
	{
		return m_Value == static_cast<S64>(i_Vaule);
	}

	template <typename T>
	bool operator!=(T i_Vaule) const
	{
		return !(*this == i_Vaule);
	}

	bool operator==(const EnumType& i_EnumType)
	{
		return ((m_Name == i_EnumType.m_Name) && (m_Value == i_EnumType.m_Value));
	}

	bool operator!=(const EnumType& i_EnumType)
	{
		return !(*this == i_EnumType);
	}

	EnumName m_Name = EnumName::ENUM_NAME_UNKNOWN;
	S64 m_Value = 0;
};

static inline std::string wstring2string(const std::wstring i_WStr)
{
	_bstr_t l_T = i_WStr.c_str();
	char* l_pChar = (char*)l_T;
	std::string l_Result = l_pChar;
	return l_Result;
}

static inline std::wstring string2wstring(const std::string i_Str)
{
	_bstr_t l_T = i_Str.c_str();
	wchar_t* l_pWChar = (wchar_t*)l_T;
	std::wstring l_Result = l_pWChar;
	return l_Result;
}

struct EditorParamData
{
	std::variant<std::monostate, S32, Bool, EnumType, F32, std::string, std::wstring,
		Vector2f, Vector3f, Vector4f, Matrix2f, Matrix3f, Matrix4f> m_Value{};

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
		switch (static_cast<EditorParamDataType>(i_DataType.m_Value))
		{
		case EditorParamDataType::Int: return std::to_string(std::get<S32>(m_Value));
		case EditorParamDataType::Bool: return std::to_string(std::get<Bool>(m_Value));
		case EditorParamDataType::RCFormat: 
		{
		std::string l_Str{};
		EnumType l_EnumTypeData = std::get<EnumType>(m_Value);
		l_Str = l_EnumTypeData.enumValueStr();
		return l_Str;
		}
		case EditorParamDataType::Float: return std::to_string(std::get<F32>(m_Value));
		case EditorParamDataType::String: return std::get<std::string>(m_Value);
		case EditorParamDataType::WString: return wstring2string(std::get<std::wstring>(m_Value));
		case EditorParamDataType::Vector2: return eigenMatrix2Str(std::get<Vector2f>(m_Value));
		case EditorParamDataType::Vector3: return eigenMatrix2Str(std::get<Vector3f>(m_Value));
		case EditorParamDataType::Vector4: return eigenMatrix2Str(std::get<Vector4f>(m_Value));
		case EditorParamDataType::Matrix2: return eigenMatrix2Str(std::get<Matrix2f>(m_Value));
		case EditorParamDataType::Matrix3: return eigenMatrix2Str(std::get<Matrix3f>(m_Value));
		case EditorParamDataType::Matrix4: return eigenMatrix2Str(std::get<Matrix4f>(m_Value));
		default: return std::string{};
		}
	}

	inline EditorParamData setData(EnumType i_DataType, const std::string& i_ParamDataStr)
	{
		EditorParamData l_ParamData{};
		switch (static_cast<EditorParamDataType>(i_DataType.m_Value))
		{
		case EditorParamDataType::Int: m_Value = stoi(i_ParamDataStr); break;
		case EditorParamDataType::Bool: m_Value = static_cast<Bool>(stoi(i_ParamDataStr)); break;
		case EditorParamDataType::RCFormat: { EnumType l_EnumTypeData{}; l_EnumTypeData.setEnumName("RCFormat"); l_EnumTypeData.setEnumValue(i_ParamDataStr); m_Value = l_EnumTypeData; } break;
		case EditorParamDataType::Float: m_Value = stof(i_ParamDataStr); break;
		case EditorParamDataType::String: m_Value = i_ParamDataStr; break;
		case EditorParamDataType::WString: m_Value = string2wstring(i_ParamDataStr); break;
		case EditorParamDataType::Vector2: m_Value = str2EigenMatrix<Vector2f>(i_ParamDataStr); break;
		case EditorParamDataType::Vector3: m_Value = str2EigenMatrix<Vector3f>(i_ParamDataStr); break;
		case EditorParamDataType::Vector4: m_Value = str2EigenMatrix<Vector4f>(i_ParamDataStr); break;
		case EditorParamDataType::Matrix2: m_Value = str2EigenMatrix<Matrix2f>(i_ParamDataStr); break;
		case EditorParamDataType::Matrix3: m_Value = str2EigenMatrix<Matrix3f>(i_ParamDataStr); break;
		case EditorParamDataType::Matrix4: m_Value = str2EigenMatrix<Matrix4f>(i_ParamDataStr); break;
		default:;
		}
		return l_ParamData;
	}
};

template <typename T>
static inline void str2EigenMatrix(const std::string& i_Str, T& o_Data, Eigen::IOFormat l_Format = Eigen::IOFormat(Eigen::FullPrecision, 4, ",", "\n", "[", "]", "[", "]"))
{
	std::stringstream l_StrStream(i_Str);
	std::string l_Line{};

	while (std::getline(l_StrStream, l_Line, ',')) {
		/*l_MatrixData.push_back(std::stof(l_Line))*/
		o_Data << std::stof(l_Line);
	}
	return;
}

struct EditorParamInfo
{
	std::string			m_Name = "";
	EnumType			m_DataType = { EnumName::EditorParamDataType, static_cast<S64>(EditorAllDataType::Int) };
	EditorParamData		m_Data{};
	Bool				m_UseRef = false;
	std::string			m_RefKey = "";

	EditorParamInfo() {};
	EditorParamInfo(const std::string& i_Name, const EditorParamData& i_Data) : m_Name(i_Name), m_Data(i_Data) {}
	EditorParamInfo(const std::string& i_Name, const EnumType& i_DataType, const EditorParamData& i_Data, Bool i_UseRef, const std::string& i_RefKey) {
		m_Name = i_Name;
		m_DataType = i_DataType;
		m_Data = i_Data;
		m_UseRef = i_UseRef;
		m_RefKey = i_RefKey;
	}

	EditorParamInfo(const EditorParamInfo& i_ParamInfo) noexcept {
		*this = i_ParamInfo;
	}

	EditorParamInfo& operator=(const EditorParamInfo& i_ParamInfo) noexcept {
		m_Name = i_ParamInfo.m_Name;
		m_DataType = i_ParamInfo.m_DataType;
		m_Data = i_ParamInfo.m_Data;
		m_UseRef = i_ParamInfo.m_UseRef;
		m_RefKey = i_ParamInfo.m_RefKey;

		return *this;
	}

	void initData() {
		switch (static_cast<EditorParamDataType>(m_DataType.m_Value))
		{
		case EditorParamDataType::Int: { m_Data.m_Value = S32{}; } break;
		case EditorParamDataType::Bool: {m_Data.m_Value = Bool{}; } break;
		case EditorParamDataType::RCFormat: { EnumType l_EnumTypeData{}; l_EnumTypeData.setEnumName("RCFormat"); m_Data.m_Value = l_EnumTypeData; } break;
		case EditorParamDataType::Float: { m_Data.m_Value = F32{}; } break;
		case EditorParamDataType::String: { m_Data.m_Value = std::string(); } break;
		case EditorParamDataType::WString: { m_Data.m_Value = std::wstring(); } break;
		case EditorParamDataType::Vector2: { m_Data.m_Value = static_cast<Vector2f>(Vector2f::Zero()); } break;
		case EditorParamDataType::Vector3: { m_Data.m_Value = static_cast<Vector3f>(Vector3f::Zero()); } break;
		case EditorParamDataType::Vector4: { m_Data.m_Value = static_cast<Vector4f>(Vector4f::Zero()); } break;
		case EditorParamDataType::Matrix2: { m_Data.m_Value = static_cast<Matrix2f>(Matrix2f::Identity()); } break;
		case EditorParamDataType::Matrix3: { m_Data.m_Value = static_cast<Matrix3f>(Matrix3f::Identity()); } break;
		case EditorParamDataType::Matrix4: { m_Data.m_Value = static_cast<Matrix4f>(Matrix4f::Identity()); } break;
		default:;
		}
	}

	void setData(const std::string& i_Key, const std::string& i_Value)
	{
		if (i_Key == "Name")
			m_Name = i_Value;
		else if (i_Key == "DataType")
			m_DataType.setEnumValue(i_Value);
		else if (i_Key == "Data")
			m_Data.setData(m_DataType, i_Value);
		else if (i_Key == "UseRef")
			m_UseRef = static_cast<Bool>(stoi(i_Value));
		else if (i_Key == "RefKey")
			m_RefKey = m_RefKey;
	}

	std::string getData(const std::string& i_Key)
	{
		if (i_Key == "Name")
			return m_Name;
		else if (i_Key == "DataType")
			return m_DataType.enumValueStr();
		else if (i_Key == "Data")
			return m_Data.getData(m_DataType);
		else if (i_Key == "UseRef")
			return std::to_string(m_UseRef);
		else if (i_Key == "RefKey")
			return  m_RefKey;

		return "";
	}
};

template<EditorParamDataType DataType>
auto getEditorData(EditorParamData i_Data)
{
	if (DataType == EditorParamDataType::Int)
		return std::get<S32>(i_Data.m_Value);
}

static inline GUID createGuid()
{
	GUID l_Guid;
	CoCreateGuid(&l_Guid);
	return l_Guid;
}

static inline std::string guidToString(const GUID& i_Guid)
{
	char l_Buf[64] = { 0 };
	_snprintf_s(
		l_Buf,
		sizeof(l_Buf),
		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		i_Guid.Data1, i_Guid.Data2, i_Guid.Data3,
		i_Guid.Data4[0], i_Guid.Data4[1],
		i_Guid.Data4[2], i_Guid.Data4[3],
		i_Guid.Data4[4], i_Guid.Data4[5],
		i_Guid.Data4[6], i_Guid.Data4[7]);
	return std::string(l_Buf);
}

class Node;

struct PinInfo
{
	friend class Pin;
	friend class NodeManager;

public:
	PinInfo() noexcept {}
	PinInfo(const NodeID i_NodeID, const PinInfo& i_Info) noexcept {
		*this = i_Info;
		m_NodeID = i_NodeID;
	}
	PinInfo(const std::string_view i_Name, const std::string_view i_MetaData, EditorAllDataType i_DataType, PinType i_PinType, PinAccessType i_AccessFlags) noexcept {
		m_Name = i_Name;
		m_MetaData = i_MetaData;
		m_DataType = i_DataType;
		m_PinType = i_PinType;
		m_PinAccessType = i_AccessFlags;
	}

	PinInfo& operator=(const PinInfo& i_Info) noexcept {
		m_Name = i_Info.m_Name;
		m_MetaData = i_Info.m_MetaData;
		m_DataType = i_Info.m_DataType;
		m_PinType = i_Info.m_PinType;
		m_PinAccessType = i_Info.m_PinAccessType;
		m_IsTemplate = i_Info.m_IsTemplate;
		m_IsConnection = i_Info.m_IsConnection;

		return *this;
	}

	template<typename T>
	void setData(const std::string& i_Key, const T& i_Value)
	{
		if (i_Key == "Name")
			m_Name = i_Value;
		else if (i_Key == "MetaData")
			m_MetaData = i_Value;
		else if (i_Key == "PinAccessType")
			m_PinAccessType.setEnumValue(i_Value);
		else if (i_Key == "DataType")
			m_DataType.setEnumValue(i_Value);
	}

	std::string getData(const std::string& i_Key)
	{
		if (i_Key == "Name")
			return m_Name;
		else if (i_Key == "MetaData")
			return m_MetaData;
		else if (i_Key == "PinAccessType")
			return m_PinAccessType.enumValueStr();
		else if (i_Key == "DataType")
			return m_DataType.enumValueStr();

		return "";
	}

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

	std::string			m_Name = "";
	std::string			m_MetaData = "";
	NodeID				m_NodeID = 0;
	std::set<LinkID>    m_LinkIDSet{};
	//EditorParamData     m_Data{};

	Bool				m_IsTemplate = false;
	Bool				m_IsConnection = false;
	EnumType			m_DataType = { EnumName::EditorAllDataType, static_cast<S64>(EditorAllDataType::Int) };
	EnumType			m_PinType = { EnumName::PinType, static_cast<S64>(PinType::Input) };
	EnumType			m_PinAccessType = { EnumName::PinAccessType, static_cast<S64>(PinAccessType::Readable) };
};

struct NodeInfo
{
	NodeInfo() noexcept {}
	NodeInfo(const NodeInfo& i_Info) noexcept { *this = i_Info; }

	NodeInfo(NodeInfo&& i_Info) noexcept {
		m_GUID = i_Info.m_GUID;

		m_ParentNodeID = i_Info.m_ParentNodeID;
		m_Name = i_Info.m_Name;
		m_TemplateName = i_Info.m_TemplateName;
		m_ClassName = i_Info.m_ClassName;
		m_Comment = i_Info.m_Comment;
		m_Type = i_Info.m_Type;

		m_TemplateConnectionOutputPinIDs = i_Info.m_TemplateConnectionOutputPinIDs;
		m_TemplateConnectionInputPinIDs = i_Info.m_TemplateConnectionInputPinIDs;
		m_TemplateInputPinIDs = i_Info.m_TemplateInputPinIDs;
		m_TemplateOutputPinIDs = i_Info.m_TemplateOutputPinIDs;
		m_TemplateParams = i_Info.m_TemplateParams;

		m_IsDynamicConnectionPin = i_Info.m_IsDynamicConnectionPin;
		m_ConnectionOutputPinIDs = i_Info.m_ConnectionOutputPinIDs;
		m_ConnectionInputPinIDs = i_Info.m_ConnectionInputPinIDs;
		m_IsDynamicInputPin = i_Info.m_IsDynamicInputPin;
		m_InputPinIDs = i_Info.m_InputPinIDs;
		m_IsDynamicOutputPin = i_Info.m_IsDynamicOutputPin;
		m_OutputPinIDs = i_Info.m_OutputPinIDs;
		m_LinkIDSet = i_Info.m_LinkIDSet;

		m_IsDynamicParam = i_Info.m_IsDynamicParam;
		m_Params = i_Info.m_Params;
	}

	NodeInfo(std::string_view i_Name, std::string_view i_ClassName, std::string_view i_Comment, NodeType i_Type, Bool i_IsDynamicInputPin, std::vector<PinID>&& i_InputPinIDs,
		Bool i_IsDynamicConnectionPin, std::vector<PinID>&& i_ConnectionInputPinIDs, std::vector<PinID>&& i_ConnectionOutputPinIDs, Bool i_IsDynamicOutputPin, std::vector<PinID>&& i_OutputPinIDs, Bool i_IsDynamicParam, std::vector<EditorParamInfo>&& i_Params) noexcept {
		m_Name = i_Name;
		m_ClassName = i_Name;
		m_Comment = i_Comment;
		m_Type = i_Type;

		m_IsDynamicConnectionPin = i_IsDynamicConnectionPin;
		m_ConnectionOutputPinIDs = i_ConnectionOutputPinIDs;
		m_ConnectionInputPinIDs = i_ConnectionInputPinIDs;

		m_IsDynamicInputPin = i_IsDynamicInputPin;
		m_InputPinIDs = i_InputPinIDs;
		m_IsDynamicOutputPin = i_IsDynamicOutputPin;
		m_OutputPinIDs = i_OutputPinIDs;
		m_IsDynamicParam = i_IsDynamicParam;
		m_Params = i_Params;

		m_GUID = guidToString(createGuid());
	}

	NodeInfo& operator=(const NodeInfo& i_Info) noexcept {
		m_GUID = guidToString(createGuid());

		m_ParentNodeID = i_Info.m_ParentNodeID;
		m_Name = i_Info.m_Name;
		m_TemplateName = i_Info.m_TemplateName;
		m_ClassName = i_Info.m_ClassName;
		m_Comment = i_Info.m_Comment;
		m_Type = i_Info.m_Type;

		m_IsDynamicConnectionPin = i_Info.m_IsDynamicConnectionPin;
		m_IsDynamicInputPin = i_Info.m_IsDynamicInputPin;
		m_IsDynamicOutputPin = i_Info.m_IsDynamicOutputPin;
		m_IsDynamicParam = i_Info.m_IsDynamicParam;
		
		return *this; 
	}

	void setData(const std::string& i_Key, const std::string& i_Value)
	{
		if (i_Key == "NodeName")
			m_Name = i_Value;
		else if (i_Key == "TemplateName")
			m_TemplateName = i_Value;
		else if (i_Key == "ClassName")
			m_ClassName = i_Value;
		else if (i_Key == "NodeType")
			m_Type.setEnumValue(i_Value);
		else if (i_Key == "Comment")
			m_Comment = i_Value;
		else if (i_Key == "IsDynamicConnectionPin")
			m_IsDynamicConnectionPin = static_cast<Bool>(std::stoi(i_Value));
		else if (i_Key == "IsDynamicInputPin")
			m_IsDynamicInputPin = static_cast<Bool>(std::stoi(i_Value));
		else if (i_Key == "IsDynamicOutputPin")
			m_IsDynamicOutputPin = static_cast<Bool>(std::stoi(i_Value));
		else if (i_Key == "IsDynamicParam")
			m_IsDynamicParam = static_cast<Bool>(std::stoi(i_Value));

		return;
	}

	std::string getData(const std::string& i_Key)
	{
		if (i_Key == "NodeName")
			return m_Name;
		else if (i_Key == "TemplateName")
			return "";
		else if (i_Key == "ClassName")
			return m_ClassName;
		else if (i_Key == "NodeType")
			return m_Type.enumValueStr();
		else if (i_Key == "Comment")
			return m_Comment;
		else if (i_Key == "IsDynamicConnectionPin")
			return  std::to_string(m_IsDynamicConnectionPin);
		else if (i_Key == "IsDynamicInputPin")
			return std::to_string(m_IsDynamicInputPin);
		else if (i_Key == "IsDynamicOutputPin")
			return std::to_string(m_IsDynamicOutputPin);
		else if (i_Key == "IsDynamicParam")
			return std::to_string(m_IsDynamicParam);

		return "";
	}

	ImColor getNodeColor(NodeType i_Type) noexcept
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
		case NodeType::TASK:
			l_Color = ImColor(255, 255, 255);
			return l_Color;
		default:
			break;
		}

		assert(false);
		return l_Color;
	}

	ImColor	color() noexcept { return getNodeColor(static_cast<NodeType>(m_Type.m_Value)); }

	const std::vector<PinID>& getPinIDList(Bool i_IsTemplate, Bool i_IsConnection, const EnumType& i_PinType) const noexcept {
		if (i_IsTemplate) {
			if (i_IsConnection) {
				if (i_PinType == PinType::Input) {
					return m_TemplateConnectionInputPinIDs;
				}
				else {
					return m_TemplateConnectionOutputPinIDs;
				}
			}
			else if (i_PinType == PinType::Input) {
				return m_TemplateInputPinIDs;
			}
			else {
				return m_TemplateOutputPinIDs;
			}
		}
		else {
			if (i_IsConnection) {
				if (i_PinType == PinType::Input) {
					return m_ConnectionInputPinIDs;
				}
				else {
					return m_ConnectionOutputPinIDs;
				}
			}
			else if (i_PinType == PinType::Input) {
				return m_InputPinIDs;
			}
			else {
				return m_OutputPinIDs;
			}
		}
	}

	const std::vector<PinID>& getPinIDList(const std::shared_ptr<PinInfo>& i_pPinInfo) const noexcept {
		return getPinIDList(i_pPinInfo->m_IsTemplate, i_pPinInfo->m_IsConnection, i_pPinInfo->m_PinType);
	}

	std::string m_GUID = "";

	NodeID m_ParentNodeID = 0;
	std::string	m_Name = "";
	std::string m_TemplateName = "";
	std::string m_ClassName = "";
	std::string	m_Comment = "";
	EnumType m_Type = { EnumName::NodeType, static_cast<S64>(NodeType::CPU) };

	std::vector<PinID> m_TemplateConnectionInputPinIDs{};
	std::vector<PinID> m_TemplateConnectionOutputPinIDs{};
	std::vector<PinID> m_TemplateInputPinIDs{};
	std::vector<PinID> m_TemplateOutputPinIDs{};
	std::vector<EditorParamInfo> m_TemplateParams{};

	Bool m_IsDynamicConnectionPin = false;
	std::vector<PinID> m_ConnectionInputPinIDs{};
	std::vector<PinID> m_ConnectionOutputPinIDs{};
	Bool m_IsDynamicInputPin = false;
	std::vector<PinID> m_InputPinIDs{};
	Bool m_IsDynamicOutputPin = false;
	std::vector<PinID> m_OutputPinIDs{};
	std::set<LinkID> m_LinkIDSet{};

	Bool m_IsDynamicParam = false;
	std::vector<EditorParamInfo> m_Params{};
};

struct LinkInfo
{
	NodeID m_OutputNodeID = 0;
	LinkID m_OutputPinID = 0;
	NodeID m_InputNodeID = 0;
	LinkID m_InputPinID = 0;

	ImColor m_Color = ImColor(0.0f);
};

struct LinkFileInfo 
{
	U32 m_PinIndex = 0;
	Bool m_IsTemplate = false;
	Bool m_IsConnection = false;
};

static inline bool splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	const ImGuiID id = window->GetID("##Splitter");
	ImRect bb;
	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

template <typename T>
static inline std::string serializeEigenMatrix(const T& i_Data, Eigen::IOFormat l_Format = Eigen::IOFormat(Eigen::FullPrecision, 4, ",", "", "", "", "", ""))
{
	std::stringstream l_StrStream{};
	l_StrStream << i_Data.format(l_Format);
	return l_StrStream.str();
}

template <typename T>
static inline void parseEigenMatrix(const std::string& i_Str, T& o_Data, Eigen::IOFormat l_Format = Eigen::IOFormat(Eigen::FullPrecision, 4, ",", "\n", "[", "]", "[", "]"))
{
	std::stringstream l_StrStream(i_Str);
	std::string l_Line{};

	while (std::getline(l_StrStream, l_Line, ',')) {
		/*l_MatrixData.push_back(std::stof(l_Line))*/
		o_Data << std::stof(l_Line);
	}
	return;
}

static inline void serializeEditorParamData(tinyxml2::XMLElement* i_XMLElement, rceditor::EnumType i_DataType, const EditorParamData& i_Data)
{
	//i_XMLElement->SetAttribute("DataType", i_DataType.enumValueStr().c_str());

	//switch (static_cast<EditorParamDataType>(i_DataType.m_Value))
	//{
	//case EditorParamDataType::Int:
	//	i_XMLElement->SetAttribute("Data", std::get<S32>(i_Data.m_Value));
	//	break;

	//case EditorParamDataType::Bool:
	//	i_XMLElement->SetAttribute("Data", std::get<Bool>(i_Data.m_Value));
	//	break;

	//case EditorParamDataType::RCFormat:
	//{
	//	i_XMLElement->SetAttribute("Data", std::get<EnumType>(i_Data.m_Value).enumNameStr().c_str());
	//}
	//	break;

	//case EditorParamDataType::Float:
	//	i_XMLElement->SetAttribute("Data", std::get<F32>(i_Data.m_Value));
	//	break;

	//case EditorParamDataType::String:
	//	i_XMLElement->SetAttribute("Data", std::get<std::string>(i_Data.m_Value).c_str());
	//	break;

	//case EditorParamDataType::WString:
	//	i_XMLElement->SetAttribute("Data", std::get<std::wstring>(i_Data.m_Value).c_str());
	//	break;

	//case EditorParamDataType::Vector2:
	//	i_XMLElement->SetAttribute("Data", serializeEigenMatrix(std::get<Vector2f>(i_Data.m_Value)).c_str());
	//	break;

	//case EditorParamDataType::Vector3:
	//	i_XMLElement->SetAttribute("Data", serializeEigenMatrix(std::get<Vector3f>(i_Data.m_Value)).c_str());
	//	break;

	//case EditorParamDataType::Vector4:
	//	i_XMLElement->SetAttribute("Data", serializeEigenMatrix(std::get<Vector4f>(i_Data.m_Value)).c_str());
	//	break;

	//case EditorParamDataType::Matrix2:
	//	i_XMLElement->SetAttribute("Data", serializeEigenMatrix(std::get<Matrix2f>(i_Data.m_Value)).c_str());
	//	break;

	//case EditorParamDataType::Matrix3:
	//	i_XMLElement->SetAttribute("Data", serializeEigenMatrix(std::get<Matrix3f>(i_Data.m_Value)).c_str());
	//	break;

	//case EditorParamDataType::Matrix4:
	//	i_XMLElement->SetAttribute("Data", serializeEigenMatrix(std::get<Matrix4f>(i_Data.m_Value)).c_str());
	//	break;

	//default:
	//	break;
	//}

	return;
}

static inline std::tuple<EditorParamDataType, EditorParamData> parseEditorParamData(tinyxml2::XMLElement* i_XMLElement)
{
	//tinyxml2::XMLElement* l_XmlDataType = i_XMLElement->FirstChildElement("DataType");
	//tinyxml2::XMLElement* l_XmlData = i_XMLElement->FirstChildElement("DataType");

	//std::string l_DataStr{};
	//EditorParamData l_Data{};

	//int l_DataTypeInt = 0;
	//l_XmlDataType->FindAttribute("DataType")->QueryIntValue(&l_DataTypeInt);
	//EditorParamDataType l_DataType = static_cast<EditorParamDataType>(l_DataTypeInt);

	//F32 l_ValueFloat{};
	//Vector2f l_ValueVector2f{};
	//Vector3f l_ValueVector3f{};
	//Vector2f l_ValueVector4f{};
	//Matrix2f l_ValueMatrix2f{};
	//Matrix3f l_ValueMatrix3f{};
	//Matrix4f l_ValueMatrix4f{};
	//switch (l_DataType)
	//{
	//case EditorParamDataType::Int:
	//{
	//	S32 l_ValueInt{};
	//	l_XmlDataType->FindAttribute("Data")->QueryIntValue(&l_ValueInt);
	//	l_Data.m_Value = static_cast<S32>(l_ValueInt);
	//}
	//	break;

	//case EditorParamDataType::Bool:
	//{
	//	Bool l_ValueBool{};
	//	l_XmlDataType->FindAttribute("Data")->QueryBoolValue(&l_ValueBool);
	//	l_Data.m_Value = l_ValueBool;
	//}
	//	break;

	//case EditorParamDataType::RCFormat:
	//{
	//	EnumType l_EnumType{};
	//	std::string l_Str{};
	//	l_EnumType.setEnumName("RCFormat");
	//	l_EnumType.se
	//	l_XmlDataType->FindAttribute("EnumName")->QueryValue(&l_Str);
	//	l_Data.m_Value = l_EnumType;
	//}

	//	break;

	//case EditorParamDataType::Float:
	//	l_XmlDataType->FindAttribute("Data")->QueryFloatValue(&l_ValueFloat);
	//	l_Data.m_Value = l_ValueFloat;
	//	break;

	//case EditorParamDataType::String:
	//	l_Data.m_Value = l_XmlDataType->FindAttribute("Data")->Value();
	//	break;

	//case EditorParamDataType::WString:
	//	l_Data.m_Value = l_XmlDataType->FindAttribute("Data")->Value();
	//	break;

	//case EditorParamDataType::Vector2:
	//	parseEigenMatrix(l_XmlDataType->FindAttribute("Data")->Value(), l_ValueVector2f);
	//	l_Data.m_Value = l_ValueVector2f;
	//	break;

	//case EditorParamDataType::Vector3:
	//	parseEigenMatrix(l_XmlDataType->FindAttribute("Data")->Value(), l_ValueVector3f);
	//	l_Data.m_Value = l_ValueVector3f;
	//	break;

	//case EditorParamDataType::Vector4:
	//	parseEigenMatrix(l_XmlDataType->FindAttribute("Data")->Value(), l_ValueVector4f);
	//	l_Data.m_Value = l_ValueVector4f;
	//	break;

	//case EditorParamDataType::Matrix2:
	//	parseEigenMatrix(l_XmlDataType->FindAttribute("Data")->Value(), l_ValueMatrix2f);
	//	l_Data.m_Value = l_ValueMatrix2f;
	//	break;

	//case EditorParamDataType::Matrix3:
	//	parseEigenMatrix(l_XmlDataType->FindAttribute("Data")->Value(), l_ValueMatrix3f);
	//	l_Data.m_Value = l_ValueMatrix3f;
	//	break;

	//case EditorParamDataType::Matrix4:
	//	parseEigenMatrix(l_XmlDataType->FindAttribute("Data")->Value(), l_ValueMatrix4f);
	//	l_Data.m_Value = l_ValueMatrix4f;
	//	break;

	//default:
	//	break;
	//}

	//return { l_DataType, l_Data };
	return {};
}

//NodeID getNodeID(U32 i_Index) noexcept { return NODE_ID_OFFSET + i_Index; }
//U32 getNodeIndex(NodeID i_ID) noexcept { return i_ID - NODE_ID_OFFSET; }
//
//LinkID getLinkID(U32 i_Index) noexcept { return LINK_ID_OFFSET + i_Index; }
//U32 getLinkIndex(LinkID i_ID) noexcept { return i_ID - LINK_ID_OFFSET; }
//
//PinID getPinID(const PinIDInfo& l_PinIDInfo) noexcept { return l_PinIDInfo.m_NodeID * PIN_BITS + static_cast<U32>(l_PinIDInfo.m_PinType) * PIN_TYPE_BITS + l_PinIDInfo.m_PinIndex; };
//PinIDInfo getPinIDInfo(PinID i_PinID) noexcept {
//	PinIDInfo l_PinInfo{};
//	l_PinInfo.m_NodeID = static_cast<U64>(i_PinID / PIN_BITS);
//	l_PinInfo.m_PinIndex = static_cast<U64>(i_PinID / PIN_TYPE_BITS);
//	l_PinInfo.m_PinType = static_cast<PinType>((i_PinID - l_PinInfo.m_NodeID - l_PinInfo.m_PinIndex) / PIN_TYPE_BITS);
//
//	return l_PinInfo;
//}

RCEDITOR_NAMESPACE_END
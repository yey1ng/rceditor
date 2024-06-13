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
using NodeGUID = std::string;

constexpr U64 TEMPLATE_PIN_ID_OFFSET = 0e8;
constexpr U64 NODE_ID_OFFSET = 1e8;
constexpr U64 LINK_ID_OFFSET = 2e8;
constexpr U64 PIN_ID_OFFSET = 3e8;

constexpr std::string_view g_RootNodeStackStr = "Root";

constexpr std::string_view g_XmlNodeStr = "RCNode";
constexpr std::string_view g_XmlGraphStr = "RCGraph";
constexpr std::string_view g_XmlTaskNodeStr = "RCTaskNode";
constexpr std::string_view g_XmlTemplateNodeStr = "RCTemplateNode";
constexpr std::string_view g_XmlFavoriteNodeStr = "RCFavoriteNode";
constexpr std::string_view g_XmlEditorInfoStr = "EditorInfo";
constexpr std::string_view g_XmlStartNodeStr = "StartNode";
constexpr std::string_view g_XmlEndNodeStr = "EndNode";

constexpr std::string_view g_XmlLinkStr = "Link";
constexpr std::string_view g_XmlGraphLinkStr = "GraphLink";
constexpr std::string_view g_XmlTaskListStr = "RCTaskList";
constexpr std::string_view g_XmlNodeListStr = "RCNodeList";
constexpr std::string_view g_XmlTaskLinkListStr = "RCTaskLinkList";
constexpr std::string_view g_XmlNodeLinkListStr = "RCNodeLinkList";

constexpr std::string_view g_XmlNodeLocationX = "LocationX";
constexpr std::string_view g_XmlNodeLocationY = "LocationY";
constexpr std::string_view g_XmlNodeGroupSizeX = "GroupSizeX";
constexpr std::string_view g_XmlNodeGroupSizeY = "GroupSizeY";

constexpr std::string_view g_XmlLinkProducerStr = "Producer";
constexpr std::string_view g_XmlLinkConsumerStr = "Consumer";
constexpr std::string_view g_XmlInputOutputListStr = "InputOutputList";
constexpr std::string_view g_XmlInputListStr = "InputList";
constexpr std::string_view g_XmlOutputListStr = "OutputList";
constexpr std::string_view g_XmlInputOutputStr = "InputOutput";
constexpr std::string_view g_XmlInputStr = "Input";
constexpr std::string_view g_XmlOutputStr = "Output";
constexpr std::string_view g_XmlTemplateParamListStr = "TemplateParamList";
constexpr std::string_view g_XmlParamListStr = "ParamList";
constexpr std::string_view g_XmlParamStr = "Param";

constexpr std::string_view g_XmlNameStr = "Name";
constexpr std::string_view g_XmlNodeNameStr = "NodeName";
constexpr std::string_view g_XmlNodeIDStr = "NodeID";
constexpr std::string_view g_XmlParentIDStr = "ParentNodeID";
constexpr std::string_view g_XmlRelativeIDStr = "RelativeNodeID";
constexpr std::string_view g_XmlLocationXStr = "LocationX";
constexpr std::string_view g_XmlLocationYStr = "LocationY";
constexpr std::string_view g_XmlPinNameStr = "PinName";
constexpr std::string_view g_XmlTemplateNameStr = "TemplateName";
constexpr std::string_view g_XmlClassNameStr = "ClassName";
constexpr std::string_view g_XmlQueueFamilyStr = "QueueFamily";
constexpr std::string_view g_XmlIsDynamicInputOutputStr = "IsDynamicInputOutput";
constexpr std::string_view g_XmlIsDynamicInputStr = "IsDynamicInput";
constexpr std::string_view g_XmlIsDynamicOutputStr = "IsDynamicOutput";
constexpr std::string_view g_XmlIsDynamicParamStr = "IsDynamicParam";
constexpr std::string_view g_XmlCommentStr = "Comment";
constexpr std::string_view g_XmlNodeTypeStr = "NodeType";
constexpr std::string_view g_XmlNodeTagStr = "NodeTag";
constexpr std::string_view g_XmlInputMetaDataStr = "InputMetaData";
constexpr std::string_view g_XmlOutputMetaDataStr = "OutputMetaData";
constexpr std::string_view g_XmlMetaDataStr = "MetaData";
constexpr std::string_view g_XmlDataTypeStr = "DataType";
constexpr std::string_view g_XmlAccessTypeStr = "AccessType";
constexpr std::string_view g_XmlUseRef = "UseRef";
constexpr std::string_view g_XmlRefKey = "RefKey";

constexpr std::string_view g_XmlErrorKey = "Error";


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

enum class EditorDataType : S64
{
	//Unknown = 0,
	Int,
	Bool,
	Float,
	String,
	WString,
	Vector2f,
	Vector3f,
	Vector4f,
	Matrix2f,
	Matrix3f,
	Matrix4f,
	RCFormat,
	Buffer,
	Image2D,
	Count
};

enum class ParamDataType : S64
{
	//Unknown = 0,
	Int,
	Bool,
	Float,
	String,
	WString,
	Vector2f,
	Vector3f,
	Vector4f,
	Matrix2f,
	Matrix3f,
	Matrix4f,
	RCFormat,
	VkBufferUsage,
	VkImageUsage,
	Count,
};

enum class PinType : S64
{
	Input,
	Output,
	InputOutput,
	//ConnectionInput,
	//ConnectionOutput,
	Count,
};

enum class PinAccessType : S64
{
	Readable,
	Writable,
	Count,
};

enum class NodeType : S64
{
	//Unknown = 0,
	CPU,
	GPU,
	PS,
	CS,
	Start,
	End,
	Composite,
	Task,
	Data,
	Count,
};

enum class NodeTag : S64
{
	NoTag = 1 << 0,
	Host = 1 << 1,
	Vulkan = 1 << 2,
	Block = 1 << 3,
};

enum class VkBufferUsage
{
	VK_BUFFER_USAGE_TRANSFER_SRC_BIT = 0x00000001,
	VK_BUFFER_USAGE_TRANSFER_DST_BIT = 0x00000002,
	VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT = 0x00000004,
	VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT = 0x00000008,
	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x00000010,
	VK_BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x00000020,
	VK_BUFFER_USAGE_INDEX_BUFFER_BIT = 0x00000040,
	VK_BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x00000080,
	VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT = 0x00000100,
	VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT = 0x00020000,
	VK_BUFFER_USAGE_VIDEO_DECODE_SRC_BIT_KHR = 0x00002000,
	VK_BUFFER_USAGE_VIDEO_DECODE_DST_BIT_KHR = 0x00004000,
	VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT = 0x00000800,
	VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT = 0x00001000,
	VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT = 0x00000200,
	VK_BUFFER_USAGE_EXECUTION_GRAPH_SCRATCH_BIT_AMDX = 0x02000000,
	VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR = 0x00080000,
	VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR = 0x00100000,
	VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR = 0x00000400,
	VK_BUFFER_USAGE_VIDEO_ENCODE_DST_BIT_KHR = 0x00008000,
	VK_BUFFER_USAGE_VIDEO_ENCODE_SRC_BIT_KHR = 0x00010000,
	VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT = 0x00200000,
	VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT = 0x00400000,
	VK_BUFFER_USAGE_PUSH_DESCRIPTORS_DESCRIPTOR_BUFFER_BIT_EXT = 0x04000000,
	VK_BUFFER_USAGE_MICROMAP_BUILD_INPUT_READ_ONLY_BIT_EXT = 0x00800000,
	VK_BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT = 0x01000000,
	VK_BUFFER_USAGE_RAY_TRACING_BIT_NV = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,
	VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
	VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
	VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
};

// 按位操作符重载
constexpr VkBufferUsage operator|(VkBufferUsage lhs, VkBufferUsage rhs) {
	return static_cast<VkBufferUsage>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr VkBufferUsage operator&(VkBufferUsage lhs, VkBufferUsage rhs) {
	return static_cast<VkBufferUsage>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr VkBufferUsage operator^(VkBufferUsage lhs, VkBufferUsage rhs) {
	return static_cast<VkBufferUsage>(static_cast<uint32_t>(lhs) ^ static_cast<uint32_t>(rhs));
}

constexpr VkBufferUsage operator~(VkBufferUsage flag) {
	return static_cast<VkBufferUsage>(~static_cast<uint32_t>(flag));
}

enum class VkImageUsage {
	VK_IMAGE_USAGE_TRANSFER_SRC_BIT = 0x00000001,
	VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x00000002,
	VK_IMAGE_USAGE_SAMPLED_BIT = 0x00000004,
	VK_IMAGE_USAGE_STORAGE_BIT = 0x00000008,
	VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00000010,
	VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x00000020,
	VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT = 0x00000040,
	VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT = 0x00000080,
	// Provided by VK_KHR_video_decode_queue
	VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR = 0x00000400,
	// Provided by VK_KHR_video_decode_queue
	VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR = 0x00000800,
	// Provided by VK_KHR_video_decode_queue
	VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR = 0x00001000,
	// Provided by VK_EXT_fragment_density_map
	VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT = 0x00000200,
	// Provided by VK_KHR_fragment_shading_rate
	VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR = 0x00000100,
	// Provided by VK_EXT_host_image_copy
	VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT = 0x00400000,
	// Provided by VK_KHR_video_encode_queue
	VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR = 0x00002000,
	// Provided by VK_KHR_video_encode_queue
	VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR = 0x00004000,
	// Provided by VK_KHR_video_encode_queue
	VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR = 0x00008000,
	// Provided by VK_EXT_attachment_feedback_loop_layout
	VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT = 0x00080000,
	// Provided by VK_HUAWEI_invocation_mask
	VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI = 0x00040000,
	// Provided by VK_QCOM_image_processing
	VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM = 0x00100000,
	// Provided by VK_QCOM_image_processing
	VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM = 0x00200000,
	// Provided by VK_NV_shading_rate_image
	VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV = VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
};

// 按位操作符重载
constexpr VkImageUsage operator|(VkImageUsage lhs, VkImageUsage rhs) {
	return static_cast<VkImageUsage>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr VkImageUsage operator&(VkImageUsage lhs, VkImageUsage rhs) {
	return static_cast<VkImageUsage>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr VkImageUsage operator^(VkImageUsage lhs, VkImageUsage rhs) {
	return static_cast<VkImageUsage>(static_cast<uint32_t>(lhs) ^ static_cast<uint32_t>(rhs));
}

constexpr VkImageUsage operator~(VkImageUsage flag) {
	return static_cast<VkImageUsage>(~static_cast<uint32_t>(flag));
}

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

enum class TaskQueueFamily : S64
{
	Graphic1,
	Graphic2,
	Compute1,
	Compute2,
	Transfer,
	Count
};

enum class TaskThread : S64
{
	Thread1,
	Thread2,
	Thread3,
	Thread4,
	Thread5,
	Count
};

enum class TaskBlock : S64
{
	Block,
	//HostBlock,
	//VulkanBlock,
	Count
};

enum class Prototype
{
	Template,
	FavoriteNormalNode,
	FavoriteChildNode,
	FavoriteCompositeNode,
	Count,
};

enum class SearchType
{
	Node,
	Template,
	//Tag,
	Count,
};

enum class LinkType
{
	Success = 0,
	SamePin,
	SameNode,
	PinTypeDifferent,
	DataTypeDifferent,
	MetaDataDifferent,
	PinAccessTypeError,
	TaskPinAccessTypeError,

	LinkTypeCount,
};

enum class WidgetTypeInt
{
	None = 0,
	Slider,
	Count,
};

enum class WidgetTypeFloat
{
	None = 0,
	Slider,
	Count,
};

enum class WidgetTypeString
{
	None = 0,
	File,
	Dir,
	Count,
};

enum class WidgetTypeWString
{
	None = 0,
	File,
	Dir,
	Count,
};

enum class WidgetTypeVector3f
{
	None = 0,
	ColorPick,
	Count,
};

enum class WidgetTypeVector4f
{
	None = 0,
	ColorPick,
	Count,
};

enum class EnumName
{
	ENUM_NAME_UNKNOWN = 0,
	VkBufferUsage,
	VkImageUsage,
	RCFormat,
	NodeType,
	PinType,
	PinAccessType,
	EditorDataType,
	ParamDataType,
	TaskQueueFamily,
	TaskThread,
	TaskBlock,
	Prototype,
	SearchType,
	NodeTag,
	WidgetTypeInt,
	WidgetTypeFloat,
	WidgetTypeString,
	WidgetTypeWString,
	WidgetTypeVector3f,
	WidgetTypeVector4f,
};

struct EnumType
{
	void setTypeName(const std::string& i_Name) noexcept { m_Name = magic_enum::enum_cast<EnumName>(i_Name).value(); }
	EnumName getTypeName() const noexcept { return m_Name; }
	S64	enumValue() const noexcept { return m_Value; }

	template <typename T>
	T getEnum(const std::string& i_Str) const noexcept { return magic_enum::enum_cast<T>(i_Str); }

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
		case EnumName::EditorDataType:
		{
			U32 l_Count = static_cast<U32>(EditorDataType::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<EditorDataType>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::ParamDataType:
		{
			U32 l_Count = static_cast<U32>(ParamDataType::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<ParamDataType>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::TaskQueueFamily:
		{
			U32 l_Count = static_cast<U32>(TaskQueueFamily::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<TaskQueueFamily>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::TaskThread:
		{
			U32 l_Count = static_cast<U32>(TaskThread::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<TaskThread>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::TaskBlock:
		{
			U32 l_Count = static_cast<U32>(TaskBlock::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<TaskBlock>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::SearchType:
		{
			U32 l_Count = static_cast<U32>(SearchType::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<SearchType>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::WidgetTypeInt:
		{
			U32 l_Count = static_cast<U32>(WidgetTypeInt::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<WidgetTypeInt>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::WidgetTypeFloat:
		{
			U32 l_Count = static_cast<U32>(WidgetTypeFloat::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<WidgetTypeFloat>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::WidgetTypeString:
		{
			U32 l_Count = static_cast<U32>(WidgetTypeString::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<WidgetTypeString>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::WidgetTypeWString:
		{
			U32 l_Count = static_cast<U32>(WidgetTypeWString::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<WidgetTypeWString>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::WidgetTypeVector3f:
		{
			U32 l_Count = static_cast<U32>(WidgetTypeVector3f::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<WidgetTypeVector3f>(i))).data());
			}
			return l_Result;
			break;
		}
		case EnumName::WidgetTypeVector4f:
		{
			U32 l_Count = static_cast<U32>(WidgetTypeVector4f::Count);
			l_Result.reserve(l_Count);
			for (U32 i = 0; i < l_Count; ++i)
			{
				l_Result.emplace_back(magic_enum::enum_name((static_cast<WidgetTypeVector4f>(i))).data());
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

	std::string enumValueStr() const noexcept
	{
		std::string l_Str = "";
		switch (m_Name)
		{
		case EnumName::VkBufferUsage:
			l_Str = magic_enum::enum_flags_name((static_cast<VkBufferUsage>(m_Value))).data();
			break;
		case EnumName::VkImageUsage:
			l_Str = magic_enum::enum_flags_name((static_cast<VkImageUsage>(m_Value))).data();
			break;
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
		case EnumName::EditorDataType:
			l_Str = magic_enum::enum_name((static_cast<EditorDataType>(m_Value))).data();
			break;
		case EnumName::ParamDataType:
			l_Str = magic_enum::enum_name((static_cast<ParamDataType>(m_Value))).data();
			break;
		case EnumName::TaskQueueFamily:
			l_Str = magic_enum::enum_name((static_cast<TaskQueueFamily>(m_Value))).data();
			break;
		case EnumName::TaskThread:
			l_Str = magic_enum::enum_name((static_cast<TaskThread>(m_Value))).data();
			break;
		case EnumName::TaskBlock:
			l_Str = magic_enum::enum_name((static_cast<TaskBlock>(m_Value))).data();
			break;
		case EnumName::Prototype:
			l_Str = magic_enum::enum_name((static_cast<Prototype>(m_Value))).data();
			break;
		case EnumName::NodeTag:
			l_Str = magic_enum::enum_flags_name((static_cast<NodeTag>(m_Value))).data();
			break;
		case EnumName::WidgetTypeInt:
			l_Str = magic_enum::enum_name((static_cast<WidgetTypeInt>(m_Value))).data();
			break;
		case EnumName::WidgetTypeFloat:
			l_Str = magic_enum::enum_name((static_cast<WidgetTypeFloat>(m_Value))).data();
			break;
		case EnumName::WidgetTypeString:
			l_Str = magic_enum::enum_name((static_cast<WidgetTypeString>(m_Value))).data();
			break;
		case EnumName::WidgetTypeWString:
			l_Str = magic_enum::enum_name((static_cast<WidgetTypeWString>(m_Value))).data();
			break;
		case EnumName::WidgetTypeVector3f:
			l_Str = magic_enum::enum_name((static_cast<WidgetTypeVector3f>(m_Value))).data();
			break;
		case EnumName::WidgetTypeVector4f:
			l_Str = magic_enum::enum_name((static_cast<WidgetTypeVector4f>(m_Value))).data();
			break;
		}

		return l_Str;
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
		case EnumName::VkBufferUsage:
		{
			auto l_Value = magic_enum::enum_flags_cast<VkBufferUsage>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::VkImageUsage:
		{
			auto l_Value = magic_enum::enum_flags_cast<VkImageUsage>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
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
		case EnumName::EditorDataType:
		{
			auto l_Value = magic_enum::enum_cast<EditorDataType>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::ParamDataType:
		{
			auto l_Value = magic_enum::enum_cast<ParamDataType>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::TaskQueueFamily:
		{
			auto l_Value = magic_enum::enum_cast<TaskQueueFamily>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::TaskThread:
		{
			auto l_Value = magic_enum::enum_cast<TaskThread>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::TaskBlock:
		{
			auto l_Value = magic_enum::enum_cast<TaskBlock>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::Prototype:
		{
			auto l_Value = magic_enum::enum_cast<Prototype>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::NodeTag:
		{
			auto l_Value = magic_enum::enum_flags_cast<NodeTag>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::WidgetTypeInt:
		{
			auto l_Value = magic_enum::enum_cast<WidgetTypeInt>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::WidgetTypeFloat:
		{
			auto l_Value = magic_enum::enum_cast<WidgetTypeFloat>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::WidgetTypeString:
		{
			auto l_Value = magic_enum::enum_cast<WidgetTypeString>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::WidgetTypeWString:
		{
			auto l_Value = magic_enum::enum_cast<WidgetTypeWString>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::WidgetTypeVector3f:
		{
			auto l_Value = magic_enum::enum_cast<WidgetTypeVector3f>(i_String);
			m_Value = static_cast<S64>(l_Value.value());
		}
		break;
		case EnumName::WidgetTypeVector4f:
		{
			auto l_Value = magic_enum::enum_cast<WidgetTypeVector4f>(i_String);
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

	template <typename T>
	bool operator&(T i_Vaule)
	{
		return m_Value & static_cast<S64>(i_Vaule);
	}

	EnumName m_Name = EnumName::ENUM_NAME_UNKNOWN;
	S64 m_Value = 0;
};

static inline bool splitter(std::string_view i_Name, bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	std::string l_Name = std::string("##Splitter_") + i_Name.data();
	const ImGuiID id = window->GetID(l_Name.c_str());
	ImRect bb;
	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

static std::string toLower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return std::tolower(c); }
	);
	return s;
}

//static inline GUID createGuid()
//{
//	GUID l_Guid;
//	CoCreateGuid(&l_Guid);
//	return l_Guid;
//}
//
//static inline std::string guidToString(const GUID& i_Guid)
//{
//	char l_Buf[64] = { 0 };
//	_snprintf_s(
//		l_Buf,
//		sizeof(l_Buf),
//		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
//		i_Guid.Data1, i_Guid.Data2, i_Guid.Data3,
//		i_Guid.Data4[0], i_Guid.Data4[1],
//		i_Guid.Data4[2], i_Guid.Data4[3],
//		i_Guid.Data4[4], i_Guid.Data4[5],
//		i_Guid.Data4[6], i_Guid.Data4[7]);
//	return std::string(l_Buf);
//}

RCEDITOR_NAMESPACE_END
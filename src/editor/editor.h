#pragma once


#include <imgui_node_editor/imgui_node_editor.h>
#include "imfilebrowser.h"
#include "utilities/builders.h"
#include "utilities/widgets.h"

#include <vector>
#include <map>
#include <base/public/base.h>
#include <application/public/application.h>
#include <external/imgui/imgui_internal.h>

RCEDITOR_NAMESPACE_BEGIN

constexpr int ACCEST_INPUT_TEXT = 0;
constexpr int REFUSE_INPUT_TEXT = 1;

struct Editor : public Application
{
	Editor(const char* i_Name) noexcept;
	Editor(const char* i_Name, int argc, char** argv) noexcept;

	void OnStart() override;
	void OnStop() override;
	void OnFrame(F32 i_DeltaTime) override;

private:
	//void touchNode(ax::NodeEditor::NodeId i_ID);
	//F32 getTouchProgress(ax::NodeEditor::NodeId i_ID);

	void menuBar() noexcept;
	void menuDialog() noexcept;
	void pinInfoDialog() noexcept;
	void paramInfoDialog() noexcept;
	void tipsDialog() noexcept;

	void templateNodeList() noexcept;
	void templateNodeTree(const std::string& i_DirStr, const std::vector<FileDetails>& i_FileVec, const std::string& i_InputStr, Bool i_IsTask, U32 i_Depth = 0) noexcept;
	Bool templateNodeTreeHasNode(const std::string& i_DirStr, const std::vector<FileDetails>& i_FileVec, const std::string& i_InputStr, Bool i_IsTask, U32 i_Depth = 0) noexcept;
	void nodeEditor() noexcept;
	void nodeDetails() noexcept;
	template <Bool IsTemplate, PinType T>
	void nodePinDetails(NodeID i_NodeID) noexcept;
	template <Bool IsTemplate>
	void nodeParamDetails(NodeID i_NodeID) noexcept;

	template <Bool IsConnection>
	ax::Drawing::IconType getNodeIconType(std::shared_ptr<NodeInfo> i_pNodeInfo) noexcept;
	template <Bool IsInput, Bool IsConnection>
	void inline drawPin(const std::vector<PinID>& i_PinIDs, ax::Drawing::IconType i_IconType, ax::NodeEditor::Utilities::BlueprintNodeBuilder* i_NodeBuilder) noexcept;
	void drawNodes() noexcept;
	void drawLable(std::string i_Str, ImColor i_Color) noexcept;

	void onCreateObject() noexcept;
	void onDelectObject() noexcept;
	void onClickObject() noexcept;
	void onClickNode() noexcept;

	void nodeContext() noexcept;
	void pinContext() noexcept;
	void linkContext() noexcept;
	void editorNodeList() noexcept;

private:
	ax::NodeEditor::EditorContext* m_Editor;
	NodeManager m_NodeManager;

	const F32 m_TouchTime = 1.0f;

	Bool m_bStyleEditor = false;
	Bool m_bHelp = false;
	Bool m_bBack = false;

	PinID m_NewLinkPinID = 0;
	PinID m_NewNodePinID = 0;

	ax::NodeEditor::NodeId m_ContextNodeID = 0;
	ax::NodeEditor::PinId  m_ContextPinID = 0;
	ax::NodeEditor::LinkId m_ContextLinkID = 0;

	NodeID m_SelectedNodeID = 0;
	std::vector<ax::NodeEditor::NodeId> m_SelectedNodes;

	ImTextureID m_HeaderBackground = nullptr;
	ImTextureID m_SaveIcon = nullptr;
	ImTextureID m_RestoreIcon = nullptr;
	ImTextureID m_BranchIcon = nullptr;
	ImTextureID m_ForEachIcon = nullptr;
	ImTextureID m_FunctionIcon = nullptr;

	const U32 m_PinIconSize = 24;

	F32 m_EditorWidth = 800.0f;
	F32 m_EditorHeight = 600.0f;
	F32 m_LeftPaneWidth = 200.0f;
	F32 m_TopPaneWidth = 100.0f;
	F32 m_MiddlePaneWidth = 400.0f;
	F32 m_RightPaneWidth = 0.0f;

	Bool m_bSaveFile = false;
	Bool m_bLoadFile = false;
	ImGui::FileBrowser m_SaveFileDialog;
	ImGui::FileBrowser m_LoadFileDialog;

	S32 m_InfoAddIndex = 0;
	Bool m_bPinInfoDialog = false;
	PinInfo m_PinInfoNeedAdd{};
	Bool m_bParamInfoDialog = false;
	ParamInfo m_ParamInfoNeedAdd{};
	Bool m_bTipsDialog = false;
	std::string m_TipsInfoStr{};

	Bool m_bSaveFavoriteFile = false;
	ImGui::FileBrowser m_SaveFavoriteDialog;
};

int Main(int argc, char** argv);

RCEDITOR_NAMESPACE_END
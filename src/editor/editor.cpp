#include "editor.h"
#include "application/public/application.h"
#include <fstream>
#include <magic_enum/magic_enum.hpp>
#include <iostream>
#include <list>
#include <mutex>
#include <regex>

RCEDITOR_NAMESPACE_BEGIN

int nodeSelectTextCallback(ImGuiInputTextCallbackData* data)
{
	//auto l_fnCheckName = [&](const char* i_pStr, int count) -> bool
	//{
	//   if (count == 0)
	//    {
	//        return REFUSE_INPUT_TEXT;
	//    }
	//    for (int i = 0; i < count; ++i)
	//    {
	//        if (i_pStr[0] >= '0' && i_pStr[0] <= '9')
	//        {
	//            return REFUSE_INPUT_TEXT;
	//        }
	//        if (i_pStr == "\0")
	//        {
	//            break;
	//        }
	//    }
	//    return ACCEST_INPUT_TEXT;
	//};

	//if(l_fnCheckName(data->Buf, data->BufTextLen) == REFUSE_INPUT_TEXT)
	//{
	//    //return REFUSE_INPUT_TEXT;
	//}

	return ACCEST_INPUT_TEXT;
}

Editor::Editor(const char* i_Name) noexcept
	: Application(i_Name)
	, m_SaveFileDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc)
	, m_LoadFileDialog(ImGuiFileBrowserFlags_CloseOnEsc)
	, m_SaveTemplateDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc)
{
	m_SaveFileDialog.SetTitle("Save File Browser");
	m_SaveFileDialog.SetTypeFilters({ ".xml" });
	m_LoadFileDialog.SetTitle("Load File Browser");
	m_LoadFileDialog.SetTypeFilters({ ".xml" });

	m_SaveTemplateDialog.SetTitle("Save File Browser");
	m_SaveTemplateDialog.SetTypeFilters({ ".xml" });
	m_SaveTemplateDialog.SetPwd(m_NodeManager.templatePath());
}

Editor::Editor(const char* name, int argc, char** argv) noexcept
	: Application(name, argc, argv)
	, m_SaveFileDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc)
	, m_LoadFileDialog(ImGuiFileBrowserFlags_CloseOnEsc)
	, m_SaveTemplateDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc)
{
	m_SaveFileDialog.SetTitle("Save File Browser");
	m_SaveFileDialog.SetTypeFilters({ ".xml" });
	m_LoadFileDialog.SetTitle("Load File Browser");
	m_LoadFileDialog.SetTypeFilters({ ".xml" });

	m_SaveTemplateDialog.SetTitle("Save File Browser");
	m_SaveTemplateDialog.SetTypeFilters({ ".xml" });
	m_SaveTemplateDialog.SetPwd(m_NodeManager.templatePath());
}

void Editor::OnStart()
{
	ax::NodeEditor::Config l_Config;
	l_Config.SettingsFile = "Default.xml";
	l_Config.UserPointer = this;

	m_Editor = ax::NodeEditor::CreateEditor(&l_Config);
	ax::NodeEditor::SetCurrentEditor(m_Editor);

	ax::NodeEditor::NavigateToContent();

	m_HeaderBackground = LoadTexture("data/BlueprintBackground.png");
	m_SaveIcon = LoadTexture("data/ic_save_white_24dp.png");
	m_RestoreIcon = LoadTexture("data/ic_restore_white_24dp.png");
	m_BranchIcon = LoadTexture("data/icon_Blueprint_Branch_16x.png");
	m_ForEachIcon = LoadTexture("data/icon_Blueprint_ForEach_16x.png");
	m_FunctionIcon = LoadTexture("data/icon_Blueprint_NewFunction_16x.png");
}

void Editor::OnStop()
{
	auto releaseTexture = [this](ImTextureID& id)
	{
		if (id)
		{
			DestroyTexture(id);
			id = nullptr;
		}
	};

	releaseTexture(m_RestoreIcon);
	releaseTexture(m_SaveIcon);
	releaseTexture(m_HeaderBackground);
	releaseTexture(m_ForEachIcon);
	releaseTexture(m_BranchIcon);
	releaseTexture(m_FunctionIcon);

	if (m_Editor)
	{
		ax::NodeEditor::DestroyEditor(m_Editor);
		m_Editor = nullptr;
	}
}

void Editor::OnFrame(F32 i_DeltaTime)
{
	const auto& l_IO = ImGui::GetIO();
	m_EditorWidth = (l_IO.DisplaySize.x - m_LeftPaneWidth) >= 0.0 ? (l_IO.DisplaySize.x - m_LeftPaneWidth) : 0;

	menuBar();
	menuDialog();

	registerNodeList();
	nodeEditor();

	nodeDetails();
}

//void Editor::touchNode(ax::NodeEditor::NodeId i_ID)
//{
//	m_NodeTouchTime[i_ID] = m_TouchTime;
//}
//
//F32 Editor::getTouchProgress(ax::NodeEditor::NodeId i_ID)
//{
//	if (const auto l_Iter = m_NodeTouchTime.find(i_ID); l_Iter != m_NodeTouchTime.end() && l_Iter->second > 0.0f)
//		return (m_TouchTime - l_Iter->second) / m_TouchTime;
//	else
//		return 0.0f;
//}

void Editor::menuBar() noexcept
{
	const auto& l_IO = ImGui::GetIO();

	if (l_IO.KeyCtrl && !l_IO.WantTextInput && ImGui::IsKeyPressed('S'))
	{
		m_bSaveFile = true;
	}

	if (l_IO.KeyCtrl && !l_IO.WantTextInput && ImGui::IsKeyPressed('R'))
	{
		m_bLoadFile = true;
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save RC AS(Ctrl + S)"))
			{
				m_bSaveFile = true;
			}

			if (ImGui::MenuItem("Load RC(Ctrl + L)"))
			{
				m_bLoadFile = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Style"))
		{
			if (ImGui::MenuItem("Edit Style"))
			{
				m_bStyleEditor = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Show Help(H)"))
			{
				m_bHelp = true;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (m_bSaveFile)
	{
		m_SaveFileDialog.Open();
		m_bSaveFile = false;
	}

	if (m_bLoadFile)
	{
		m_LoadFileDialog.Open();
		m_bLoadFile = false;
	}

	if (m_bStyleEditor)
	{
		m_bStyleEditor = false;
	}
}

void Editor::menuDialog() noexcept
{
	m_SaveFileDialog.Display();
	if (m_SaveFileDialog.HasSelected())
	{
		std::string l_FileName = m_SaveFileDialog.GetSelected().string();
		m_SaveFileDialog.ClearSelected();

		const auto l_Pos = l_FileName.find(".xml");
		if (std::string::npos == l_Pos)
		{
			l_FileName += ".xml";
		}

		ax::NodeEditor::Config& l_Config = ax::NodeEditor::GetConfig(ax::NodeEditor::GetCurrentEditor());
		l_Config.SettingsFile = l_FileName;

		ax::NodeEditor::SaveData(&m_NodeManager);

		//m_Console.AddLog("Save Graph to %s.", l_FileName.c_str());
	}

	m_LoadFileDialog.Display();
	if (m_LoadFileDialog.HasSelected())
	{
		const std::string l_FileName = m_LoadFileDialog.GetSelected().string();
		m_LoadFileDialog.ClearSelected();

		ax::NodeEditor::Config& l_Config = ax::NodeEditor::GetConfig(ax::NodeEditor::GetCurrentEditor());
		l_Config.SettingsFile = l_FileName;

		ax::NodeEditor::LoadData(&m_NodeManager);

		//m_Console.AddLog("Load Graph from %s.", l_FileName.c_str());
	}

	m_SaveTemplateDialog.Display();
	if (m_SaveTemplateDialog.HasSelected())
	{
		//m_SaveTemplateDialog.SetPwd(m_NodeManager.templatePath());

		std::string l_FileName = m_SaveTemplateDialog.GetSelected().string();
		m_SaveTemplateDialog.ClearSelected();

		const auto l_Pos = l_FileName.find(".xml");
		if (std::string::npos == l_Pos)
		{
			l_FileName += ".xml";
		}

		ax::NodeEditor::Config& l_Config = ax::NodeEditor::GetConfig(ax::NodeEditor::GetCurrentEditor());
		l_Config.SettingsFile = l_FileName;

		m_NodeManager.saveRCTemplate(l_FileName, m_ContextNodeID.AsU64());
	}
}

void Editor::registerNodeList() noexcept
{
	const auto& l_IO = ImGui::GetIO();

	splitter(true, 4.0f, &m_LeftPaneWidth, &m_MiddlePaneWidth, 50.0f, 50.0f);

	ImGui::NewLine();
	ImGui::Text("FPS: %.2f (%.2gms)", l_IO.Framerate, l_IO.Framerate ? 1000.0f / l_IO.Framerate : 0.0f);

	if (m_NodeManager.curParentNodeID() != 0)
	{
		ImGui::SameLine(0, (m_LeftPaneWidth - 125 < 0 ? 100 : m_LeftPaneWidth - 125));
		if (ImGui::Button("Back to Parent Node(B)", ImVec2(250, 36)))
			m_bBack = true;
	}
	else
	{
		ImGui::SameLine(0, m_LeftPaneWidth + 125);
		// Set a new font scale
		ImGui::SetWindowFontScale(2.0f);
		ImGui::TextUnformatted("");
		ImGui::SetWindowFontScale(1.0f);
	}

	ImGui::BeginChild("Selection", ImVec2(m_LeftPaneWidth - 4, 0));

	F32 l_LeftPaneWidth = ImGui::GetContentRegionAvail().x;

	{
		ImGui::BeginHorizontal("Style Editor", ImVec2(l_LeftPaneWidth, 0));
		ImGui::Spring(0.0f, 0.0f);

		if (ImGui::Button("Zoom"))
		{
		}
		if (ImGui::Button("Flow"))
		{
		}

		ImGui::EndHorizontal();
	}

	const U32 l_SaveIconWidth = GetTextureWidth(m_SaveIcon);
	const U32 l_SaveIconHeight = GetTextureWidth(m_SaveIcon);
	const U32 l_RestoreIconWidth = GetTextureWidth(m_RestoreIcon);
	const U32 l_RestoreIconHeight = GetTextureWidth(m_RestoreIcon);

	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetCursorScreenPos(),
		ImGui::GetCursorScreenPos() + ImVec2(m_LeftPaneWidth, ImGui::GetTextLineHeight()),
		ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
	ImGui::Spacing(); ImGui::SameLine();
	ImGui::TextUnformatted("Node Library");
	ImGui::SetNextItemWidth(m_LeftPaneWidth);

	auto toLower = [&](std::string s) ->std::string {
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return std::tolower(c); }
		);
		return s;
	};

	static char l_Buffer[128] = "";
	ImGui::InputTextWithHint("", "Search", l_Buffer, 127, ImGuiInputTextFlags_EnterReturnsTrue);
	std::string l_InputLower = toLower(l_Buffer);

	registerNodeTree(m_NodeManager.templatePath().string(), m_NodeManager.templateNodeFileMap().at(m_NodeManager.templatePath().string()), l_InputLower);

	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetCursorScreenPos(),
		ImGui::GetCursorScreenPos() + ImVec2(m_LeftPaneWidth, ImGui::GetTextLineHeight()),
		ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
	ImGui::Spacing(); ImGui::SameLine();

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
		for (const auto& l_Node : m_NodeManager.nodeMap())
			ax::NodeEditor::Flow(l_Node.first);

	ImGui::EndChild();
}

void Editor::registerNodeTree(const std::string& i_DirStr, const std::vector<FileDetails>& i_FileVec, const std::string& i_InputStr, U32 i_Depth) noexcept
{
	auto toLower = [&](std::string s) ->std::string {
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return std::tolower(c); }
		);
		return s;
	};

	for (const auto& l_FileDetails : i_FileVec)
	{
		if (l_FileDetails.m_Type == TemplateFileType::Dir)
		{
			if (ImGui::CollapsingHeader(l_FileDetails.m_FilenameStr.c_str()))
			{
				ImGui::Indent();
				registerNodeTree(l_FileDetails.m_FilenameStr, m_NodeManager.templateNodeFileMap().at(l_FileDetails.m_FilenameStr), i_InputStr);
				ImGui::Unindent();
			}
		}

		else if (l_FileDetails.m_Type == TemplateFileType::Template)
		{
			std::string l_NodeNameLower = toLower(l_FileDetails.m_FilenameStr);
			if (l_NodeNameLower.find(i_InputStr) == std::string::npos)
			{
				continue;
			}

			Bool l_IsSelected = false;
			ImGui::Selectable(l_FileDetails.m_FilenameStr.c_str(), &l_IsSelected);
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover))
			{
				ImGui::SetDragDropPayload("DRAG_TEXT", l_FileDetails.m_FilenameStr.c_str(), sizeof(char) * (l_FileDetails.m_FilenameStr.size() + 1), ImGuiCond_Once);
				ImGui::Text(l_FileDetails.m_FilenameStr.c_str());
				ImGui::EndDragDropSource();
			}
		}
	}
}

void Editor::nodeEditor() noexcept
{
	ImGui::SameLine(0.0f, 14.0f);
	ax::NodeEditor::Begin("Node editor", ImVec2(m_EditorWidth - m_RightPaneWidth, 0));

	drawNodes();

	onCreateObject();
	onDelectObject();
	onClickObject();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

	nodeContext();
	pinContext();
	linkContext();
	editorNodeList();

	ImGui::PopStyleVar();

	ax::NodeEditor::End();
}

void Editor::nodeDetails() noexcept
{
	ImGui::SameLine(0.0f, 4.0f);

	m_SelectedNodes.resize(ax::NodeEditor::GetSelectedObjectCount());
	const int l_NodeCount = ax::NodeEditor::GetSelectedNodes(m_SelectedNodes.data(), static_cast<int>(m_SelectedNodes.size()));
	m_SelectedNodes.resize(l_NodeCount);

	if (m_SelectedNodes.size() != 1)
	{
		m_RightPaneWidth = 0.0f;
		return;
	}

	m_SelectedNodeID = m_SelectedNodes[0].AsU64();
	m_RightPaneWidth = 600.0f;
	const auto& l_IO = ImGui::GetIO();

	ax::NodeEditor::NodeId l_NodeID = 0;
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_NodeManager.nodeInfo(m_SelectedNodeID);

	if (l_pNodeInfo)
	{
		ImGui::BeginChild("Node Info");
		ImGui::Indent();
		F32 l_RightPaneWidth = ImGui::GetContentRegionAvail().x;

		{
			ImGui::BeginVertical("Node Info", ImVec2(0, 0), 30);
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImGui::GetCursorScreenPos(),
				ImGui::GetCursorScreenPos() + ImVec2(l_RightPaneWidth, ImGui::GetTextLineHeight()),
				ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::Text("Node Info");

			std::string l_PrefixStr = "##" + std::to_string(m_SelectedNodeID);
			std::string l_TitleStr = "Node Info" + std::to_string(m_SelectedNodeID);
			l_PrefixStr += "Node Info";

			if (ImGui::BeginTable(l_TitleStr.c_str(), 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
			{
				ImGui::TableSetupColumn(std::string("Name" + l_PrefixStr).c_str(), ImGuiTableFlags_SizingMask_, 110);
				ImGui::TableSetupColumn(std::string("Data" + l_PrefixStr).c_str(), ImGuiTableColumnFlags_WidthStretch);

				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("NodeName");

					ImGui::TableSetColumnIndex(1);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(1));
					ImGui::InputText((l_PrefixStr + "_Name").c_str(), &l_pNodeInfo->m_Name, ImGuiInputTextFlags_EnterReturnsTrue);
				}

				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Template");

					ImGui::TableSetColumnIndex(1);
					ImGui::Text(l_pNodeInfo->m_TemplateName.c_str());
				}

				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Class");

					ImGui::TableSetColumnIndex(1);
					ImGui::Text(l_pNodeInfo->m_ClassName.c_str());
				}

				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Description");

					ImGui::TableSetColumnIndex(1);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(1));
					ImGui::InputTextMultiline((l_PrefixStr + "_Description").c_str(), &l_pNodeInfo->m_Comment, ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue);
				}

				ImGui::EndTable();
			}
			ImGui::EndVertical();
		}
		nodePinDetails<true, PinType::Connection>(m_SelectedNodeID);
		nodePinDetails<true, PinType::Input>(m_SelectedNodeID);
		nodePinDetails<true, PinType::Output>(m_SelectedNodeID);
		nodeParamDetails<true>(m_SelectedNodeID);

		nodePinDetails<false, PinType::Connection>(m_SelectedNodeID);
		nodePinDetails<false, PinType::Input>(m_SelectedNodeID);
		nodePinDetails<false, PinType::Output>(m_SelectedNodeID);
		nodeParamDetails<false>(m_SelectedNodeID);

		ImGui::Unindent();
		ImGui::EndChild();
	}
}

template <Bool IsTemplate, PinType T>
void Editor::nodePinDetails(NodeID i_NodeID) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_NodeManager.nodeInfo(i_NodeID);
	if (!l_pNodeInfo) return;

	if (!IsTemplate)
	{
		if ((T == PinType::Connection && !l_pNodeInfo->m_IsDynamicConnectionPin) ||
			(T == PinType::Input && !l_pNodeInfo->m_IsDynamicInputPin) ||
			(T == PinType::Output && !l_pNodeInfo->m_IsDynamicOutputPin))
			return;
	}

	std::string l_PrefixStr = "##" + std::to_string(i_NodeID);
	std::string l_TitleStr = l_PrefixStr;
	std::vector<PinID>* l_pPinIDs{};

	if (IsTemplate)
		l_PrefixStr += l_PrefixStr + "_Template";

	if (T == PinType::Connection)
	{
		l_PrefixStr += "_Connection";

		if (!IsTemplate)
		{
			l_TitleStr = "Connection" + l_PrefixStr;
			l_pPinIDs = &l_pNodeInfo->m_ConnectionOutputPinIDs;
		}
		else
		{
			l_TitleStr = "Template Connection" + l_PrefixStr;
			l_pPinIDs = &l_pNodeInfo->m_TemplateConnectionOutputPinIDs;
		}
	}
	else if (T == PinType::Input)
	{
		l_PrefixStr += "_Input";

		if (!IsTemplate)
		{
			l_TitleStr = "Input" + l_PrefixStr;
			l_pPinIDs = &l_pNodeInfo->m_InputPinIDs;
		}
		else
		{
			l_TitleStr = "Template Input" + l_PrefixStr;
			l_pPinIDs = &l_pNodeInfo->m_TemplateInputPinIDs;
		}
	}
	else if (T == PinType::Output)
	{
		l_PrefixStr += "_Output";

		if (!IsTemplate)
		{
			if (!l_pNodeInfo->m_IsDynamicOutputPin) return;
			l_TitleStr = "Output" + l_PrefixStr;
			l_pPinIDs = &l_pNodeInfo->m_OutputPinIDs;
		}
		else
		{
			l_TitleStr = "Template Output" + l_PrefixStr;
			l_pPinIDs = &l_pNodeInfo->m_TemplateOutputPinIDs;
		}
	}

	U32 l_Index = 0;
	Bool l_IsSwapPrev = false;
	Bool l_IsSwapNext = false;
	Bool l_IsDelete = false;
	U32 l_CurSelIndex = 0;

	if (ImGui::CollapsingHeader(l_TitleStr.c_str()))
	{
		if (!IsTemplate && ImGui::Button((std::string("Add") + l_PrefixStr).c_str()))
		{
			m_NodeManager.addPinInfo<T>(i_NodeID, "Temp");
		}

		U32 l_TableNum = 7;
		if(IsTemplate)
			l_TableNum = 4;

		l_PrefixStr += "_Pin List";
		if (ImGui::BeginTable(l_PrefixStr.c_str(), l_TableNum, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
		{
			if (IsTemplate)
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("MetaData", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
				ImGui::TableSetupColumn("AccessType", ImGuiTableFlags_SizingMask_, 100);
				ImGui::TableHeadersRow();
			}
			else
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("MetaData", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
				ImGui::TableSetupColumn("AccessType", ImGuiTableFlags_SizingMask_, 100);
				ImGui::TableSetupColumn(" Up ", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(" Down ", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(" Del ", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableHeadersRow();
			}

			for (auto l_PinID : *l_pPinIDs)
			{
				U32 l_TableIndex = 0;

				auto l_pPinInfo = m_NodeManager.pinInfo(l_PinID);
				if (!l_pPinInfo) continue;

				l_PrefixStr += "_" + std::to_string(l_PinID);

				ImGui::TableNextRow();
				// Name
				if (IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->m_Name.c_str(), ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::InputText((l_PrefixStr + "_Name").c_str(), &l_pPinInfo->m_Name, ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}

				// MetaData
				if (IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->m_MetaData.c_str(), ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::InputText((l_PrefixStr + "_MetaData").c_str(), &l_pPinInfo->m_MetaData, ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}

				// DataType
				if (IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->m_DataType.enumValueStr().c_str());
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					S32 l_DataTypeIndex = static_cast<S32>(l_pPinInfo->m_DataType.m_Value);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Combo((l_PrefixStr + "_DataType").c_str(), &l_DataTypeIndex, l_pPinInfo->m_DataType.enumAllTypeStr().c_str(), 5);
					l_pPinInfo->m_DataType.m_Value = l_DataTypeIndex;
					ImGui::PopItemWidth();
				}

				// AccessType
				if (IsTemplate || T == PinType::Input) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->m_PinAccessType.enumValueStr().c_str());
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					S32 l_AccessTypeIndex = static_cast<S32>(l_pPinInfo->m_PinAccessType.m_Value);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Combo((l_PrefixStr + "_AccessType").c_str(), &l_AccessTypeIndex, l_pPinInfo->m_PinAccessType.enumAllTypeStr().c_str(), 5);
					l_pPinInfo->m_PinAccessType.m_Value = l_AccessTypeIndex;
					ImGui::PopItemWidth();
				}

				if (!IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushID((l_PrefixStr + "_Up").c_str());
					if (ImGui::SmallButton("Up"))
					{
						l_IsSwapPrev = true;
						l_CurSelIndex = l_Index;
					}
					ImGui::PopID();
				}

				if (!IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushID((l_PrefixStr + "_Down").c_str());
					if (ImGui::SmallButton("Down"))
					{
						l_IsSwapNext = true;
						l_CurSelIndex = l_Index;
					}
					ImGui::PopID();
				}

				if (!IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushID((l_PrefixStr + "_Del").c_str());
					if (ImGui::SmallButton("Del"))
					{
						l_IsDelete = true;
						l_CurSelIndex = l_Index;
					}
					ImGui::PopID();
				}

				if (T == PinType::Connection)
				{
					auto l_pConnectionInputPinInfo = m_NodeManager.pinInfo(l_PinID + 1);
					assert(l_pConnectionInputPinInfo && (l_pConnectionInputPinInfo->m_PinType == PinType::Input));

					l_pConnectionInputPinInfo->m_Name = l_pPinInfo->m_Name;
					l_pConnectionInputPinInfo->m_MetaData = l_pPinInfo->m_MetaData;
					l_pConnectionInputPinInfo->m_DataType = l_pPinInfo->m_DataType;
					l_pConnectionInputPinInfo->m_PinAccessType = PinAccessType::Writable;
				}

				l_Index++;
			}

			if (l_IsDelete){
				m_NodeManager.delPinInfo<T>(i_NodeID, l_CurSelIndex);
			}
			else if (l_IsSwapPrev){
				if (T == PinType::Connection) {
					{
						if (const auto l_It = std::next(l_pNodeInfo->m_ConnectionOutputPinIDs.begin(), l_CurSelIndex); l_It != l_pNodeInfo->m_ConnectionOutputPinIDs.begin())
						{
							const auto l_PrevElement = std::prev(l_It);
							std::swap(*l_It, *l_PrevElement);
						}
					}

					{
						if (const auto l_It = std::next(l_pNodeInfo->m_ConnectionInputPinIDs.begin(), l_CurSelIndex); l_It != l_pNodeInfo->m_ConnectionInputPinIDs.begin())
						{
							const auto l_PrevElement = std::prev(l_It);
							std::swap(*l_It, *l_PrevElement);
						}
					}
				}
				else
				{
					if (const auto l_It = std::next(l_pPinIDs->begin(), l_CurSelIndex); l_It != l_pPinIDs->begin())
					{
						const auto l_PrevElement = std::prev(l_It);
						std::swap(*l_It, *l_PrevElement);
					}
				}
			}
			else if (l_IsSwapNext) {
				if (T == PinType::Connection) {
					{
						const auto l_It = std::next(l_pNodeInfo->m_ConnectionOutputPinIDs.begin(), l_CurSelIndex);
						if (const auto l_NextElement = std::next(l_It); l_NextElement != l_pNodeInfo->m_ConnectionOutputPinIDs.end())
						{
							std::swap(*l_It, *l_NextElement);
						}
					}

					{
						const auto l_It = std::next(l_pNodeInfo->m_ConnectionInputPinIDs.begin(), l_CurSelIndex);
						if (const auto l_NextElement = std::next(l_It); l_NextElement != l_pNodeInfo->m_ConnectionInputPinIDs.end())
						{
							std::swap(*l_It, *l_NextElement);
						}
					}
				}
				else
				{
					const auto l_It = std::next(l_pPinIDs->begin(), l_CurSelIndex);
					if (const auto l_NextElement = std::next(l_It); l_NextElement != l_pPinIDs->end())
					{
						std::swap(*l_It, *l_NextElement);
					}
				}
			}

			ImGui::EndTable();
		}
	}
}

template <Bool IsTemplate>
void Editor::nodeParamDetails(NodeID i_NodeID) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_NodeManager.nodeInfo(i_NodeID);
	assert(l_pNodeInfo);

	if (!IsTemplate && !l_pNodeInfo->m_IsDynamicParam) return;

	std::string l_PrefixStr = "##" + std::to_string(i_NodeID);
	std::string l_TitleStr{};
	std::vector<EditorParamInfo>* l_pParamInfos = nullptr;

	if (IsTemplate) {
		l_TitleStr = "Template Param" + l_PrefixStr;
		l_PrefixStr += "_Template_Param";
		l_pParamInfos = &l_pNodeInfo->m_TemplateParams;
	}
	else {
		l_TitleStr = "Param" + l_PrefixStr;
		l_PrefixStr += "_Param";
		l_pParamInfos = &l_pNodeInfo->m_Params;
	}

	if (ImGui::CollapsingHeader(l_TitleStr.c_str()))
	{
		if (!IsTemplate && ImGui::Button((std::string("Add") + l_PrefixStr).c_str()))
		{
			EditorParamData l_EditorParamData;
			l_EditorParamData.m_Value = 0;
			l_pParamInfos->emplace_back("Temp", l_EditorParamData);
		}

		U32 l_TableNum = 6;
		if (IsTemplate)
			l_TableNum = 5;

		if (ImGui::BeginTable((std::string("ParamList") + "##" + l_PrefixStr).c_str(), l_TableNum, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
		{
			l_PrefixStr += "_ParamList";
			if (IsTemplate)
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
				ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Use", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("RefKey", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();
			}
			else
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
				ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Use", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("RefKey", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn(" Del ", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableHeadersRow();
			}

			Bool l_IsDelete = false;
			U32 l_CurSelIndex{};
			for (U32 i = 0; i < l_pParamInfos->size(); ++i)
			{
				l_PrefixStr += std::to_string(i);
				auto& l_ParamInfo = (*l_pParamInfos)[i];

				U32 l_TableIndex = 0;
				ImGui::TableNextRow();

				//Name
				if (IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_ParamInfo.m_Name.c_str());
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::InputText((l_PrefixStr + "_Name").c_str(), &l_ParamInfo.m_Name, ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}

				// DataType
				if (IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_ParamInfo.m_DataType.enumValueStr().c_str());
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					S32 l_DataTypeIndex = static_cast<S32>(l_ParamInfo.m_DataType.m_Value);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Combo((l_PrefixStr + "_DataType").c_str(), &l_DataTypeIndex, l_ParamInfo.m_DataType.enumAllTypeStr().c_str(), 5);
					auto l_PrevDataTypeIndex = l_ParamInfo.m_DataType.m_Value;
					if (l_PrevDataTypeIndex != l_DataTypeIndex)
					{
						l_ParamInfo.m_DataType.m_Value = l_DataTypeIndex;
						l_ParamInfo.initData();
					}
					ImGui::PopItemWidth();
				}

				// Data
				if (IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_ParamInfo.getData("Data").c_str());
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					std::string l_Str = l_ParamInfo.getData("Data");
					ImGui::InputText((l_PrefixStr + "_Data").c_str(), &l_Str, ImGuiInputTextFlags_EnterReturnsTrue);
					l_ParamInfo.setData("Data", l_Str);
					ImGui::PopItemWidth();
				}

				// UseRef
				if (IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					auto l_UseRef = l_ParamInfo.m_UseRef;
					ImGui::Checkbox((l_PrefixStr + "_UseRef").c_str(), &l_UseRef);
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Checkbox((l_PrefixStr + "_UseRef").c_str(), &l_ParamInfo.m_UseRef);
					ImGui::PopItemWidth();
				}

				// RefKey
				if (IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_ParamInfo.m_RefKey.c_str());
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::InputText((l_PrefixStr + "_RefKey").c_str(), &l_ParamInfo.m_RefKey, ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}

				//Del
				if (!IsTemplate)
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushID((l_PrefixStr + "_Del").c_str());
					if (ImGui::SmallButton("Del"))
					{
						l_IsDelete = true;
						l_CurSelIndex = i;
					}
					ImGui::PopID();
				}
			}

			if (l_IsDelete)
			{
				l_IsDelete = false;
				l_pParamInfos->erase(l_pParamInfos->begin() + l_CurSelIndex);
			}

			ImGui::EndTable();
		}
	}
}

template <Bool IsInput, Bool IsConnection>
void inline Editor::drawPin(const std::vector<PinID>& i_PinIDs, ax::NodeEditor::Utilities::BlueprintNodeBuilder* i_NodeBuilder) noexcept
{
	for (const auto& l_PinID : i_PinIDs)
	{
		std::shared_ptr<PinInfo> l_pPinInfo = m_NodeManager.pinInfo(l_PinID);
		if (!l_pPinInfo)
		{
			assert(false);
			continue;
		}

		auto l_Alpha = ImGui::GetStyle().Alpha;
		if (m_NewLinkPinID && !m_NodeManager.canAddLink(l_PinID, m_NewLinkPinID) && l_PinID != m_NewLinkPinID)
			l_Alpha = l_Alpha * (48.0 / 255.0f);

		ax::Drawing::IconType l_IconType = ax::Drawing::IconType::Circle;
		ImColor l_Color = l_pPinInfo->color();
		l_Color.Value.w = l_Alpha;

		if (IsInput) {
			if (IsConnection) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, l_Alpha);
				i_NodeBuilder->Input(l_PinID);
				ax::Widgets::Icon(ImVec2(static_cast<F32>(m_PinIconSize), static_cast<F32>(m_PinIconSize)), ax::Drawing::IconType::Flow, l_pPinInfo->m_LinkIDSet.size() > 0, l_Color, ImColor(32, 32, 32, static_cast<S32>(l_Alpha * 255)));
				ImGui::Spring(0);
				ImGui::TextUnformatted(l_pPinInfo->m_Name.c_str());
				ImGui::Spring(0);
				i_NodeBuilder->EndInput();
				ImGui::PopStyleVar();
			}
			else {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, l_Alpha);
				i_NodeBuilder->Input(l_PinID);
				ax::Widgets::Icon(ImVec2(static_cast<F32>(m_PinIconSize), static_cast<F32>(m_PinIconSize)), ax::Drawing::IconType::Circle, l_pPinInfo->m_LinkIDSet.size() > 0, l_Color, ImColor(32, 32, 32, static_cast<S32>(l_Alpha * 255)));
				ImGui::Spring(0);
				ImGui::TextUnformatted(l_pPinInfo->m_Name.c_str());
				ImGui::Spring(0);
				i_NodeBuilder->EndInput();
				ImGui::PopStyleVar();
			}

		}
		else {
			if (IsConnection) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, l_Alpha);
				i_NodeBuilder->Output(l_PinID);
				//ImGui::TextUnformatted(l_pPinInfo->m_Name.c_str());
				ax::Widgets::Icon(ImVec2(static_cast<F32>(m_PinIconSize), static_cast<F32>(m_PinIconSize)), ax::Drawing::IconType::Flow, l_pPinInfo->m_LinkIDSet.size() > 0, l_Color, ImColor(32, 32, 32, static_cast<S32>(l_Alpha * 255)));
				ImGui::Spring(0);
				i_NodeBuilder->EndOutput();
				ImGui::PopStyleVar();
			}
			else {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, l_Alpha);
				i_NodeBuilder->Output(l_PinID);
				ImGui::TextUnformatted(l_pPinInfo->m_Name.c_str());
				ImGui::Spring(0);
				ax::Widgets::Icon(ImVec2(static_cast<F32>(m_PinIconSize), static_cast<F32>(m_PinIconSize)), ax::Drawing::IconType::Circle, l_pPinInfo->m_LinkIDSet.size() > 0, l_Color, ImColor(32, 32, 32, static_cast<S32>(l_Alpha * 255)));
				ImGui::Spring(0);
				i_NodeBuilder->EndOutput();
				ImGui::PopStyleVar();
			}
		}
	}
}

void Editor::drawPin(PinID i_PinID, U32 i_Alpha) noexcept
{
	std::shared_ptr<PinInfo> i_pPinInfo = m_NodeManager.pinInfo(i_PinID);

	ax::Drawing::IconType iconType;
	ImColor l_Color = i_pPinInfo->color();
	l_Color.Value.w = i_Alpha / 255.0f;

	ax::Drawing::IconType l_IconType = ax::Drawing::IconType::Circle;

	if (i_pPinInfo->m_PinType == PinType::Input)
	{
		ax::Widgets::Icon(ImVec2(static_cast<F32>(m_PinIconSize), static_cast<F32>(m_PinIconSize)), l_IconType, i_pPinInfo->m_LinkIDSet.size() > 0, l_Color, ImColor(32, 32, 32, i_Alpha));
		ImGui::TextUnformatted(i_pPinInfo->m_Name.c_str());
	}
	else
	{
		ImGui::TextUnformatted(i_pPinInfo->m_Name.c_str());
		ax::Widgets::Icon(ImVec2(static_cast<F32>(m_PinIconSize), static_cast<F32>(m_PinIconSize)), l_IconType, i_pPinInfo->m_LinkIDSet.size() > 0, l_Color, ImColor(32, 32, 32, i_Alpha));
	}
}

void Editor::drawNodes() noexcept
{
	auto l_CursorTopLeft = ImGui::GetCursorScreenPos();

	ax::NodeEditor::Utilities::BlueprintNodeBuilder l_Builder(m_HeaderBackground, GetTextureWidth(m_HeaderBackground), GetTextureHeight(m_HeaderBackground));
	{
		//draw drag nodes
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* l_Payload = ImGui::AcceptDragDropPayload("DRAG_TEXT"))
			{
				std::string l_NodeName = static_cast<const char*>(l_Payload->Data);

				const auto l_NewNodePos = ImGui::GetMousePos();
				const float l_FixedX = l_NewNodePos.x - m_LeftPaneWidth > 0.0f ? l_NewNodePos.x - m_LeftPaneWidth : 100.0f;
				const float l_FixedY = l_NewNodePos.y - 100.0f;
				const ImVec2 l_FixedPos = ImVec2(l_FixedX, l_FixedY);

				NodeID l_NodeID = m_NodeManager.addNodeInfo(l_NodeName);
				NodeID m_ParentNodeInfo = m_NodeManager.curParentNodeID();
				m_NodeManager.nodeInfo(l_NodeID)->m_ParentNodeID = m_ParentNodeInfo;
				//m_NodeManager.updateCurInfo();

				ax::NodeEditor::SetNodePosition(l_NodeID, l_FixedPos);
			}
			ImGui::EndDragDropTarget();
		}

		// draw nodes
		for (auto l_CurNodeID : m_NodeManager.curNodeSet())
		{
			std::shared_ptr<NodeInfo> l_pCurNodeInfo = m_NodeManager.nodeInfo(l_CurNodeID);
			if (!l_pCurNodeInfo)
			{
				assert(false);
				continue;
			}

			{
				l_Builder.Begin(l_CurNodeID);
				{
					l_Builder.Header(l_pCurNodeInfo->color());
					ImGui::Spring(0);
					ImGui::TextUnformatted(l_pCurNodeInfo->m_Name.c_str());
					ImGui::Spring(1);
					ImGui::Dummy(ImVec2(0, 28));
					ImGui::Spring(0);
					l_Builder.EndHeader();
				}

				{
					l_Builder.TemplatePin();
					drawPin<true, true>(l_pCurNodeInfo->m_TemplateConnectionInputPinIDs, &l_Builder);
					drawPin<true, false>(l_pCurNodeInfo->m_TemplateInputPinIDs, &l_Builder);

					drawPin<false, true>(l_pCurNodeInfo->m_TemplateConnectionOutputPinIDs, &l_Builder);
					drawPin<false, false>(l_pCurNodeInfo->m_TemplateOutputPinIDs, &l_Builder);
					l_Builder.EndTemplatePin();

				}

				{
					l_Builder.NormalPin();
					drawPin<true, true>(l_pCurNodeInfo->m_ConnectionInputPinIDs, &l_Builder);
					drawPin<true, false>(l_pCurNodeInfo->m_InputPinIDs, &l_Builder);

					drawPin<false, true>(l_pCurNodeInfo->m_ConnectionOutputPinIDs, &l_Builder);
					drawPin<false, false>(l_pCurNodeInfo->m_OutputPinIDs, &l_Builder);
					l_Builder.EndNormalPin();
				}

				l_Builder.End();
			}
		}
	}

	for (auto l_LinkID : m_NodeManager.curLinkSet())
	{
		std::shared_ptr<LinkInfo> l_pLinkInfo = m_NodeManager.linkMap()[l_LinkID];
		ax::NodeEditor::Link(l_LinkID, l_pLinkInfo->m_OutputPinID, l_pLinkInfo->m_InputPinID, l_pLinkInfo->m_Color, 2.0f);
	}

	ImGui::SetCursorScreenPos(l_CursorTopLeft);
}

void Editor::drawLable(std::string i_Str, ImColor i_Color) noexcept
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
	const auto l_Size = ImGui::CalcTextSize(i_Str.c_str());

	const auto l_Padding = ImGui::GetStyle().FramePadding;
	const auto l_Spacing = ImGui::GetStyle().ItemSpacing;

	ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(l_Spacing.x, -l_Spacing.y));

	const auto l_RectMin = ImGui::GetCursorScreenPos() - l_Padding;
	const auto l_RectMax = ImGui::GetCursorScreenPos() + l_Size + l_Padding;

	const auto l_DrawList = ImGui::GetWindowDrawList();
	l_DrawList->AddRectFilled(l_RectMin, l_RectMax, i_Color, l_Size.y * 0.15f);
	ImGui::TextUnformatted(i_Str.c_str());
}

void Editor::onCreateObject() noexcept
{
	if (!ax::NodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
		m_NewLinkPinID = 0;
	}
	else
	{
		ax::NodeEditor::PinId l_StartPinID = 0, l_EndPinID = 0;
		if (ax::NodeEditor::QueryNewLink(&l_StartPinID, &l_EndPinID) && l_StartPinID != l_EndPinID)
		{
			auto l_pStartPinInfo = m_NodeManager.pinInfo(l_StartPinID.AsU64());
			auto l_pEndPinInfo = m_NodeManager.pinInfo(l_EndPinID.AsU64());

			m_NewLinkPinID = l_StartPinID.AsU64() ? l_StartPinID.AsU64() : l_EndPinID.AsU64();

			if (l_pStartPinInfo->m_PinType == PinType::Input)
			{
				std::swap(l_pStartPinInfo, l_pEndPinInfo);
				std::swap(l_StartPinID, l_EndPinID);
			}

			if (l_pStartPinInfo && l_pEndPinInfo)
			{
				if (l_StartPinID == l_EndPinID)
				{
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (l_pStartPinInfo->m_PinType == l_pEndPinInfo->m_PinType)
				{
					drawLable("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (l_pStartPinInfo->m_NodeID == l_pEndPinInfo->m_NodeID)
				{
					drawLable("x Cannot connect to self", ImColor(45, 32, 32, 180));
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
				}
				else if (l_pStartPinInfo->m_DataType != l_pEndPinInfo->m_DataType)
				{
					drawLable("x Incompatible Pin Data Type", ImColor(45, 32, 32, 180));
					ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				}
				else if (l_pEndPinInfo->m_PinAccessType != PinAccessType::Writable && l_pStartPinInfo->m_PinAccessType != l_pEndPinInfo->m_PinAccessType)
				{
					drawLable("x Incompatible Pin Access Type", ImColor(45, 32, 32, 180));
					ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				}
				else if (!l_pEndPinInfo->m_MetaData.empty() && (l_pStartPinInfo->m_MetaData != l_pEndPinInfo->m_MetaData))
				{
					drawLable("x Incompatible Pin MetaData", ImColor(45, 32, 32, 180));
					ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				}
				else
				{
					drawLable("+ Create Link", ImColor(32, 45, 32, 180));
					if (ax::NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
					{
						m_NodeManager.addLinkInfo(l_StartPinID.AsU64(), l_EndPinID.AsU64());
						//m_NodeManager.updateCurInfo();
					}
				}
			}
		}

		ax::NodeEditor::PinId l_PinID = 0;
		if (ax::NodeEditor::QueryNewNode(&l_PinID))
		{
			m_NewLinkPinID = l_PinID.AsU64();
			if (m_NewLinkPinID)
			{
				drawLable("+ Create Node", ImColor(32, 45, 32, 180));
			}

			if (ax::NodeEditor::AcceptNewItem())
			{
				m_NewNodePinID = m_NewLinkPinID;
				m_NewLinkPinID = 0;
				ax::NodeEditor::Suspend();
				ImGui::OpenPopup("Create New Node");
				ax::NodeEditor::Resume();
			}
		}
	}
	ax::NodeEditor::EndCreate();
}

void Editor::onDelectObject() noexcept
{
	if (ax::NodeEditor::BeginDelete())
	{
		ax::NodeEditor::LinkId l_LinkID = 0;
		while (ax::NodeEditor::QueryDeletedLink(&l_LinkID))
		{
			if (ax::NodeEditor::AcceptDeletedItem())
			{
				m_NodeManager.delLinkInfo(l_LinkID.AsU64());
				//if (m_NodeManager.delLinkInfo(l_LinkID.AsU64()))
				//	m_NodeManager.updateCurInfo();
			}
		}

		ax::NodeEditor::NodeId l_NodeID = 0;
		while (ax::NodeEditor::QueryDeletedNode(&l_NodeID))
		{
			U64 l_NodeIDU64 = l_NodeID.AsU64();
			if (ax::NodeEditor::AcceptDeletedItem())
			{
				if (m_NodeManager.nodeInfo(l_NodeIDU64))
				{
					m_NodeManager.delNodeInfo(l_NodeIDU64);
					//m_NodeManager.updateCurInfo();
				}
			}
		}
	}

	ax::NodeEditor::EndDelete();
}

void Editor::onClickObject() noexcept
{
	auto& l_IO = ImGui::GetIO();

	auto l_OpenPopupPosition = ImGui::GetMousePos();
	ax::NodeEditor::Suspend();

	if (ax::NodeEditor::ShowNodeContextMenu(&m_ContextNodeID))
		ImGui::OpenPopup("Node Context Menu");
	else if (ax::NodeEditor::ShowPinContextMenu(&m_ContextPinID))
		ImGui::OpenPopup("Pin Context Menu");
	else if (ax::NodeEditor::ShowLinkContextMenu(&m_ContextLinkID))
		ImGui::OpenPopup("Link Context Menu");
	else if (ax::NodeEditor::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("Create New Node");
		m_NewNodePinID = 0;
	}

	if (m_bHelp || (!l_IO.WantTextInput && ImGui::IsKeyPressed('H')))
	{
		ImGui::OpenPopup("Help");
	}
	ax::NodeEditor::Resume();
}

void Editor::nodeContext() noexcept
{
	ax::NodeEditor::Suspend();
	if (ImGui::BeginPopup("Node Context Menu"))
	{
		auto l_pNodeInfo = m_NodeManager.nodeInfo(m_ContextNodeID.AsU64());

		ImGui::TextUnformatted("Node Context Menu");
		ImGui::Separator();
		if (l_pNodeInfo)
		{
			ImGui::Text("Template: %s", l_pNodeInfo->m_TemplateName.c_str());
			ImGui::Text("Class: %s", l_pNodeInfo->m_ClassName.c_str());
		}
		else
			ImGui::Text("Unknown node: %p", m_ContextNodeID.AsPointer());
		ImGui::Separator();

		//if (ImGui::MenuItem("Save Template"))
		//{
		//	m_SaveTemplateDialog.Open();
		//}

		if (ImGui::MenuItem("Delete"))
		{
			ax::NodeEditor::DeleteNode(m_ContextNodeID);
			m_NodeManager.delNodeInfo(m_ContextNodeID.AsU64());
		}

		ImGui::EndPopup();
	}
	ax::NodeEditor::Resume();
}

void Editor::pinContext() noexcept
{
	ax::NodeEditor::Suspend();
	if (ImGui::BeginPopup("Pin Context Menu"))
	{
		auto l_pPinInfo = m_NodeManager.pinInfo(m_ContextPinID.AsU64());

		ImGui::TextUnformatted("Pin Context Menu");
		ImGui::Separator();
		if (l_pPinInfo)
		{
			ImGui::Text("ID: %d", m_ContextPinID.AsU64());
			if (l_pPinInfo->m_NodeID > NODE_ID_OFFSET)
				ImGui::Text("Node: %d", l_pPinInfo->m_NodeID);
			else
				ImGui::Text("Node: %s", "<none>");

			ImGui::Text("Type: %s", l_pPinInfo->m_DataType.enumValueStr().c_str());
			ImGui::Text("MeteData: %s", l_pPinInfo->m_MetaData.c_str());
		}
		else
			ImGui::Text("Unknown pin: %p", m_ContextPinID.AsPointer());

		ImGui::EndPopup();
	}
	ax::NodeEditor::Resume();
}

void Editor::linkContext() noexcept
{
	ax::NodeEditor::Suspend();
	if (ImGui::BeginPopup("Link Context Menu"))
	{
		auto l_pLinkInfo = m_NodeManager.linkInfo(m_ContextLinkID.AsU64());
		auto l_pStartNodeInfo = m_NodeManager.nodeInfo(l_pLinkInfo->m_OutputNodeID);
		auto l_pEndNodeInfo = m_NodeManager.nodeInfo(l_pLinkInfo->m_InputNodeID);
		auto l_pStartPinInfo = m_NodeManager.pinInfo(l_pLinkInfo->m_OutputPinID);
		auto l_pEndPinInfo = m_NodeManager.pinInfo(l_pLinkInfo->m_InputPinID);

		ImGui::TextUnformatted("Link Context Menu");
		ImGui::Separator();
		if (l_pLinkInfo && l_pStartNodeInfo && l_pEndNodeInfo && l_pStartPinInfo && l_pEndPinInfo)
		{
			ImGui::Text("Start Node: %s", l_pStartNodeInfo->m_Name.c_str());
			ImGui::Text("End Node: %s", l_pEndNodeInfo->m_Name.c_str());
			ImGui::Text("Start Pin: %s", l_pStartPinInfo->m_Name.c_str());
			ImGui::Text("End Pin: %s", l_pEndPinInfo->m_Name.c_str());
		}
		else
		{
			ImGui::Text("Unknown link: %d", m_ContextLinkID.AsPointer());
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Delete"))
		{
			ax::NodeEditor::DeleteLink(m_ContextLinkID);
			m_NodeManager.delLinkInfo(m_ContextLinkID.AsU64());
		}
		ImGui::EndPopup();
	}
	ax::NodeEditor::Resume();
}

void Editor::editorNodeList() noexcept
{
	auto l_NodePosition = ImGui::GetMousePos();

	ax::NodeEditor::Suspend();
	auto toLower = [&](std::string s) ->std::string {
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return std::tolower(c); }
		);
		return s;
		};

	if (ImGui::BeginPopup("Create New Node"))
	{
		std::list<std::string> l_NodeList;

		NodeID l_NodeID = 0;
		std::shared_ptr<NodeInfo> l_pNodeInfo = nullptr;
		static char l_Buffer[128] = "";
		static Bool l_WasActive = false;
		constexpr F32 l_IntputTextWidth = 300.0f;
		ImGui::PushItemWidth(l_IntputTextWidth);
		ImGui::InputText("##edit", l_Buffer, 127);
		ImGui::PopItemWidth();
		if (!l_WasActive)
		{
			ImGui::SetKeyboardFocusHere();
			l_WasActive = true;
		}

		if (ImGui::IsItemActive() && !l_WasActive)
		{
			ax::NodeEditor::EnableShortcuts(false);
			l_WasActive = true;
		}
		else if (!ImGui::IsItemActive() && l_WasActive)
		{
			ax::NodeEditor::EnableShortcuts(true);
			l_WasActive = false;
		}

		std::string l_InputLower = toLower(l_Buffer);
		for (auto& l_Pair : m_NodeManager.registeredNodes())
		{
			auto& l_NodeName = l_Pair.first;

			std::string l_NodeNameLower = toLower(l_NodeName);
			if (l_NodeNameLower.find(l_InputLower) != std::string::npos)
				l_NodeList.emplace_back(l_NodeName);
		}

		for (auto& l_NodeName : l_NodeList)
		{
			if (ImGui::MenuItem(l_NodeName.c_str()))
			{
				l_NodeID = m_NodeManager.addNodeInfo(l_NodeName);
				//m_NodeManager.updateCurInfo();
				const float l_FixedX = l_NodePosition.x - m_LeftPaneWidth > 0.0f ? l_NodePosition.x - m_LeftPaneWidth : 100.0f;
				const float l_FixedY = l_NodePosition.y - 100.0f;
				const ImVec2 l_FixedPos = ImVec2(l_FixedX, l_FixedY);

				ax::NodeEditor::SetNodePosition(l_NodeID, l_FixedPos);
			}
		}

		//l_pNodeInfo = m_NodeManager.nodeInfo(l_NodeID);
		//if (l_pNodeInfo)
		//{
		//	std::shared_ptr<PinInfo> l_pStartPin = m_NodeManager.pinInfo(m_NewNodePinID);
		//	ax::NodeEditor::SetNodePosition(l_NodeID, l_NodePosition);
			//	if (l_pStartPin)
		//	{
		//		std::vector<PinID> l_InputPinIDs{};
		//		std::vector<PinID> l_OutputPinIDs{};

		//		l_InputPinIDs.insert(l_InputPinIDs.end(), l_pNodeInfo->m_TemplateConnectionInputPinIDs.begin(), l_pNodeInfo->m_TemplateConnectionInputPinIDs.end());
		//		l_InputPinIDs.insert(l_InputPinIDs.end(), l_pNodeInfo->m_TemplateInputPinIDs.begin(), l_pNodeInfo->m_TemplateInputPinIDs.end());
		//		l_InputPinIDs.insert(l_InputPinIDs.end(), l_pNodeInfo->m_ConnectionInputPinIDs.begin(), l_pNodeInfo->m_ConnectionInputPinIDs.end());
		//		l_InputPinIDs.insert(l_InputPinIDs.end(), l_pNodeInfo->m_InputPinIDs.begin(), l_pNodeInfo->m_InputPinIDs.end());

		//		l_OutputPinIDs.insert(l_OutputPinIDs.end(), l_pNodeInfo->m_TemplateConnectionOutputPinIDs.begin(), l_pNodeInfo->m_TemplateConnectionOutputPinIDs.end());
		//		l_OutputPinIDs.insert(l_OutputPinIDs.end(), l_pNodeInfo->m_TemplateOutputPinIDs.begin(), l_pNodeInfo->m_TemplateOutputPinIDs.end());
		//		l_OutputPinIDs.insert(l_OutputPinIDs.end(), l_pNodeInfo->m_ConnectionOutputPinIDs.begin(), l_pNodeInfo->m_ConnectionOutputPinIDs.end());
		//		l_OutputPinIDs.insert(l_OutputPinIDs.end(), l_pNodeInfo->m_OutputPinIDs.begin(), l_pNodeInfo->m_OutputPinIDs.end());

		//		auto& l_PinIDs = l_pStartPin->m_PinType == PinType::Input ? l_OutputPinIDs : l_InputPinIDs;

		//		for (auto& l_PinID : l_PinIDs)
		//		{
		//			if (m_NodeManager.canAddLink(l_PinID, m_NewNodePinID))
		//			{
		//				m_NodeManager.addLinkInfo(l_PinID, m_NewNodePinID);
		//				m_NodeManager.updateCurInfo();
		//				break;
		//			}
		//		}
		//	}
		//}

		ImGui::EndPopup();
	}
	ax::NodeEditor::Resume();
}


RCEDITOR_NAMESPACE_END

int Main(int argc, char** argv)
{
	rceditor::Editor l_Edtior("Blueprints", argc, argv);

	if (l_Edtior.Create())
		return l_Edtior.Run();

	return 0;
}

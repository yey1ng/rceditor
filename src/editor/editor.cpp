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
	, m_SaveFavoriteDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc)
{
	m_SaveFileDialog.SetTitle("Save File Browser");
	m_SaveFileDialog.SetTypeFilters({ ".xml" });
	m_LoadFileDialog.SetTitle("Load File Browser");
	m_LoadFileDialog.SetTypeFilters({ ".xml" });

	m_SaveFavoriteDialog.SetTitle("Save File Browser");
	m_SaveFavoriteDialog.SetTypeFilters({ ".xml" });
	m_SaveFavoriteDialog.SetPwd(m_NodeManager.templatePath());
}

Editor::Editor(const char* name, int argc, char** argv) noexcept
	: Application(name, argc, argv)
	, m_SaveFileDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc)
	, m_LoadFileDialog(ImGuiFileBrowserFlags_CloseOnEsc)
	, m_SaveFavoriteDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc)
{
	m_SaveFileDialog.SetTitle("Save File Browser");
	m_SaveFileDialog.SetTypeFilters({ ".xml" });
	m_LoadFileDialog.SetTitle("Load File Browser");
	m_LoadFileDialog.SetTypeFilters({ ".xml" });

	m_SaveFavoriteDialog.SetTitle("Save File Browser");
	m_SaveFavoriteDialog.SetTypeFilters({ ".xml" });
	m_SaveFavoriteDialog.SetPwd(m_NodeManager.templatePath());
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

	templateNodeList();
	nodeEditor();

	nodeDetails();

	pinInfoDialog();
	paramInfoDialog();
	tipsDialog();
}

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

		ax::NodeEditor::ClearSelection();
		ax::NodeEditor::LoadData(&m_NodeManager);

		//m_Console.AddLog("Load Graph from %s.", l_FileName.c_str());
	}

	m_SaveFavoriteDialog.Display();
	if (m_SaveFavoriteDialog.HasSelected())
	{
		std::string l_FileName = m_SaveFavoriteDialog.GetSelected().string();
		m_SaveFavoriteDialog.ClearSelected();

		const auto l_Pos = l_FileName.find(".xml");
		if (std::string::npos == l_Pos)
		{
			l_FileName += ".xml";
		}

		ax::NodeEditor::Config& l_Config = ax::NodeEditor::GetConfig(ax::NodeEditor::GetCurrentEditor());
		l_Config.SettingsFile = l_FileName;


		auto l_pNodeInfo = m_NodeManager.nodeInfo(m_ContextNodeID.AsU64());
		if (l_pNodeInfo)
			l_pNodeInfo->saveFavoriteData(l_FileName, &m_NodeManager);
	}
}

void Editor::pinInfoDialog() noexcept
{
	auto l_pSelectedNodeInfo = m_NodeManager.nodeInfo(m_SelectedNodeID);
	if (!l_pSelectedNodeInfo)
	{
		m_bPinInfoDialog = false;
		return;
	}

	PinType l_PinType = m_PinInfoNeedAdd.getPinTypeValue();
	ImVec2 l_ScreenSize = ImGui::GetIO().DisplaySize;

	static ImVec2 l_WindowPos{};
	ImGui::SetNextWindowPos(l_WindowPos, ImGuiCond_Always);
	if (m_SelectedNodeID && m_bPinInfoDialog && ImGui::Begin("Add Pin Info", &m_bPinInfoDialog, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
	{
		ImVec2 l_WindowSize = ImGui::GetWindowSize();
		l_WindowPos = ImVec2((l_ScreenSize.x - l_WindowSize.x) * 0.5f, (l_ScreenSize.y - l_WindowSize.y) * 0.5f); // 计算窗口的居中位置

		std::string l_PrefixStr = "##Add Pin Info Table";

		U32 l_ColumnsCount = 5;
		if (m_PinInfoNeedAdd.getPinTypeValue() == PinType::InputOutput)
			l_ColumnsCount = 6;
		S32 l_PinIndex = -1;

		if (ImGui::BeginTable("Add Pin Info Table", l_ColumnsCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
		{
			if (m_PinInfoNeedAdd.getPinTypeValue() == PinType::InputOutput)
			{
				ImGui::TableSetupColumn("Index", ImGuiTableFlags_SizingMask_, 70);
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("InputMetaData", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("OutputMetaData", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
				ImGui::TableSetupColumn("AccessType", ImGuiTableFlags_SizingMask_, 100);
				ImGui::TableHeadersRow();
			}
			else
			{
				ImGui::TableSetupColumn("Index", ImGuiTableFlags_SizingMask_, 70);
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("MetaData", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
				ImGui::TableSetupColumn("AccessType", ImGuiTableFlags_SizingMask_, 100);
				ImGui::TableHeadersRow();
			}

			U32 l_TableIndex = 0;
			ImGui::TableNextRow();

			// Index
			{
				ImGui::TableSetColumnIndex(l_TableIndex++);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				auto l_IndexListStr = l_pSelectedNodeInfo->pinIDList().getIndexListStr(l_PinType);
				ImGui::Combo((l_PrefixStr + "_Index").c_str(), &m_InfoAddIndex, l_IndexListStr.c_str(), 5);
				ImGui::PopItemWidth();
			}

			// Name
			ImGui::TableSetColumnIndex(l_TableIndex++);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
			ImGui::InputText((l_PrefixStr + "_Name").c_str(), &m_PinInfoNeedAdd.m_Name, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			//MetaData
			{
				if (l_PinType == PinType::InputOutput || l_PinType == PinType::Input)
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::InputText((l_PrefixStr + "_InputMetaData").c_str(), &m_PinInfoNeedAdd.m_InputMetaData, ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}
				if (l_PinType == PinType::InputOutput || l_PinType == PinType::Output)
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::InputText((l_PrefixStr + "_OutputMetaData").c_str(), &m_PinInfoNeedAdd.m_OutputMetaData, ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}
			}

			// DataType
			{
				ImGui::TableSetColumnIndex(l_TableIndex++);
				S32 l_DataTypeIndex = static_cast<S32>(m_PinInfoNeedAdd.getDataTypeValue());
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				ImGui::Combo((l_PrefixStr + "_DataType").c_str(), &l_DataTypeIndex, m_PinInfoNeedAdd.m_DataType.enumAllTypeStr().c_str(), 5);
				m_PinInfoNeedAdd.setDataTypeValue(static_cast<EditorDataType>(l_DataTypeIndex));
				ImGui::PopItemWidth();
			}

			// AccessType
			if (!l_pSelectedNodeInfo->canChangePinAccessType(m_PinInfoNeedAdd.getPinTypeValue())) {
				m_PinInfoNeedAdd.setPinAccessTypeValue(PinAccessType::Readable);
				ImGui::TableSetColumnIndex(l_TableIndex++);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				ImGui::Text(m_PinInfoNeedAdd.m_PinAccessType.enumValueStr().c_str());
				ImGui::PopItemWidth();
			}
			else {
				ImGui::TableSetColumnIndex(l_TableIndex++);
				S32 l_AccessTypeIndex = static_cast<S32>(m_PinInfoNeedAdd.getPinAccessTypeValue());
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				ImGui::Combo((l_PrefixStr + "_AccessType").c_str(), &l_AccessTypeIndex, m_PinInfoNeedAdd.m_PinAccessType.enumAllTypeStr().c_str(), 5);
				m_PinInfoNeedAdd.setPinAccessTypeValue(static_cast<PinAccessType>(l_AccessTypeIndex));
				ImGui::PopItemWidth();
			}
			ImGui::EndTable();
		}

		if (ImGui::Button("Add"))
		{
			//std::shared_ptr<NodeInfo> l_pKeyMapNodeInfo = l_pSelectedNodeInfo;
			//if (l_pSelectedNodeInfo->getNodeType() == NodeType::Start || l_pSelectedNodeInfo->getNodeType() == NodeType::Start)
			//	l_pKeyMapNodeInfo = m_NodeManager.nodeInfo(l_pSelectedNodeInfo->getParentNodeID());
			if (l_pSelectedNodeInfo->hasNameKey(m_PinInfoNeedAdd.getName()))
			{
				m_bTipsDialog = true;
				m_TipsInfoStr = "Has Repeat Pin Name";
			}
			else
			{
				l_pSelectedNodeInfo->addPinInfo(m_PinInfoNeedAdd, m_InfoAddIndex - 1, &m_NodeManager);
				m_bPinInfoDialog = false;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::End();
	}
}

void Editor::paramInfoDialog() noexcept
{
	auto getParamIndexListStr = [](const std::vector<ParamInfo>& i_ParamInfoList) -> std::string {
		std::string l_IndexListStr = "-1";
		for (U32 i = 0; i < i_ParamInfoList.size(); ++i)
		{
			l_IndexListStr += '\0' + std::to_string(i);
		}
		return l_IndexListStr;
	};

	auto l_pSelectedNodeInfo = m_NodeManager.nodeInfo(m_SelectedNodeID);
	if (!l_pSelectedNodeInfo)
	{
		m_bParamInfoDialog = false;
		return;
	}

	ImVec2 l_ScreenSize = ImGui::GetIO().DisplaySize;

	static ImVec2 l_WindowPos{};
	ImGui::SetNextWindowPos(l_WindowPos, ImGuiCond_Always);
	if (m_SelectedNodeID && m_bParamInfoDialog && ImGui::Begin("Add Param Info", &m_bParamInfoDialog, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
	{
		ImVec2 l_WindowSize = ImGui::GetWindowSize();
		l_WindowPos = ImVec2((l_ScreenSize.x - l_WindowSize.x) * 0.5f, (l_ScreenSize.y - l_WindowSize.y) * 0.5f);

		std::string l_PrefixStr = "##Add Param Info Table";

		U32 l_ColumnsCount = 5;
		if (ImGui::BeginTable("Add Pin Info Table", l_ColumnsCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
		{
			ImGui::TableSetupColumn("Index", ImGuiTableFlags_SizingMask_, 70);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
			ImGui::TableSetupColumn("Use", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("RefKey", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			U32 l_TableIndex = 0;
			ImGui::TableNextRow();
			// Index
			{
				ImGui::TableSetColumnIndex(l_TableIndex++);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				auto l_IndexListStr = getParamIndexListStr(l_pSelectedNodeInfo->params());
				ImGui::Combo((l_PrefixStr + "_Index").c_str(), &m_InfoAddIndex, l_IndexListStr.c_str(), 5);
				ImGui::PopItemWidth();
			}

			// Name
			ImGui::TableSetColumnIndex(l_TableIndex++);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
			ImGui::InputText((l_PrefixStr + "_Name").c_str(), &m_ParamInfoNeedAdd.m_Name, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			// DataType
			{
				ImGui::TableSetColumnIndex(l_TableIndex++);
				S32 l_DataTypeIndex = static_cast<S32>(m_ParamInfoNeedAdd.m_DataType.m_Value);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				ImGui::Combo((l_PrefixStr + "_DataType").c_str(), &l_DataTypeIndex, m_ParamInfoNeedAdd.m_DataType.enumAllTypeStr().c_str(), 5);
				m_ParamInfoNeedAdd.m_DataType.m_Value = l_DataTypeIndex;
				m_ParamInfoNeedAdd.m_Data.initData(m_ParamInfoNeedAdd.m_DataType);
				ImGui::PopItemWidth();
			}

			// UseRef
			{
				ImGui::TableSetColumnIndex(l_TableIndex++);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				ImGui::Checkbox((l_PrefixStr + "_UseRef").c_str(), &m_ParamInfoNeedAdd.m_UseRef);
				ImGui::PopItemWidth();
			}

			// RefKey
			{
				ImGui::TableSetColumnIndex(l_TableIndex++);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				ImGui::InputText((l_PrefixStr + "_RefKey").c_str(), &m_ParamInfoNeedAdd.m_RefKey, ImGuiInputTextFlags_EnterReturnsTrue);
				ImGui::PopItemWidth();
			}
			ImGui::EndTable();
		}

		if (ImGui::Button("Add"))
		{
			if (l_pSelectedNodeInfo->hasNameKey(m_ParamInfoNeedAdd.m_Name))
			{
				m_bTipsDialog = true;
				m_TipsInfoStr = "Has Repeat Param Name";
			}
			else
			{
				l_pSelectedNodeInfo->addParam(m_ParamInfoNeedAdd, m_InfoAddIndex - 1);
				m_bParamInfoDialog = false;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::End();
	}
}

void Editor::tipsDialog() noexcept
{
	if (m_bTipsDialog)
		ImGui::OpenPopup("Someting Error");

	ImVec2 l_ScreenSize = ImGui::GetIO().DisplaySize;
	static ImVec2 l_WindowSize = ImVec2(200, 200);

	ImVec2 l_WindowPos((l_ScreenSize.x - l_WindowSize.x) * 0.5f, (l_ScreenSize.y - l_WindowSize.y) * 0.5f);
	ImGui::SetNextWindowPos(l_WindowPos, ImGuiCond_Always);

	if (ImGui::BeginPopupModal("Someting Error", &m_bTipsDialog, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		ImGui::Text(m_TipsInfoStr.c_str());
		ImGui::EndPopup();
	}
}

void Editor::templateNodeList() noexcept
{
	const auto& l_IO = ImGui::GetIO();

	splitter(true, 4.0f, &m_LeftPaneWidth, &m_MiddlePaneWidth, 50.0f, 50.0f);

	ImGui::NewLine();
	ImGui::Text("FPS: %.2f (%.2gms)", l_IO.Framerate, l_IO.Framerate ? 1000.0f / l_IO.Framerate : 0.0f);

	auto l_CurParentNodeID = m_NodeManager.curParentNodeID();
	if(m_NodeManager.taskTagInfoMap().find(l_CurParentNodeID) == m_NodeManager.taskTagInfoMap().end())
	{
		ImGui::SameLine(0, (m_LeftPaneWidth - 125 < 0 ? 100 : m_LeftPaneWidth - 125));
		if (ImGui::Button("Back to Parent Node(B)", ImVec2(250, 36)))
		{
			m_NodeManager.popCurParentNodeID();
			ax::NodeEditor::ClearSelection();
		}
	}
	else
	{
		ImGui::SameLine(0, m_LeftPaneWidth + 125);
		// Set a new font scale
		ImGui::SetWindowFontScale(1.8f);
		ImGui::TextUnformatted("");
		ImGui::SetWindowFontScale(1.0f);
	}

	ImGui::SameLine();
	// Set a new font scale
	ImGui::SetWindowFontScale(1.8f);
	std::string l_StackTitleStr{};
	m_NodeManager.getCurStackStr(m_NodeManager.curParentNodeID(), l_StackTitleStr);
	ImGui::TextUnformatted(l_StackTitleStr.c_str());
	ImGui::SetWindowFontScale(1.0f);

	ImGui::NewLine();	
	ImGui::SameLine(m_LeftPaneWidth + 18, 0);

	auto l_CurTagNodeID = m_NodeManager.curTagNodeID();
	auto l_TaskTagFlags = l_CurTagNodeID == 0 ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
	for (auto l_It = m_NodeManager.taskTagInfoMap().begin(); l_It != m_NodeManager.taskTagInfoMap().end(); ++l_It)
	{
		if (l_CurTagNodeID == l_It->first)
		{
			l_It->second->m_TabItemFlags = ImGuiTabItemFlags_SetSelected;
		}
		else
		{
			l_It->second->m_TabItemFlags = ImGuiTabItemFlags_None;
		}
		//l_It->second->m_TabItemFlags = l_CurTagNodeID == l_It->first ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
	}

	//static auto l_SelIndex = 0;
	//if (ImGui::BeginTabBar("Task Tab Bar", ImGuiTabBarFlags_AutoSelectNewTabs))
	//{
	//	if (ImGui::BeginTabItem("Task Editor1", nullptr, l_SelIndex == 0 ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None))
	//	{
	//		l_SelIndex = 0;
	//		ImGui::EndTabItem();
	//	}
	//	if (ImGui::BeginTabItem("Task Editor2", nullptr, l_SelIndex == 1 ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None))
	//	{
	//		l_SelIndex = 1;
	//		ImGui::EndTabItem();
	//	}
	//	if (ImGui::BeginTabItem("Task Editor3", nullptr, l_SelIndex == 2 ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None))
	//	{
	//		l_SelIndex = 2;
	//		ImGui::EndTabItem();
	//	}

	//	ImGui::EndTabBar();
	//}

	if (ImGui::BeginTabBar("Task Tab Bar"))
	{
		if (ImGui::BeginTabItem("Task Editor1", nullptr))
		{
			m_NodeManager.selTaskTagInfo(0);
			ImGui::EndTabItem();
		}

		S32 l_NeedDelNodeID = -1;
		NodeID l_ChangeNodeID = m_NodeManager.getTagNodeChangeNodeID();
		for (auto l_It = m_NodeManager.taskTagInfoMap().begin(); l_It != m_NodeManager.taskTagInfoMap().end(); ++l_It)
		{
			if (l_It->first == 0)
			{
				continue;
			}

			auto l_NodeID = l_It->first;
			auto l_pNodeInfo = m_NodeManager.nodeInfo(l_NodeID);
			std::string l_TitleStr = l_pNodeInfo->getNodeName() + "##" + std::to_string(l_pNodeInfo->getNodeID());
			l_TitleStr += l_pNodeInfo->getNodeID();
			if (l_pNodeInfo)
			{
				auto l_TabItemFlag = ImGuiTabItemFlags_None;
				if (l_ChangeNodeID == l_NodeID)
				{
					l_TabItemFlag = ImGuiTabItemFlags_SetSelected;
					m_NodeManager.setTagNodeChangeNodeID(0);
				}

				if (ImGui::BeginTabItem(l_TitleStr.c_str(), &l_It->second->m_IsShow, l_TabItemFlag))
				{
					m_NodeManager.selTaskTagInfo(l_NodeID);
					ImGui::EndTabItem();
				}
			}

			if (!l_It->second->m_IsShow)
				l_NeedDelNodeID = l_NodeID;
		}

		if(l_NeedDelNodeID > 0)
		{
			m_NodeManager.delTaskTagInfo(l_NeedDelNodeID);
		}

		ImGui::EndTabBar();
	}

	ImGui::BeginChild("Selection", ImVec2(m_LeftPaneWidth - 4, 0));
	{
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

		templateNodeTree(m_NodeManager.templatePath().string(), m_NodeManager.templateNodeFileMap().at(m_NodeManager.templatePath().string()), l_InputLower, m_NodeManager.curParentNodeID() == 0);

		ImGui::GetWindowDrawList()->AddRectFilled(
			ImGui::GetCursorScreenPos(),
			ImGui::GetCursorScreenPos() + ImVec2(m_LeftPaneWidth, ImGui::GetTextLineHeight()),
			ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
		ImGui::Spacing(); ImGui::SameLine();

		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
			for (const auto& l_Node : m_NodeManager.nodeMap())
				ax::NodeEditor::Flow(l_Node.first);
		
	}
	ImGui::EndChild();
}

void Editor::templateNodeTree(const std::string& i_DirStr, const std::vector<FileDetails>& i_FileVec, const std::string& i_InputStr, Bool i_IsTask, U32 i_Depth) noexcept
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
			if (!templateNodeTreeHasNode(l_FileDetails.m_FilenameStr, m_NodeManager.templateNodeFileMap().at(l_FileDetails.m_FilenameStr), i_InputStr, i_IsTask)) continue;

			if (ImGui::CollapsingHeader(l_FileDetails.m_FilenameStr.c_str()))
			{
				ImGui::Indent();
				templateNodeTree(l_FileDetails.m_FilenameStr, m_NodeManager.templateNodeFileMap().at(l_FileDetails.m_FilenameStr), i_InputStr, i_IsTask);
				ImGui::Unindent();
			}
		}

		else if (l_FileDetails.m_Type == TemplateFileType::Template)
		{
			std::string l_NodeNameLower = toLower(l_FileDetails.m_FilenameStr);
			auto l_pTemplateNodeInfo = m_NodeManager.templateNodeInfoMap().at(l_FileDetails.m_FilenameStr);
			if ((l_pTemplateNodeInfo && (i_IsTask && l_pTemplateNodeInfo->getNodeType() != NodeType::Task) || (!i_IsTask && l_pTemplateNodeInfo->getNodeType() == NodeType::Task)) || l_NodeNameLower.find(i_InputStr) == std::string::npos)
				continue;

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

Bool Editor::templateNodeTreeHasNode(const std::string& i_DirStr, const std::vector<FileDetails>& i_FileVec, const std::string& i_InputStr, Bool i_IsTask, U32 i_Depth) noexcept
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
			return templateNodeTreeHasNode(l_FileDetails.m_FilenameStr, m_NodeManager.templateNodeFileMap().at(l_FileDetails.m_FilenameStr), i_InputStr, i_IsTask);
		}
		else if (l_FileDetails.m_Type == TemplateFileType::Template)
		{
			std::string l_NodeNameLower = toLower(l_FileDetails.m_FilenameStr);
			auto l_pTemplateNodeInfo = m_NodeManager.templateNodeInfoMap().at(l_FileDetails.m_FilenameStr);
			if ((l_pTemplateNodeInfo && (i_IsTask && l_pTemplateNodeInfo->getNodeType() != NodeType::Task) || (!i_IsTask && l_pTemplateNodeInfo->getNodeType() == NodeType::Task)) || l_NodeNameLower.find(i_InputStr) == std::string::npos)
				continue;

			return true;
		}
	}

	return false;
}

void Editor::nodeEditor() noexcept
{
	ImGui::SameLine(0.0f, 14.0f);
	//std::string l_Test = "Node editor##" + std::to_string(m_NodeManager.curTagNodeID());

	ax::NodeEditor::Begin("Node editor", ImVec2(m_EditorWidth, 0));
	{
		drawNodes();

		onCreateObject();
		onDelectObject();
		onClickNode();
		onClickObject();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		nodeContext();
		pinContext();
		linkContext();
		editorNodeList();

		ImGui::PopStyleVar();
		
	}
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
		ImGui::SetNextWindowPos(ImVec2(l_IO.DisplaySize.x - m_RightPaneWidth, 0));
		ImGui::SetNextWindowSize(ImVec2(m_RightPaneWidth, l_IO.DisplaySize.y));
		ImGui::Begin("Node Info", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoCollapse);
		//ImGui::End();

		//ImGui::BeginChild("Node Info");
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
					ImGui::InputText((l_PrefixStr + "_Name").c_str(),  &l_pNodeInfo->nodeName(), ImGuiInputTextFlags_EnterReturnsTrue);
				}

				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("NodeID");

					ImGui::TableSetColumnIndex(1);
					ImGui::Text(std::to_string(l_pNodeInfo->getNodeID()).c_str());
				}

				if (l_pNodeInfo->getNodeType() == NodeType::CPU || l_pNodeInfo->getNodeType() == NodeType::GPU)
				{
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Template");

						ImGui::TableSetColumnIndex(1);
						ImGui::Text(l_pNodeInfo->getTemplateNodeName().c_str());
					}

					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Class");

						ImGui::TableSetColumnIndex(1);
						ImGui::Text(l_pNodeInfo->getClassName().c_str());
					}
				}

				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Description");

					ImGui::TableSetColumnIndex(1);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(1));
					ImGui::InputTextMultiline((l_PrefixStr + "_Description").c_str(), &l_pNodeInfo->comment(), ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue);
				}

				ImGui::EndTable();
			}
			ImGui::EndVertical();
		}
		if (l_pNodeInfo->getNodeType() == NodeType::Composite || l_pNodeInfo->getNodeType() == NodeType::Start || l_pNodeInfo->getNodeType() == NodeType::End) {
			nodePinDetails<false, PinType::Input>(m_SelectedNodeID);
			nodePinDetails<false, PinType::Output>(m_SelectedNodeID);
		}
		else if (l_pNodeInfo->getNodeType() == NodeType::Task)
		{
			nodePinDetails<false, PinType::Input>(m_SelectedNodeID);
			nodePinDetails<false, PinType::Output>(m_SelectedNodeID);

			nodeParamDetails<false>(m_SelectedNodeID);
		}
		else {
			nodePinDetails<true, PinType::InputOutput>(m_SelectedNodeID);
			nodePinDetails<true, PinType::Input>(m_SelectedNodeID);
			nodePinDetails<true, PinType::Output>(m_SelectedNodeID);
			nodeParamDetails<true>(m_SelectedNodeID);

			nodePinDetails<false, PinType::InputOutput>(m_SelectedNodeID);
			nodePinDetails<false, PinType::Input>(m_SelectedNodeID);
			nodePinDetails<false, PinType::Output>(m_SelectedNodeID);
			nodeParamDetails<false>(m_SelectedNodeID);
		}

		ImGui::Unindent();
		//ImGui::EndChild();
		ImGui::End();
	}
}

template <Bool IsTemplate, PinType T>
void Editor::nodePinDetails(NodeID i_NodeID) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_NodeManager.nodeInfo(i_NodeID);
	if (!l_pNodeInfo) return;

	//if ((l_pNodeInfo->m_Type == NodeType::Composite || l_pNodeInfo->m_Type == NodeType::Start || l_pNodeInfo->m_Type == NodeType::End) && (IsTemplate || T == PinType::InputOutput))
	//	return;

	if (!IsTemplate)
	{
		if ((T == PinType::InputOutput && !l_pNodeInfo->getIsDynamicInputOutputPin()) ||
			(T == PinType::Input && !l_pNodeInfo->getIsDynamicInputPin()) ||
			(T == PinType::Output && !l_pNodeInfo->getIsDynamicOutputPin()))
			return;
	}

	std::string l_PrefixStr = "##" + std::to_string(i_NodeID);
	std::string l_TitleStr = l_PrefixStr;
	std::vector<PinID>* l_pPinIDs{};
	if (IsTemplate)
	{
		l_PrefixStr += l_PrefixStr + "_Template";
	}

	{
		if (T == PinType::InputOutput)
		{
			l_PrefixStr += "_InputOutput";

			if (!IsTemplate)
			{
				l_TitleStr = "InputOutput" + l_PrefixStr;
				l_pPinIDs = &l_pNodeInfo->pinIDList().inputOutputOutputPinIDs();
			}
			else
			{
				l_TitleStr = "Template InputOutput" + l_PrefixStr;
				l_pPinIDs = &l_pNodeInfo->templatePinIDList().inputOutputOutputPinIDs();
			}
		}
		else if (T == PinType::Input)
		{
			l_PrefixStr += "_Input";

			if (!IsTemplate)
			{
				l_TitleStr = "Input" + l_PrefixStr;
				l_pPinIDs = &l_pNodeInfo->pinIDList().inputPinIDs();
			}
			else
			{
				l_TitleStr = "Template Input" + l_PrefixStr;
				l_pPinIDs = &l_pNodeInfo->templatePinIDList().inputPinIDs();
			}
		}
		else if (T == PinType::Output)
		{
			l_PrefixStr += "_Output";

			if (!IsTemplate)
			{
				l_TitleStr = "Output" + l_PrefixStr;
				l_pPinIDs = &l_pNodeInfo->pinIDList().outputPinIDs();
			}
			else
			{
				l_TitleStr = "Template Output" + l_PrefixStr;
				l_pPinIDs = &l_pNodeInfo->templatePinIDList().outputPinIDs();
			}
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
			m_bPinInfoDialog = true;
			m_bParamInfoDialog = false;
			m_PinInfoNeedAdd = PinInfo{};
			m_PinInfoNeedAdd.setName("TempPin");
			m_PinInfoNeedAdd.m_PinType = T;
			m_PinInfoNeedAdd.setIsInputOutput(T == PinType::InputOutput);
			m_InfoAddIndex = 0;
		}

		U32 l_TableNum = 4;
		if(T == PinType::InputOutput)
			l_TableNum += 1;
		if(!IsTemplate)
			l_TableNum += 1;

		l_PrefixStr += "_Pin List";
		if (ImGui::BeginTable(l_PrefixStr.c_str(), l_TableNum, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			if (T != PinType::InputOutput) {

				ImGui::TableSetupColumn("MetaData", ImGuiTableColumnFlags_WidthStretch);
			}
			else {
				ImGui::TableSetupColumn("InputMetaData", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("OutputMetaData", ImGuiTableColumnFlags_WidthStretch);
			}
			ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
			ImGui::TableSetupColumn("AccessType", ImGuiTableFlags_SizingMask_, 100);
			if(!IsTemplate)
				ImGui::TableSetupColumn(" Del ", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			for (auto l_PinID : *l_pPinIDs)
			{
				PinInfoChangeFlags l_IsChange = 0;
				U32 l_TableIndex = 0;

				auto l_pPinInfo = m_NodeManager.pinInfo(l_PinID);
				if (!l_pPinInfo) continue;

				l_PrefixStr += "_" + std::to_string(l_PinID);

				ImGui::TableNextRow();
				// Name
				//if (IsTemplate)
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->m_Name.c_str(), ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}
				//else {
				//	ImGui::TableSetColumnIndex(l_TableIndex++);
				//	ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				//	std::string l_PrevStr = l_pPinInfo->m_InputMetaData;
				//	ImGui::InputText((l_PrefixStr + "_MetaData").c_str(), &l_pPinInfo->m_InputMetaData, ImGuiInputTextFlags_EnterReturnsTrue);
				//	if (l_PrevStr != l_pPinInfo->m_InputMetaData)
				//		l_IsChange |= PinInfoChangeFlags_MetaChange;
				//	ImGui::PopItemWidth();
				//}

				// MetaData
				//if (IsTemplate)
				if(T != PinType::InputOutput)
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->getMetaData().c_str(), ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->m_InputMetaData.c_str(), ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();

					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->m_OutputMetaData.c_str(), ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}
				//else {
				//	ImGui::TableSetColumnIndex(l_TableIndex++);
				//	ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				//	std::string l_PrevStr = l_pPinInfo->m_InputMetaData;
				//	ImGui::InputText((l_PrefixStr + "_MetaData").c_str(), &l_pPinInfo->m_InputMetaData, ImGuiInputTextFlags_EnterReturnsTrue);
				//	if (l_PrevStr != l_pPinInfo->m_InputMetaData)
				//		l_IsChange |= PinInfoChangeFlags_MetaChange;
				//	ImGui::PopItemWidth();
				//}

				// DataType
				//if (IsTemplate) 
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->getDataType().enumValueStr().c_str());
					ImGui::PopItemWidth();
				}
				//else {
				//	ImGui::TableSetColumnIndex(l_TableIndex++);
				//	S32 l_DataTypeIndex = static_cast<S32>(l_pPinInfo->getDataTypeValue());
				//	ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				//	ImGui::Combo((l_PrefixStr + "_DataType").c_str(), &l_DataTypeIndex, l_pPinInfo->m_DataType.enumAllTypeStr().c_str(), 5);
				//	if (l_DataTypeIndex != l_pPinInfo->m_DataType.m_Value)
				//	{
				//		l_IsChange |= PinInfoChangeFlags_DataType;
				//		l_pPinInfo->setDataTypeValue(static_cast<EditorDataType>(l_DataTypeIndex));
				//	}
				//	ImGui::PopItemWidth();
				//}

				// AccessType
				//if (!l_pNodeInfo->canChangePinAccessType() && (IsTemplate || T == PinType::Input)) 
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->m_PinAccessType.enumValueStr().c_str());
					ImGui::PopItemWidth();
				}
				//else {
				//	ImGui::TableSetColumnIndex(l_TableIndex++);
				//	S32 l_AccessTypeIndex = static_cast<S32>(l_pPinInfo->getPinAccessTypeValue());
				//	ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				//	ImGui::Combo((l_PrefixStr + "_AccessType").c_str(), &l_AccessTypeIndex, l_pPinInfo->m_PinAccessType.enumAllTypeStr().c_str(), 5);
				//	if (l_AccessTypeIndex != l_pPinInfo->m_PinAccessType.m_Value)
				//	{
				//		l_IsChange |= PinInfoChangeFlags_PinAccessType;
				//		l_pPinInfo->setPinAccessTypeValue(static_cast<PinAccessType>(l_AccessTypeIndex));
				//	}
				//	ImGui::PopItemWidth();
				//}

				//if (!IsTemplate) {
				//	ImGui::TableSetColumnIndex(l_TableIndex++);
				//	ImGui::PushID((l_PrefixStr + "_Up").c_str());
				//	if (ImGui::SmallButton("Up"))
				//	{
				//		l_IsSwapPrev = true;
				//		l_CurSelIndex = l_Index;
				//	}
				//	ImGui::PopID();
				//}

				//if (!IsTemplate) {
				//	ImGui::TableSetColumnIndex(l_TableIndex++);
				//	ImGui::PushID((l_PrefixStr + "_Down").c_str());
				//	if (ImGui::SmallButton("Down"))
				//	{
				//		l_IsSwapNext = true;
				//		l_CurSelIndex = l_Index;
				//	}
				//	ImGui::PopID();
				//}

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


				l_pNodeInfo->changePinInfo(T, l_Index, &m_NodeManager, l_pPinInfo, l_IsChange);
				++l_Index;
				//m_NodeManager.changeSpecNodePinInfo<T>(l_pNodeInfo, l_pPinInfo, l_Index);
			}

			if (l_IsDelete){

				l_pNodeInfo->delPinInfo(T, l_CurSelIndex, &m_NodeManager);
			}
			else if (l_IsSwapPrev || l_IsSwapNext) {
				l_pNodeInfo->swapPinID(T, l_CurSelIndex, l_IsSwapPrev ? true : false, &m_NodeManager);
			}
			//	if (T == PinType::InputOutput) {
			//		{
			//			if (const auto l_It = std::next(l_pNodeInfo->m_ConnectionOutputPinIDs.begin(), l_CurSelIndex); l_It != l_pNodeInfo->m_ConnectionOutputPinIDs.begin())
			//			{
			//				const auto l_PrevElement = std::prev(l_It);
			//				std::swap(*l_It, *l_PrevElement);
			//			}
			//		}

			//		{
			//			if (const auto l_It = std::next(l_pNodeInfo->m_ConnectionInputPinIDs.begin(), l_CurSelIndex); l_It != l_pNodeInfo->m_ConnectionInputPinIDs.begin())
			//			{
			//				const auto l_PrevElement = std::prev(l_It);
			//				std::swap(*l_It, *l_PrevElement);
			//			}
			//		}
			//	}
			//	else
			//	{
			//		if (const auto l_It = std::next(l_pPinIDs->begin(), l_CurSelIndex); l_It != l_pPinIDs->begin())
			//		{
			//			const auto l_PrevElement = std::prev(l_It);
			//			std::swap(*l_It, *l_PrevElement);
			//		}
			//	}
			//}
			//else if (l_IsSwapNext) {
			//	if (T == PinType::InputOutput) {
			//		{
			//			const auto l_It = std::next(l_pNodeInfo->m_ConnectionOutputPinIDs.begin(), l_CurSelIndex);
			//			if (const auto l_NextElement = std::next(l_It); l_NextElement != l_pNodeInfo->m_ConnectionOutputPinIDs.end())
			//			{
			//				std::swap(*l_It, *l_NextElement);
			//			}
			//		}

			//		{
			//			const auto l_It = std::next(l_pNodeInfo->m_ConnectionInputPinIDs.begin(), l_CurSelIndex);
			//			if (const auto l_NextElement = std::next(l_It); l_NextElement != l_pNodeInfo->m_ConnectionInputPinIDs.end())
			//			{
			//				std::swap(*l_It, *l_NextElement);
			//			}
			//		}
			//	}
			//	else
			//	{
			//		const auto l_It = std::next(l_pPinIDs->begin(), l_CurSelIndex);
			//		if (const auto l_NextElement = std::next(l_It); l_NextElement != l_pPinIDs->end())
			//		{
			//			std::swap(*l_It, *l_NextElement);
			//		}
			//	}
			//}

			ImGui::EndTable();
		}
	}
}

template <Bool IsTemplate>
void Editor::nodeParamDetails(NodeID i_NodeID) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_NodeManager.nodeInfo(i_NodeID);
	assert(l_pNodeInfo);

	if (!IsTemplate && !l_pNodeInfo->getIsDynamicParam()) return; // NodeInfo add function isShowParam()/ isShowTemplateParam();

	std::string l_PrefixStr = "##" + std::to_string(i_NodeID);
	std::string l_TitleStr{};
	std::vector<ParamInfo>* l_pParamInfos = nullptr;

	if (IsTemplate) {
		l_TitleStr = "Template Param" + l_PrefixStr;
		l_PrefixStr += "_Template_Param";
		l_pParamInfos = &l_pNodeInfo->templateParams();
	}
	else {
		l_TitleStr = "Param" + l_PrefixStr;
		l_PrefixStr += "_Param";
		l_pParamInfos = &l_pNodeInfo->params();
	}

	if (ImGui::CollapsingHeader(l_TitleStr.c_str()))
	{
		if (!IsTemplate && l_pNodeInfo->getNodeType() != NodeType::Task && ImGui::Button((std::string("Add") + l_PrefixStr).c_str()))
		{
			//l_pNodeInfo->addParam();
			m_bPinInfoDialog = false;
			m_bParamInfoDialog = true;

			S32 m_InfoAddIndex = 0;
			ParamInfo m_ParamInfoNeedAdd{};
			m_ParamInfoNeedAdd.m_Name = "TempParam";
		}

		U32 l_TableNum = 6;
		if (IsTemplate)
			l_TableNum = 5;

		if (ImGui::BeginTable((std::string("ParamList") + "##" + l_PrefixStr).c_str(), l_TableNum, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
		{
			l_PrefixStr += "_ParamList";

			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
			ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Use", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("RefKey", ImGuiTableColumnFlags_WidthStretch);
			if (!IsTemplate)
				ImGui::TableSetupColumn(" Del ", ImGuiTableColumnFlags_WidthFixed);

			ImGui::TableHeadersRow();

			Bool l_IsDelete = false;
			U32 l_CurSelIndex{};
			for (U32 i = 0; i < l_pParamInfos->size(); ++i)
			{
				l_PrefixStr += std::to_string(i);
				auto& l_ParamInfo = (*l_pParamInfos)[i];

				U32 l_TableIndex = 0;
				ImGui::TableNextRow();

				//Name
				//if (IsTemplate) 
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_ParamInfo.m_Name.c_str());
					ImGui::PopItemWidth();
				}
				//else {
				//	ImGui::TableSetColumnIndex(l_TableIndex++);
				//	ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				//	ImGui::InputText((l_PrefixStr + "_Name").c_str(), &l_ParamInfo.m_Name, ImGuiInputTextFlags_EnterReturnsTrue);
				//	ImGui::PopItemWidth();
				//}

				// DataType
				//if (IsTemplate) 
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_ParamInfo.m_DataType.enumValueStr().c_str());
					ImGui::PopItemWidth();
				}
				//else {
				//	ImGui::TableSetColumnIndex(l_TableIndex++);
				//	S32 l_DataTypeIndex = static_cast<S32>(l_ParamInfo.m_DataType.m_Value);
				//	ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				//	ImGui::Combo((l_PrefixStr + "_DataType").c_str(), &l_DataTypeIndex, l_ParamInfo.m_DataType.enumAllTypeStr().c_str(), 5);
				//	auto l_PrevDataTypeIndex = l_ParamInfo.m_DataType.m_Value;
				//	if (l_PrevDataTypeIndex != l_DataTypeIndex)
				//	{
				//		l_ParamInfo.m_DataType.m_Value = l_DataTypeIndex;
				//		l_ParamInfo.initData();
				//	}
				//	ImGui::PopItemWidth();
				//}

				// Data
				if (IsTemplate) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_ParamInfo.m_Data.getData(l_ParamInfo.m_DataType).c_str());
					ImGui::PopItemWidth();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					std::string l_Str = l_ParamInfo.m_Data.getData(l_ParamInfo.m_DataType);
					ImGui::InputText((l_PrefixStr + "_Data").c_str(), &l_Str, ImGuiInputTextFlags_EnterReturnsTrue);
					l_ParamInfo.m_Data.setData(l_ParamInfo.m_DataType, l_Str);
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

template <Bool IsConnection>
ax::Drawing::IconType Editor::getNodeIconType(std::shared_ptr<NodeInfo> i_pNodeInfo) noexcept
{
	if (IsConnection)
		return ax::Drawing::IconType::Flow;

	if (i_pNodeInfo->getNodeType() == NodeType::Start || i_pNodeInfo->getNodeType() == NodeType::End || i_pNodeInfo->getNodeType() == NodeType::Composite)
		return ax::Drawing::IconType::RoundSquare;
	else if (i_pNodeInfo->getNodeType() == NodeType::Task)
		return ax::Drawing::IconType::Square;
	else
		return ax::Drawing::IconType::Circle;
}

template <Bool IsInput, Bool IsInputOutput>
void inline Editor::drawPin(const std::vector<PinID>& i_PinIDs, ax::Drawing::IconType i_IconType, ax::NodeEditor::Utilities::BlueprintNodeBuilder* i_NodeBuilder) noexcept
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
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, l_Alpha);
			i_NodeBuilder->Input(l_PinID);
			ax::Widgets::Icon(ImVec2(static_cast<F32>(m_PinIconSize), static_cast<F32>(m_PinIconSize)), i_IconType, l_pPinInfo->m_LinkIDSet.size() > 0, l_Color, ImColor(32, 32, 32, static_cast<S32>(l_Alpha * 255)));
			ImGui::Spring(0);
			ImGui::TextUnformatted(l_pPinInfo->m_Name.c_str());
			ImGui::Spring(0);
			i_NodeBuilder->EndInput();
			ImGui::PopStyleVar();
		}
		else {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, l_Alpha);
			i_NodeBuilder->Output(l_PinID);
			if (!IsInputOutput)
				ImGui::TextUnformatted(l_pPinInfo->m_Name.c_str());
			ax::Widgets::Icon(ImVec2(static_cast<F32>(m_PinIconSize), static_cast<F32>(m_PinIconSize)), i_IconType, l_pPinInfo->m_LinkIDSet.size() > 0, l_Color, ImColor(32, 32, 32, static_cast<S32>(l_Alpha * 255)));
			ImGui::Spring(0);
			i_NodeBuilder->EndOutput();
			ImGui::PopStyleVar();
		}
	}
}

void Editor::drawNodes() noexcept
{
	auto l_CursorTopLeft = ImGui::GetCursorScreenPos();

	//Save Cur Editor Info
	auto l_pCurTaskTagInfo = m_NodeManager.curTaskTagInfo();
	if (l_pCurTaskTagInfo)
	{
		if (m_NodeManager.isTagNodeChange()) {
			ax::NodeEditor::SetNavigateZoom(l_pCurTaskTagInfo->m_Zoom);
			ax::NodeEditor::SetNavigateScroll(ImVec2(l_pCurTaskTagInfo->m_ViewScroll.x, l_pCurTaskTagInfo->m_ViewScroll.y));
			ax::NodeEditor::SetNavigateVisibleRect(ImRect(l_pCurTaskTagInfo->m_VisibleRect.xMin, l_pCurTaskTagInfo->m_VisibleRect.yMin, l_pCurTaskTagInfo->m_VisibleRect.xMax, l_pCurTaskTagInfo->m_VisibleRect.yMax));
			m_NodeManager.setTagNodeChange(false);
		}
		else {
			l_pCurTaskTagInfo->m_Zoom = ax::NodeEditor::GetNavigateZoom();
			l_pCurTaskTagInfo->m_ViewScroll.x = ax::NodeEditor::GetNavigateScroll().x;
			l_pCurTaskTagInfo->m_ViewScroll.y = ax::NodeEditor::GetNavigateScroll().y;
			l_pCurTaskTagInfo->m_VisibleRect.xMin = ax::NodeEditor::GetNavigateVisibleRect().Min.x;
			l_pCurTaskTagInfo->m_VisibleRect.xMax = ax::NodeEditor::GetNavigateVisibleRect().Min.y;
			l_pCurTaskTagInfo->m_VisibleRect.xMax = ax::NodeEditor::GetNavigateVisibleRect().Max.x;
			l_pCurTaskTagInfo->m_VisibleRect.yMax = ax::NodeEditor::GetNavigateVisibleRect().Max.y;
		}
	}

	ax::NodeEditor::Utilities::BlueprintNodeBuilder l_Builder(m_HeaderBackground, GetTextureWidth(m_HeaderBackground), GetTextureHeight(m_HeaderBackground));
	{
		//draw drag nodes
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* l_Payload = ImGui::AcceptDragDropPayload("DRAG_TEXT"))
			{
				std::string l_NodeName = static_cast<const char*>(l_Payload->Data);

				const auto l_NewNodePos = ImGui::GetMousePos();
				const ImVec2 l_FixedPos = ImVec2(l_NewNodePos.x, l_NewNodePos.y);

				NodeID m_ParentNodeID = m_NodeManager.curParentNodeID();
				auto l_pNodeInfo = m_NodeManager.instanceNodeInfo(l_NodeName, m_ParentNodeID);
				
				//m_NodeManager.updateCurInfo();

				ax::NodeEditor::SetNodePosition(l_pNodeInfo->getNodeID(), l_FixedPos);
			}
			ImGui::EndDragDropTarget();
		}

		// draw nodes
		for (auto l_CurNodeID : m_NodeManager.curStackInfo()->m_NodeIDList)
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
					ImGui::TextUnformatted(l_pCurNodeInfo->getNodeName().c_str());
					ImGui::Spring(1);
					ImGui::Dummy(ImVec2(0, 28));
					ImGui::Spring(0);
					l_Builder.EndHeader();
				}

				{
					l_Builder.TemplatePin();
					if (l_pCurNodeInfo->getNodeType() == NodeType::CPU || l_pCurNodeInfo->getNodeType() == NodeType::GPU)
					drawPin<true, true>(l_pCurNodeInfo->templatePinIDList().inputOutputInputPinIDs(), getNodeIconType<true>(l_pCurNodeInfo), &l_Builder);
					drawPin<true, false>(l_pCurNodeInfo->templatePinIDList().inputPinIDs(), getNodeIconType<false>(l_pCurNodeInfo), &l_Builder);

					drawPin <false, true> (l_pCurNodeInfo->templatePinIDList().inputOutputOutputPinIDs(), getNodeIconType<true>(l_pCurNodeInfo), &l_Builder);
					drawPin<false, false>(l_pCurNodeInfo->templatePinIDList().outputPinIDs(), getNodeIconType<false>(l_pCurNodeInfo), &l_Builder);
					l_Builder.EndTemplatePin();

				}

				{
					l_Builder.NormalPin();
					drawPin<true, true>(l_pCurNodeInfo->pinIDList().inputOutputInputPinIDs(), getNodeIconType<true>(l_pCurNodeInfo), &l_Builder);
					drawPin<true, false>(l_pCurNodeInfo->pinIDList().inputPinIDs(), getNodeIconType<false>(l_pCurNodeInfo), &l_Builder);

					drawPin <false, true>(l_pCurNodeInfo->pinIDList().inputOutputOutputPinIDs(), getNodeIconType<true>(l_pCurNodeInfo), &l_Builder);
					drawPin<false, false>(l_pCurNodeInfo->pinIDList().outputPinIDs(), getNodeIconType<false>(l_pCurNodeInfo), &l_Builder);
					l_Builder.EndNormalPin();
				}

				l_Builder.End();
			}
		}
	}

	for (auto l_LinkID : m_NodeManager.curStackInfo()->m_LinkIDList)
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

			if (l_pStartPinInfo->getPinType() == PinType::Input)
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
				else if (l_pStartPinInfo->getPinType() == l_pEndPinInfo->getPinType())
				{
					drawLable("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (l_pStartPinInfo->getNodeID() == l_pEndPinInfo->getNodeID())
				{
					drawLable("x Cannot connect to self", ImColor(45, 32, 32, 180));
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
				}
				else if (l_pStartPinInfo->getDataType() != l_pEndPinInfo->getDataType())
				{
					drawLable("x Incompatible Pin Data Type", ImColor(45, 32, 32, 180));
					ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				}
				else if (l_pEndPinInfo->getPinAccessType() != PinAccessType::Writable && l_pStartPinInfo->getPinAccessType() != l_pEndPinInfo->getPinAccessType())
				{
					drawLable("x Incompatible Pin Access Type", ImColor(45, 32, 32, 180));
					ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				}
				else if (!l_pEndPinInfo->getMetaData().empty() && (l_pStartPinInfo->getMetaData() != l_pEndPinInfo->getMetaData()))
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
				auto l_pNodeInfo = m_NodeManager.nodeInfo(l_NodeIDU64);
				if (l_pNodeInfo && (l_pNodeInfo->getNodeType() != NodeType::Start && l_pNodeInfo->getNodeType() != NodeType::End))
				{
					m_NodeManager.delNodeInfo(l_NodeIDU64);
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

void Editor::onClickNode() noexcept
{
	if (m_SelectedNodeID == 0) return;
	auto l_pNodeInfo = m_NodeManager.nodeInfo(m_SelectedNodeID);
	if (!l_pNodeInfo || (l_pNodeInfo->getNodeType() != NodeType::Composite && l_pNodeInfo->getNodeType() != NodeType::Task)) return;

	const auto& io = ImGui::GetIO();
	if (m_SelectedNodeID == ax::NodeEditor::GetHoveredNode().AsU64())
	{
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (l_pNodeInfo->getNodeType() == NodeType::Task) {
				m_NodeManager.addTaskTagInfo(m_SelectedNodeID);
			}
			else
			{
				m_NodeManager.pushCurParentNodeID(m_SelectedNodeID);
			}
			ax::NodeEditor::ClearSelection();
		}
	}
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
			ImGui::Text("Template: %s", l_pNodeInfo->getTemplateNodeName().c_str());
			ImGui::Text("Class: %s", l_pNodeInfo->getClassName().c_str());
		}
		else
			ImGui::Text("Unknown node: %p", m_ContextNodeID.AsPointer());

		if (ImGui::MenuItem("Save Favorite Node"))
		{
			m_SaveFavoriteDialog.Open();
		}

		if (l_pNodeInfo->getNodeType() != NodeType::Start && l_pNodeInfo->getNodeType() != NodeType::End)
		{
			ImGui::Separator();

			if (ImGui::MenuItem("Delete"))
			{
				ax::NodeEditor::DeleteNode(m_ContextNodeID);
				m_NodeManager.delNodeInfo(m_ContextNodeID.AsU64());
			}
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
			if (l_pPinInfo->getNodeID() > NODE_ID_OFFSET)
				ImGui::Text("Node: %d", l_pPinInfo->getNodeID());
			else
				ImGui::Text("Node: %s", "<none>");

			ImGui::Text("Type: %s", l_pPinInfo->getDataType().enumValueStr().c_str());
			ImGui::Text("MeteData: %s", l_pPinInfo->getMetaData().c_str());
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
			ImGui::Text("Start Node: %s", l_pStartNodeInfo->getNodeName().c_str());
			ImGui::Text("End Node: %s", l_pEndNodeInfo->getNodeName().c_str());
			ImGui::Text("Start Pin: %s", l_pStartPinInfo->getName().c_str());
			ImGui::Text("End Pin: %s", l_pEndPinInfo->getName().c_str());
		}
		else
		{
			ImGui::Text("Unknown link: %d", m_ContextLinkID.AsPointer());
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Delete"))
		{
			ax::NodeEditor::DeleteLink(m_ContextLinkID);
			//m_NodeManager.delLinkInfo(m_ContextLinkID.AsU64());
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
		for (auto& l_Pair : m_NodeManager.templateNodeInfoMap())
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
				NodeID m_ParentNodeID = m_NodeManager.curParentNodeID();
				auto l_pNodeInfo = m_NodeManager.instanceNodeInfo(l_NodeName, m_ParentNodeID);
				//m_NodeManager.updateCurInfo();
				const float l_FixedX = l_NodePosition.x - m_LeftPaneWidth > 0.0f ? l_NodePosition.x - m_LeftPaneWidth : 100.0f;
				const float l_FixedY = l_NodePosition.y - 100.0f;
				const ImVec2 l_FixedPos = ImVec2(l_FixedX, l_FixedY);

				ax::NodeEditor::SetNodePosition(l_pNodeInfo->getNodeID(), l_FixedPos);
			}
		}

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

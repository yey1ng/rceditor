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
	, m_SettingDialog(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc)
	, m_SelFileDialog(ImGuiFileBrowserFlags_CloseOnEsc)
	, m_SelDirDialog(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc)
{
	std::string l_SetttingFilename = "Setttings.xml";
	std::ifstream l_SettingFile(l_SetttingFilename);

	m_SaveFileDialog.SetTitle("Save File Browser");
	m_SaveFileDialog.SetTypeFilters({ ".rc" });
	m_LoadFileDialog.SetTitle("Load File Browser");
	m_LoadFileDialog.SetTypeFilters({ ".rc" });
	m_SaveFavoriteDialog.SetTitle("Save File Browser");
	m_SaveFavoriteDialog.SetTypeFilters({ ".fav" });
	m_SettingDialog.SetTitle("Save WorkingDir Browser");
	m_SelFileDialog.SetTitle("Sel File");
	m_SelDirDialog.SetTitle("Sel Dir");

	if (l_SettingFile.good())
	{
		m_EditorSetting.loadSetting();
		initWorkDir();
	}
	else
	{
		m_bSetting = true;
	}
}

Editor::Editor(const char* name, int argc, char** argv) noexcept
	: Application(name, argc, argv)
	, m_SaveFileDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc)
	, m_LoadFileDialog(ImGuiFileBrowserFlags_CloseOnEsc)
	, m_SaveFavoriteDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc)
	, m_SettingDialog(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc)
	, m_SelFileDialog(ImGuiFileBrowserFlags_CloseOnEsc)
	, m_SelDirDialog(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc)
{
	std::string l_SetttingFilename = "Setttings.xml";
	std::ifstream l_SettingFile(l_SetttingFilename);

	m_SaveFileDialog.SetTitle("Save File Browser");
	m_SaveFileDialog.SetTypeFilters({ ".rc" });
	m_LoadFileDialog.SetTitle("Load File Browser");
	m_LoadFileDialog.SetTypeFilters({ ".rc" });
	m_SaveFavoriteDialog.SetTitle("Save File Browser");
	m_SaveFavoriteDialog.SetTypeFilters({ ".fav" });
	m_SettingDialog.SetTitle("Save WorkingDir Browser");
	m_SelFileDialog.SetTitle("Sel File");
	m_SelDirDialog.SetTitle("Sel Dir");

	if (l_SettingFile.good())
	{
		m_EditorSetting.loadSetting();
		initWorkDir();
	}
	else
	{
		m_bSetting = true;
	}
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
	splitter("left", true, 4.0f, &m_LeftPaneWidth, &m_EditorWidth, 50.0f, 50.0f);

	auto l_TestWidth = (l_IO.DisplaySize.x - m_RightPaneWidth) >= 0.0 ? (l_IO.DisplaySize.x - m_RightPaneWidth): 0;
	splitter("right", true, 4.0f, &l_TestWidth, &m_RightPaneWidth, 50.0f, 50.0f);

	m_SelectedNodes.resize(ax::NodeEditor::GetSelectedObjectCount());
	const int l_NodeCount = ax::NodeEditor::GetSelectedNodes(m_SelectedNodes.data(), static_cast<int>(m_SelectedNodes.size()));
	m_SelectedNodes.resize(l_NodeCount);

	if (m_SelectedNodes.size() == 1)
	{
		m_SelectedNodeID = m_SelectedNodes[0].AsU64();
	}
	else
	{
		m_SelectedNodeID = 0;
	}

	menuBar();
	menuDialog();

	nodePrototypeList();
	nodeEditor();

	nodeDetails();
	
	pinInfoDialog();
	paramInfoDialog();
	tipsDialog();
	settingDialog();
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

			if (ImGui::MenuItem("Setting"))
			{
				m_bSetting = true;
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

	if (m_bSetting)
	{
		m_SettingDialog.Open();
		m_bSetting = false;
	}
}

void Editor::menuDialog() noexcept
{
	m_SaveFileDialog.Display();
	if (m_SaveFileDialog.HasSelected())
	{
		std::string l_FileName = m_SaveFileDialog.GetSelected().string();
		m_SaveFileDialog.ClearSelected();

		const auto l_Pos = l_FileName.find(".rc");
		if (std::string::npos == l_Pos)
		{
			l_FileName += ".rc";
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

	m_SettingDialog.Display();
	if (m_SettingDialog.HasSelected())
	{
		std::string l_DirName = m_SettingDialog.GetSelected().string();
		m_SettingDialog.ClearSelected();

		m_EditorSetting.setWorkingDir(l_DirName);
		m_EditorSetting.saveSetting();

		initWorkDir();
	}

	m_SaveFavoriteDialog.Display();
	if (m_SaveFavoriteDialog.HasSelected())
	{
		std::string l_FilePath = m_SaveFavoriteDialog.GetSelected().string();
		m_SaveFavoriteDialog.ClearSelected();

		const auto l_Pos = l_FilePath.find(".fav");
		if (std::string::npos == l_Pos)
		{
			l_FilePath += ".fav";
		}

		ax::NodeEditor::Config& l_Config = ax::NodeEditor::GetConfig(ax::NodeEditor::GetCurrentEditor());
		l_Config.SettingsFile = l_FilePath;

		auto l_pNodeInfo = m_NodeManager.nodeInfo(m_ContextNodeID.AsU64());
		if (l_pNodeInfo)
		{
			//FileName
			S32 l_PosLeft = l_FilePath.rfind("\\", l_FilePath.length());
			S32 l_PosRight = l_FilePath.rfind(".fav", l_FilePath.length());
			std::string l_FileName = l_FilePath.substr(l_PosLeft + 1, l_PosRight - l_PosLeft - 1);
			//Dir Key
			std::string l_DirStr = l_FilePath.substr(0, l_PosLeft);
			S32 l_PosDir = l_DirStr.rfind("\\", l_DirStr.length());
			std::string l_PathKeyStr = l_DirStr.substr(l_PosDir + 1, l_DirStr.length() - l_PosDir) + "##" + l_DirStr.substr(0, l_PosDir);

			tinyxml2::XMLDocument l_XMLDocument{};
			tinyxml2::XMLElement* l_XmlNode = l_XMLDocument.NewElement(g_XmlFavoriteNodeStr.data());
			auto l_pNodePrototype = l_pNodeInfo->getNodePrototype(l_FileName, &m_NodeManager);
			if (!l_pNodePrototype) return;

			l_pNodePrototype->saveData(l_XmlNode);

			l_XMLDocument.InsertEndChild(l_XmlNode);
			l_XMLDocument.SaveFile(l_FilePath.c_str());

			auto& l_NodePrototypeFileMap = m_NodeManager.nodePrototypeFileMap();
			auto& l_NodePrototypeMap = m_NodeManager.nodePrototypeMap();
			if (l_NodePrototypeMap.find(l_FileName) == l_NodePrototypeMap.end())
				l_NodePrototypeFileMap[l_PathKeyStr].emplace_back(PrototypeFileType::Prototype, l_FileName);
			l_NodePrototypeMap[l_FileName] = l_pNodePrototype;
		}
	}

	m_SelFileDialog.Display();
	if (m_SelFileDialog.HasSelected())
	{
		std::string l_Path = m_SelFileDialog.GetSelected().string();
		m_SelFileDialog.ClearSelected();

		//Relative Path
		std::filesystem::path l_FullPath = std::filesystem::absolute(l_Path);
		std::filesystem::path l_RelativePath = std::filesystem::relative(l_FullPath, m_EditorSetting.m_WorkingDir);

		m_SelPath.second = l_RelativePath.string();
	}

	m_SelDirDialog.Display();
	if (m_SelDirDialog.HasSelected())
	{
		std::string l_Path = m_SelDirDialog.GetSelected().string();
		m_SelDirDialog.ClearSelected();

		//Relative Path
		std::filesystem::path l_FullPath = std::filesystem::absolute(l_Path);
		std::filesystem::path l_RelativePath = std::filesystem::relative(l_FullPath, m_EditorSetting.m_WorkingDir);

		m_SelPath.second = l_RelativePath.string();
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
		l_WindowSize.y += 100;
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
				auto l_IndexListStr = l_pSelectedNodeInfo->getIndexListStr(l_PinType);
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
			Bool l_CanAddPin = false;
			if (std::dynamic_pointer_cast<ChildNodeInfo>(l_pSelectedNodeInfo))
			{
				auto l_pParentNodeInfo = m_NodeManager.nodeInfo(l_pSelectedNodeInfo->getAssociationNodeID());
				if (l_pParentNodeInfo && !l_pParentNodeInfo->hasPinKey(m_PinInfoNeedAdd.getName()))
					l_CanAddPin = true;
			}
			else
			{
				if (!l_pSelectedNodeInfo->hasPinKey(m_PinInfoNeedAdd.getName()))
					l_CanAddPin = true;
				
			}

			if (!l_CanAddPin)
			{
				m_bTipsDialog = true;
				m_TipsInfoStr = "Has Repeat Pin Name";
			}
			else
			{
				l_pSelectedNodeInfo->addPinInfo2(m_PinInfoNeedAdd, &m_NodeManager, m_InfoAddIndex - 1);
				m_bPinInfoDialog = false;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::End();
	}
}

void Editor::paramInfoDialog() noexcept
{
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
		if(m_ParamInfoNeedAdd.useSlider())
			l_ColumnsCount = 7;
		else if(m_ParamInfoNeedAdd.getDataType() == ParamDataType::Int || m_ParamInfoNeedAdd.getDataType() == ParamDataType::Float 
			|| m_ParamInfoNeedAdd.getDataType() == ParamDataType::String || m_ParamInfoNeedAdd.getDataType() == ParamDataType::WString
			|| m_ParamInfoNeedAdd.getDataType() == ParamDataType::Vector3f || m_ParamInfoNeedAdd.getDataType() == ParamDataType::Vector4f)
			l_ColumnsCount = 6;

		if (ImGui::BeginTable("Add Pin Info Table", l_ColumnsCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
		{
			ImGui::TableSetupColumn("Index", ImGuiTableFlags_SizingMask_, 70);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
			ImGui::TableSetupColumn("Use", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("RefKey", ImGuiTableColumnFlags_WidthStretch);
			if(l_ColumnsCount >= 6)
				ImGui::TableSetupColumn("WidgetType", ImGuiTableColumnFlags_WidthStretch);
			if (l_ColumnsCount >= 7)
			{
				ImGui::TableSetupColumn("Min Max", ImGuiTableFlags_SizingMask_, 100);
			}

			ImGui::TableHeadersRow();

			U32 l_TableIndex = 0;
			ImGui::TableNextRow();
			// Index
			{
				ImGui::TableSetColumnIndex(l_TableIndex++);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				auto l_IndexListStr = l_pSelectedNodeInfo->paramList().getIndexListStr();
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
				S32 l_DataTypeIndex = static_cast<S32>(m_ParamInfoNeedAdd.getDataType());
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				ImGui::Combo((l_PrefixStr + "_DataType").c_str(), &l_DataTypeIndex, m_ParamInfoNeedAdd.getEnumTypeStr().c_str(), 5);
				if(l_DataTypeIndex != static_cast<S32>(m_ParamInfoNeedAdd.getDataType()))
					m_ParamInfoNeedAdd.setDataType(static_cast<ParamDataType>(l_DataTypeIndex));
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

			//WidgetType
			if (l_ColumnsCount > 5)
			{
				ImGui::TableSetColumnIndex(l_TableIndex++);
				S32 l_WidgetTypeIndex = static_cast<S32>(m_ParamInfoNeedAdd.getWidgetType().m_Value);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				ImGui::Combo((l_PrefixStr + "_WidgetType").c_str(), &l_WidgetTypeIndex, m_ParamInfoNeedAdd.getWidgetType().enumAllTypeStr().c_str(), 5);
				m_ParamInfoNeedAdd.getWidgetType().m_Value = l_WidgetTypeIndex;
				ImGui::PopItemWidth();
			}

			//Min Max
			if (l_ColumnsCount == 7)
			{
				ImGui::TableSetColumnIndex(l_TableIndex++);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				if (m_ParamInfoNeedAdd.getDataType() == ParamDataType::Int)
				{
					S32 l_MinMax[2] = { m_ParamInfoNeedAdd.getIntMin(), m_ParamInfoNeedAdd.getIntMax() };
					ImGui::InputInt2((l_PrefixStr + "_MinMax").c_str(), l_MinMax);
					m_ParamInfoNeedAdd.setIntMin(l_MinMax[0]);
					m_ParamInfoNeedAdd.setIntMax(l_MinMax[1]);
				}
				else if (m_ParamInfoNeedAdd.getDataType() == ParamDataType::Float)
				{
					F32 l_MinMax[2] = { m_ParamInfoNeedAdd.getFloatMin(), m_ParamInfoNeedAdd.getFloatMax() };
					ImGui::InputFloat2((l_PrefixStr + "_MinMax").c_str(), l_MinMax);
					m_ParamInfoNeedAdd.setFloatMin(l_MinMax[0]);
					m_ParamInfoNeedAdd.setFloatMax(l_MinMax[1]);
				}
				ImGui::PopItemWidth();
			}
			ImGui::EndTable();
		}

		if (ImGui::Button("Add"))
		{
			if (l_pSelectedNodeInfo->hasParamKey(m_ParamInfoNeedAdd.m_Name))
			{
				m_bTipsDialog = true;
				m_TipsInfoStr = "Has Repeat Param Name";
			}
			else
			{
				l_pSelectedNodeInfo->addParam(m_ParamInfoNeedAdd, false, m_InfoAddIndex - 1);
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

void Editor::settingDialog() noexcept
{
	ImVec2 l_ScreenSize = ImGui::GetIO().DisplaySize;

	static ImVec2 l_WindowPos{};
	ImGui::SetNextWindowPos(l_WindowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize({ 620, 120 }, ImGuiCond_FirstUseEver);
	if (m_bSetting && ImGui::Begin("Setting Path", &m_bSetting, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
	{
		ImVec2 l_WindowSize = ImGui::GetWindowSize();
		l_WindowPos = ImVec2((l_ScreenSize.x - l_WindowSize.x) * 0.5f, (l_ScreenSize.y - l_WindowSize.y) * 0.5f);

		std::string l_PrefixStr = "##Setting Path";

		ImGui::Text("Prototype Path");
		ImGui::SameLine(ImGui::CalcTextSize("Prototype Path").x + 20);

		ImGui::InputText((l_PrefixStr + "_Name").c_str(), &m_EditorSetting.m_PrototypePath, ImGuiInputTextFlags_EnterReturnsTrue);

		if (ImGui::Button("Save"))
		{
			m_EditorSetting.saveSetting();
			m_bSetting = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::End();
	}
}

void Editor::nodePrototypeList() noexcept
{
	const auto& l_IO = ImGui::GetIO();

	ImGui::NewLine();
	ImGui::BeginChild("Selection", ImVec2(m_LeftPaneWidth - 4, 0));
	{
		ImGui::Text("FPS: %.2f (%.2gms)", l_IO.Framerate, l_IO.Framerate ? 1000.0f / l_IO.Framerate : 0.0f);

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

		static char l_Buffer[128] = "";
		ImGui::InputTextWithHint("", "Search", l_Buffer, 127, ImGuiInputTextFlags_EnterReturnsTrue);
		std::string l_InputLower = toLower(l_Buffer);

		if (!m_EditorSetting.m_PrototypePath.empty())
		{
			NodeTag l_NodeTag = NodeTag::NoTag;
			if (m_NodeManager.curParentNodeID() != 0)
			{
				auto l_pParentNodeInfo = m_NodeManager.nodeInfo(m_NodeManager.curParentNodeID());
				if (l_pParentNodeInfo)
				{
					if (l_pParentNodeInfo->isTaskNode())
					{
						l_NodeTag = l_pParentNodeInfo->getNodeTag();
					}
					else
					{
						auto l_pParentTaskNodeInfo = m_NodeManager.nodeInfo(l_pParentNodeInfo->getTaskNodeID());
						if (l_pParentTaskNodeInfo)
							l_NodeTag = l_pParentTaskNodeInfo->getNodeTag();
					}
				}
			}

			nodePrototypeTree(m_EditorSetting.m_PrototypePath, m_NodeManager.nodePrototypeFileMap().at(m_EditorSetting.m_PrototypePath), l_InputLower, m_NodeManager.curParentNodeID() == 0, l_NodeTag);
		}

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

void Editor::nodePrototypeTree(const std::string& i_DirStr, const std::vector<FileDetails>& i_FileVec, const std::string& i_InputStr, Bool i_IsTask, NodeTag i_TaskNodeTag, U32 i_Depth) noexcept
{
	for (const auto& l_FileDetails : i_FileVec)
	{
		if (l_FileDetails.m_Type == PrototypeFileType::Dir)
		{
			if (!nodePrototypeTreeHasNode(l_FileDetails.m_FilenameStr, m_NodeManager.nodePrototypeFileMap().at(l_FileDetails.m_FilenameStr), i_InputStr, i_IsTask, i_TaskNodeTag)) continue;

			if (ImGui::CollapsingHeader(l_FileDetails.m_FilenameStr.c_str()))
			{
				ImGui::Indent();
				nodePrototypeTree(l_FileDetails.m_FilenameStr, m_NodeManager.nodePrototypeFileMap().at(l_FileDetails.m_FilenameStr), i_InputStr, i_IsTask, i_TaskNodeTag);
				ImGui::Unindent();
			}
		}

		else if (l_FileDetails.m_Type == PrototypeFileType::Prototype)
		{
			std::string l_NodeNameLower = toLower(l_FileDetails.m_FilenameStr);
			auto l_pNodePrototypeInfo = m_NodeManager.nodePrototypeMap().at(l_FileDetails.m_FilenameStr);
			//if ((l_pNodePrototypeInfo && (i_IsTask && !l_pNodePrototypeInfo->isTaskNode()) || (!i_IsTask && l_pNodePrototypeInfo->isTaskNode())) || l_NodeNameLower.find(i_InputStr) == std::string::npos)
			//	continue;

			if (!filterNodePrototype(l_FileDetails.m_FilenameStr, i_InputStr, i_TaskNodeTag, i_IsTask))
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

Bool Editor::nodePrototypeTreeHasNode(const std::string& i_DirStr, const std::vector<FileDetails>& i_FileVec, const std::string& i_InputStr, Bool i_IsTask, NodeTag i_TaskNodeTag, U32 i_Depth) noexcept
{
	for (const auto& l_FileDetails : i_FileVec)
	{
		if (l_FileDetails.m_Type == PrototypeFileType::Dir)
		{
			return nodePrototypeTreeHasNode(l_FileDetails.m_FilenameStr, m_NodeManager.nodePrototypeFileMap().at(l_FileDetails.m_FilenameStr), i_InputStr, i_IsTask, i_TaskNodeTag);
		}
		else if (l_FileDetails.m_Type == PrototypeFileType::Prototype)
		{
			std::string l_NodeNameLower = toLower(l_FileDetails.m_FilenameStr);
			auto l_pNodePrototypeInfo = m_NodeManager.nodePrototypeMap().at(l_FileDetails.m_FilenameStr);
			if ((l_pNodePrototypeInfo && (i_IsTask && !l_pNodePrototypeInfo->isTaskNode()) || (!i_IsTask && l_pNodePrototypeInfo->isTaskNode())) || l_NodeNameLower.find(i_InputStr) == std::string::npos)
				continue;

			return true;
		}
	}

	return false;
}

void Editor::nodeEditor() noexcept
{
	ImGui::SameLine(0.0f, 12.0f);
	ImGui::BeginChild("Node Editor Info", ImVec2(m_MiddlePaneWidth, 0));
	{
		Bool l_HasChange = false;

		// Back to Parent Node
		auto l_CurParentNodeID = m_NodeManager.curParentNodeID();
		if (m_NodeManager.taskTagInfoMap().find(l_CurParentNodeID) == m_NodeManager.taskTagInfoMap().end())
		{
			if (ImGui::Button("Back to Parent Node(B)", ImVec2(250, 36)))
			{
				auto l_test1 = ImGui::GetItemRectMin();
				auto l_test2 = ImGui::GetItemRectMax();

				m_NodeManager.popCurParentNodeID();
				ax::NodeEditor::ClearSelection();
			}
		}
		else
		{
			//ImGui::SameLine(0, 125);
			// Set a new font scale
			ImGui::SetWindowFontScale(1.8f);
			ImGui::TextUnformatted("");
			ImGui::SetWindowFontScale(1.0f);
		}

		// Cur Stack Str
		ImGui::SameLine();
		// Set a new font scale
		ImGui::SetWindowFontScale(1.8f);
		std::string l_StackTitleStr{};
		m_NodeManager.getCurStackStr(m_NodeManager.curParentNodeID(), l_StackTitleStr);
		ImGui::TextUnformatted(l_StackTitleStr.c_str());
		ImGui::SetWindowFontScale(1.0f);
			
		// Search Node
		std::string l_PrevStr = m_SearchStr;
		bool l_IsEnter = ImGui::InputTextWithHint("##Search Node", "Search", &m_SearchStr, ImGuiInputTextFlags_EnterReturnsTrue);
		m_SearchStr = toLower(m_SearchStr);
		if (l_PrevStr != m_SearchStr)
		{
			l_HasChange = true;
		}

		ImGui::SameLine(0.0f, 2.0f);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver, ImVec2(1.0f, 0.0f));
		S32 l_DataTypeIndex = static_cast<S32>(m_SearchType.m_Value);
		ImGui::Combo("##SearchNode_DataType", &l_DataTypeIndex, m_SearchType.enumAllTypeStr().c_str(), 2);
		if (static_cast<S32>(m_SearchType.m_Value) != l_DataTypeIndex)
		{
			l_HasChange = true;
			m_SearchType = l_DataTypeIndex;
		}

		if (l_HasChange)
		{
			m_NodeManager.highLightSearchNode(m_SearchStr, static_cast<SearchType>(m_SearchType.m_Value));

			auto l_SearchNodeList = m_NodeManager.searchNodeList(m_SearchStr, static_cast<SearchType>(m_SearchType.m_Value));
			l_HasChange = false;

			//ax::NodeEditor::ClearSelection();
			//for (auto l_NodeID : l_SearchNodeList)
			//{
			//	ax::NodeEditor::SelectNode(l_NodeID, true);
			//}

			////add empty Node
			//if (l_SearchNodeList.size() == 1)
			//{
			//	ax::NodeEditor::SelectNode(l_SearchNodeList[0], true);
			//}
		}
			
		// Task Tab Bar
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
		}

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

			if (l_NeedDelNodeID > 0)
			{
				m_NodeManager.closeTaskTagInfo(l_NeedDelNodeID);
			}

			ImGui::EndTabBar();
		}
	}

	auto& l_IO = ImGui::GetIO();
	m_MiddlePaneWidth = (l_IO.DisplaySize.x - m_LeftPaneWidth - m_RightPaneWidth - 12) >= 0.0 ? (l_IO.DisplaySize.x - m_LeftPaneWidth - m_RightPaneWidth - 12) : 0;
	ax::NodeEditor::Begin("Node editor", ImVec2(m_MiddlePaneWidth, 0));
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

	ImGui::EndChild();
}

void Editor::nodeDetails() noexcept
{
	const auto& l_IO = ImGui::GetIO();

	ImGui::SameLine(l_IO.DisplaySize.x - m_RightPaneWidth - 6, 0);
	ImGui::BeginChild("Node Details", ImVec2(m_RightPaneWidth, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar );
	{
		ax::NodeEditor::NodeId l_NodeID = 0;
		std::shared_ptr<NodeInfo> l_pNodeInfo = m_NodeManager.nodeInfo(m_SelectedNodeID);
		if (l_pNodeInfo)
		{
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
						ImGui::InputText((l_PrefixStr + "_Name").c_str(), &l_pNodeInfo->nodeName(), ImGuiInputTextFlags_EnterReturnsTrue);
						ImGui::PopItemWidth();
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
							ImGui::Text(l_pNodeInfo->getTemplateName().c_str());
						}
					}

					ImGui::EndTable();
				}

				if (ImGui::CollapsingHeader("Template Description"))
				{
					auto l_TemplateNodeInfo = m_NodeManager.nodePrototypeMap()[l_pNodeInfo->getTemplateName()];
					assert(l_TemplateNodeInfo);
					ImGui::InputTextMultiline("##Template Description", &l_TemplateNodeInfo->getComment(), ImVec2(l_RightPaneWidth, 0), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ReadOnly);
				}

				if (ImGui::CollapsingHeader("Description"))
				{
					ImGui::InputTextMultiline("##Description", &l_pNodeInfo->getComment(), ImVec2(l_RightPaneWidth, 0), ImGuiInputTextFlags_EnterReturnsTrue);
				}

				if (l_pNodeInfo->getNodeType() == NodeType::Composite) {
					nodePinDetails<PinType::Input>(m_SelectedNodeID);
					nodePinDetails<PinType::Output>(m_SelectedNodeID);
				}
				else if (l_pNodeInfo->getNodeType() == NodeType::Start)
				{
					nodePinDetails<PinType::Output>(m_SelectedNodeID);
				}
				else if (l_pNodeInfo->getNodeType() == NodeType::End)
				{
					nodePinDetails<PinType::Input>(m_SelectedNodeID);
				}
				else if (l_pNodeInfo->getNodeType() == NodeType::Data)
				{
					nodePinDetails<PinType::Output>(m_SelectedNodeID);
					nodeParamDetails(m_SelectedNodeID);
				}
				else if (l_pNodeInfo->isTaskNode())
				{
					nodePinDetails<PinType::Input>(m_SelectedNodeID);
					nodePinDetails<PinType::Output>(m_SelectedNodeID);

					nodeParamDetails(m_SelectedNodeID);
					//QueueFamily
					nodeTaskEU(m_SelectedNodeID);
				}
				else {
					nodePinDetails<PinType::InputOutput>(m_SelectedNodeID);
					nodePinDetails<PinType::Input>(m_SelectedNodeID);
					nodePinDetails<PinType::Output>(m_SelectedNodeID);

					nodeParamDetails(m_SelectedNodeID);
				}
				ImGui::EndVertical();
			}
			ImGui::Unindent();
		}
	}

	ImGui::EndChild();
}

template <PinType T>
void Editor::nodePinDetails(NodeID i_NodeID) noexcept
{
	auto getPinIDList = [](PinIDList* i_pTemplatePinIDList, PinIDList* i_pPinIDList)->std::vector<PinID> {
		std::vector<PinID> l_PinIDList{};

		if (i_pTemplatePinIDList)
		{
			for (auto l_PinID : i_pTemplatePinIDList->pinIDList())
			{
				l_PinIDList.push_back(l_PinID);
			}
		}
		if (i_pPinIDList)
		{
			for (auto l_PinID : i_pPinIDList->pinIDList())
			{
				l_PinIDList.push_back(l_PinID);
			}
		}

		return l_PinIDList;
	};

	std::shared_ptr<NodeInfo> l_pNodeInfo = m_NodeManager.nodeInfo(i_NodeID);
	if (!l_pNodeInfo) return;

	Bool l_IsShowAddButton = false;
	PinIDList* l_pPinIDList{};
	PinIDList* l_pTemplatePinIDList{};

	std::string l_PrefixStr = "##" + std::to_string(i_NodeID);
	std::string l_TitleStr = l_PrefixStr;
	if (T == PinType::InputOutput)
	{
		l_PrefixStr += "_InputOutput";
		l_TitleStr = "InputOutput" + l_PrefixStr;
		
		l_pTemplatePinIDList = &l_pNodeInfo->templateInputOutputOutputPinIDList();
		if (l_pNodeInfo->getIsDynamicInputOutputPin())
		{
			l_IsShowAddButton = true;
			l_pPinIDList = &l_pNodeInfo->inputOutputOutputPinIDList();
		}
	}
	else if (T == PinType::Input)
	{
		l_PrefixStr += "_Input";
		l_TitleStr = "Input" + l_PrefixStr;
			
		l_pTemplatePinIDList = &l_pNodeInfo->templateInputPinIDList();
		if (l_pNodeInfo->getIsDynamicInputPin())
		{
			l_IsShowAddButton = true;
			l_pPinIDList = &l_pNodeInfo->inputPinIDList();
		}
	}
	else if (T == PinType::Output)
	{
		l_PrefixStr += "_Output";
		l_TitleStr = "Output" + l_PrefixStr;
			
		l_pTemplatePinIDList = &l_pNodeInfo->templateOutputPinIDList();
		if (l_pNodeInfo->getIsDynamicOutputPin())
		{
			l_IsShowAddButton = true;
			l_pPinIDList = &l_pNodeInfo->outputPinIDList();
		}

	}

	auto l_PinIDList = getPinIDList(l_pTemplatePinIDList, l_pPinIDList);

	U32 l_Index = 0;
	Bool l_IsDelete = false;
	U32 l_CurSelIndex = 0;

	if (ImGui::CollapsingHeader(l_TitleStr.c_str()))
	{
		if (l_IsShowAddButton && ImGui::Button((std::string("Add") + l_PrefixStr).c_str()))
		{
			m_bPinInfoDialog = true;
			m_bParamInfoDialog = false;
			m_PinInfoNeedAdd = PinInfo{};
			m_PinInfoNeedAdd.setName("TempPin");
			m_PinInfoNeedAdd.m_PinType = T;
			m_PinInfoNeedAdd.setIsInputOutput(T == PinType::InputOutput);
			m_InfoAddIndex = 0;
		}

		U32 l_TableNum = 5;
		if (T == PinType::InputOutput)
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
			ImGui::TableSetupColumn(" Del ", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			for (auto l_PinID : l_PinIDList)
			{
				U32 l_TableIndex = 0;
				std::string l_PinPrefixStr = l_PrefixStr + "_" + std::to_string(l_PinID);
				auto l_pPinInfo = m_NodeManager.pinInfo(l_PinID);
				if (!l_pPinInfo) continue;

				ImGui::TableNextRow();

				// Pin Name
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->getName().c_str(), ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}

				// MetaData
				if (T != PinType::InputOutput)
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

				// DataType
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->getDataType().enumValueStr().c_str());
					ImGui::PopItemWidth();
				}

				// AccessType
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_pPinInfo->m_PinAccessType.enumValueStr().c_str());
					ImGui::PopItemWidth();
				}

				if (!l_pPinInfo->getIsTemplate()) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushID((l_PinPrefixStr + "_Del").c_str());
					if (ImGui::SmallButton("Del"))
					{
						l_IsDelete = true;
						l_CurSelIndex = l_Index;
					}
					ImGui::PopID();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					ImGui::PushID((l_PinPrefixStr + "_Del").c_str());
					if (ImGui::SmallButton("Del"))
					{
						l_IsDelete = false;
					}
					ImGui::PopID();
					ImGui::PopStyleVar();
				}

				if(!l_pPinInfo->getIsTemplate())
					++l_Index;
			}

			if (l_IsDelete) {
				l_IsDelete = false;
				l_pNodeInfo->delPinInfo2(T, l_CurSelIndex, &m_NodeManager);
			}

			ImGui::EndTable();
		}
	}
}

void Editor::nodeParamDetails(NodeID i_NodeID) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_NodeManager.nodeInfo(i_NodeID);
	assert(l_pNodeInfo);

	std::string l_PrefixStr = "##" + std::to_string(i_NodeID) + "_Param";
	std::string l_TitleStr = "Param" + l_PrefixStr;
	ParamInfoList* l_pTemplateParamInfoList = &l_pNodeInfo->templateParamList();
	ParamInfoList* l_pParamInfoList = &l_pNodeInfo->paramList();

	std::vector<ParamInfo*> l_ParamInfoList{};
	l_ParamInfoList.reserve(l_pTemplateParamInfoList->paramInfoList().size() + l_pParamInfoList->paramInfoList().size());
	for (ParamInfo& l_ParamInfo : l_pTemplateParamInfoList->paramInfoList())
	{
		l_ParamInfoList.push_back(&l_ParamInfo);
	}
	for (ParamInfo& l_ParamInfo : l_pParamInfoList->paramInfoList())
	{
		l_ParamInfoList.push_back(&l_ParamInfo);
	}

	if (ImGui::CollapsingHeader(l_TitleStr.c_str()))
	{
		if (l_pNodeInfo->getIsDynamicParam() && !l_pNodeInfo->isTaskNode() && ImGui::Button((std::string("Add") + l_PrefixStr).c_str()))
		{
			//l_pNodeInfo->addParam();
			m_bPinInfoDialog = false;
			m_bParamInfoDialog = true;

			S32 m_InfoAddIndex = 0;
			ParamInfo m_ParamInfoNeedAdd{};
			m_ParamInfoNeedAdd.m_Name = "TempParam";
		}

		if (ImGui::BeginTable((std::string("ParamInfo") + l_PrefixStr).c_str(), 5, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
		{
			l_PrefixStr += "_ParamList";
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
			ImGui::TableSetupColumn("Use", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("RefKey", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn(" Del ", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			ImGui::EndTable();
		}

		Bool l_IsDelete = false;
		U32 l_CurSelIndex = 0;
		for (U32 i = 0; i < l_ParamInfoList.size(); ++i)
		{
			std::string l_ParamPerfixStr = l_PrefixStr + std::to_string(i);
			auto l_ParamInfo = l_ParamInfoList[i];
			U32 l_TableNum = 5;

			ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(1.f, 0.f, 0.f, 1.0f));
			if (ImGui::BeginTable((std::string("ParamInfo") + l_PrefixStr + std::to_string(i)).c_str(), 5, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("DataType", ImGuiTableFlags_SizingMask_, 90);
				ImGui::TableSetupColumn("Use", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("RefKey", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn(" Del ", ImGuiTableColumnFlags_WidthFixed);
				//ImGui::TableHeadersRow();

				U32 l_TableIndex = 0;
				ImGui::TableNextRow();
				// Param Name
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);

					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_ParamInfo->m_Name.c_str());
					ImGui::PopItemWidth();

				}

				// DataType
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text(l_ParamInfo->getDataTypeStr().c_str());
					ImGui::PopItemWidth();
				}

				// UseRef
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Checkbox((l_ParamPerfixStr + "_UseRef").c_str(), &l_ParamInfo->m_UseRef);
					ImGui::PopItemWidth();
				}

				// RefKey
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::InputText((l_ParamPerfixStr + "_RefKey").c_str(), &l_ParamInfo->m_RefKey, ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
				}

				//Del
				if (i >= l_pTemplateParamInfoList->paramInfoList().size()) {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushID((l_ParamPerfixStr + "_Del").c_str());
					if (ImGui::SmallButton("Del"))
					{
						l_IsDelete = true;
						l_CurSelIndex = i - l_pTemplateParamInfoList->paramInfoList().size();
					}
					ImGui::PopID();
				}
				else {
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					ImGui::PushID((l_ParamPerfixStr + "_Del").c_str());
					if (ImGui::SmallButton("Del"))
					{
						l_IsDelete = false;
					}
					ImGui::PopID();
					ImGui::PopStyleVar();
				}
				ImGui::EndTable();
				ImGui::PopStyleColor();
			}
			if (l_IsDelete)
			{
				l_IsDelete = false;
				l_pNodeInfo->delParam2(l_CurSelIndex, &m_NodeManager);
			}

			//ParamData
			auto l_ParamDataTableNum = 2;
			if (l_ParamInfo->useFileBrowser())
				l_ParamDataTableNum = 3;
			if (ImGui::BeginTable((std::string("ParamData") + l_PrefixStr + std::to_string(i)).c_str(), l_ParamDataTableNum, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
			{
				ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
				if(l_ParamDataTableNum >= 3)
					ImGui::TableSetupColumn("Button", ImGuiTableColumnFlags_WidthFixed);

				U32 l_TableIndex = 0;
				ImGui::TableNextRow();

				// Data
				{
					ImGui::TableSetColumnIndex(l_TableIndex++);
					ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
					ImGui::Text("Data");
					ImGui::PopItemWidth();
				}

				// Value
				ImGui::TableSetColumnIndex(l_TableIndex++);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
				auto& l_ParamData = l_ParamInfo->getData();
				switch (l_ParamInfo->getDataType())
				{
				case ParamDataType::Int:
				{
					S32 l_Data = std::get<S32>(l_ParamData.value());
					if (l_ParamInfo->useSlider())
						ImGui::SliderInt((l_ParamPerfixStr + "_Data").c_str(), &l_Data, l_ParamInfo->getIntMin(), l_ParamInfo->getIntMax());
					else
						ImGui::InputInt((l_ParamPerfixStr + "_Data").c_str(), &l_Data);
					l_ParamData.value() = l_Data;
				}
					break;
				case ParamDataType::Bool:
				{
					Bool l_Data = std::get<Bool>(l_ParamData.value());
					if(l_Data)
						ImGui::Checkbox(("True" + l_ParamPerfixStr + "_Data").c_str(), &l_Data);
					else
						ImGui::Checkbox(("False" + l_ParamPerfixStr + "_Data").c_str(), &l_Data);
					l_ParamData.value() = l_Data;
				}
					break;
				case ParamDataType::String:
				{
					std::string l_Data = std::get<std::string>(l_ParamData.value());
					if (!l_ParamInfo->useFileBrowser())
						ImGui::InputText((l_ParamPerfixStr + "_Data").c_str(), &l_Data);
					else
					{
						if (l_ParamInfo->getWidgetType() == WidgetTypeString::File)
						{
							ImGui::Text(l_Data.c_str());

						}
						else if (l_ParamInfo->getWidgetType() == WidgetTypeString::Dir)
						{
							ImGui::Text(l_Data.c_str());
						}

						// Button
						ImGui::TableSetColumnIndex(l_TableIndex++);
						ImGui::PushID((l_ParamPerfixStr + "_Sel").c_str());
						if (ImGui::SmallButton("..."))
						{
							if (l_ParamInfo->getWidgetType() == WidgetTypeString::File)
							{
								m_SelPath.first = l_ParamPerfixStr;
								m_SelPath.second = l_Data;
								m_SelFileDialog.Open();
							}
							else if (l_ParamInfo->getWidgetType() == WidgetTypeString::Dir)
							{
								m_SelPath.first = l_ParamPerfixStr;
								m_SelPath.second = l_Data;
								m_SelDirDialog.Open();
							}
						}
						ImGui::PopID();

						if (m_SelPath.first == l_ParamPerfixStr && !m_SelPath.second.empty())
						{
							l_Data = m_SelPath.second;
							m_SelPath.second.clear();
						}
					}

					l_ParamData.value() = l_Data;
				}
					break;
				case ParamDataType::WString:
				{
					//std::wstring l_WStrData = std::get<std::wstring>(l_ParamData.value());
					//ImGui::InputText((l_ParamPerfixStr + "_Data").c_str(), &l_WStrData);
					//l_ParamData.value() = l_WStrData;
				}
				break;
				case ParamDataType::Float:
				{
					F32 l_Data = std::get<F32>(l_ParamData.value());
					if(l_ParamInfo->useSlider())
						ImGui::SliderFloat((l_ParamPerfixStr + "_Data").c_str(), &l_Data, l_ParamInfo->getFloatMin(), l_ParamInfo->getFloatMax());
					else
						ImGui::InputFloat((l_ParamPerfixStr + "_Data").c_str(), &l_Data);
					l_ParamData.value() = l_Data;
				}
				break;
				case ParamDataType::Vector2f:
				{
					auto l_Data = std::get<Vector2f>(l_ParamData.value());
					ImGui::InputFloat2((l_ParamPerfixStr + "_Data").c_str(), l_Data.data());
					l_ParamData.value() = l_Data;
				}
				break;
				case ParamDataType::Vector3f:
				{
					auto l_Data = std::get<Vector3f>(l_ParamData.value());
					if (l_ParamInfo->useColorPick())
						ImGui::ColorEdit3((l_ParamPerfixStr + "_Data").c_str(), l_Data.data());
					else
						ImGui::InputFloat3((l_ParamPerfixStr + "_Data").c_str(), l_Data.data());
					l_ParamData.value() = l_Data;
				}
				break;
				case ParamDataType::Vector4f:
				{
					auto l_Data = std::get<Vector4f>(l_ParamData.value());
					if(l_ParamInfo->useColorPick())
						ImGui::ColorEdit4((l_ParamPerfixStr + "_Data").c_str(), l_Data.data());
					else
						ImGui::InputFloat4((l_ParamPerfixStr + "_Data").c_str(), l_Data.data());
					l_ParamData.value() = l_Data;
				}
				break;
				case ParamDataType::Matrix2f:
				{
					auto l_Data = std::get<Matrix2f>(l_ParamData.value());
					ImGui::InputFloat2((l_ParamPerfixStr + "_Data0").c_str(), l_Data.row(0).data());
					ImGui::InputFloat2((l_ParamPerfixStr + "_Data1").c_str(), l_Data.row(1).data());
					l_ParamData.value() = l_Data;
				}
				break;
				case ParamDataType::Matrix3f:
				{
					auto l_Data = std::get<Matrix3f>(l_ParamData.value());
					ImGui::InputFloat3((l_ParamPerfixStr + "_Data0").c_str(), l_Data.row(0).data());
					ImGui::InputFloat3((l_ParamPerfixStr + "_Data1").c_str(), l_Data.row(1).data());
					ImGui::InputFloat3((l_ParamPerfixStr + "_Data2").c_str(), l_Data.row(2).data());
					l_ParamData.value() = l_Data;
				}
				break;
				case ParamDataType::Matrix4f:
				{
					auto l_Data = std::get<Matrix4f>(l_ParamData.value());
					ImGui::InputFloat4((l_ParamPerfixStr + "_Data0").c_str(), l_Data.row(0).data());
					ImGui::InputFloat4((l_ParamPerfixStr + "_Data1").c_str(), l_Data.row(1).data());
					ImGui::InputFloat4((l_ParamPerfixStr + "_Data2").c_str(), l_Data.row(2).data());
					ImGui::InputFloat4((l_ParamPerfixStr + "_Data3").c_str(), l_Data.row(3).data());
					l_ParamData.value() = l_Data;
				}
				break;
				case ParamDataType::RCFormat :
				{
					auto l_Data = std::get<EnumType>(l_ParamData.value());
					static S32 l_Value = static_cast<S32>(l_Data.m_Value);
					ImGui::Combo((l_ParamPerfixStr + "_Data").c_str(), &l_Value, l_Data.enumAllTypeStr().c_str(), 5);
					l_Data.m_Value = l_Value;
					l_ParamData.value() = l_Data;
				}
				break;
				case ParamDataType::VkBufferUsage:
				{
					auto l_Data = std::get<EnumType>(l_ParamData.value());
					std::string l_DataStr = l_Data.enumValueStr();
					ImGui::InputText((l_ParamPerfixStr + "_Data").c_str(), &l_DataStr);
					l_Data.setEnumValue(l_DataStr);
					l_ParamData.value() = l_Data;
				}
				break;
				case ParamDataType::VkImageUsage:
				{
					auto l_Data = std::get<EnumType>(l_ParamData.value());
					std::string l_DataStr = l_Data.enumValueStr();
					ImGui::InputText((l_ParamPerfixStr + "_Data").c_str(), &l_DataStr);
					l_Data.setEnumValue(l_DataStr);
					l_ParamData.value() = l_Data;
				}
				break;
				default:
					break;
				}

				ImGui::EndTable();
			}
		}
	}
}

void Editor::nodeTaskEU(NodeID i_NodeID) noexcept
{
	std::shared_ptr<NodeInfo> l_pNodeInfo = m_NodeManager.nodeInfo(i_NodeID);
	assert(l_pNodeInfo);
	std::shared_ptr<TaskNodeInfo> l_pTaskNodeInfo = std::dynamic_pointer_cast<TaskNodeInfo>(l_pNodeInfo);
	assert(l_pTaskNodeInfo);
	auto l_TaskEU = l_pTaskNodeInfo->getQueueFamilyType();

	std::string l_PrefixStr = "##" + std::to_string(i_NodeID) + "_TaskEU";

	if (ImGui::BeginTable((std::string("EU") + "##" + l_PrefixStr).c_str(), 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupColumn("Name",  ImGuiTableFlags_SizingMask_ | ImGuiTableColumnFlags_NoHeaderWidth, 150);
		ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHeaderWidth);
		//ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		auto l_TableIndex = 0;
		// Name
		{
			ImGui::TableSetColumnIndex(l_TableIndex++);
			//ImGui::Text(l_TaskEU.enumNameStr().c_str());
			ImGui::Text("TaskEU");
		}

		// Data
		if(l_TaskEU.m_Name != EnumName::TaskBlock)
		{
			static S32 l_EUValue = static_cast<S32>(l_TaskEU.m_Value);
			ImGui::TableSetColumnIndex(l_TableIndex++);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
			ImGui::Combo((l_PrefixStr + "_EU").c_str(), &l_EUValue, l_TaskEU.enumAllTypeStr().c_str(), 5);
			ImGui::PopItemWidth();
			l_TaskEU.m_Value = l_EUValue;
		}
		else
		{
			ImGui::TableSetColumnIndex(l_TableIndex++);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(l_TableIndex));
			ImGui::Text(l_TaskEU.enumValueStr().c_str());
			ImGui::PopItemWidth();
		}

		l_pTaskNodeInfo->setQueueFamilyType(l_TaskEU);
		ImGui::EndTable();
	}
}

template <Bool IsConnection>
ax::Drawing::IconType Editor::getNodeIconType(std::shared_ptr<NodeInfo> i_pNodeInfo) noexcept
{
	if (IsConnection)
		return ax::Drawing::IconType::Flow;

	if (i_pNodeInfo->getNodeType() == NodeType::Start || i_pNodeInfo->getNodeType() == NodeType::End || i_pNodeInfo->getNodeType() == NodeType::Composite)
		return ax::Drawing::IconType::RoundSquare;
	else if (i_pNodeInfo->isTaskNode())
		return ax::Drawing::IconType::Square;
	else
		return ax::Drawing::IconType::Circle;
}

template <Bool IsInput, Bool IsInputOutput>
void inline Editor::drawPin(const PinIDList& i_PinIDList, ax::Drawing::IconType i_IconType, ax::NodeEditor::Utilities::BlueprintNodeBuilder* i_NodeBuilder) noexcept
{
	for (const auto& l_PinID : i_PinIDList.pinIDList())
	{
		std::shared_ptr<PinInfo> l_pPinInfo = m_NodeManager.pinInfo(l_PinID);
		if (!l_pPinInfo)
		{
			assert(false);
			continue;
		}

		auto l_Alpha = ImGui::GetStyle().Alpha;
		if (m_NewLinkPinID && m_NodeManager.canAddLink(l_PinID, m_NewLinkPinID) != LinkType::Success && l_PinID != m_NewLinkPinID)
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
			l_pCurTaskTagInfo->m_VisibleRect.yMin = ax::NodeEditor::GetNavigateVisibleRect().Min.y;
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

				NodeID l_ParentNodeID = m_NodeManager.curParentNodeID();
				auto l_pPrototypeNode = m_NodeManager.nodePrototypeMap()[l_NodeName];
				auto l_pNodeInfo = m_NodeManager.instanceNodeInfo(l_NodeName, l_ParentNodeID);
				if ((l_pNodeInfo->getNodeType() == NodeType::Composite || l_pNodeInfo->isTaskNode()) && !std::dynamic_pointer_cast<FavoriteCompositeNodePrototype>(l_pPrototypeNode))
				{
					l_pNodeInfo->createChildNode(&m_NodeManager);
				}
				//m_NodeManager.updateCurInfo();

				ax::NodeEditor::SetNodePosition(l_pNodeInfo->getNodeID(), l_FixedPos);
			}
			ImGui::EndDragDropTarget();
		}

		// draw nodes
		for (auto l_CurNodeID : m_NodeManager.curSubRCInfo()->m_NodeIDList)
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
					//if (l_pCurNodeInfo->getNodeType() == NodeType::CPU || l_pCurNodeInfo->getNodeType() == NodeType::GPU)
					drawPin<true, true>(l_pCurNodeInfo->templateInputOutputInputPinIDList(), getNodeIconType<true>(l_pCurNodeInfo), &l_Builder);
					drawPin<true, false>(l_pCurNodeInfo->templateInputPinIDList(), getNodeIconType<false>(l_pCurNodeInfo), &l_Builder);

					drawPin <false, true> (l_pCurNodeInfo->templateInputOutputOutputPinIDList(), getNodeIconType<true>(l_pCurNodeInfo), &l_Builder);
					drawPin<false, false>(l_pCurNodeInfo->templateOutputPinIDList(), getNodeIconType<false>(l_pCurNodeInfo), &l_Builder);
					l_Builder.EndTemplatePin();

				}

				{
					l_Builder.NormalPin();
					drawPin<true, true>(l_pCurNodeInfo->inputOutputInputPinIDList(), getNodeIconType<true>(l_pCurNodeInfo), &l_Builder);
					drawPin<true, false>(l_pCurNodeInfo->inputPinIDList(), getNodeIconType<false>(l_pCurNodeInfo), &l_Builder);

					drawPin <false, true>(l_pCurNodeInfo->inputOutputOutputPinIDList(), getNodeIconType<true>(l_pCurNodeInfo), &l_Builder);
					drawPin<false, false>(l_pCurNodeInfo->outputPinIDList(), getNodeIconType<false>(l_pCurNodeInfo), &l_Builder);
					l_Builder.EndNormalPin();
				}

				l_Builder.End();
			}
		}
	}

	for (auto l_LinkID : m_NodeManager.curSubRCInfo()->m_LinkIDList)
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

			auto l_LinkType = m_NodeManager.canAddLink(l_pStartPinInfo->getPinID(), l_pEndPinInfo->getPinID());
			switch (l_LinkType)
			{
			case rceditor::LinkType::Success:
			{
				drawLable("+ Create Link", ImColor(32, 45, 32, 180));
				if (ax::NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
				{
					m_NodeManager.addLinkInfo(l_StartPinID.AsU64(), l_EndPinID.AsU64());
				}
			}
				break;
			case rceditor::LinkType::SamePin:
				ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				break;
			case rceditor::LinkType::SameNode:
			{
				drawLable("x Cannot connect to self", ImColor(45, 32, 32, 180));
				ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
			}
				break;
			case rceditor::LinkType::PinTypeDifferent:
			{
				drawLable("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
				ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
			}
				break;
			case rceditor::LinkType::DataTypeDifferent:
			{
				drawLable("x Incompatible Pin Data Type", ImColor(45, 32, 32, 180));
				ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
			}
				break;
			case rceditor::LinkType::MetaDataDifferent:
			{
				drawLable("x Incompatible Pin MetaData", ImColor(45, 32, 32, 180));
				ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
			}
				break;
			case rceditor::LinkType::PinAccessTypeError:
			{
				drawLable("x Incompatible Pin Access Type", ImColor(45, 32, 32, 180));
				ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
			}
				break;
			case rceditor::LinkType::TaskPinAccessTypeError:
			{
				drawLable("x Incompatible Task Pin Access Type", ImColor(45, 32, 32, 180));
				ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
			}
				break;
			default:
				break;
			}

			//if (l_pStartPinInfo && l_pEndPinInfo)
			//{
			//	if (l_StartPinID == l_EndPinID)
			//	{
			//		ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
			//	}
			//	else if (l_pStartPinInfo->getPinType() == l_pEndPinInfo->getPinType())
			//	{
			//		drawLable("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
			//		ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
			//	}
			//	else if (l_pStartPinInfo->getNodeID() == l_pEndPinInfo->getNodeID())
			//	{
			//		drawLable("x Cannot connect to self", ImColor(45, 32, 32, 180));
			//		ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
			//	}
			//	else if (l_pStartPinInfo->getDataType() != l_pEndPinInfo->getDataType())
			//	{
			//		drawLable("x Incompatible Pin Data Type", ImColor(45, 32, 32, 180));
			//		ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
			//	}
			//	else if (l_pEndPinInfo->getPinAccessType() != PinAccessType::Writable && l_pStartPinInfo->getPinAccessType() != l_pEndPinInfo->getPinAccessType())
			//	{
			//		drawLable("x Incompatible Pin Access Type", ImColor(45, 32, 32, 180));
			//		ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
			//	}
			//	else if (!l_pEndPinInfo->getMetaData().empty() && (l_pStartPinInfo->getMetaData() != l_pEndPinInfo->getMetaData()))
			//	{
			//		drawLable("x Incompatible Pin MetaData", ImColor(45, 32, 32, 180));
			//		ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
			//	}
			//	else
			//	{
			//		drawLable("+ Create Link", ImColor(32, 45, 32, 180));
			//		if (ax::NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
			//		{
			//			m_NodeManager.addLinkInfo(l_StartPinID.AsU64(), l_EndPinID.AsU64());
			//			//m_NodeManager.updateCurInfo();
			//		}
			//	}
			//}
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
	if (!l_pNodeInfo || (l_pNodeInfo->getNodeType() != NodeType::Composite && !l_pNodeInfo->isTaskNode())) return;

	const auto& io = ImGui::GetIO();
	if (m_SelectedNodeID == ax::NodeEditor::GetHoveredNode().AsU64())
	{
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (l_pNodeInfo->isTaskNode()) {
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
			ImGui::Text("NodeID: %s", std::to_string(l_pNodeInfo->getNodeID()).c_str());
			ImGui::Text("Template: %s", l_pNodeInfo->getTemplateName().c_str());
			//ImGui::Text("Class: %s", l_pNodeInfo->getClassName().c_str());
		}
		else
			ImGui::Text("Unknown node: %p", m_ContextNodeID.AsPointer());

		if (l_pNodeInfo && l_pNodeInfo->getNodeType() == NodeType::PS || l_pNodeInfo->getNodeType() == NodeType::CS)
		{
			if (ImGui::MenuItem("Generate Shader Head"))
			{
				generateShaderHead();
			}
		}

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
	NodeID l_ParentNodeID = m_NodeManager.curParentNodeID();
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

		NodeTag l_NodeTag = NodeTag::NoTag;
		if (m_NodeManager.curParentNodeID() != 0)
		{
			auto l_pParentNodeInfo = m_NodeManager.nodeInfo(m_NodeManager.curParentNodeID());
			if (l_pParentNodeInfo)
			{
				if (l_pParentNodeInfo->isTaskNode())
				{
					l_NodeTag = l_pParentNodeInfo->getNodeTag();
				}
				else
				{
					auto l_pParentTaskNodeInfo = m_NodeManager.nodeInfo(l_pParentNodeInfo->getTaskNodeID());
					if (l_pParentTaskNodeInfo)
						l_NodeTag = l_pParentTaskNodeInfo->getNodeTag();
				}
			}
		}

		for (auto& l_Pair : m_NodeManager.nodePrototypeMap())
		{
			auto& l_NodeName = l_Pair.first;
			if(filterNodePrototype(l_NodeName, toLower(l_Buffer), l_NodeTag, l_ParentNodeID == 0))
				l_NodeList.emplace_back(l_NodeName);

			//if (l_NodeNameLower.find(l_InputLower) != std::string::npos)
			//{
			//	auto l_pNodePrototypeInfo = m_NodeManager.nodePrototypeMap().at(l_NodeName);
			//	if(!l_pNodePrototypeInfo) continue;
			//	if (m_ParentNodeID == 0 && l_pNodePrototypeInfo->isTaskNode())
			//		l_NodeList.emplace_back(l_NodeName);
			//	else if (m_ParentNodeID != 0 && !l_pNodePrototypeInfo->isTaskNode())
			//		l_NodeList.emplace_back(l_NodeName);
			//}
		}

		for (auto& l_NodeName : l_NodeList)
		{
			if (ImGui::MenuItem(l_NodeName.c_str()))
			{
				auto l_pNodeInfo = m_NodeManager.instanceNodeInfo(l_NodeName, l_ParentNodeID);
				auto l_pNodePrototypeInfo = m_NodeManager.nodePrototypeMap().at(l_NodeName);
				if(!l_pNodeInfo || !l_pNodePrototypeInfo) continue;

				if ((l_pNodeInfo->getNodeType() == NodeType::Composite || l_pNodeInfo->isTaskNode()) && !std::dynamic_pointer_cast<FavoriteCompositeNodePrototype>(l_pNodePrototypeInfo))
				{
					l_pNodeInfo->createChildNode(&m_NodeManager);
				}
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

Bool Editor::filterNodePrototype(std::string_view i_NodePrototypeName, std::string_view i_SearchStr, NodeTag i_NodeTag, Bool i_IsTask)
{
	auto l_pNodePrototypeInfo = m_NodeManager.nodePrototypeMap().at(i_NodePrototypeName.data());
	if (!l_pNodePrototypeInfo) return false;
	if (!i_IsTask && !l_pNodePrototypeInfo->isTaskNode())
	{
		auto l_pTemplateNodeInfo = std::dynamic_pointer_cast<TemplateNodePrototype>(m_NodeManager.nodePrototypeMap().at(l_pNodePrototypeInfo->getTemplateName()));
		if (!l_pTemplateNodeInfo) return false;

		if (!(static_cast<U64>(l_pTemplateNodeInfo->getNodeTag()) & static_cast<U64>(i_NodeTag)))
			return false;
	}

	if (i_SearchStr.empty()) return true;

	std::string l_NodeNameLower = toLower(i_NodePrototypeName.data());
	if (l_NodeNameLower.find(i_SearchStr) != std::string::npos)
		return true;
	
	return false;
}

void Editor::initWorkDir() noexcept
{
	m_NodeManager.loadPrototype(m_EditorSetting.m_PrototypePath);

	m_SaveFileDialog.SetPwd(m_EditorSetting.m_RCDataPath);
	m_LoadFileDialog.SetPwd(m_EditorSetting.m_RCDataPath);
	m_SaveFavoriteDialog.SetPwd(m_EditorSetting.m_FavoritePath);
	m_SelFileDialog.SetPwd(m_EditorSetting.m_WorkingDir);
	m_SelDirDialog.SetPwd(m_EditorSetting.m_WorkingDir);
}

void Editor::generateShaderHead() noexcept
{
	auto hasParamInfo = [](const std::shared_ptr<NodeInfo> i_pNodeInfo, std::string i_ParamName) -> Bool {
		return i_pNodeInfo->hasParamKey(i_ParamName);
	};

	auto findParamInfo = [](const std::shared_ptr<NodeInfo> i_pNodeInfo, std::string i_ParamName) -> ParamInfo {
		Bool l_IsFind = false;
		ParamInfo l_Result{};

		for (const auto& l_TempParamInfo : i_pNodeInfo->templateParamList().paramInfoList())
		{
			if (i_ParamName == l_TempParamInfo.getName())
			{
				l_IsFind = true;
				l_Result = l_TempParamInfo;
			}
		}

		if (!l_IsFind)
		{
			for (const auto& l_TempParamInfo : i_pNodeInfo->paramList().paramInfoList())
			{
				if (i_ParamName == l_TempParamInfo.getName())
				{
					l_IsFind = true;
					l_Result = l_TempParamInfo;
				}
			}
		}

		return l_Result;
	};

	auto getBindingTexture2DStr = [](U32 i_BindingIndex, std::string i_NameStr, U32 i_SetIndex = 0) -> std::string {
		char l_Buffer[100];
		sprintf_s(l_Buffer, 100, "layout(set = %d, binding = %d) uniform texture2D %s;\n", i_SetIndex, i_BindingIndex, i_NameStr.c_str());
		return l_Buffer;
	};

	auto getBindingSampler2DStr = [](U32 i_BindingIndex, std::string i_NameStr, U32 i_SetIndex = 0) -> std::string {
		char l_Buffer[100];
		sprintf_s(l_Buffer, 100, "layout(set = %d, binding = %d) uniform sampler2D %s;\n", i_SetIndex, i_BindingIndex, i_NameStr.c_str());
		return l_Buffer;
	};

	auto getBindingImage2DStr = [](U32 i_BindingIndex, std::string i_NameStr, std::string i_Format, U32 i_SetIndex = 0) -> std::string {
		char l_Buffer[100];
		sprintf_s(l_Buffer, 100, "layout(set = %d, binding = %d, %s) uniform image2D %s;\n", i_SetIndex, i_BindingIndex, i_Format.c_str(), i_NameStr.c_str());
		return l_Buffer;
	};

	auto getLocationStr = [](U32 i_BindingIndex, std::string i_NameStr) -> std::string {
		char l_Buffer[100];
		sprintf_s(l_Buffer, 100, "layout(location = %d) out vec4 o_%s;\n", i_BindingIndex, i_NameStr.c_str());
		return l_Buffer;
	};

	auto getBindingUniformBufferStr = [](U32 i_BindingIndex, std::string i_NameStr, std::string i_StructStr, U32 i_SetIndex = 0) -> std::string {
		char l_Buffer[1024];
		std::string l_BindingStr = R"(layout(set = %d, binding = %d) uniform %s_struct 
{ 
	%s
}%s;
)";

		sprintf_s(l_Buffer, 1024, l_BindingStr.c_str(), i_SetIndex, i_BindingIndex, i_NameStr.c_str(), i_StructStr.c_str(), i_NameStr.c_str());
		return l_Buffer;
	};

	auto getBindingUniformTexelBufferStr = [](U32 i_BindingIndex, std::string i_NameStr, U32 i_SetIndex = 0) -> std::string {
		char l_Buffer[100];
		sprintf_s(l_Buffer, 100, "layout(set = %d, binding = %d) uniform samplerBuffer %s;\n", i_SetIndex, i_BindingIndex, i_NameStr.c_str());
		return l_Buffer;
	};

	auto getBindingStorageBufferStr = [](U32 i_BindingIndex, std::string i_NameStr, std::string i_StructStr, U32 i_SetIndex = 0) -> std::string {
		char l_Buffer[1024];
		std::string l_BindingStr = R"(layout(set = %d, binding = %d) buffer %s_struct 
{ 
	%s
};
)";

		sprintf_s(l_Buffer, 1024, l_BindingStr.c_str(), i_SetIndex, i_BindingIndex, i_NameStr.c_str(), i_StructStr.c_str());
		return l_Buffer;
	};

	auto getBindingStorageTexelBufferStr = [](U32 i_BindingIndex, std::string i_NameStr, std::string i_Format, U32 i_SetIndex = 0) -> std::string {
		char l_Buffer[100];
		sprintf_s(l_Buffer, 100, "layout(set = %d, binding = %d, %s) imageBuffer %s;\n", i_SetIndex, i_BindingIndex, i_Format.c_str(), i_NameStr.c_str());
		return l_Buffer;
	};

	auto l_pNodeInfo = m_NodeManager.nodeInfo(m_ContextNodeID.AsU64());
	if(!l_pNodeInfo) return;

	std::string l_ShaderStr = "#version 460\n";
	U32 l_BindingIndex = 0;
	U32 l_LocationInIndex = 0;
	U32 l_LocationOutIndex = 0;
	for (auto l_Pair : l_pNodeInfo->pinKeyMap())
	{
		Bool l_IsInputOutput = (l_Pair.second.first == l_Pair.second.second);
		std::shared_ptr<PinInfo> l_pPinInfo{};
		if(l_Pair.second.first > 0)
			l_pPinInfo = m_NodeManager.pinInfo(l_Pair.second.first);
		else if (l_Pair.second.second > 0)
			l_pPinInfo = m_NodeManager.pinInfo(l_Pair.second.second);
		if (!l_pPinInfo) continue;

		if (l_pPinInfo->getDataTypeValue() == EditorDataType::Image2D)
		{
			std::string l_UsageKey = l_pPinInfo->getName() + ".usage";
			std::string l_FormatKey = l_pPinInfo->getName() + ".format";
			std::string l_SamplerKey = l_pPinInfo->getName() + ".sampler";
			//std::string l_AccessKey = l_pPinInfo->getName() + ".access";
		
			VkImageUsage l_Usage = VkImageUsage::VK_IMAGE_USAGE_SAMPLED_BIT;
			if (hasParamInfo(l_pNodeInfo, l_UsageKey))
			{
				auto l_ParamInfo = findParamInfo(l_pNodeInfo, l_UsageKey);
				l_Usage = static_cast<VkImageUsage>(std::get<EnumType>(l_ParamInfo.getData().value()).m_Value);
			}

			std::string l_FormatStr{};
			if (hasParamInfo(l_pNodeInfo, l_FormatKey))
			{
				auto l_ParamInfo = findParamInfo(l_pNodeInfo, l_FormatKey);
				l_FormatStr = std::get<std::string>(l_ParamInfo.getData().value());
			}

			Bool l_HasSampler = false;
			if (hasParamInfo(l_pNodeInfo, l_SamplerKey))
			{
				auto l_ParamInfo = findParamInfo(l_pNodeInfo, l_SamplerKey);
				l_FormatStr = std::get<Bool>(l_ParamInfo.getData().value());
			}
		
			if (l_Usage == (l_Usage & VkImageUsage::VK_IMAGE_USAGE_SAMPLED_BIT))
			{
				if(!l_HasSampler)
					l_ShaderStr += getBindingTexture2DStr(l_BindingIndex, l_pPinInfo->getName());
				else
					l_ShaderStr += getBindingTexture2DStr(l_BindingIndex, l_pPinInfo->getName());
				l_BindingIndex++;
			}
			else if (l_Usage == (l_Usage & VkImageUsage::VK_IMAGE_USAGE_STORAGE_BIT))
			{
				l_ShaderStr += getBindingImage2DStr(l_BindingIndex, l_FormatStr, l_pPinInfo->getName());
				l_BindingIndex++;
			}
			else if (l_Usage == (l_Usage & VkImageUsage::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT))
			{
				l_ShaderStr += getLocationStr(l_LocationOutIndex, l_pPinInfo->getName());
				l_LocationOutIndex++;
			}
		}

		else if (l_pPinInfo->getDataTypeValue() == EditorDataType::Buffer)
		{
			std::string l_UsageKey = l_pPinInfo->getName() + ".usage";
			std::string l_FormatKey = l_pPinInfo->getName() + ".format";
			std::string l_StrucrKey = l_pPinInfo->getName() + ".struct";

			VkBufferUsage l_Usage = VkBufferUsage::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			if (hasParamInfo(l_pNodeInfo, l_UsageKey))
			{
				auto l_ParamInfo = findParamInfo(l_pNodeInfo, l_UsageKey);
				l_Usage = static_cast<VkBufferUsage>(std::get<EnumType>(l_ParamInfo.getData().value()).m_Value);
			}

			std::string l_FormatStr{};
			if (hasParamInfo(l_pNodeInfo, l_FormatKey))
			{
				auto l_ParamInfo = findParamInfo(l_pNodeInfo, l_FormatKey);
				l_FormatStr = std::get<std::string>(l_ParamInfo.getData().value());
			}

			std::string l_StructStr{};
			if (hasParamInfo(l_pNodeInfo, l_StrucrKey))
			{
				auto l_ParamInfo = findParamInfo(l_pNodeInfo, l_StrucrKey);
				l_StructStr = std::get<std::string>(l_ParamInfo.getData().value());
			}
			
			if (l_Usage == (l_Usage & VkBufferUsage::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
			{
				l_ShaderStr += getBindingUniformBufferStr(l_BindingIndex, l_pPinInfo->getName(), l_StructStr);
				l_BindingIndex++;
			}
			else if (l_Usage == (l_Usage & VkBufferUsage::VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT))
			{
				l_ShaderStr += getBindingUniformTexelBufferStr(l_BindingIndex, l_pPinInfo->getName());
				l_BindingIndex++;
			}
			else if (l_Usage == (l_Usage & VkBufferUsage::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT))
			{
				l_ShaderStr += getBindingStorageBufferStr(l_BindingIndex, l_pPinInfo->getName(), l_StructStr);
				l_BindingIndex++;
			}
			else if (l_Usage == (l_Usage & VkBufferUsage::VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT))
			{
				l_ShaderStr += getBindingStorageTexelBufferStr(l_BindingIndex, l_pPinInfo->getName(), l_FormatStr);
				l_BindingIndex++;
			}
		}	
	}

	//l_ShaderStr += "\n\0";
	SetClipboardText(l_ShaderStr);
}

void Editor::SetClipboardText(const std::string& text) {
	if (!OpenClipboard(nullptr)) {
		return;
	}

	EmptyClipboard();

	HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
	if (!hGlob) {
		CloseClipboard();
		return;
	}

	memcpy(GlobalLock(hGlob), text.c_str(), text.size() + 1);
	GlobalUnlock(hGlob);

	SetClipboardData(CF_TEXT, hGlob);

	CloseClipboard();
}

void Editor::EditorSetting::loadSetting()
{
	tinyxml2::XMLDocument l_XmlDocument{};
	tinyxml2::XMLError l_Error = l_XmlDocument.LoadFile("Setttings.xml");
	assert(l_Error == tinyxml2::XML_SUCCESS);

	std::shared_ptr<NodePrototype> l_pNodePrototype{};
	tinyxml2::XMLElement* l_XmlSettings = l_XmlDocument.RootElement();

	if (l_XmlSettings->FirstChildElement("WorkingDir"))
	{
		const char* l_Str = l_XmlSettings->FirstChildElement("WorkingDir")->GetText();
		if (l_Str)
		{
			setWorkingDir(l_Str);
		}
	}
}

void Editor::EditorSetting::saveSetting()
{
	tinyxml2::XMLDocument l_XmlDocument{};
	tinyxml2::XMLElement* l_XmlSettings = l_XmlDocument.NewElement("Settings");
	tinyxml2::XMLElement* l_XmlWorkingDir = l_XmlSettings->InsertNewChildElement("WorkingDir");
	l_XmlWorkingDir->InsertFirstChild(l_XmlDocument.NewText(m_WorkingDir.c_str()));
	l_XmlDocument.InsertEndChild(l_XmlSettings);
	l_XmlDocument.SaveFile("Setttings.xml");
}

RCEDITOR_NAMESPACE_END

int Main(int argc, char** argv)
{
	rceditor::Editor l_Edtior("Blueprints", argc, argv);

	if (l_Edtior.Create())
		return l_Edtior.Run();

	return 0;
}

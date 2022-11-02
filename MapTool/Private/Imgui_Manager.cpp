#include "stdafx.h"
#include "..\Public\Imgui_Manager.h"

#include "GameInstance.h"
//#include "PickingMgr.h"
//#include "BaseObj.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

// Helper to wire demo markers located in code to a interactive browser
//typedef void(*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
//
//extern ImGuiDemoMarkerCallback  GImGuiDemoMarkerCallback;
//
//ImGuiDemoMarkerCallback         GImGuiDemoMarkerCallback = NULL;
//
//#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)

CImgui_Manager::CImgui_Manager()
{
}

HRESULT CImgui_Manager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	Update_NewFrame();

	ShowGui();
}

void CImgui_Manager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::ShowGui()
{
	ImGui::Begin(u8"Editor", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
		| ImGuiWindowFlags_AlwaysVerticalScrollbar);

	Menu_Bar();
	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::GetIO().NavActive = false;
	ImGui::GetIO().WantCaptureMouse = true;
	if (m_bshow_demo_window)
		ImGui::ShowDemoWindow(&m_bshow_demo_window);
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Tool");                          // Create a window called "Hello, world!" and append into it.
		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &m_bshow_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &m_bshow_demo_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		
		ImGui::End();
	}
	ImGui::End();

}

void CImgui_Manager::Menu_Bar()
{


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
				m_bSave = true;
			ImGui::Separator();
			if (ImGui::MenuItem("Load"))
				m_bLoad = true;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("Show Mouse Pos", NULL, &m_bShowSimpleMousePos);
			ImGui::Separator();
			ImGui::MenuItem("Show Picked Object", NULL, &m_bShowPickedObject);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Style Editor", NULL, &m_bShow_app_style_editor);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	
}

void CImgui_Manager::Terrain_Map()
{
}

void CImgui_Manager::Update_NewFrame(void)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CImgui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

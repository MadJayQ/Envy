#pragma once

#include "subsystem.h"
#include "csgosdk.h"
#include "esp.h"

#include <memory>
#include <d3d9.h>

namespace Envy
{

	using oDrawModelExecuteFn = Function<void, THISCALL, _IVModelRender_*, IMatRenderContext*, void*, const ModelRenderInfo_t&, matrix3x4_t*>;

	//static constexpr const char* ChamsMaterial =
	//R"###("VertexLitGeneric"
	//{
	//	"$basetexture" "VGUI/white"
	//	"$envmap" "env_cubemap"
	//	"$ignorez"     "0"
	//	"$model"       "1"
	//	"$nocull"      "0"
	//	"$selfillum"   "1"
	//    "$normalmapalphaenvmapmask" 1
 //       "$envmapcontrast" 1
	//})###";

	static constexpr const char* ChamsMaterialRef =
	R"###("VertexLitGeneric"
	{
		"$baseTexture" 			"vgui/white"
		"$envmap"		"env_cubemap"
		"$ignorez"		"0"
		"$envmapfresnel" ".6"
		//"$basemapalphaenvmapmask" "1"
		"$normalmapalphaenvmapmask" 1

		"$phong" "1"
		"$phongexponent" 5
		"$phongboost" "1"
		"$phongfresnelranges" "[0 .5 1]"

		"$rimlight" 1
		"$rimlightexponent" 2
		"$rimlightboost" .2	
		"$alpha" 1
	})###";

	static constexpr const char* ChamsMaterialBackRef =
	R"###("VertexLitGeneric"
	{
		"$baseTexture" 			"vgui/white"
		"$envmap"		"env_cubemap"
		"$ignorez"		"1"
		"$envmapfresnel" ".6"
		//"$basemapalphaenvmapmask" "1"
		"$normalmapalphaenvmapmask" 1

		"$phong" "1"
		"$phongexponent" 5
		"$phongboost" "1"
		"$phongfresnelranges" "[0 .5 1]"

		"$rimlight" 1
		"$rimlightexponent" 2
		"$rimlightboost" .2	
		"$alpha" 1
	})###";
	static constexpr const char* ChamsMaterial =
		R"###("VertexLitGeneric"
	{
		"$baseTexture" 			"vgui/white"
		"$ignorez"		"0"
		"$phong" "1"
		"$phongexponent" 5
		"$phongboost" "1"
		"$phongfresnelranges" "[0 .5 1]"

		"$rimlight" 1
		"$rimlightexponent" 2
		"$rimlightboost" .2	
		"$alpha" 1
	})###";

	static constexpr const char* ChamsMaterialBack =
		R"###("VertexLitGeneric"
	{
		"$baseTexture" 			"vgui/white"
		"$ignorez"		"1"
		"$phong" "1"
		"$phongexponent" 5
		"$phongboost" "1"
		"$phongfresnelranges" "[0 .5 1]"

		"$rimlight" 1
		"$rimlightexponent" 2
		"$rimlightboost" .2	
		"$alpha" 1
	})###";

	class VisualSubsystem : public ICheatSubsystem
	{
	public:
		VisualSubsystem()
		{

		}
		virtual void Init() override;
		bool OnPaintTraverse(SourceEngine::vgui::VPANEL& panel, bool& forceRepaint, bool& allowForce);
		void OnPostScreenEffects();
		bool OnDrawModelExecute(SourceEngine::IMatRenderContext* context, void* state, const SourceEngine::ModelRenderInfo_t& model, SourceEngine::matrix3x4_t* boneToWorld, oDrawModelExecuteFn  oDME);
		void SetNextTurnTime(float nextTurnTime) { m_flNextTurnTime = nextTurnTime; }
		void SetAnimTime(float lastTurnTime) { m_flAnimNextTime = lastTurnTime; }

		void SetRealYaw(float yaw) { m_flRealYaw = yaw; }
		void SetFakeYaw(float yaw) { m_flFakeYaw = yaw; }

		void OverrideBoneMatrix(SourceEngine::C_BasePlayer* player, SourceEngine::matrix3x4_t* m) { m_ESP->OverrideBoneMatrix(player, m); }

	private:
		std::unique_ptr<ESP> m_ESP;

		float m_flNextTurnTime = 0.f;
		float m_flAnimNextTime = 0.f;
		float m_flRealYaw = 0.f;
		float m_flFakeYaw = 0.f;

		SourceEngine::IMaterial* m_pChamsMaterial;
		SourceEngine::IMaterial* m_pChamsMaterialBack;
		SourceEngine::IMaterial* m_pChamsMaterialRef;
		SourceEngine::IMaterial* m_pChamsMaterialBackRef;

	};
	extern IDirect3DDevice9* g_D3DDevice;
}
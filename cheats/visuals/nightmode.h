#pragma once

#include "..\..\includes.hpp"

class MaterialBackup //-V802
{ //-V802
public:
	MaterialHandle_t handle;
	IMaterial* material;
	float color[3];
	float alpha;
	bool translucent;
	bool nodraw;

	MaterialBackup(MaterialHandle_t h, IMaterial* p) 
	{
		handle = h;
		material = p;

		material->GetColorModulation(&color[0], &color[1], &color[2]);
		alpha = material->GetAlphaModulation();

		translucent = material->GetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT);
		nodraw = material->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW);
	}

	void restore() 
	{
		material->ColorModulate(color[0], color[1], color[2]);
		material->AlphaModulate(alpha);
		material->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, translucent);
		material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, nodraw);
	}
};

class nightmode : public singleton <nightmode> 
{
public:
	void clear_stored_materials();
	void modulate(MaterialHandle_t i, IMaterial *material, bool backup);
	void apply();
	void remove();
};
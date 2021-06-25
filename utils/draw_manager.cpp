// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\includes.hpp"
#include "draw_manager.h"

vgui::HFont fonts[FONT_MAX];

void render::set_alphafactor(float factor)
{
	alpha_factor = factor;
}

float render::get_alphafactor() 
{
	return alpha_factor;
}

void render::rect(int x, int y, int w, int h, Color color) 
{
	if (!m_surface())
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	m_surface()->DrawSetColor(color);
	m_surface()->DrawOutlinedRect(x, y, x + w, y + h);
}

void render::rect_filled(int x, int y, int w, int h, Color color)
{
	if (!m_surface())
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	m_surface()->DrawSetColor(color);
	m_surface()->DrawFilledRect(x, y, x + w, y + h);
}

void render::setup_states() const {
	this->device->SetVertexShader(nullptr);
	this->device->SetPixelShader(nullptr);
	this->device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	this->device->SetRenderState(D3DRS_LIGHTING, FALSE);
	this->device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	this->device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	this->device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	this->device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	this->device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	this->device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	this->device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	this->device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	this->device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	this->device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	this->device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	this->device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	this->device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	this->device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	this->device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	this->device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	this->device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	this->device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}

void render::invalidate_objects() 
{
	this->device = nullptr;
}

void render::restore_objects(LPDIRECT3DDEVICE9 device)
{
	this->device = device;
	this->device->GetViewport(&m_viewport);
}

void render::gradient(int x, int y, int w, int h, Color first, Color second, GradientType type) 
{
	if (!m_surface())
		return;

	first.SetAlpha(static_cast<int>(first.a() * alpha_factor));
	second.SetAlpha(static_cast<int>(second.a() * alpha_factor));

	auto filled_rect_fade = [&](bool reversed, float alpha) {
		using Fn = void(__thiscall*)(VOID*, int, int, int, int, unsigned int, unsigned int, bool);
		call_virtual< Fn >(m_surface(), g_ctx.indexes.at(17)) (
			m_surface(), x, y, x + w, y + h,
			reversed ? alpha : 0,
			reversed ? 0 : alpha,
			type == GRADIENT_HORIZONTAL);
	};

	static auto blend = [](const Color & first, const Color & second, float t) -> Color {
		return Color(
			first.r() + t * (second.r() - first.r()),
			first.g() + t * (second.g() - first.g()),
			first.b() + t * (second.b() - first.b()),
			first.a() + t * (second.a() - first.a()));
	};

	if (first.a() == 255 || second.a() == 255) {
		m_surface()->DrawSetColor(blend(first, second, 0.5f));
		m_surface()->DrawFilledRect(x, y, x + w, y + h);
	}

	m_surface()->DrawSetColor(first);
	filled_rect_fade(true, first.a());

	m_surface()->DrawSetColor(second);
	filled_rect_fade(false, second.a());
}

void render::rounded_box(int x, int y, int w, int h, int points, int radius, Color color) 
{
	if (!m_surface())
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	Vertex_t *round = new Vertex_t[4 * points]; //-V121

	for (int i = 0; i < 4; i++) {
		int _x = x + ((i < 2) ? (w - radius) : radius);
		int _y = y + ((i % 3) ? (h - radius) : radius);

		float a = 90.f * i;

		for (int j = 0; j < points; j++) {
			float _a = DEG2RAD(a + (j / (float)(points - 1)) * 90.f);

			round[(i * points) + j] = Vertex_t(Vector2D(_x + radius * sin(_a), _y - radius * cos(_a)));
		}
	}

	auto surface = m_surface();

	static int Texture = surface->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	surface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	surface->DrawSetColor(color);
	surface->DrawSetTexture(Texture);
	
	surface->DrawTexturedPolygon(4 * points, round);
}

void render::grid(int x, int y, int w, int h, Color first, Color second, Color third, GradientType type)
{
	if (!m_surface())
		return;

	first.SetAlpha(static_cast<int>(first.a() * alpha_factor));
	second.SetAlpha(static_cast<int>(second.a() * alpha_factor));
	third.SetAlpha(static_cast<int>(third.a() * alpha_factor));

	auto filled_rect_fade = [&](bool reversed, float alpha) {
		using Fn = void(__thiscall*)(VOID*, int, int, int, int, unsigned int, unsigned int, bool);
		call_virtual< Fn >(m_surface(), g_ctx.indexes.at(17)) (
			m_surface(), x, y, x + w, y + h,
			reversed ? alpha : 0,
			reversed ? 0 : alpha,
			type == GRADIENT_HORIZONTAL);
	};

	static auto blend = [](const Color & first, const Color & second, const Color & third, float t) -> Color {
		return Color(
			first.r() + t * (second.r() - third.r()),
			first.g() + t * (second.g() - third.g()),
			first.b() + t * (second.b() - third.b()),
			first.a() + t * (second.a() - third.a()));
	};

	if (first.a() == 255 || second.a() == 255) {
		m_surface()->DrawSetColor(blend(first, second, third, 0.5f));
		m_surface()->DrawFilledRect(x, y, x + w, y + h);
	}

	m_surface()->DrawSetColor(first); //-V807
	filled_rect_fade(true, first.a());

	m_surface()->DrawSetColor(second);
	filled_rect_fade(false, second.a());

	m_surface()->DrawSetColor(third);
	filled_rect_fade(false, third.a());
}

void render::circle(int x, int y, int points, int radius, Color color) 
{
	if (!m_surface())
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	static bool once = true;

	static std::vector<float> temppointsx;
	static std::vector<float> temppointsy;

	if (once)
	{
		float step = (float)DirectX::XM_PI * 2.0f / points;
		for (float a = 0; a < (DirectX::XM_PI * 2.0f); a += step) //-V1034
		{
			temppointsx.push_back(cosf(a));
			temppointsy.push_back(sinf(a));
		}
		once = false;
	}

	std::vector<int> pointsx;
	std::vector<int> pointsy;
	std::vector<Vertex_t> vertices;

	for (int i = 0; i < temppointsx.size(); i++)
	{
		float eeks = radius * temppointsx[i] + x;
		float why = radius * temppointsy[i] + y;
		pointsx.push_back(eeks);
		pointsy.push_back(why);

		vertices.emplace_back(Vertex_t(Vector2D(eeks, why)));
	}

	m_surface()->DrawSetColor(color);
	m_surface()->DrawPolyLine(pointsx.data(), pointsy.data(), points); // only if you want en extra outline
}

void render::circle_filled(int x, int y, int points, int radius, Color color) 
{
	if (!m_surface())
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	static bool once = true;

	static std::vector<float> temppointsx;
	static std::vector<float> temppointsy;

	if (once)
	{
		float step = (float)DirectX::XM_PI * 2.0f / points;
		for (float a = 0; a < (DirectX::XM_PI * 2.0f); a += step) //-V1034
		{
			temppointsx.push_back(cosf(a));
			temppointsy.push_back(sinf(a));
		}
		once = false;
	}

	std::vector<int> pointsx;
	std::vector<int> pointsy;
	std::vector<Vertex_t> vertices;

	for (int i = 0; i < temppointsx.size(); i++)
	{
		float eeks = radius * temppointsx[i] + x;
		float why = radius * temppointsy[i] + y;
		pointsx.push_back(eeks);
		pointsy.push_back(why);

		vertices.emplace_back(Vertex_t(Vector2D(eeks, why)));
	}

	m_surface()->DrawSetColor(color);
	m_surface()->DrawTexturedPolygon(points, vertices.data());
}

void render::triangle(Vector2D point_one, Vector2D point_two, Vector2D point_three, Color color) 
{
	if (!m_surface())
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	Vertex_t verts[3] = {
		Vertex_t(point_one),
		Vertex_t(point_two),
		Vertex_t(point_three)
	};

	auto surface = m_surface();

	static int texture = surface->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	surface->DrawSetTextureRGBA(texture, buffer, 1, 1);
	surface->DrawSetColor(color);
	surface->DrawSetTexture(texture);

	surface->DrawTexturedPolygon(3, verts);
}

void render::line(int x, int y, int x2, int y2, Color color) 
{
	if (!m_surface())
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	m_surface()->DrawSetColor(color);
	m_surface()->DrawLine(x, y, x2, y2);
}

void render::text(vgui::HFont font, int x, int y, Color color, DWORD flags, const char* msg, ...) 
{
	if (!m_surface())
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	va_list va_alist;
	char buffer[1024];
	va_start(va_alist, msg);
	_vsnprintf(buffer, sizeof(buffer), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];

	MultiByteToWideChar(CP_UTF8, 0, buffer, 256, wbuf, 256);

	auto surface = m_surface();

	int width, height;
	surface->GetTextSize(font, wbuf, width, height);

	if (!(flags & HFONT_CENTERED_NONE))
	{
		if (flags & HFONT_CENTERED_X)
			x -= width * 0.5f;

		if (flags & HFONT_CENTERED_Y)
			y -= height * 0.5f;
	}

	surface->DrawSetTextFont(font);
	surface->DrawSetTextColor(color);
	surface->DrawSetTextPos(x, y);
	surface->DrawPrintText(wbuf, wcslen(wbuf));
}

void render::wtext(vgui::HFont font, int x, int y, Color color, DWORD flags, wchar_t* msg) 
{
	if (!m_surface())
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	wchar_t buffer[256];
	va_list args;

	va_start(args, msg);
	wvsprintfW(buffer, msg, args);
	va_end(args);

	auto surface = m_surface();

	int width, height;
	surface->GetTextSize(font, buffer, width, height);

	if (!(flags & HFONT_CENTERED_NONE)) {
		if (flags & HFONT_CENTERED_X)
			x -= width * 0.5f;

		if (flags & HFONT_CENTERED_Y)
			y -= height * 0.5f;
	}

	surface->DrawSetTextFont(font);
	surface->DrawSetTextColor(color);
	surface->DrawSetTextPos(x, y);
	surface->DrawPrintText(buffer, wcslen(buffer));
}

int render::text_width(vgui::HFont font, const char* msg, ...) 
{
	if (!m_surface())
		return 0.0f;

	va_list va_alist;
	char buffer[1024];
	va_start(va_alist, msg);
	_vsnprintf(buffer, sizeof(buffer), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];

	MultiByteToWideChar(CP_UTF8, 0, buffer, 256, wbuf, 256);

	int width, height;
	m_surface()->GetTextSize(font, wbuf, width, height);

	return width;
}

void render::DrawFilled3DBox(const Vector& origin, int width, int height, Color outline, Color filling)
{
	if (!m_surface())
		return;

	float difw = float (width / 2);
	float difh = float (height / 2);

	Vector boxVectors[8] =
	{
		Vector(origin.x - difw, origin.y - difh, origin.z - difw),
		Vector(origin.x - difw, origin.y - difh, origin.z + difw),
		Vector(origin.x + difw, origin.y - difh, origin.z + difw),
		Vector(origin.x + difw, origin.y - difh, origin.z - difw),
		Vector(origin.x - difw, origin.y + difh, origin.z - difw),
		Vector(origin.x - difw, origin.y + difh, origin.z + difw),
		Vector(origin.x + difw, origin.y + difh, origin.z + difw),
		Vector(origin.x + difw, origin.y + difh, origin.z - difw),
	};

	static Vector vec0, vec1, vec2, vec3,
		vec4, vec5, vec6, vec7;

	if (math::world_to_screen(boxVectors[0], vec0) &&
		math::world_to_screen(boxVectors[1], vec1) &&
		math::world_to_screen(boxVectors[2], vec2) &&
		math::world_to_screen(boxVectors[3], vec3) &&
		math::world_to_screen(boxVectors[4], vec4) &&
		math::world_to_screen(boxVectors[5], vec5) &&
		math::world_to_screen(boxVectors[6], vec6) &&
		math::world_to_screen(boxVectors[7], vec7))
	{
		Vector lines[12][2];
		lines[0][0] = vec0;
		lines[0][1] = vec1;
		lines[1][0] = vec1;
		lines[1][1] = vec2;
		lines[2][0] = vec2;
		lines[2][1] = vec3;
		lines[3][0] = vec3;
		lines[3][1] = vec0;

		lines[4][0] = vec4;
		lines[4][1] = vec5;
		lines[5][0] = vec5;
		lines[5][1] = vec6;
		lines[6][0] = vec6;
		lines[6][1] = vec7;
		lines[7][0] = vec7;
		lines[7][1] = vec4;

		lines[8][0] = vec0;
		lines[8][1] = vec4;

		lines[9][0] = vec1;
		lines[9][1] = vec5;

		lines[10][0] = vec2;
		lines[10][1] = vec6;

		lines[11][0] = vec3;
		lines[11][1] = vec7;

		for (int i = 0; i < 12; i++)
			render::get().line(lines[i][0].x, lines[i][0].y, lines[i][1].x, lines[i][1].y, outline);
	}
}

void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
{
	if (!s) //-V550
	{
		out_r = out_g = out_b = v;
		return;
	}

	h = fmod(h, 1.0f) / (60.0f / 360.0f);

	auto i = (int)h;

	auto f = h - (float)i;
	auto p = v * (1.0f - s);
	auto q = v * (1.0f - s * f);
	auto t = v * (1.0f - s * (1.0f - f));

	switch (i)
	{
	case 0: 
		out_r = v;
		out_g = t;
		out_b = p; 
		break;
	case 1:
		out_r = q; 
		out_g = v; 
		out_b = p;
		break;
	case 2: 
		out_r = p; 
		out_g = v; 
		out_b = t; 
		break;
	case 3:
		out_r = p; 
		out_g = q; 
		out_b = v;
		break;
	case 4: 
		out_r = t; 
		out_g = p; 
		out_b = v;
		break;
	default: 
		out_r = v; 
		out_g = p; 
		out_b = q;
		break;
	}
}

void render::Draw3DCircle(const Vector& origin, float radius, Color color)
{
	static auto prevScreenPos = ZERO; //-V656
	static auto step = M_PI * 2.0f / 72.0f;

	auto screenPos = ZERO;

	for (auto rotation = 0.0f; rotation <= M_PI * 2.0f; rotation += step) //-V1034
	{
		Vector pos(radius * cos(rotation) + origin.x, radius * sin(rotation) + origin.y, origin.z);

		Ray_t ray;
		trace_t trace;
		CTraceFilterWorldOnly filter;

		ray.Init(origin, pos);
		m_trace()->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (math::world_to_screen(trace.endpos, screenPos))
		{
			if (!prevScreenPos.IsZero())
				line(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, color);

			prevScreenPos = screenPos;
		}
	}
}

void render::Draw3DFilledCircle(const Vector& origin, float radius, Color color)
{
	static auto prevScreenPos = ZERO; //-V656
	static auto step = M_PI * 2.0f / 72.0f;

	auto screenPos = ZERO;
	auto screen = ZERO;

	if (!math::world_to_screen(origin, screen))
		return;

	for (auto rotation = 0.0f; rotation <= M_PI * 2.0f; rotation += step) //-V1034
	{
		Vector pos(radius * cos(rotation) + origin.x, radius * sin(rotation) + origin.y, origin.z);

		if (math::world_to_screen(pos, screenPos))
		{
			if (!prevScreenPos.IsZero() && prevScreenPos.IsValid() && screenPos.IsValid() && prevScreenPos != screenPos)
			{
				line(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, color);
				triangle(Vector2D(screen.x, screen.y), Vector2D(screenPos.x, screenPos.y), Vector2D(prevScreenPos.x, prevScreenPos.y), Color(color.r(), color.g(), color.b(), color.a() / 2));
			}

			prevScreenPos = screenPos;
		}
	}
}

void render::Draw3DRainbowCircle(const Vector& origin, float radius)
{
	static auto hue_offset = 0.0f;
	static auto prevScreenPos = ZERO; //-V656
	static auto step = M_PI * 2.0f / 72.0f;

	auto screenPos = ZERO;

	for (auto rotation = 0.0f; rotation <= M_PI * 2.0f; rotation += step) //-V1034
	{
		Vector pos(radius * cos(rotation) + origin.x, radius * sin(rotation) + origin.y, origin.z);

		Ray_t ray;
		trace_t trace;
		CTraceFilterWorldOnly filter;

		ray.Init(origin, pos);
		m_trace()->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (math::world_to_screen(trace.endpos, screenPos))
		{
			if (!prevScreenPos.IsZero())
			{
				auto hue = RAD2DEG(rotation) + hue_offset;

				float r, g, b;
				ColorConvertHSVtoRGB(hue / 360.0f, 1.0f, 1.0f, r, g, b);

				render::get().line(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, Color(r, g, b));
			}

			prevScreenPos = screenPos;
		}
	}

	hue_offset += m_globals()->m_frametime * 200.0f;
}

RECT render::viewport()
{
	RECT viewport = { 0, 0, 0, 0 };

	static int width, height;
	m_engine()->GetScreenSize(width, height);
	viewport.right = width;
	viewport.bottom = height;

	return viewport;
}
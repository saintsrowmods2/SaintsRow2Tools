#include "D3DWrapper.h"
#include "HookedFuncs.h"

extern vector<char*> g_debugLines;

CDirect3DHelper::CDirect3DHelper(void)
{
	//
	// create text buffers
	//
	textBufferA_ = new CHAR[MAX_TEXT_BUFFER]; // any larger would piss off 640x480 users
	textBufferW_ = new WCHAR[MAX_TEXT_BUFFER];
}

CDirect3DHelper::~CDirect3DHelper(void)
{
	//
	// free text buffer
	//
	delete[] textBufferA_;
	delete[] textBufferW_;
}


VOID CDirect3DHelper::InitText()
{
	D3DXCreateFont(
		Direct3DDevice9_,
		FONT_SIZE,
		0,
		FW_NORMAL,
		0,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		_T("Verdana"),
		&font_);

	textRect_.left = 10;
	textRect_.right = MAX_TEXT_BUFFER;

	fontColor_ = D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff); // white
}

HRESULT CDirect3DHelper::DrawDebugText()
{
	if(font_ == NULL)
		InitText();

	RECT r;
	r.top = FONT_SIZE * 4;
	r.left = 10;
	r.right = MAX_TEXT_BUFFER;
	r.bottom = r.top + (FONT_SIZE * 10);

	for (vector<char*>::iterator iter = g_debugLines.begin(); iter != g_debugLines.end(); ++iter)
	{
		/*IDirect3DSurface9* surface;
		this->Direct3DDevice9_->lpVtbl->GetRenderTarget(
			Direct3DDevice9_,
			0,
			&surface);

		this->Direct3DDevice9_->lpVtbl->SetRenderState(Direct3DDevice9_, D3DRS_ALPHABLENDENABLE, 1);
		HRESULT hr = this->Direct3DDevice9_->lpVtbl->ColorFill(
			Direct3DDevice9_,
			surface,
			&r,
			D3DCOLOR_ARGB(100, 0x00, 0x00, 0x00));*/

		this->font_->lpVtbl->DrawTextA(
			font_, 
			NULL,
			*iter,
			-1,
			&r,
			0,
			fontColor_);

		r.top = r.top + FONT_SIZE;
	}

	return ERROR_SUCCESS;
}

HRESULT CDirect3DHelper::DrawTextA(LPCSTR string, INT line, ...)
{
	if(font_ == NULL)
		InitText();

	//
	// validate text string is within spec (this isn't perfect due to args)
	//
	if(string == NULL)
		return E_INVALIDARG;

	size_t length = strlen(string);

	if(length < MIN_TEXT_BUFFER || length > MAX_TEXT_BUFFER)
		return E_INVALIDARG;

	va_list vl;
	va_start(vl, line);

	vsnprintf(textBufferA_, MAX_TEXT_BUFFER, string, vl);

	va_end(vl);

	//
	// adjust rect to take line number into account
	//
	textRect_.top = FONT_SIZE * line;
	textRect_.bottom = textRect_.top + FONT_SIZE;

	this->font_->lpVtbl->DrawTextA(
		font_, 
		NULL,
		textBufferA_,
		-1,
		&textRect_,
		0,
		fontColor_);

	return ERROR_SUCCESS;
}

HRESULT CDirect3DHelper::DrawTextW(LPCWSTR string, INT line, ...)
{
	if(font_ == NULL)
		InitText();

	//
	// validate text string is within spec (this isn't perfect due to args)
	//
	if(string == NULL)
		return E_INVALIDARG;

	size_t length = wcslen(string);

	if(length < MIN_TEXT_BUFFER || length > MAX_TEXT_BUFFER)
		return E_INVALIDARG;

	va_list vl;
	va_start(vl, line);

	_vsnwprintf(textBufferW_, MAX_TEXT_BUFFER, string, vl);

	va_end(vl);

	//
	// adjust rect to take line number into account
	//
	textRect_.top = FONT_SIZE * line;
	textRect_.bottom = textRect_.top + FONT_SIZE;

	this->font_->lpVtbl->DrawTextW(
		font_, 
		NULL,
		textBufferW_,
		-1,
		&textRect_,
		0,
		fontColor_);

	return ERROR_SUCCESS;
}

VOID CDirect3DHelper::SetDevicePtr(IDirect3DDevice9* ptr)
{
	Direct3DDevice9_ = ptr;
}

IDirect3DDevice9* CDirect3DHelper::GetDevicePtr()
{
	return Direct3DDevice9_;
}

VOID CDirect3DHelper::SetInterfacePtr(IDirect3D9* ptr)
{
	Direct3D9_ = ptr;
}

IDirect3D9* CDirect3DHelper::GetInterfacePtr()
{
	return Direct3D9_;
}
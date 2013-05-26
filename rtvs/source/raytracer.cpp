#include "../header/raytracer.h"

// ---------- constructor ----------
RaytracerInterface::RaytracerInterface(){
  // initialise
  ZeroMemory( this, sizeof(RaytracerInterface) );
}

RaytracerInterface::RaytracerInterface(int width, int height)
{
	// initialise
  ZeroMemory( this, sizeof(RaytracerInterface) );
  m_width = width;
  m_height = height;
}

void RaytracerInterface::raytrace(int x, int y)
{
  // render a pixel (x? and R, G, B values)
  texture[(y * m_width + x) * 4] = x * 0.5f; //B
  texture[(y * m_width + x) * 4 + 1] = (m_width - x) * 0.5f; //G
  texture[(y * m_width + x) * 4 + 2] = y * 0.5f; //R
  texture[(y * m_width + x) * 4 + 3] = 0;
}

void RaytracerInterface::traceNextLine()
{
  // render a whole line
  for (int x = 0; x < m_width; x++) {
    raytrace(x, m_lastLine); // continue where we left off
  }
  m_lastLine++;

  // wrap around on last line (or stop?)
  if (m_lastLine >= m_height) {
    m_lastLine = 0;
  }
}

HRESULT RaytracerInterface::render(LPDIRECT3DTEXTURE9 &pTexture)
{
  // just memcpy your image to pRect here
  
  returnvalue = pTexture->LockRect(0, &lr, NULL, 0);
  if (FAILED(returnvalue))
    return E_FAIL;
	
	UCHAR* pRect = (UCHAR*) lr.pBits; 

  memcpy(pRect,texture,sizeof(texture));

	returnvalue = pTexture->UnlockRect(0);
	if (FAILED(returnvalue))
    return E_FAIL;

  // ok
  return NOERROR;
}

HRESULT RaytracerInterface::save(LPDIRECT3DTEXTURE9 &pTexture)
{
  returnvalue = D3DXSaveTextureToFile("render.jpg", D3DXIFF_JPG, pTexture, NULL);

  if (FAILED(returnvalue))
    return E_FAIL;

  // ok
  return NOERROR;
}


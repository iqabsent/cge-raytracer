#include "../header/raytracerInterface.h"
#include "../header/common.h"
#include "../header/raytracer.h"
#include "../header/scene.h"
#include "../header/common.h"

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
  m_shouldRender = false;
  m_isRenderDone = false;
  tracer = new Raytracer::Engine();
// this should be tunneled into to control scene from AntTweakBar
  tracer->GetScene()->InitScene();
  tracer->SetTarget( m_width, m_height );
// this should be using parameterized values
  m_cameraPosition = Raytracer::vector3( -1.0f, 0, 0 );
  m_cameraTarget = Raytracer::vector3( 0, 0, -1.0f );
  m_cameraTarget.Normalize();
  tracer->InitRender( m_cameraPosition, m_cameraPosition - m_cameraTarget);
}

void RaytracerInterface::resetRender(LPDIRECT3DTEXTURE9 &pTexture)
{
  m_lastLine = 0;
  m_shouldRender = false;
  m_isRenderDone = false;
  ZeroMemory(&m_texture, sizeof(m_texture));
  render(pTexture);
  m_cameraTarget.Normalize();
  tracer->InitRender( m_cameraPosition, m_cameraPosition - Raytracer::vector3(m_cameraTarget.x,-m_cameraTarget.y,m_cameraTarget.z));
}

void RaytracerInterface::startRender()
{
  m_shouldRender = true;
}

bool RaytracerInterface::traceNextLine()
{
  // render a whole line
  Raytracer::Primitive* lastprim = 0;
  Raytracer::vector3 lpos = tracer->m_P1 + (float)m_lastLine * tracer->m_DY;
  for (int x = 0; x < m_width; x++) {
    //raytrace(x, m_lastLine); // continue where we left off

    // fire primary rays
    Raytracer::Color acc( 0, 0, 0 );
    Raytracer::Primitive* prim = tracer->RenderRay( lpos, acc );
    int red, green, blue;
    if (prim != lastprim)
    {
      lastprim = prim;
      tracer->RenderRay( lpos - tracer->m_DX * 0.5f, acc );
      tracer->RenderRay( lpos - tracer->m_DY * 0.5f, acc );
      tracer->RenderRay( lpos - tracer->m_DX * 0.5f - tracer->m_DY * 0.5f, acc );
      red = (int)(acc.r * 64);
      green = (int)(acc.g * 64);
      blue = (int)(acc.b * 64);
      if (red > 255) red = 255;
      if (green > 255) green = 255;
      if (blue > 255) blue = 255;
    }
    else
    {
      red = (int)(acc.r * 256);
      green = (int)(acc.g * 256);
      blue = (int)(acc.b * 256);
      if (red > 255) red = 255;
      if (green > 255) green = 255;
      if (blue > 255) blue = 255;
    }

    //m_Dest[m_PPos++] = (red << 16) + (green << 8) + blue;
    m_texture[(m_lastLine * m_width + x) * 4] = blue; //B
    m_texture[(m_lastLine * m_width + x) * 4 + 1] = green; //G
    m_texture[(m_lastLine * m_width + x) * 4 + 2] = red; //R
    m_texture[(m_lastLine * m_width + x) * 4 + 3] = 0;

    lpos += tracer->m_DX;
  }
  
  m_lastLine++;

  // wrap around on last line (or stop?)
  if (m_lastLine >= m_height) {
    m_lastLine = 0;
    m_shouldRender = false;
    m_isRenderDone = true;
  }

  return m_shouldRender;
}

HRESULT RaytracerInterface::render(LPDIRECT3DTEXTURE9 &pTexture)
{
  // just memcpy your image to pRect here
  
  returnvalue = pTexture->LockRect(0, &lr, NULL, 0);
  if (FAILED(returnvalue))
    return E_FAIL;
	
	UCHAR* pRect = (UCHAR*) lr.pBits; 

  memcpy(pRect,m_texture,sizeof(m_texture));

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


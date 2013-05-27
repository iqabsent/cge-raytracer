////////////////////////////////////////////////////////////////////////////////

// ---------- rtvsD3dApp.h ----------

#ifndef _raytracerInterface_
#define _raytracerInterface_

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "raytracer.h"

static const int WIDTH = 1280;
static const int HEIGHT = 900;

class RaytracerInterface
{

public:

  // constructor/destructor & functions
  RaytracerInterface();
	RaytracerInterface(int width, int height);
  bool traceNextLine();
  void resetRender(LPDIRECT3DTEXTURE9 &pTexture);
  void startRender();
	//void raytrace(int x, int y); // returns RGB result for a ray
  HRESULT save(LPDIRECT3DTEXTURE9 &pTexture);
  HRESULT render(LPDIRECT3DTEXTURE9 &pTexture); // places texture rendered so far in given buffer

  // properties
  UCHAR m_texture[WIDTH * HEIGHT * 4];
  int m_lastLine, m_width, m_height;
  bool m_shouldRender;
  bool m_isRenderDone;

  // scene cam stuff
  Raytracer::vector3 m_cameraPosition;
  Raytracer::vector3 m_cameraTarget;

  // used for stuff
  HRESULT returnvalue;
  D3DLOCKED_RECT lr;
  LPDIRECT3DSURFACE9 surface;
  D3DDISPLAYMODE mode;

  Raytracer::Engine* tracer;
};

#endif _raytracerInterface_

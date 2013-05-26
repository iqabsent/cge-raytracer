////////////////////////////////////////////////////////////////////////////////

// ---------- rtvsD3dApp.h ----------

#ifndef _raytracerInterface_
#define _raytracerInterface_

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

static const int WIDTH = 512;
static const int HEIGHT = 512;

class RaytracerInterface
{

public:

  // constructor/destructor & functions
  RaytracerInterface();
	RaytracerInterface(int width, int height);
  void traceNextLine();
	void raytrace(int x, int y); // returns RGB result for a ray
  HRESULT save(LPDIRECT3DTEXTURE9 &pTexture);
  HRESULT render(LPDIRECT3DTEXTURE9 &pTexture); // places texture rendered so far in given buffer

  // properties
  UCHAR texture[WIDTH * HEIGHT * 4];
  int m_lastLine, m_width, m_height;

  // used for stuff
  HRESULT returnvalue;
  D3DLOCKED_RECT lr;
  LPDIRECT3DSURFACE9 surface;
  D3DDISPLAYMODE mode;
};

#endif _raytracerInterface_

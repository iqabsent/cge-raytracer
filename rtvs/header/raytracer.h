////////////////////////////////////////////////////////////////////////////////

// ---------- rtvsD3dApp.h ----------

#ifndef _raytracer_
#define _raytracer_

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "common.h"

class Raytracer
{

public:

  // constructor/destructor & functions
  Raytracer();
	Raytracer(int width, int height);
  void traceNextLine();
	void raytrace(int x, int y); // returns RGB result for a ray
  HRESULT render(LPDIRECT3DTEXTURE9 &pTexture); // places texture rendered so far in given buffer

  // properties
  UCHAR texture[WIDTH * HEIGHT * 4];
  int m_lastLine, m_width, m_height;

  // used for stuff
  HRESULT returnvalue;
  D3DLOCKED_RECT lr;
};

#endif _raytracer_

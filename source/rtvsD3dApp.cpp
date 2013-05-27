#include "../header/rtvsD3dApp.h"

// ---------- constructor ----------
rtvsD3dApp::rtvsD3dApp()
{
	// initialise
  ZeroMemory( this, sizeof(rtvsD3dApp) );
}

////////////////////////////////////////////////////////////////////////////////
// ---------- framework ----------
bool rtvsD3dApp::cleanup ()
{
	return true;
}

// ---------- framework : cleanup dx ----------
bool rtvsD3dApp::cleanupDX (LPDIRECT3DDEVICE9 pd3dDevice)
{

  // ---- invalidate the texture object ----
  if( pTexture != NULL )
  {
      int nNewRefCount = pTexture->Release();

      if( nNewRefCount > 0 )
      {
          static char strError[256];
          sprintf_s ( strError, 256,
			"The texture object failed to cleanup properly.\n"
              "Release() returned a reference count of %d",
			nNewRefCount );
          MessageBox( NULL, strError, "ERROR", MB_OK | MB_ICONEXCLAMATION );
      }

      pTexture = NULL;
  }
	
  // ---- invalidate the quad vertex buffer object ----
  if( pQuadVertexBuffer != NULL )
  {
      int nNewRefCount = pQuadVertexBuffer->Release();

      if( nNewRefCount > 0 )
      {
          static char strError[256];
          sprintf_s ( strError, 256,
			"The quad vertex buffer object failed to cleanup properly.\n"
              "Release() returned a reference count of %d",
			nNewRefCount );
          MessageBox( NULL, strError, "ERROR", MB_OK | MB_ICONEXCLAMATION );
      }
      pQuadVertexBuffer = NULL;
  }

  TwTerminate();//cleanup anttweakbar

	// ok
	return true;

}

// ---------- framework : display ----------
bool rtvsD3dApp::display (LPDIRECT3DDEVICE9 pd3dDevice)
{
 	// clear backbuffers
  pd3dDevice->Clear( 0,
	NULL,
	D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
	D3DCOLOR_COLORVALUE(0.0f,0.3f,0.7f,1.0f),
	1.0f,
	0);

  // check if should render
  if (m_antShouldRender) { // ant says render
    if (
      !m_pTracer->m_shouldRender // tracer isn't rendering
      && !m_pTracer->m_isRenderDone // tracer isn't done
    ) { 
      // tell it to render
      m_pTracer->startRender();
    }
    else if (
      !m_pTracer->m_shouldRender // tracer isn't rendering
      && m_pTracer->m_isRenderDone // tracer is done
    ) {
      // reset and tell it to render
      m_pTracer->resetRender(pTexture);
      m_pTracer->startRender();
    }
  }
  else // ant says don't render
  {
    if(m_pTracer->m_shouldRender) // tracer is rendering
      m_pTracer->m_shouldRender = false; // tell it to stop
  }

  // if tracer should be rendering
  if(m_pTracer->m_shouldRender)
  {
    // trace line by line & check if done
    m_antShouldRender = m_pTracer->traceNextLine();

    // try to render
    returnvalue = m_pTracer->render(pTexture);
    if (FAILED(returnvalue))
      return false;
  }

	// display solid textured quad
	pd3dDevice->SetMaterial( &quadMtrl );
	pd3dDevice->SetTexture( 0, pTexture );
	pd3dDevice->SetStreamSource( 0, pQuadVertexBuffer, 0, sizeof(QuadVertex) );
	pd3dDevice->SetFVF( QuadVertex::FVF_Flags );
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, numQuadTriangles );

	TwDraw();  // draw the tweak bar GUI

	// ok
	return true;

}

// ---------- framework : setup ----------
bool rtvsD3dApp::setup ()
{
	// setup a material for the textured quad
  ZeroMemory( &quadMtrl, sizeof(D3DMATERIAL9) );
	quadMtrl.Ambient.r = 1.0f;
	quadMtrl.Ambient.g = 1.0f;
	quadMtrl.Ambient.b = 1.0f;

  // create a raytracer
  m_pTracer = new RaytracerInterface(WIDTH, HEIGHT);
  m_antShouldRender = false;

	// ok
	return true;
}

// ---------- framework : setup dx ----------
bool rtvsD3dApp::setupDX (LPDIRECT3DDEVICE9 pd3dDevice)
{
  pd3dDevice->SetRenderState( D3DRS_LIGHTING , TRUE );
  pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
  pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_GAUSSIANQUAD );

  // ---- create a texture object ----
  //D3DXCreateTextureFromFile( pd3dDevice, "image/baboon.jpg", &pTexture );

  // get mode
  D3DDISPLAYMODE mode;
  returnvalue = pd3dDevice->GetDisplayMode(0, &mode);
  if (FAILED(returnvalue))
    return E_FAIL;

  // try create texture
  returnvalue = pd3dDevice->CreateTexture(WIDTH, HEIGHT, 1, 0,
		mode.Format, D3DPOOL_MANAGED, &pTexture, NULL);

	if (FAILED(returnvalue))
    return E_FAIL;

	
	// ---- QUAD ----
  void *pVertices = NULL;
  // ---- initialise quad vertex data ----
 	QuadVertex quadVertices[] =
	{
		//    x      y     z      nx     ny     nz    tu     tv
		{  1.28f,  0.93f, 2.0f,   0.0f,  1.0f,  0.0f,  0.0f,  0.0f }, //top right
		{ -1.28f,  0.93f, 2.0f,   0.0f,  1.0f,  0.0f,  1.0f,  0.0f }, //top left
		{  1.28f, -0.93f, 2.0f,   0.0f,  1.0f,  0.0f,  0.0f,  1.0f },
		{ -1.28f, -0.93f, 2.0f,   0.0f,  1.0f,  0.0f,  1.0f,  1.0f }
	};

	// ---- create quad vertex buffer ----
	int numQuadVertices = sizeof(quadVertices) / ( sizeof(float) * 8 /* +  sizeof(DWORD)*/);
	numQuadTriangles = numQuadVertices / 2;
	pd3dDevice->CreateVertexBuffer( numQuadVertices*sizeof(QuadVertex),
                                      D3DUSAGE_WRITEONLY,
                                      QuadVertex::FVF_Flags,
                                      D3DPOOL_MANAGED, // does not have to be properly Released before calling IDirect3DDevice9::Reset
                                      //D3DPOOL_DEFAULT,   // must be Released properly before calling IDirect3DDevice9::Reset
                                      &pQuadVertexBuffer, NULL );

	// ---- block copy into quad vertex buffer ----
	pVertices = NULL;
	pQuadVertexBuffer->Lock( 0, sizeof(quadVertices), (void**)&pVertices, 0 );
	memcpy( pVertices, quadVertices, sizeof(quadVertices) );
	pQuadVertexBuffer->Unlock();

  setupAntTW(pd3dDevice);

	// ok
	return true;
}

// ------ AntTweakBar stuff ----------------
// ------ AntTweakBar functions ------------
void TW_CALL TW_cancel(void *clientData) //AntTweakBar Buttons
{
  rtvsD3dApp* cancelPointer = (rtvsD3dApp*)clientData;
  cancelPointer->resetRender();
}
void TW_CALL TW_reset(void *clientData) //AntTweakBar Buttons
{
  rtvsD3dApp* cancelPointer = (rtvsD3dApp*)clientData;
  cancelPointer->resetSettings();
}
void TW_CALL TW_save(void *clientData) //AntTweakBar Buttons
{
  rtvsD3dApp* savePointer = (rtvsD3dApp*)clientData;
  savePointer->save();
}

// ------ AntTweakBar setup -----------------
bool rtvsD3dApp::setupAntTW(LPDIRECT3DDEVICE9 pd3dDevice)
{
  TwInit(TW_DIRECT3D9, pd3dDevice); // for Direct3D 9

  TwBar *myBar;
  myBar = TwNewBar("Options");

//   TwStructMember primitive[] = {
//     { "name", TW_TYPE_FLOAT, offsetof(primitive, property), " Min=-5 Max=5 " },
//     { "direction", TW_TYPE_DIR3F, offsetof(wave, z), " Min=-5 Max=5 " }
//   };

 struct position {
   float x, y, z;
 };

  TwStructMember positionMembers[] = {
    {"x", TW_TYPE_FLOAT, offsetof(position, x), " Min=-10 Max=10 " },
    {"y", TW_TYPE_FLOAT, offsetof(position, y), " Min=-10 Max=10 " },
    {"z", TW_TYPE_FLOAT, offsetof(position, z), " Min=-10 Max=10 " }
  };
  TwType positionType = TwDefineStruct("Position", positionMembers, 3, sizeof(position), NULL, NULL);



  TwAddSeparator(myBar, "sepCamera", " group='Settings' " );
  TwAddButton(myBar, "Camera", NULL, NULL, " group='Settings' ");
  TwAddVarRW(myBar, "Position", positionType, &m_pTracer->m_cameraPosition, " group='Settings' ");
  TwAddVarRW(myBar, "camTarget", TW_TYPE_DIR3F, &m_pTracer->m_cameraTarget, " group='Settings' label=Direction Min=-1 Max=1 ");

  TwAddSeparator(myBar, "sepReset", " group='Settings' ");
  TwAddButton(myBar, "Reset", TW_reset, this, " label='Reset' group='Settings' ");

  TwAddSeparator(myBar, "sepActions", NULL);
  TwAddVarRW(myBar, "Render", TW_TYPE_BOOLCPP, &m_antShouldRender, NULL);
  TwAddButton(myBar, "Cancel", TW_cancel, this, " label='Cancel' ");
  TwAddButton(myBar, "Save", TW_save, this, " label='Save' ");

  TwDefine(" Settings/camPos group='Camera' ");
  TwDefine(" Settings/camTarget group='Camera' ");
  TwDefine(" myBar/Primitives group='Settings' ");

  return true;
}

// ---------- framework : save to file ----------
bool rtvsD3dApp::save()
{
  return m_pTracer->save(pTexture);
}

// ---------- framework : reset render ----------
void rtvsD3dApp::resetRender()
{
  m_pTracer->resetRender(pTexture);
  m_antShouldRender = false;
}

// ---------- framework : reset render ----------
void rtvsD3dApp::resetSettings()
{
  m_pTracer->m_cameraPosition = Raytracer::vector3( 0, 0, 0 );
  m_pTracer->m_cameraTarget = Raytracer::vector3( 0, 0, -1.0f );
  m_pTracer->m_cameraTarget.Normalize();
}
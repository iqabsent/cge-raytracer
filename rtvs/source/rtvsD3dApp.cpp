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

	// set render states
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  // trace line by line
  pTracer->traceNextLine();

  // try to render
  returnvalue = pTracer->render(pTexture);
  if (FAILED(returnvalue))
    return false;

	// display solid textured quad
	pd3dDevice->SetMaterial( &quadMtrl );
	pd3dDevice->SetTexture( 0, pTexture );
	pd3dDevice->SetStreamSource( 0, pQuadVertexBuffer, 0, sizeof(QuadVertex) );
	pd3dDevice->SetFVF( QuadVertex::FVF_Flags );
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, numQuadTriangles );

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
  pTracer = new Raytracer(WIDTH, HEIGHT);

	// ok
	return true;
}

// ---------- framework : setup dx ----------
bool rtvsD3dApp::setupDX (LPDIRECT3DDEVICE9 pd3dDevice)
{

  pd3dDevice->SetRenderState( D3DRS_LIGHTING , TRUE);

  // ---- create a texture object ----
  //D3DXCreateTextureFromFile( pd3dDevice, "image/baboon.jpg", &pTexture );

  // try create
  returnvalue = pd3dDevice->CreateTexture(WIDTH, HEIGHT, 1, 0,
		D3DFMT_X8B8G8R8, D3DPOOL_MANAGED, &pTexture, NULL);

	if (FAILED(returnvalue))
    return E_FAIL;

	// ---- block copy into axis vertex buffer ----
	void *pVertices = NULL;

	// ---- QUAD ----
  // ---- initialise quad vertex data ----
 	QuadVertex quadVertices[] =
	{
		//    x      y     z      nx     ny     nz    tu     tv
		{  1.0f,  1.0f, 2.82f,   0.0f,  1.0f,  0.0f,  0.0f,  0.0f },
		{ -1.0f,  1.0f, 2.82f,   0.0f,  1.0f,  0.0f,  1.0f,  0.0f },
		{  1.0f, -1.0f, 2.82f,   0.0f,  1.0f,  0.0f,  0.0f,  1.0f },
		{ -1.0f, -1.0f, 2.82f,   0.0f,  1.0f,  0.0f,  1.0f,  1.0f }
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

	// ok
	return true;

}

// ---------- framework : save to file ----------
bool rtvsD3dApp::save(LPDIRECT3DDEVICE9 pd3dDevice)
{
  return pTracer->save(pTexture);
}
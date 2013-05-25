////////////////////////////////////////////////////////////////////////////////

// ---------- rtvsD3dApp.h ----------
/*!
\file rtvsD3dWinLite.h
\brief interface for the rtvsD3dApp class
\author Gareth Edwards
*/

#ifndef _rtvsD3dApp_
#define _rtvsD3dApp_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ---------- include ----------
#include <windows.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>

////////////////////////////////////////////////////////////////////////////////

// ---------- FVF - Flexible Vertex Formats ----------
struct QuadVertex
{
    float x , y, z;
    float nx, ny, nz;
	  DWORD diffuse;
    float tu, tv;

    enum FVF
    {
        FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1
    };
};

// ---------- rtvsD3dApp class interface ----------
class rtvsD3dApp  
{

public:
	LPDIRECT3DVERTEXBUFFER9	pQuadVertexBuffer;
	DWORD numQuadTriangles;
	LPDIRECT3DTEXTURE9 pTexture;
	D3DMATERIAL9 quadMtrl;
	
	// constructor/destructor
	rtvsD3dApp();
	bool cleanup();
	bool cleanupDX(LPDIRECT3DDEVICE9);
	bool display(LPDIRECT3DDEVICE9);
	bool setup();
	bool setupDX(LPDIRECT3DDEVICE9);
};

////////////////////////////////////////////////////////////////////////////////

#endif // _rtvsD3dApp_
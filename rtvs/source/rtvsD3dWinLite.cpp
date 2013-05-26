////////////////////////////////////////////////////////////////////////////////
// ---------- rtvsD3dWinLite.cpp ----------

// ---------- housekeeping defintions ----------

#define STRICT
#define WIN32_LEAN_AND_MEAN

// ---------- include windows headers ----------
#include <windows.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "../header/rtvsD3dWinLite.h"

////////////////////////////////////////////////////////////////////////////////
// ---------- windows ----------

// ---------- WinMain ----------
int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR     lpCmdLine,
                    int       nCmdShow )
{

	// local
  WNDCLASSEX	winClass;
  MSG			uMsg;
	char*		winClassName = "rtvsD3dWinLite";

	// initialise message
   memset(&uMsg,0,sizeof(uMsg));

	// init
  winClass.lpszClassName = winClassName;
  winClass.cbSize        = sizeof(WNDCLASSEX);
  winClass.style         = CS_HREDRAW | CS_VREDRAW;
  winClass.lpfnWndProc   = WindowProc;
  winClass.hInstance     = hInstance;
  winClass.hIcon         = NULL;
  winClass.hIconSm       = NULL;
  winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
  winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  winClass.lpszMenuName  = NULL;
  winClass.cbClsExtra    = 0;
  winClass.cbWndExtra    = 0;

	// IF register class fails THEN end
  if( !RegisterClassEx(&winClass) )
      return E_FAIL;

	// create window
  g_hWnd = CreateWindowEx( NULL, winClassName, 
                            "Gamlin / Van der Merwe - Raytracer",
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            0, 0, 1200, 800, NULL, NULL, hInstance, NULL );

	// IF create window fails THEN end
  if( g_hWnd == NULL )
      return E_FAIL;

	// housekeeping
  ShowWindow( g_hWnd, nCmdShow );
  UpdateWindow( g_hWnd );

	// create application
	g_app = new rtvsD3dApp();

	// non-device one time application setup 
	g_app->setup();

	// non-device one time setup
  setup();

	// device dependent setup
  setupDX();

	// WHILE NOT quit, handle messages and display
  while( uMsg.message != WM_QUIT )
  {
    if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
    { 
      // handle message
      TranslateMessage( &uMsg );
      DispatchMessage( &uMsg );
    }
    else
      // render line

      // show what's been rendered so far
      display();
  }

	// device dependent cleanup
  cleanupDX();

	// non-device dependant one time cleanup
	cleanup();

	// non-device one time application cleanup
	g_app->cleanup();

	// unregister windows class
  UnregisterClass(
		winClassName,
		winClass.hInstance
  );

  return uMsg.wParam;
}


// ---------- WindowProc ----------
LRESULT CALLBACK WindowProc( HWND   hWnd,
                             UINT   msg,
                             WPARAM wParam,
                             LPARAM lParam )
{
    static POINT ptLastMousePosit;
    static POINT ptCurrentMousePosit;
    static bool bMousing;
    
    switch( msg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;

				case VK_F1:
					g_bHandleWindowResize = !g_bHandleWindowResize;
					break;
            }
        }
        break;

        case WM_LBUTTONDOWN:
        {
        }
        break;

        case WM_LBUTTONUP:
        {
        }
        break;

        case WM_MOUSEMOVE:
        {
        }
        break;

        case WM_SIZE:
        {
			    if( g_bHandleWindowResize == true )
			    {
				    // If the device is not NULL and the WM_SIZE message is not a
				    // SIZE_MINIMIZED event, resize the device's swap buffers to match
				    // the new window size.
				    if( g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED )
				    {
					    cleanupDX();

					    g_d3dpp.BackBufferWidth  = LOWORD(lParam);
					    g_d3dpp.BackBufferHeight = HIWORD(lParam);

					    HRESULT hr = g_pd3dDevice->Reset( &g_d3dpp );

                        if( hr == D3DERR_INVALIDCALL )
                        {
                            MessageBox( NULL,
							    "Call to Reset() failed with D3DERR_INVALIDCALL!",
                                "ERROR", MB_OK | MB_ICONEXCLAMATION );
                        }

					    setupDX();
				    }
			    }
        }
        break;

        case WM_CLOSE:
        {
            PostQuitMessage(0); 
        }
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;

        default:
        {
            return DefWindowProc( hWnd, msg, wParam, lParam );
        }
        break;
    }

	// done
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// ---------- framework ----------

// ---------- cleanup ----------
void cleanup( void )
{
	// ---- cleanup DirectX device dependant objects ----
  cleanupDX();

	// ---- cleanup the device ----
  if( g_pd3dDevice != NULL )
  {
      int nNewRefCount = g_pd3dDevice->Release();

      if( nNewRefCount > 0 )
      {
          static char strError[256];
          sprintf_s( strError, 256,
			"The device object failed to cleanup properly. \n"
              "Release() returned a reference count of %d",
			nNewRefCount );
          MessageBox( NULL, strError, "ERROR", MB_OK | MB_ICONEXCLAMATION );
      }

      g_pd3dDevice = NULL;
  }

	// ---- release d3d ----
  SAFE_RELEASE( g_pD3D )
}


// ---------- cleanup DX ----------
HRESULT cleanupDX( void )
{
	// cleanup the application
	g_app->cleanupDX(g_pd3dDevice);

	// done ok
  return S_OK;
}

// ---------- display ----------
void display( void )
{
	// begin
  g_pd3dDevice->BeginScene();

	// display
	g_app->display(g_pd3dDevice);

	// end
  g_pd3dDevice->EndScene();

	// present
  g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


// ---------- setup ----------
void setup( void )
{

	// ---- direct3d initialisation ----
  g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	// ---- display mode ----
  D3DDISPLAYMODE d3ddm;
  g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );
  ZeroMemory( &g_d3dpp, sizeof(g_d3dpp) );

	// ---- set present parameters ----
  g_d3dpp.Windowed               = TRUE;
  g_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  g_d3dpp.BackBufferFormat       = d3ddm.Format;
  g_d3dpp.EnableAutoDepthStencil = TRUE;
  g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
  g_d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

	// ---- create the d3d device ----
  g_pD3D->CreateDevice( D3DADAPTER_DEFAULT,
	  D3DDEVTYPE_HAL,
	  g_hWnd,
	  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
	  &g_d3dpp,
	  &g_pd3dDevice
  );

}


// ---------- setup DX ----------
HRESULT setupDX ( void )
{

  // ---- projection matrix ----
	D3DXMATRIX matProj;
  D3DXMatrixPerspectiveFovLH(
    &matProj, D3DXToRadian( 45.0f ),
    (float)(g_d3dpp.BackBufferWidth) / float(g_d3dpp.BackBufferHeight), 
    0.1f,
    100.0f
  );
  g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// ---- render states ----
  g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
  g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 1.0f ) );

	// ---- application ----
	g_app->setupDX(g_pd3dDevice);

	// done
	return S_OK;

}

////////////////////////////////////////////////////////////////////////////////
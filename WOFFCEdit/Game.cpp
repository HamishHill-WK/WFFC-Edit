//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "DisplayObject.h"
#include <string>
#include<iostream>

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
	m_displayList.clear();
	
	//initial Settings
	//modes
	m_grid = false;
	m_wireFrame = false;
    m_lastID = -1;
    m_CameraManager = new CameraManager();    

    intpoint = { 0, 0, 0 };
}

Game::~Game()
{
    delete m_CameraManager;

#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    GetClientRect(window, &m_ScreenDimensions);

#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

void Game::SetGridState(bool state)
{
	m_grid = state;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick(InputCommands *Input)
{
	//copy over the input commands so we have a local version to use elsewhere.
	m_InputCommands = *Input;
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    m_CameraManager->Update(m_InputCommands);

	//apply camera vectors
    m_view = Matrix::CreateLookAt(m_CameraManager->getCamPosition(), m_CameraManager->getCamLookAt(), Vector3::UnitY);

    m_batchEffect->SetView(m_view);
    m_batchEffect->SetWorld(Matrix::Identity);

	m_displayChunk.m_terrainEffect->SetView(m_view);
	m_displayChunk.m_terrainEffect->SetWorld(Matrix::Identity);

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif 
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

	if (m_grid)
	{
		// Draw procedurally generated dynamic grid
		const XMVECTORF32 xaxis = { 512.f, 0.f, 0.f };
		const XMVECTORF32 yaxis = { 0.f, 0.f, 512.f };
		DrawGrid(xaxis, yaxis, g_XMZero, 512, 512, Colors::Gray);
	}

	//RENDER OBJECTS FROM SCENEGRAPH
	int numRenderObjects = m_displayList.size();
	for (int i = 0; i < numRenderObjects; i++)
	{
		m_deviceResources->PIXBeginEvent(L"Draw model");
		const XMVECTORF32 scale = { m_displayList[i].m_scale.x, m_displayList[i].m_scale.y, m_displayList[i].m_scale.z };
		const XMVECTORF32 translate = { m_displayList[i].m_position.x, m_displayList[i].m_position.y, m_displayList[i].m_position.z };

		//convert degrees into radians for rotation matrix
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y *3.1415 / 180,
															m_displayList[i].m_orientation.x *3.1415 / 180,
															m_displayList[i].m_orientation.z *3.1415 / 180);

		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

		m_displayList[i].m_model->Draw(context, *m_states, local, m_view, m_projection, m_wireFrame);	//last variable in draw,  make TRUE for wireframe

		m_deviceResources->PIXEndEvent();
	}

	//RENDER TERRAIN
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(),0);
	context->RSSetState(m_states->CullNone());

    if(m_wireFrame)
	    context->RSSetState(m_states->Wireframe());		//uncomment for wireframe

	//Render the batch,  This is handled in the Display chunk becuase it has the potential to get complex
	m_displayChunk.RenderBatch(m_deviceResources);

    //CAMERA POSITION ON HUD
    m_sprites->Begin();
    WCHAR   Buffer[256];
    std::wstring var = L"Cam X: " + TruncateFloatToString(m_CameraManager->getCamPosition().x) + L" Cam Y: " + TruncateFloatToString(m_CameraManager->getCamPosition().y) + L" Cam Z: " + TruncateFloatToString(m_CameraManager->getCamPosition().z);
   
    std::wstring var2 = L"Default";
    std::wstring var3 = L"Default";

    if(m_InputCommands.mode_rotate)
        var2 = L"Object Mode: Rotate";    
    
    if(m_InputCommands.mode_translate)
        var2 = L"Object Mode: Translate";    
    
    if(m_InputCommands.mode_scale)
        var2 = L"Object Mode: scale"; 

    if (m_CameraManager->camType == main)
        var3 = L"Current Cam: Main Camera";    
    
    if (m_CameraManager->camType == cine)
        var3 = L"Current Cam: Cinematic Cam " + std::to_wstring(m_CameraManager->currentCam);    
    
    if (m_CameraManager->camType == still)
        var3 = L"Current Cam: Static Cam " + std::to_wstring(m_CameraManager->currentCam);

    // Define the outline color
    XMVECTOR outlineColor = Colors::Black;

    // Set the outline offset
    float outlineOffset = 0.75f; // Adjust this value as needed
    float scale = 0.9f;
    // Lines 245-258 draw black outlines around the strings rendered in the camera view
    m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(10 - outlineOffset, 10 - outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(10 + outlineOffset, 10 - outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(10 - outlineOffset, 10 + outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(10 + outlineOffset, 10 + outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);

    m_font->DrawString(m_sprites.get(), var3.c_str(), XMFLOAT2(10 - outlineOffset, 30 - outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var3.c_str(), XMFLOAT2(10 + outlineOffset, 30 - outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var3.c_str(), XMFLOAT2(10 - outlineOffset, 30 + outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var3.c_str(), XMFLOAT2(10 + outlineOffset, 30 + outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);

    m_font->DrawString(m_sprites.get(), var2.c_str(), XMFLOAT2(10 - outlineOffset, 50 - outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var2.c_str(), XMFLOAT2(10 + outlineOffset, 50 - outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var2.c_str(), XMFLOAT2(10 - outlineOffset, 50 + outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var2.c_str(), XMFLOAT2(10 + outlineOffset, 50 + outlineOffset), outlineColor, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);


    m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(10, 10), Colors::Yellow, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var3.c_str(), XMFLOAT2(10, 30), Colors::Orange, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_font->DrawString(m_sprites.get(), var2.c_str(), XMFLOAT2(10, 50), Colors::Red, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(scale, scale), SpriteEffects_None, .0f);
    m_sprites->End();

    m_deviceResources->Present();
}

std::wstring Game::TruncateFloatToString(float value)
{
    std::wstring str = std::to_wstring(value);
    size_t decimalPos = str.find(L'.');
    if (decimalPos != std::wstring::npos && decimalPos + 2 < str.length())
        str = str.substr(0, decimalPos + 2);
    return str;
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

void Game::DoubleLClick(int i) {
    if(i != -1)
        m_CameraManager->mainCamera->setCamTarget(m_displayList[i].m_position, false);
}

void Game::TripleLClick(int i) {
    if(i != -1)
        m_CameraManager->mainCamera->setCamTarget(m_displayList[i].m_position, true);
}

//this function updates the object passed through with new data from scene graph. used for object manipulation 
void Game::updateObj(SceneObject objId, int obj)
{
    m_displayList.at(obj).m_position.x = objId.posX;
    m_displayList.at(obj).m_position.y = objId.posY;
    m_displayList.at(obj).m_position.z = objId.posZ;    
    
    m_displayList.at(obj).m_orientation.x = objId.rotX;
    m_displayList.at(obj).m_orientation.y = objId.rotY;
    m_displayList.at(obj).m_orientation.z = objId.rotZ;

    m_displayList.at(obj).m_scale.x = objId.scaX;
    m_displayList.at(obj).m_scale.y = objId.scaY;
    m_displayList.at(obj).m_scale.z = objId.scaZ;
}

//terrain raising function 
void Game::chunk() {
    //setup near and far planes of frustum with mouse X and mouse y passed down from Toolmain.
    const XMVECTOR nearSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 0.0f, 1.0f);
    const XMVECTOR farSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 1.0f, 1.0f);

    //Unproject the points on the near and far plane
    const XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, m_world);
    const XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, m_world);

    //get the line cast from the mouse
    const XMVECTOR lineCast = XMVector3Normalize(farPoint - nearPoint);

    float closestTerrainDist = FLT_MAX;
    DirectX::SimpleMath::Vector3 closestTerrainIntersection = Vector3(FLT_MAX);
    //loop through quads to check for line intersection
    for (size_t i = 0; i < TERRAINRESOLUTION - 1; i++)
    {
        for (size_t j = 0; j < TERRAINRESOLUTION - 1; j++)
        {
            XMVECTOR v1 = XMLoadFloat3(&m_displayChunk.m_terrainGeometry[i][j].position);
            XMVECTOR v2 = XMLoadFloat3(&m_displayChunk.m_terrainGeometry[i][j + 1].position);
            XMVECTOR v3 = XMLoadFloat3(&m_displayChunk.m_terrainGeometry[i + 1][j + 1].position);
            XMVECTOR v4 = XMLoadFloat3(&m_displayChunk.m_terrainGeometry[i + 1][j].position);

            XMVECTOR normal = XMVector3Normalize(XMVector3Cross(v2 - v1, v3 - v1));
            float d = -XMVectorGetX(XMVector3Dot(normal, v1));
            XMVECTOR plane = XMVectorSetW(normal, d);   //create plane from positisons 

            XMVECTOR intersects = XMPlaneIntersectLine(plane, nearPoint, farPoint); //find intersection

            if (!XMVector3Equal(intersects, XMVectorZero()))
            {
                Vector3 point;
                XMStoreFloat3(&point, intersects);

                if (point.x >= std::min(XMVectorGetX(v1), XMVectorGetX(v2)) && point.x <= std::max(XMVectorGetX(v1), XMVectorGetX(v2)) &&
                    point.z >= std::min(XMVectorGetZ(v1), XMVectorGetZ(v4)) && point.z <= std::max(XMVectorGetZ(v1), XMVectorGetZ(v4)))
                {
                    float distance = Vector3::Distance(point, m_CameraManager->getCamPosition());
                    if (distance < closestTerrainDist)
                    {
                        closestTerrainIntersection = point;
                        closestTerrainDist = distance;  //determine if intersection is closest one found, if so store it 
                    }
                }
            }

        }
    }
       
    //check which polygon contains closest point and raise geoemtry
    for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            const float distance = Vector3::Distance(Vector3(closestTerrainIntersection.x, 0, closestTerrainIntersection.z), Vector3(m_displayChunk.m_terrainGeometry[i][j].position.x, 0, m_displayChunk.m_terrainGeometry[i][j].position.z));
            if (distance < 15)
            {
                if (distance < 10)
                    m_displayChunk.m_terrainGeometry[i][j].position.y += 0.5f;
                else
                {
                    const float falloff = 1 - ((distance - 10) / (15 - 10));
                    m_displayChunk.m_terrainGeometry[i][j].position.y += 0.5f * falloff;
                }
                m_displayChunk.m_terrainGeometry[i][j].position.y = std::max(0.f, m_displayChunk.m_terrainGeometry[i][j].position.y);
                m_displayChunk.m_terrainGeometry[i][j].position.y = std::min(63.f, m_displayChunk.m_terrainGeometry[i][j].position.y);
            }
        }
    }
}

int Game::MousePicking()
{
    int selectedID = -1;
    float pickedDistance = 0;
    float closestDistance = 100000;

    //setup near and far planes of frustum with mouse X and mouse y passed down from Toolmain. 
        //they may look the same but note, the difference in Z
    const XMVECTOR nearSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 0.0f, 1.0f);
    const XMVECTOR farSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 1.0f, 1.0f);

    //Loop through entire display list of objects and pick with each in turn. 
    for (int i = 0; i < m_displayList.size(); i++)
    {
        //Get the scale factor and translation of the object
        const XMVECTORF32 scale = { m_displayList[i].m_scale.x,		m_displayList[i].m_scale.y,		m_displayList[i].m_scale.z };
        const XMVECTORF32 translate = { m_displayList[i].m_position.x,		m_displayList[i].m_position.y,	m_displayList[i].m_position.z };

        //convert euler angles into a quaternion for the rotation of the object
        XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y * 3.1415 / 180, m_displayList[i].m_orientation.x * 3.1415 / 180,
            m_displayList[i].m_orientation.z * 3.1415 / 180);

        //create set the matrix of the selected object in the world based on the translation, scale and rotation.
        XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

        //Unproject the points on the near and far plane, with respect to the matrix we just created.
        XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

        XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

        //turn the transformed points into our picking vector. 
        XMVECTOR pickingVector = farPoint - nearPoint;
        pickingVector = XMVector3Normalize(pickingVector);

        //loop through mesh list for object
        for (int y = 0; y < m_displayList[i].m_model.get()->meshes.size(); y++)
        {
            //checking for ray intersection
            if (m_displayList[i].m_model.get()->meshes[y]->boundingBox.Intersects(nearPoint, pickingVector, pickedDistance))
            {
                if (closestDistance >= pickedDistance) {
                    closestDistance = pickedDistance;

                    selectedID = i;
                    //highlight object 
                    m_displayList[selectedID].m_model->UpdateEffects([&](IEffect* effect) {
                        auto highlight = dynamic_cast<IEffectFog*>(effect);
                        if (highlight) {
                            highlight->SetFogEnabled(true); highlight->SetFogStart(0); highlight->SetFogEnd(0);  highlight->SetFogColor(Colors::Red);}  
                        });
                    //unhighlight prev object 
                    if(m_lastID != -1 && m_lastID != selectedID)
                        m_displayList[m_lastID].m_model->UpdateEffects([&](IEffect* effect) {
                            auto highlight = dynamic_cast<IEffectFog*>(effect);
                            if (highlight){ highlight->SetFogEnabled(false); highlight->SetFogStart(0); highlight->SetFogEnd(0); highlight->SetFogColor(Colors::Red);
                           } //unhighlight 
                            });
                }
            }
        }
    }

    //if we got a hit.  return it.  
    m_lastID = selectedID;
    return selectedID;
}

void XM_CALLCONV Game::DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color)
{
    m_deviceResources->PIXBeginEvent(L"Draw grid");

    auto context = m_deviceResources->GetD3DDeviceContext();
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    context->RSSetState(m_states->CullCounterClockwise());

    m_batchEffect->Apply(context);

    context->IASetInputLayout(m_batchInputLayout.Get());

    m_batch->Begin();

    xdivs = std::max<size_t>(1, xdivs);
    ydivs = std::max<size_t>(1, ydivs);

    for (size_t i = 0; i <= xdivs; ++i)
    {
        float fPercent = float(i) / float(xdivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, yAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, yAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    for (size_t i = 0; i <= ydivs; i++)
    {
        float fPercent = float(i) / float(ydivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, xAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, xAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    m_batch->End();

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

void Game::BuildDisplayList(std::vector<SceneObject> * SceneGraph)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	if (!m_displayList.empty())		//is the vector empty
	{
		m_displayList.clear();		//if not, empty it
	}

	//for every item in the scenegraph
	int numObjects = SceneGraph->size();
	for (int i = 0; i < numObjects; i++)
	{
		
		//create a temp display object that we will populate then append to the display list.
		DisplayObject newDisplayObject;
		
		//load model
		std::wstring modelwstr = StringToWCHART(SceneGraph->at(i).model_path);							//convect string to Wchar
		newDisplayObject.m_model = Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)

		//Load Texture
		std::wstring texturewstr = StringToWCHART(SceneGraph->at(i).tex_diffuse_path);								//convect string to Wchar
		HRESULT rs;
		rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource

		//if texture fails.  load error default
		if (rs)
		{
			CreateDDSTextureFromFile(device, L"database/data/Error.dds", nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
		}

		//apply new texture to models effect
		newDisplayObject.m_model->UpdateEffects([&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{	
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject.m_texture_diffuse);			
			}
		});

		//set position
		newDisplayObject.m_position.x = SceneGraph->at(i).posX;
		newDisplayObject.m_position.y = SceneGraph->at(i).posY;
		newDisplayObject.m_position.z = SceneGraph->at(i).posZ;
		
		//setorientation
		newDisplayObject.m_orientation.x = SceneGraph->at(i).rotX;
		newDisplayObject.m_orientation.y = SceneGraph->at(i).rotY;
		newDisplayObject.m_orientation.z = SceneGraph->at(i).rotZ;

		//set scale
		newDisplayObject.m_scale.x = SceneGraph->at(i).scaX;
		newDisplayObject.m_scale.y = SceneGraph->at(i).scaY;
		newDisplayObject.m_scale.z = SceneGraph->at(i).scaZ;

		//set wireframe / render flags
		newDisplayObject.m_render		= SceneGraph->at(i).editor_render;
		newDisplayObject.m_wireframe	= SceneGraph->at(i).editor_wireframe;

		newDisplayObject.m_light_type		= SceneGraph->at(i).light_type;
		newDisplayObject.m_light_diffuse_r	= SceneGraph->at(i).light_diffuse_r;
		newDisplayObject.m_light_diffuse_g	= SceneGraph->at(i).light_diffuse_g;
		newDisplayObject.m_light_diffuse_b	= SceneGraph->at(i).light_diffuse_b;
		newDisplayObject.m_light_specular_r = SceneGraph->at(i).light_specular_r;
		newDisplayObject.m_light_specular_g = SceneGraph->at(i).light_specular_g;
		newDisplayObject.m_light_specular_b = SceneGraph->at(i).light_specular_b;
		newDisplayObject.m_light_spot_cutoff = SceneGraph->at(i).light_spot_cutoff;
		newDisplayObject.m_light_constant	= SceneGraph->at(i).light_constant;
		newDisplayObject.m_light_linear		= SceneGraph->at(i).light_linear;
		newDisplayObject.m_light_quadratic	= SceneGraph->at(i).light_quadratic;
		
		m_displayList.push_back(newDisplayObject);	
	}
}

void Game::BuildDisplayChunk(ChunkObject * SceneChunk)
{
	//populate our local DISPLAYCHUNK with all the chunk info we need from the object stored in toolmain
	//which, to be honest, is almost all of it. Its mostly rendering related info so...
	m_displayChunk.PopulateChunkData(SceneChunk);		//migrate chunk data
	m_displayChunk.LoadHeightMap(m_deviceResources);
	m_displayChunk.m_terrainEffect->SetProjection(m_projection);
	m_displayChunk.InitialiseBatch();
}

void Game::SaveDisplayChunk(ChunkObject * SceneChunk)
{
	m_displayChunk.SaveHeightMap();			//save heightmap to file.
}

//stores id of copied object
void Game::copyObj(int oldObj)
{
    m_copiedID = oldObj;
}

//instantiates object from copiedID. most of this was copied from build display list 
void Game::pasteObj(std::vector<SceneObject>& SceneGraph)   //scenegraph is passed in as a reference so that it can be updated with the new object. This allows the new object to be copied, saved and manipulated
{
    auto device = m_deviceResources->GetD3DDevice();
    auto devicecontext = m_deviceResources->GetD3DDeviceContext();

    DisplayObject newDisplayObject;

    //load model
    std::wstring modelwstr = StringToWCHART(SceneGraph.at(m_copiedID).model_path);							//convect string to Wchar
    newDisplayObject.m_model = Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)

    //Load Texture
    std::wstring texturewstr = StringToWCHART(SceneGraph.at(m_copiedID).tex_diffuse_path);								//convect string to Wchar
    HRESULT rs;
    rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource

    //if texture fails.  load error default
    if (rs)
    {
        CreateDDSTextureFromFile(device, L"database/data/Error.dds", nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
    }

    //apply new texture to models effect
    newDisplayObject.m_model->UpdateEffects([&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
    {
        auto lights = dynamic_cast<BasicEffect*>(effect);
        if (lights)
        {
            lights->SetTexture(newDisplayObject.m_texture_diffuse);
        }
    });

    //set position     
    newDisplayObject.m_position.x = m_CameraManager->getCamLookAt().x;  //changed position of object to appear in front of the camera. 
    newDisplayObject.m_position.y = m_CameraManager->getCamLookAt().y;
    newDisplayObject.m_position.z = m_CameraManager->getCamLookAt().z;

    //setorientation
    newDisplayObject.m_orientation.x = SceneGraph.at(m_copiedID).rotX;
    newDisplayObject.m_orientation.y = SceneGraph.at(m_copiedID).rotY;
    newDisplayObject.m_orientation.z = SceneGraph.at(m_copiedID).rotZ;

    //set scale
    newDisplayObject.m_scale.x = SceneGraph.at(m_copiedID).scaX;
    newDisplayObject.m_scale.y = SceneGraph.at(m_copiedID).scaY;
    newDisplayObject.m_scale.z = SceneGraph.at(m_copiedID).scaZ;

    //set wireframe / render flags
    newDisplayObject.m_render = SceneGraph.at(m_copiedID).editor_render;
    newDisplayObject.m_wireframe = SceneGraph.at(m_copiedID).editor_wireframe;

    newDisplayObject.m_light_type = SceneGraph.at(m_copiedID).light_type;
    newDisplayObject.m_light_diffuse_r = SceneGraph.at(m_copiedID).light_diffuse_r;
    newDisplayObject.m_light_diffuse_g = SceneGraph.at(m_copiedID).light_diffuse_g;
    newDisplayObject.m_light_diffuse_b = SceneGraph.at(m_copiedID).light_diffuse_b;
    newDisplayObject.m_light_specular_r = SceneGraph.at(m_copiedID).light_specular_r;
    newDisplayObject.m_light_specular_g = SceneGraph.at(m_copiedID).light_specular_g;
    newDisplayObject.m_light_specular_b = SceneGraph.at(m_copiedID).light_specular_b;
    newDisplayObject.m_light_spot_cutoff = SceneGraph.at(m_copiedID).light_spot_cutoff;
    newDisplayObject.m_light_constant = SceneGraph.at(m_copiedID).light_constant;
    newDisplayObject.m_light_linear = SceneGraph.at(m_copiedID).light_linear;
    newDisplayObject.m_light_quadratic = SceneGraph.at(m_copiedID).light_quadratic;

    SceneObject s = SceneGraph.at(m_copiedID);
    s.posX = m_CameraManager->getCamLookAt().x;     //object position set to cam pos before it is added to scene graph. 
    s.posY = m_CameraManager->getCamLookAt().y;
    s.posZ = m_CameraManager->getCamLookAt().z;

   // s.posY += 1.0f;
    SceneGraph.push_back(s);    //new object passed back to scene graph
    m_displayList.push_back(newDisplayObject);  //and display list 
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif


#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);

    m_fxFactory = std::make_unique<EffectFactory>(device);
	m_fxFactory->SetDirectory(L"database/data/"); //fx Factory will look in the database directory
	m_fxFactory->SetSharing(false);	//we must set this to false otherwise it will share effects based on the initial tex loaded (When the model loads) rather than what we will change them to.

    m_sprites = std::make_unique<SpriteBatch>(context);

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    m_batchEffect = std::make_unique<BasicEffect>(device);
    m_batchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");

    m_shape = GeometricPrimitive::CreateTeapot(context, 4.f, 8);

    // SDKMESH has to use clockwise winding with right-handed coordinates, so textures are flipped in U
    m_model = Model::CreateFromSDKMESH(device, L"tiny.sdkmesh", *m_fxFactory);
	

    // Load textures
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf())
    );

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"windowslogo.dds", nullptr, m_texture2.ReleaseAndGetAddressOf())
    );
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );

    m_batchEffect->SetProjection(m_projection);
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_batch.reset();
    m_batchEffect.reset();
    m_font.reset();
    m_shape.reset();
    m_model.reset();
    m_texture1.Reset();
    m_texture2.Reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

std::wstring StringToWCHART(std::string s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "ParticleEmitter.h"

//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
    footstepTimer = 0.0f;
    particleEmitter.SetPosition(DirectX::SimpleMath::Vector3(-15.0f, 30.0f, 15.0f));
    particleAngle = 0.001f;
}

Game::~Game()
{
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
    //int sign = -1;
	m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    
	//setup light
	m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(0.0f, 15.0f, 0.0f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	//setup camera
    
	m_Camera01.setPosition(Vector3(0.0f, 10.0f, 10.0f));
	m_Camera01.setRotation(Vector3(-90.0f, -180.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 

	
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

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"night-sound4.wav");
    m_effect3 = m_soundEffect->CreateInstance();
    m_footstepsEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"footsteps3.wav");
    m_effect2 = m_footstepsEffect->CreateInstance();
    m_night = std::make_unique<SoundEffect>(m_audEngine.get(), L"morning-sound.wav");
    m_effect1 = m_night->CreateInstance();

    m_effect1->Play(true);
    m_effect2->Play(true);
    m_effect3->Play(true);

    m_effect2->SetVolume(0.0f);
    m_effect3->SetVolume(0.0f);

#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game
	
	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	//Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

	
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{


    particleEmitter.EmitParticles();
    footstepTimer += timer.GetElapsedSeconds() * 50;

    if (footstepTimer > 15.0f)
    {
        m_effect2->SetVolume(0.0f);
    }

    deltaTime = timer.GetElapsedSeconds() * 50;

    //note that currently.  Delta-time is not considered in the game object movement. 
    if (m_gameInputCommands.rotLeft)
    {
        Vector3 rotation = m_Camera01.getRotation();
        rotation.y = rotation.y + m_Camera01.getRotationSpeed() * deltaTime;
        m_Camera01.setRotation(rotation);
    }
    if (m_gameInputCommands.rotRight)
    {
        Vector3 rotation = m_Camera01.getRotation();
        rotation.y = rotation.y - m_Camera01.getRotationSpeed() * deltaTime;
        m_Camera01.setRotation(rotation);
    }
    if (m_gameInputCommands.rotUp)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position.y +=  m_Camera01.getMoveSpeed();
        m_Camera01.setPosition(position);
    }
    if (m_gameInputCommands.rotDown)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position.y -= m_Camera01.getMoveSpeed();
        m_Camera01.setPosition(position);
    }
    if (m_gameInputCommands.left)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position -= (m_Camera01.getLeft() * m_Camera01.getMoveSpeed()); //add the forward vector
        m_Camera01.setPosition(position);
        footstepTimer = 0.0f;
        m_effect2->SetVolume(0.4f);
    }
    if (m_gameInputCommands.right)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position += (m_Camera01.getLeft() * m_Camera01.getMoveSpeed()); //add the forward vector
        footstepTimer = 0.0f;
        m_effect2->SetVolume(0.4f);
        m_Camera01.setPosition(position);
    }
    if (m_gameInputCommands.forward)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed()); //add the forward vector
        m_Camera01.setPosition(position);
        footstepTimer = 0.0f;
        m_effect2->SetVolume(0.4f);
    }
    if (m_gameInputCommands.back)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed()); //add the forward vector
        m_Camera01.setPosition(position);
        footstepTimer = 0.0f;
        m_effect2->SetVolume(0.4f);
    }



    if (m_Camera01.getPosition().x > 90.0f)
    {
        m_Camera01.setPosition(DirectX::SimpleMath::Vector3(-90.0f, m_Camera01.getPosition().y, m_Camera01.getPosition().z));
    }
    if (m_Camera01.getPosition().x < -90.0f)
    {
        m_Camera01.setPosition(DirectX::SimpleMath::Vector3(90.0f, m_Camera01.getPosition().y, m_Camera01.getPosition().z));
    }
    if (m_Camera01.getPosition().z > 90.0f)
    {
        m_Camera01.setPosition(DirectX::SimpleMath::Vector3(m_Camera01.getPosition().x, m_Camera01.getPosition().y, -90.0f));
    }
    if (m_Camera01.getPosition().z < -90.0f)
    {
        m_Camera01.setPosition(DirectX::SimpleMath::Vector3(m_Camera01.getPosition().x, m_Camera01.getPosition().y, 90.0f));
    }


	m_Camera01.Update();	//camera update.

	m_view = m_Camera01.getCameraMatrix();
	m_world = Matrix::Identity;


    //QUATERNION ROTATION FOR LIGHT REVOLVING
    Quaternion pos = { m_Light.getPosition().x, m_Light.getPosition().y, m_Light.getPosition().z, 1.0f };
    Quaternion particleEmitterRotation = SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 0, 1), particleAngle);//SimpleMath::Quaternion(0.0f, 1.0f, 0.0f, 1.0f);//::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 1, 0), 2.0f * (XM_PI / 100));
    Quaternion particleEmitterRotationInverse = particleEmitterRotation;
    particleEmitterRotationInverse.Conjugate();
    Quaternion finalPos = SimpleMath::Quaternion::Concatenate(pos, particleEmitterRotation);
    finalPos = SimpleMath::Quaternion::Concatenate(particleEmitterRotationInverse, finalPos);
    m_Light.setPosition(finalPos.x, finalPos.y, finalPos.z);

    if (finalPos.y <= 0.0f)
    {
        m_effect1->SetVolume(0.0f);
        m_effect3->SetVolume(1.0f);
    }
    else
    {
        m_effect3->SetVolume(0.0f);
        m_effect1->SetVolume(1.0f);
    }

    if (particleEmitter.GetPosition().x <= -150.0f)
    {
        sign *= -1;
    }
    else if (particleEmitter.GetPosition().x >= 150.0f)
    {
        sign *= -1;
    }
    particleEmitter.SetPosition(SimpleMath::Vector3(particleEmitter.GetPosition().x + sign, particleEmitter.GetPosition().y, particleEmitter.GetPosition().z));

    particleEmitter.Update(timer);
    


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

            //m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

  
	if (m_input.Quit())
	{
		ExitGame();
	}
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
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();

	//Set Rendering states. 
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());

    RenderModels();

    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

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

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

void Game::RenderModels()
{

    auto context = m_deviceResources->GetD3DDeviceContext();
    
    
    //prepare transform for terrain object. 
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(3.0f);
    SimpleMath::Matrix translation = SimpleMath::Matrix::CreateTranslation(0.0f, -0.6f, 0.0f);
    m_world = m_world * scale * translation;

    //setup and draw terrain
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, terrainTexture.Get());
    terrainModel.Render(context);

    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    scale = SimpleMath::Matrix::CreateScale(2.0f);
    translation = SimpleMath::Matrix::CreateTranslation(0.0f, -0.6f, 0.0f);
    m_world = m_world * scale * translation;

    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, japaneseTempleRoofTexture.Get());
    japaneseTempleRoofModel.Render(context);


    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, japaneseTempleBaseTexture.Get());
    japaneseTempleBaseModel.Render(context);

    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    scale = SimpleMath::Matrix::CreateScale(2.9f);
    
    m_world = m_world * scale;

    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, wallTexture.Get());
    wallModel.Render(context);

    m_world = SimpleMath::Matrix::Identity; //set world back to identity

    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, treeTexture.Get());
    treeModel.Render(context);

    
    //PARTICLE RENDERING

    //std::vector<Particle> 
    particleCollection = particleEmitter.GetParticleCollection();
    int i = 0;
    for (auto& particle : particleCollection)
    {
        if (!particle.active)
        {
            continue;

        }
        else
        {
            m_world = SimpleMath::Matrix::Identity; //set world back to identity
            scale = SimpleMath::Matrix::CreateScale(0.7f);


            m_world = m_world * scale * particle.translation;
            m_BasicShaderPair.EnableShader(context);
            if (i % 2 == 0)
            {
                m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, particleRedTexture.Get());
                particleRedModel.Render(context);
                i++;
            }
            else
            {
                m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, particleBlueTexture.Get());
                particleBlueModel.Render(context);
                i++;
            }
            
            
        }

    }
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

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//setup our test model
	terrainModel.InitializeModel(device, "Models/snowTerrain.obj");
    japaneseTempleBaseModel.InitializeModel(device, "Models/BaseDivideBy100.obj");
    japaneseTempleRoofModel.InitializeModel(device, "Models/RoofsDividedBy100.obj");
    treeModel.InitializeModel(device, "Models/Attached_trees.obj");
    particleRedModel.InitializeSphere(device);
    particleBlueModel.InitializeSphere(device);
    wallModel.InitializeModel(device, "Models/skybox_final.obj");
	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");

    //load Textures
	CreateDDSTextureFromFile(device, L"Textures/grass.dds",		nullptr, terrainTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/roof.dds",		nullptr, japaneseTempleRoofTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/base.dds", nullptr, japaneseTempleBaseTexture.ReleaseAndGetAddressOf()); 
    CreateDDSTextureFromFile(device, L"Textures/redParticle.dds", nullptr, particleRedTexture.ReleaseAndGetAddressOf()); 
    CreateDDSTextureFromFile(device, L"Textures/blueParticle.dds", nullptr, particleBlueTexture.ReleaseAndGetAddressOf()); 
    CreateDDSTextureFromFile(device, L"Textures/tree1.dds", nullptr, treeTexture.ReleaseAndGetAddressOf()); 
    CreateDDSTextureFromFile(device, L"Textures/skybox-final.dds", nullptr, wallTexture.ReleaseAndGetAddressOf()); 
    
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
        100.0f
    );
}


void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

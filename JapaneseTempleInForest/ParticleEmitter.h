#pragma once
using namespace DirectX;
#include "pch.h"
#include "Particle.h"
#include "StepTimer.h"

struct ParticleDetails
{
	DirectX::SimpleMath::Vector3 position;
	float lifeTime = 1.0f;
};

class ParticleEmitter
{
	

public:
	ParticleEmitter();
	void EmitParticles();
	void Update(DX::StepTimer const& timer);
	void SetPosition(DirectX::SimpleMath::Vector3 position);
	std::vector<Particle> GetParticleCollection();
	DirectX::SimpleMath::Vector3 GetPosition();

	

private:

	std::vector<Particle> particleCollection;
	int particleCount;
	DirectX::SimpleMath::Vector3 emitterPosition;
};
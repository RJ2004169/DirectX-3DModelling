#include "pch.h"
#include "ParticleEmitter.h"
#include "Particle.h"
#include "StepTimer.h"


ParticleEmitter::ParticleEmitter()
{
	particleCount = 0;
	particleCollection.resize(5000);
}


void ParticleEmitter::EmitParticles()
{
	if (particleCount < 5000)
	{
		Particle& particle = particleCollection[particleCount];
		particle.active = true;
		particle.position = emitterPosition;
		particleCount++;
	}
	

}

void ParticleEmitter::Update(DX::StepTimer const& timer)
{
	int sign = 1;
	float deltaTime = timer.GetElapsedSeconds() * 30;
	for (auto& particle : particleCollection)
	{
		//make this or and see if it stops after lifetime or circles first
		if (!particle.active)
			continue;
		if (particle.travelCount > 500)
		{
			particle.position = emitterPosition;
			particle.travelCount = 0;
		}
		float angle = 0.3 *particle.travelCount;
		particle.travelCount++;

		//ARCHIMEDES SPIRAL PLOTTING
		particle.position.x += ((0.1 + 0.05 * angle*0.5) * cos(angle))* deltaTime;
		particle.position.y += ((0.1 + 0.05 * angle*0.5) * sin(angle))* deltaTime;
		particle.position.z += sign;

		if (particle.travelCount % 50 == 0)
		{
			sign *= -1;
		}
		particle.translation = SimpleMath::Matrix::CreateTranslation(particle.position.x, particle.position.y, particle.position.z);
		particle.lifeRemaining -= 0.01f;
	}
}

void ParticleEmitter::SetPosition(DirectX::SimpleMath::Vector3 pos)
{
	emitterPosition = pos;
}

std::vector<Particle> ParticleEmitter::GetParticleCollection()
{
	return particleCollection;
}

DirectX::SimpleMath::Vector3 ParticleEmitter::GetPosition()
{
	return emitterPosition;
}

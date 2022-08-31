#pragma once
using namespace DirectX;


struct Particle
{
	DirectX::SimpleMath::Vector3 position;
	float lifeTime = 1.0f, lifeRemaining = 10.0f;
	bool active = false;
	int travelCount = 0;
	SimpleMath::Matrix translation;
};

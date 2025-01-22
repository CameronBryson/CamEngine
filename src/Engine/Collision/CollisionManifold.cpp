#include "pch.hpp"
#include "CollisionManifold.hpp"
#include <cmath>

#include "Engine/Util/GameSettings.hpp"
#include "glm/ext/quaternion_geometric.hpp"

CollisionManifold::CollisionManifold(const glm::vec3& normal, const float penetration_depth, CTransform& transform1, CTransform& transform2,
									 CDynamicBody* dynamic_body1, CDynamicBody* dynamic_body2)
	: penetrationDepth(penetration_depth),
	  normal(normal),
	  mDynamicBody1(dynamic_body1),
	  mDynamicBody2(dynamic_body2),
	  mTransform1(transform1),
	  mTransform2(transform2)
{
	printf("Depth: %f\n", penetrationDepth);
	printf("Normal%f\n %f\n %f\n", normal.x, normal.y, normal.z);
}
void CollisionManifold::resolveCollision() const
{
	if (penetrationDepth == 0.0f)
	{
		return;
	}
	/*if (std::abs(penetrationDepth) < settings::EPSILON)
	{
		return;
	}*/

	if( mDynamicBody1 != nullptr && mDynamicBody2 != nullptr )
	{
	resolveDynamicVsDynamic();
	}
	else if( mDynamicBody1 != nullptr )
	{
	resolveDynamicVsNotDynamic(true);
	}
	else if( mDynamicBody2 != nullptr )
	{
	resolveDynamicVsNotDynamic(false);
	}
	// No action needed for STATIC vs STATIC
}
void CollisionManifold::resolveDynamicVsDynamic() const
{
	mTransform1.position -= normal * penetrationDepth * 0.5f;
	mTransform2.position += normal * penetrationDepth * 0.5f;
	const glm::vec3 relativeVelocity = mDynamicBody2->velocity - mDynamicBody1->velocity;
	const float velocityAlongNormal = glm::dot(relativeVelocity, normal);
	const glm::vec3 impulse = velocityAlongNormal * normal;
	const float e = (mDynamicBody1->elasticity + mDynamicBody2->elasticity) / 2.0f;
	mDynamicBody1->velocity += impulse * e;
	mDynamicBody2->velocity -= impulse * e;
}
void CollisionManifold::resolveDynamicVsNotDynamic(bool is_first) const
{
	if( is_first )
	{
	mTransform1.position -= normal * penetrationDepth;

		const float velocityAlongNormal = glm::dot(mDynamicBody1->velocity, normal);
	mDynamicBody1->velocity -= 2.0f * velocityAlongNormal * normal * mDynamicBody1->elasticity;
	}
	else
	{
	mTransform2.position += normal * penetrationDepth;

		const float velocityAlongNormal = glm::dot(mDynamicBody2->velocity, normal);
	mDynamicBody2->velocity -= 2.0f * velocityAlongNormal * normal * mDynamicBody2->elasticity;
	}
}


#include "Collision.hpp"
CollisionManifold::CollisionManifold(const glm::vec3 normal, const float penetration_depth, CTransform& transform1, CTransform& transform2,
    CDynamicBody* dynamic_body1, CDynamicBody* dynamic_body2)
    : penetrationDepth(penetration_depth),
      normal(normal),
      dynamicBody1(dynamic_body1),
      dynamicBody2(dynamic_body2),
      transform1(transform1),
      transform2(transform2)
{
    printf("Depth: %f\n", penetrationDepth);
    printf("Normal%f\n %f\n %f\n", normal.x, normal.y, normal.z);
}
void CollisionManifold::resolveCollision() const
{
    if(penetrationDepth == 0){
	return;
    }

    if( dynamicBody1 != nullptr && dynamicBody2 != nullptr )
    {
	resolveDynamicVsDynamic();
    }
    else if( dynamicBody1 != nullptr )
    {
	resolveDynamicVsNotDynamic(true);
    }
    else if( dynamicBody2 != nullptr )
    {
	resolveDynamicVsNotDynamic(false);
    }
    // No action needed for STATIC vs STATIC
}
void CollisionManifold::resolveDynamicVsDynamic() const
{
    transform1.position -= normal * penetrationDepth * 0.5f;
    transform2.position += normal * penetrationDepth * 0.5f;
    glm::vec3 relativeVelocity = dynamicBody2->velocity - dynamicBody1->velocity;
    float velocityAlongNormal = glm::dot(relativeVelocity, normal);
    glm::vec3 impulse = velocityAlongNormal * normal;
    float e = (dynamicBody1->elasticity + dynamicBody2->elasticity) / 2.0f;
    dynamicBody1->velocity += impulse * e;
    dynamicBody2->velocity -= impulse * e;
}
void CollisionManifold::resolveDynamicVsNotDynamic(bool is_first) const
{
    if( is_first )
    {
	transform1.position -= normal * penetrationDepth;

	float velocityAlongNormal = glm::dot(dynamicBody1->velocity, normal);
	dynamicBody1->velocity -= 2.0f * velocityAlongNormal * normal * dynamicBody1->elasticity;
    }
    else
    {
	transform2.position += normal * penetrationDepth;

	float velocityAlongNormal = glm::dot(dynamicBody2->velocity, normal);
	dynamicBody2->velocity -= 2.0f * velocityAlongNormal * normal * dynamicBody2->elasticity;
    }
}


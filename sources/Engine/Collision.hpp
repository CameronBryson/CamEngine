#pragma once
#include "glm/glm.hpp"

#include "Components.hpp"

class collision_manifold
{
public:
    collision_manifold(const glm::vec3 normal, const float penetration_depth, c_transform& transform1,
                       c_transform& transform2, c_dynamic_body* dynamic_body1, c_dynamic_body* dynamic_body2)
        : penetration_depth_(penetration_depth), normal(normal), dynamic_body1(dynamic_body1), dynamic_body2(dynamic_body2), transform1_(transform1),
          transform2_(transform2)
    {

        printf("Depth: %f\n", penetration_depth_);
        printf("Normal%f\n %f\n %f\n", normal.x, normal.y, normal.z);
    }
    ~collision_manifold() = default;

    void resolve_collision() const
    {
        if (dynamic_body1!=nullptr&&dynamic_body2!=nullptr)
        {
            resolve_dynamic_vs_dynamic();
        }
        else if (dynamic_body1!=nullptr)
        {
            resolve_dynamic_vs_not_dynamic(true);
        }
        else if (dynamic_body2!=nullptr)
        {
            resolve_dynamic_vs_not_dynamic(false);
        }
        // No action needed for STATIC vs STATIC
    }

    float penetration_depth_ = 0;
    glm::vec3 normal;

private:
    void resolve_dynamic_vs_dynamic() const
    {
        transform1_.position -= normal * penetration_depth_ * 0.5f;
        transform2_.position += normal * penetration_depth_ * 0.5f;
        glm::vec3 relativeVelocity = dynamic_body2->velocity- dynamic_body1->velocity;
        float velocityAlongNormal = glm::dot(relativeVelocity,normal);
        glm::vec3 impulse = velocityAlongNormal * normal;
        dynamic_body1->velocity+=impulse;
        dynamic_body2->velocity-=impulse;

    }

    void resolve_dynamic_vs_not_dynamic(bool is_first) const
    {
        if(is_first)
        {
            transform1_.position -= normal * penetration_depth_;

            float velocityAlongNormal = glm::dot(dynamic_body1->velocity,normal);
            dynamic_body1->velocity -= 2.0f * velocityAlongNormal * normal;
        }
        else
        {
            transform2_.position += normal * penetration_depth_;

            float velocityAlongNormal = glm::dot(dynamic_body2->velocity,normal);
            dynamic_body2->velocity -= 2.0f * velocityAlongNormal * normal;
        }
    }

    c_dynamic_body* dynamic_body1;
    c_dynamic_body* dynamic_body2;
    c_transform& transform1_;
    c_transform& transform2_;
};

#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"


namespace implemented
{
    class Camera_DroneSim
    {
     public:
        Camera_DroneSim()
        {
            position    = glm::vec3(0, 2, 5);
            forward     = glm::vec3(0, 0, -1);
            up          = glm::vec3(0, 1, 0);
            right       = glm::vec3(1, 0, 0);
            distanceToTarget = 4;
        }

        Camera_DroneSim(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            Set(position, center, up);
        }

        ~Camera_DroneSim()
        { }

        // Update Camera_DroneSim
        void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            this->position = position;
            forward     = glm::normalize(center - position);
            right       = glm::cross(forward, up);
            this->up    = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            // Translates the Camera_DroneSim using the `dir` vector computed from
            // `forward`. Movement will always keep the Camera_DroneSim at the same
            // height. For example, if you rotate your head up/down, and then
            // walk forward, then you will still keep the same relative
            // distance (height) to the ground!
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            // TODO(student): Translate the Camera_DroneSim using the `forward` vector.
            // What's the difference between `TranslateForward()` and
            // `MoveForward()`?
            position = position + glm::normalize(forward) * distance;

        }

        void TranslateUpward(float distance)
        {
            // TODO(student): Translate the Camera_DroneSim using the `up` vector.
            position = position + glm::normalize(up) * distance;

        }

        void TranslateRight(float distance)
        {
            // TODO(student): See instructions below. Read the entire thing!
            // You need to translate the Camera_DroneSim using the `right` vector.
            // Usually, however, translation using Camera_DroneSim's `right` vector
            // is not very useful, because if the Camera_DroneSim is rotated around the
            // `forward` vector, then the translation on the `right` direction
            // will have an undesired effect, more precisely, the Camera_DroneSim will
            // get closer or farther from the ground. The solution is to
            // actually use the projected `right` vector (projected onto the
            // ground plane), which makes more sense because we will keep the
            // same distance from the ground plane.
            glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
            position += dir * distance;
        }

        void RotateFirstPerson_OX(float angle)
        {
            // TODO(student): Compute the new `forward` and `up` vectors.
            // Don't forget to normalize the vectors! Use `glm::rotate()`.

            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, right);
            forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(forward, 1.0f)));
            up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(up, 1.0f)));

        }

        void RotateFirstPerson_OY(float angle)
        {
            // TODO(student): Compute the new `forward`, `up` and `right`
            // vectors. Use `glm::rotate()`. Don't forget to normalize the
            // vectors!
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(forward, 1.0f)));
            right = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(right, 1.0f)));
            up = glm::normalize(glm::cross(right, forward));

        }

        void RotateFirstPerson_OZ(float angle)
        {
            // TODO(student): Compute the new `right` and `up`. This time,
            // `forward` stays the same. Use `glm::rotate()`. Don't forget
            // to normalize the vectors!
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, forward);
            right = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(right, 1.0f)));
            up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(up, 1.0f)));
        }

        void RotateThirdPerson_OX(float angle)
        {
            // TODO(student): Rotate the Camera_DroneSim in third-person mode around
            // the OX axis. Use `distanceToTarget` as translation distance.
            position -= forward * distanceToTarget;
            RotateFirstPerson_OX(angle);
            position += forward * distanceToTarget;
        }

        void RotateThirdPerson_OY(float angle)
        {
            // TODO(student): Rotate the Camera_DroneSim in third-person mode around
            // the OY axis.
            position -= forward * distanceToTarget;
            RotateFirstPerson_OY(angle);
            position += forward * distanceToTarget;

        }

        void RotateThirdPerson_OZ(float angle)
        {
            // TODO(student): Rotate the Camera_DroneSim in third-person mode around
            // the OZ axis.
            position -= forward * distanceToTarget;
            RotateFirstPerson_OZ(angle);
            position += forward * distanceToTarget;

        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

     public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
        
    };
}   // namespace implemented

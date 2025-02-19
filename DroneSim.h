#pragma once

#include "components/simple_scene.h"
#include "lab_m1/DroneSim/DroneSim_camera.h"


namespace m1
{
    class DroneSim : public gfxc::SimpleScene
    {
     public:
        DroneSim();
        ~DroneSim();

        void Init() override;
        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void GenerateGrid(const std::string& name, int m, int n, float size);
        Mesh* CreateCone(const std::string& name, int resolution, float radius, float height, glm::vec3 color);
        Mesh* CreateCylinder(const std::string& name, int resolution, float radius, float height, glm::vec3 color);
        void RenderTree(glm::vec3 position);
        void PlaceTreesRandomly(int treeCount, float terrainSize, float minDistance);
        bool CheckCollision(const glm::vec3& droneMin, const glm::vec3& droneMax, const glm::vec3& treeMin, const glm::vec3& treeMax);
        Mesh* CreateCubeShape(const std::string& name, glm::vec3 color);
        Mesh* CreateCircle(const std::string& name, int resolution, float radius, glm::vec3 color);
        void HandleDroneCollision();
        void assembleDrone(glm::vec3 dronePosition, float deltaTimeSeconds);
        glm::vec3 getRandomFieldPosition(float terrainSize);
        void handlePackage(float terrainSize, const std::vector<glm::vec3>& treePositions);
        void handleCompass(const glm::vec3& targetPosition);
        void RenderMinimap();

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        

     protected:
        implemented::Camera_DroneSim *Camera_DroneSim;
        implemented::Camera_DroneSim *Camera_Minimap;

        glm::mat4 projectionMatrix;
        bool renderCamera_DroneSimTarget;

        float fov = RADIANS(60);
        float nearPlane = 0.01f;
        float farPlane = 200.0f;
        float orthoSize = 5.0f;
        bool isPerspective = true;
        float orthoWidth = 5.0f;
        float orthoHeight = 5.0f;


        float propellerRotation = 0.0f;
        glm::vec3 dronePosition;

        glm::vec3 body1_scale = glm::vec3(1.5f, 0.1f, 0.1f);
        glm::vec3 body2_scale = glm::vec3(1.5f, 0.1f, 0.1f);
        glm::vec3 connection_cube_scale = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 propeller_scale = glm::vec3(0.4f, 0.02f, 0.02f);
        glm::vec3 propeller_offset = glm::vec3(0.7f, 1.15f, 0.7f);
        float body_height = 1.0f;

        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        bool placedTrees = false;
        std::vector<glm::vec3> treePositions;

        struct Hitbox {
            glm::vec3 min;
            glm::vec3 max;
        };

        std::vector<Hitbox> treeHitboxes;
        glm::vec3 previousDronePosition;

        glm::vec3 randomCubePosition;
        bool cubePlaced;
        bool circlePlaced;
        glm::vec3 circlePosition;
        bool packageAttached;
        int deliveredCount;

    };
}   // namespace m1

#include "lab_m1/DroneSim/DroneSim.h"
#include "lab_m1/DroneSim/DroneSim_camera.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


DroneSim::DroneSim()
{
}


DroneSim::~DroneSim()
{
}

void DroneSim::Init()
{
    // Setez camera
    renderCamera_DroneSimTarget = false;
    Camera_DroneSim = new implemented::Camera_DroneSim();
    Camera_DroneSim->Set(glm::vec3(0, 10, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    Camera_Minimap = new implemented::Camera_DroneSim();
    Camera_Minimap->Set(glm::vec3(0, 60, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));


    // Generez formele
    meshes["cone"] = CreateCone("cone", 32, 0.5f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    meshes["cylinder"] = CreateCylinder("cylinder", 32, 0.3f, 1.0f, glm::vec3(0.55f, 0.27f, 0.07f));
    meshes["droneCube"] = CreateCubeShape("droneCube", glm::vec3(0.2f, 0.2f, 0.2f));
    meshes["propellerCube"] = CreateCubeShape("propellerCube", glm::vec3(0.0f, 0.0f, 0.0f));
    meshes["targetCube"] = CreateCubeShape("targetCube", glm::vec3(1.0f, 0.0f, 0.0f));
    meshes["circle"] = CreateCircle("circle", 32, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    meshes["compass"] = CreateCircle("compass", 32, 2.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    meshes["cylinderMini"] = CreateCylinder("cylinderMini", 32, 0.3f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    meshes["coneMini"] = CreateCone("coneMini", 32, 0.5f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    dronePosition = glm::vec3(0, 0, 0);

    // Adaug shader-ul pt teren
    Shader* terrainShader = new Shader("TerrainShader");
    terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "DroneSim", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "DroneSim", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    terrainShader->CreateAndLink();
    shaders["TerrainShader"] = terrainShader;

    GenerateGrid("terrain", 100, 100, 50.0f);

    // Gestionez variabile globale
    placedTrees = false;
    previousDronePosition = glm::vec3(0, 0, 0);
    cubePlaced = false;
    circlePlaced = false;
    packageAttached = false;
    deliveredCount = 0;
}

void DroneSim::FrameStart()
{
    // Din laborator
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.0f, 0.9f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

Mesh* DroneSim::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    // Din laborator
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}

void DroneSim::GenerateGrid(const std::string& name, int numRows, int numCols, float gridSize) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Distanta dintre randuri / coloane
    float colSpace = gridSize / numCols;
    float rowSpace = gridSize / numRows;

    // Centrez terenul
    float halfWidth = gridSize / 2.0f;
    float halfHeight = gridSize / 2.0f;

    // Generez vertecsii
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {

            float x = col * colSpace - halfWidth;
            float z = row * rowSpace - halfHeight;

            glm::vec3 position = glm::vec3(x, 0, z);
            glm::vec3 normal = glm::vec3(0, 1, 0);
            glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);

            vertices.emplace_back(position, color, normal);
        }
    }

    // Generez indicii
    for (int row = 0; row < numRows - 1; row++) {
        for (int col = 0; col < numCols - 1; col++) {

            int upLeft = row * numCols + col;
            int bottomLeft = (row + 1) * numCols + col;

            int upRight = upLeft + 1;
            int bottomRight = bottomLeft + 1;

            // Primul triunghi
            indices.push_back(upLeft);
            indices.push_back(bottomLeft);
            indices.push_back(upRight);

            // Al doilea triunghi
            indices.push_back(upRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh* gridMesh = CreateMesh("terrain", vertices, indices);
    meshes["terrain"] = gridMesh;
}

Mesh* DroneSim::CreateCubeShape(const std::string& name, glm::vec3 color)
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-0.5f, -0.5f,  0.5f), color),
        VertexFormat(glm::vec3(0.5f, -0.5f,  0.5f), color),
        VertexFormat(glm::vec3(0.5f,  0.5f,  0.5f), color),
        VertexFormat(glm::vec3(-0.5f,  0.5f,  0.5f), color),
        VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f), color),
        VertexFormat(glm::vec3(0.5f, -0.5f, -0.5f), color),
        VertexFormat(glm::vec3(0.5f,  0.5f, -0.5f), color),
        VertexFormat(glm::vec3(-0.5f,  0.5f, -0.5f), color),
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        3, 2, 6, 6, 7, 3,
        1, 2, 6, 6, 5, 1,
        0, 3, 7, 7, 4, 0
    };

    return CreateMesh(name.c_str(), vertices, indices);
}


Mesh* DroneSim::CreateCircle(const std::string& name, int resolution, float radius, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float angleStep = 2.0f * glm::pi<float>() / resolution;
    glm::vec3 center = glm::vec3(0, 0, 0);

    vertices.emplace_back(center, color);

    for (int i = 0; i <= resolution; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.emplace_back(glm::vec3(x, 0, z), color);

        if (i > 0) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }

    indices.push_back(0);
    indices.push_back(resolution);
    indices.push_back(1);

    return CreateMesh(name.c_str(), vertices, indices);
}


Mesh* DroneSim::CreateCone(const std::string& name, int resolution, float radius, float height, glm::vec3 color) {
    Mesh* baseCircle = CreateCircle(name + "_base", resolution, radius, color);
    const std::vector<VertexFormat>& baseVertices = baseCircle->vertices;
    const std::vector<unsigned int>& baseIndices = baseCircle->indices;

    std::vector<VertexFormat> vertices = baseVertices;
    std::vector<unsigned int> indices = baseIndices;

    glm::vec3 apexPosition = glm::vec3(0, height, 0);
    vertices.emplace_back(apexPosition, color);
    int apexIndex = vertices.size() - 1;

    for (int i = 1; i < resolution; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(apexIndex);
    }

    indices.push_back(resolution);
    indices.push_back(1);
    indices.push_back(apexIndex);

    return CreateMesh(name.c_str(), vertices, indices);
}


Mesh* DroneSim::CreateCylinder(const std::string& name, int resolution, float radius, float height, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    Mesh* baseCircle = CreateCircle(name + "_base", resolution, radius, color);
    Mesh* topCircle = CreateCircle(name + "_top", resolution, radius, color);

    for (const auto& vertex : baseCircle->vertices) {
        vertices.emplace_back(vertex.position, color, vertex.normal);
    }
    for (const auto& vertex : topCircle->vertices) {
        vertices.emplace_back(vertex.position + glm::vec3(0, height, 0), color, vertex.normal);
    }

    for (int i = 0; i < resolution; i++) {
        int nextIndex = (i + 1) % resolution;
        indices.push_back(i);
        indices.push_back(resolution + i);
        indices.push_back(resolution + nextIndex);

        indices.push_back(i);
        indices.push_back(resolution + nextIndex);
        indices.push_back(nextIndex);
    }

    vertices.emplace_back(glm::vec3(0, 0, 0), color);
    for (const auto& index : baseCircle->indices) {
        indices.push_back(index);
    }

    vertices.emplace_back(glm::vec3(0, height, 0), color);
    for (const auto& index : topCircle->indices) {
        indices.push_back(index + resolution);
    }

    return CreateMesh(name.c_str(), vertices, indices);
}


glm::vec3 DroneSim::getRandomFieldPosition(float terrainSize) {
    float x = -terrainSize / 2.0f + static_cast<float>(rand()) / RAND_MAX * terrainSize;
    float y = 0.0f;
    float z = -terrainSize / 2.0f + static_cast<float>(rand()) / RAND_MAX * terrainSize;

    return glm::vec3(x, y, z);
}


void DroneSim::RenderTree(glm::vec3 position) {
    glm::mat4 modelMatrix;
    glm::mat4 savedMatrix;

    // Tunchiul
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f, 3.0f, 1.5f));
    savedMatrix = modelMatrix;
    RenderMesh(meshes["cylinder"], shaders["VertexNormal"], modelMatrix);

    // Hitbox
    glm::vec3 min = position - glm::vec3(1.5f, 0.0f, 1.5f);
    glm::vec3 max = position + glm::vec3(1.5f, 10.0f, 1.5f);
    treeHitboxes.push_back({ min, max });

    // Coroana copacului
    for (int i = 0; i < 3; i++) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, position + glm::vec3(0.0f, 3.0f + i * 1.5f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f - i, 2.5f, 3.0f - i));
        RenderMesh(meshes["cone"], shaders["VertexNormal"], modelMatrix);
    }

}


void DroneSim::PlaceTreesRandomly(int treeCount, float terrainSize, float minDistance) {
    for (int i = 0; i < treeCount; ++i) {
        glm::vec3 position;
        bool validPosition = false;

        // Repet pana gasesc o pozitie valida
        while (!validPosition) {
            position = getRandomFieldPosition(terrainSize);

            validPosition = true;

            if (glm::length(glm::vec2(position.x, position.z)) < 5.0f) {
                validPosition = false;
                continue;
            }

            for (const auto& other : treePositions) {
                if (glm::distance(position, other) < minDistance) {
                    validPosition = false;
                    break;
                }
            }
        }

        treePositions.push_back(position);
    }
}

void DroneSim::assembleDrone(glm::vec3 dronePosition, float deltaTimeSeconds) {
    glm::mat4 modelMatrix;

    // Jumatate din corpul dronei
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, dronePosition);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, body_height, 0));
    modelMatrix = glm::scale(modelMatrix, body1_scale);
    RenderMesh(meshes["droneCube"], shaders["VertexNormal"], modelMatrix);

    // Jumatate din corpul dronei
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, dronePosition);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, body_height, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, body2_scale);
    RenderMesh(meshes["droneCube"], shaders["VertexNormal"], modelMatrix);

    // Cuburi intermediare
    glm::vec3 cubes[] = {
        glm::vec3(0.7f, body_height + 0.1f, 0),
        glm::vec3(-0.7f, body_height + 0.1f, 0),
        glm::vec3(0.0f, body_height + 0.1f, 0.7f),
        glm::vec3(0.0f, body_height + 0.1f, -0.7f)
    };

    for (const auto& pos : cubes) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePosition);
        modelMatrix = glm::translate(modelMatrix, pos);
        modelMatrix = glm::scale(modelMatrix, connection_cube_scale);
        RenderMesh(meshes["droneCube"], shaders["VertexNormal"], modelMatrix);
    }

    // Elice
    propellerRotation += deltaTimeSeconds * 360.0f;
    if (propellerRotation > 360.0f) {
        propellerRotation -= 360.0f;
    }

    glm::vec3 propellerPositions[] = {
        glm::vec3(propeller_offset.x, propeller_offset.y, 0.0f),
        glm::vec3(-propeller_offset.x, propeller_offset.y, 0.0f),
        glm::vec3(0.0f, propeller_offset.y, propeller_offset.z),
        glm::vec3(0.0f, propeller_offset.y, -propeller_offset.z)
    };

    for (const auto& pos : propellerPositions) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePosition);
        modelMatrix = glm::translate(modelMatrix, pos);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(propellerRotation), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, propeller_scale);
        RenderMesh(meshes["propellerCube"], shaders["VertexNormal"], modelMatrix);
    }
}


bool DroneSim::CheckCollision(const glm::vec3& droneMin, const glm::vec3& droneMax, const glm::vec3& treeMin, const glm::vec3& treeMax) {
    return (droneMin.x <= treeMax.x && droneMax.x >= treeMin.x) &&
        (droneMin.y <= treeMax.y && droneMax.y >= treeMin.y) &&
        (droneMin.z <= treeMax.z && droneMax.z >= treeMin.z);
}

void DroneSim::HandleDroneCollision() {
    // Calculez hitbox-ul dronei
    glm::vec3 droneMin = dronePosition - glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 droneMax = dronePosition + glm::vec3(0.2f, 0.2f, 0.2f);

    // Verific coliziunile
    bool collisionDetected = false;
    for (const auto& hitbox : treeHitboxes) {
        if (CheckCollision(droneMin, droneMax, hitbox.min, hitbox.max)) {
            collisionDetected = true;
            break;
        }
    }

    // Daca nu exista coliziuni, actualizez pozitia dronei
    if (!collisionDetected) {
        dronePosition = Camera_DroneSim->GetTargetPosition() + glm::vec3(0.0f, -1.0f, 0.0f);
    }
    else {
        // Revin la pozitia anterioara in caz de coliziune
        Camera_DroneSim->position = previousDronePosition;
    }

    glm::vec3 droneOffset = glm::vec3(0.0f, -1.0f, 0.0f);
    dronePosition = Camera_DroneSim->GetTargetPosition() + droneOffset;


    // Ma asigur ca drona nu poate intra sub teren
    if (dronePosition.y < 0.0) {
        dronePosition.y = 0.0;
    }

    if (Camera_DroneSim->position.y < 0.5 + 1) {
        Camera_DroneSim->position.y = 0.5 + 1;
    }
}

void DroneSim::handlePackage(float terrainSize, const std::vector<glm::vec3>& treePositions) {
    glm::mat4 modelMatrix;

    if (!cubePlaced) {
        bool validPosition = false;

        while (!validPosition) {
            randomCubePosition = getRandomFieldPosition(terrainSize);
            validPosition = true;

            // Ma asigur ca nu se spawneaza cubul in copaci
            for (const auto& treePosition : treePositions) {
                float minDistance = 2.0f;
                if (glm::distance(randomCubePosition, treePosition) < minDistance) {
                    validPosition = false;
                    break;
                }
            }
        }

        cubePlaced = true;
    }

    glm::vec3 droneBottom = dronePosition + glm::vec3(0.0f, -1.0f, 0.0f);

    if (!packageAttached && glm::distance(droneBottom, randomCubePosition) < 1.0f) {
        packageAttached = true;
    }

    if (!packageAttached) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, randomCubePosition);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f)); // Cube size
        RenderMesh(meshes["targetCube"], shaders["VertexNormal"], modelMatrix);
    }
    else {
        // Daca pachetul e atasat de drona, ii modificam pozitia simultan cu drona
        randomCubePosition = dronePosition + glm::vec3(0.0f, -0.8f, 0.0f);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, randomCubePosition);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
        RenderMesh(meshes["targetCube"], shaders["VertexNormal"], modelMatrix);

        // Odata ce pachetul e preluat, randez si cercul
        if (!circlePlaced) {
            bool validPosition = false;

            while (!validPosition) {
                circlePosition = getRandomFieldPosition(terrainSize);

                validPosition = true;

                // Ma asigur de faptul ca cercul nu e spawnat intr-un copac
                for (const auto& treePosition : treePositions) {
                    float minDistance = 2.0f;
                    if (glm::distance(circlePosition, treePosition) < minDistance) {
                        validPosition = false;
                        break;
                    }
                }
            }

            circlePlaced = true;
        }

        // Contorizez livrarea cu succes a pachetului
        if (abs(droneBottom.x - circlePosition.x) < 1.0f && abs(droneBottom.z - circlePosition.z) < 1.0f) {
            cubePlaced = false;
            circlePlaced = false;
            packageAttached = false;

            cout << "DELIVERY SUCCESSFUL! -- Earned points: " << ++deliveredCount << endl;
        }

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, circlePosition);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f)); // Circle size
        RenderMesh(meshes["circle"], shaders["VertexNormal"], modelMatrix);
    }
}

void DroneSim::handleCompass(const glm::vec3& targetPosition) {
    glm::mat4 modelMatrix;
    glm::vec3 arrowPosition = glm::vec3(0, 1, 0);

    // Calculam directia
    glm::vec3 direction = targetPosition - arrowPosition;
    direction.y = 0;

    if (glm::length(direction) > 0.001f) {
        direction = glm::normalize(direction);
    }
    else {
        direction = glm::vec3(1, 0, 0);
    }

    // Baza busolei
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, arrowPosition);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(2.5f, 2.5f, 2.5f));
    RenderMesh(meshes["compass"], shaders["VertexNormal"], modelMatrix);

    // Calculez unghiul spre care arata busola
    float angle = glm::atan(direction.x, direction.z);

    // Randez tija busolei
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, arrowPosition);
    modelMatrix = glm::rotate(modelMatrix, angle - RADIANS(90), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(7.5f, 0.5f, 0.5f));
    RenderMesh(meshes["targetCube"], shaders["VertexNormal"], modelMatrix);

    // Randez prima parte a sagetii
    glm::mat4 headMatrix1 = glm::mat4(1);
    headMatrix1 = glm::translate(headMatrix1, arrowPosition);
    headMatrix1 = glm::rotate(headMatrix1, angle - RADIANS(90), glm::vec3(0, 1, 0));
    headMatrix1 = glm::translate(headMatrix1, glm::vec3(2.55f, 0, 1.18f));
    headMatrix1 = glm::rotate(headMatrix1, RADIANS(40.0f), glm::vec3(0, 1, 0));
    headMatrix1 = glm::scale(headMatrix1, glm::vec3(3.5f, 0.5f, 0.5f));
    RenderMesh(meshes["targetCube"], shaders["VertexNormal"], headMatrix1);

    // Randez a doua parte a sagetii
    glm::mat4 headMatrix2 = glm::mat4(1);
    headMatrix2 = glm::translate(headMatrix2, arrowPosition);
    headMatrix2 = glm::rotate(headMatrix2, angle - RADIANS(90), glm::vec3(0, 1, 0));
    headMatrix2 = glm::translate(headMatrix2, glm::vec3(2.55f, 0, -1.18f));
    headMatrix2 = glm::rotate(headMatrix2, RADIANS(-40.0f), glm::vec3(0, 1, 0));
    headMatrix2 = glm::scale(headMatrix2, glm::vec3(3.5f, 0.5f, 0.5f));
    RenderMesh(meshes["targetCube"], shaders["VertexNormal"], headMatrix2);
}

void DroneSim::RenderMinimap() {
    // Setez minimap-ul in coltul de jos stanga
    glm::ivec2 resolution = window->GetResolution();
    int minimapSize = 350;
    glViewport(0, 0, minimapSize, minimapSize);

    glClear(GL_DEPTH_BUFFER_BIT);

    // Randez terenul pe minimap
    glm::mat4 modelMatrix = glm::mat4(1);
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], modelMatrix);

    // Calculez unghiul sagetii
    glm::vec3 forwardXZ = glm::normalize(glm::vec3(Camera_DroneSim->forward.x, 0, Camera_DroneSim->forward.z));
    float arrowAngle = glm::atan(forwardXZ.x, forwardXZ.z);

    // Randez drona = conul
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 0.5f, 0));
    modelMatrix = glm::rotate(modelMatrix, arrowAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 1.0f, 2.0f));
    RenderMesh(meshes["coneMini"], shaders["VertexNormal"], modelMatrix);

    // Randez pachetul = cubul
    if (cubePlaced && !packageAttached) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, randomCubePosition);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f, 4.0f, 4.0f));
        RenderMesh(meshes["targetCube"], shaders["VertexNormal"], modelMatrix);
    }

    // Randez zona de livrare = coilindrul
    if (circlePlaced) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(circlePosition.x, -1, circlePosition.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(4.5f, 15.5f, 4.5f));
        RenderMesh(meshes["cylinderMini"], shaders["VertexNormal"], modelMatrix);
    }
}


void DroneSim::Update(float deltaTimeSeconds)
{
    HandleDroneCollision();

    assembleDrone(dronePosition, deltaTimeSeconds);

    glm::mat4 modelMatrix;
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], modelMatrix);

    if (!placedTrees) {
        placedTrees = true;
        PlaceTreesRandomly(15, 50.0f, 2.0f);
        cout << " *****************************SCORE********************************* " << endl;

    }

    for (const auto& position : treePositions) {
        RenderTree(position);
    }

    handlePackage(50.0f, treePositions);

    glm::vec3 targetPosition = packageAttached ? circlePosition : randomCubePosition;
    handleCompass(targetPosition);

    RenderMinimap();
}


void DroneSim::FrameEnd()
{
    DrawCoordinateSystem(Camera_DroneSim->GetViewMatrix(), projectionMatrix);
}


void DroneSim::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    // Din laborator
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(Camera_DroneSim->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void DroneSim::OnInputUpdate(float deltaTime, int mods)
{
    previousDronePosition = Camera_DroneSim->position;

    float cameraSpeed = 10.0f * deltaTime;

    if (window->KeyHold(GLFW_KEY_W)) {
        Camera_DroneSim->TranslateForward(cameraSpeed);
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        Camera_DroneSim->TranslateForward(-cameraSpeed);
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        Camera_DroneSim->TranslateRight(-cameraSpeed);
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        Camera_DroneSim->TranslateRight(cameraSpeed);
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        Camera_DroneSim->TranslateUpward(-cameraSpeed);
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        Camera_DroneSim->TranslateUpward(cameraSpeed);
    }

    dronePosition = Camera_DroneSim->GetTargetPosition();

    if (isPerspective) {
        float fovSpeed = 3.0f * deltaTime;

        if (window->KeyHold(GLFW_KEY_UP)) {
            fov += fovSpeed;
            if (fov > RADIANS(120))
                fov = RADIANS(120);
            projectionMatrix = glm::perspective(fov, window->props.aspectRatio, nearPlane, farPlane);
        }

        if (window->KeyHold(GLFW_KEY_DOWN)) {
            fov -= fovSpeed;
            if (fov < RADIANS(30))
                fov = RADIANS(30);
            projectionMatrix = glm::perspective(fov, window->props.aspectRatio, nearPlane, farPlane);
        }

        float orthoAdjustSpeed = 2.0f * deltaTime;

        if (window->KeyHold(GLFW_KEY_RIGHT)) {
            orthoWidth += orthoAdjustSpeed;
            projectionMatrix = glm::ortho(-orthoWidth * window->props.aspectRatio, orthoWidth * window->props.aspectRatio, -orthoHeight, orthoHeight, nearPlane, farPlane);
        }

        if (window->KeyHold(GLFW_KEY_LEFT)) {
            orthoWidth = std::max(orthoWidth - orthoAdjustSpeed, 1.0f);  // Keep width > 0
            projectionMatrix = glm::ortho(-orthoWidth * window->props.aspectRatio, orthoWidth * window->props.aspectRatio, -orthoHeight, orthoHeight, nearPlane, farPlane);
        }
    }

}


void DroneSim::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_T)
    {
        renderCamera_DroneSimTarget = !renderCamera_DroneSimTarget;
    }
    if (key == GLFW_KEY_O) {
        // orthographic projection
        isPerspective = false;
        projectionMatrix = glm::ortho(-orthoSize * window->props.aspectRatio,
            orthoSize * window->props.aspectRatio,
            -orthoSize, orthoSize,
            nearPlane, farPlane);
    }

    if (key == GLFW_KEY_P) {
        // perspective projection
        isPerspective = true;
        projectionMatrix = glm::perspective(fov, window->props.aspectRatio, nearPlane, farPlane);
    }
}


void DroneSim::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void DroneSim::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCamera_DroneSimTarget = false;

            Camera_DroneSim->RotateFirstPerson_OX(-deltaY * sensivityOX);
            Camera_DroneSim->RotateFirstPerson_OY(-deltaX * sensivityOY);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCamera_DroneSimTarget = true;

            Camera_DroneSim->RotateThirdPerson_OX(-deltaY * sensivityOX);
            Camera_DroneSim->RotateThirdPerson_OY(-deltaX * sensivityOY);
        }
    }
}


void DroneSim::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void DroneSim::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void DroneSim::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void DroneSim::OnWindowResize(int width, int height)
{
}

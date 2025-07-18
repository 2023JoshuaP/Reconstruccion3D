#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
    glm::vec3 color;
    glm::vec3 position;
    glm::vec3 scale;
    bool visible;
    
    Mesh() : VAO(0), VBO(0), EBO(0), color(1.0f), position(0.0f), scale(1.0f), visible(false) {}
};

struct Organ {
    std::string name;
    std::string filename;
    glm::vec3 anatomicalPosition;
    glm::vec3 anatomicalScale;
    glm::vec3 color;
    char key;
    std::string description;
};

class FrogAnatomyViewer {
    private:
        GLFWwindow* window;
        std::map<std::string, Mesh> organs;
        std::vector<Organ> organInfo;
        
        GLuint shaderProgram;
        GLint modelLoc, viewLoc, projLoc, colorLoc, lightPosLoc, viewPosLoc;
        
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        
        float cameraAngleX = 0.0f;
        float cameraAngleY = 0.0f;
        float cameraDistance = 5.0f;
        
        bool wireframeMode = false;
        
        float rotationSpeed = 0.03f;
        
        bool keys[1024];
        bool keyPressed[1024];
        

    public:
        FrogAnatomyViewer() {
            setupOrganInfo();

            for (int i = 0; i < 1024; i++) {
                keys[i] = false;
                keyPressed[i] = false;
            }
        }
        
        ~FrogAnatomyViewer() {
            cleanup();
        }
        
        void setupOrganInfo() {
            organInfo = {
                {"blood", "bloodMasks_extraction_points_mesh.obj", 
                glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f), glm::vec3(0.8f, 0.1f, 0.1f), 
                'B', "Sistema circulatorio - Sangre"},

                {"brain", "brainMasks_extraction_points_mesh.obj", 
                glm::vec3(0.9f, 0.10f, -0.2f), glm::vec3(0.85f), glm::vec3(0.9f, 0.7f, 0.9f), 
                'R', "Sistema nervioso - Cerebro"},

                {"duodenum", "duodenumMasks_extraction_points_mesh.obj", 
                glm::vec3(-0.2f, -0.02f, 0.1f), glm::vec3(0.5f), glm::vec3(0.95f, 0.7f, 0.4f), 
                'D', "Sistema digestivo - Duodeno"},

                {"eyes", "eyeMasks_extraction_points_mesh.obj", 
                glm::vec3(1.1f, 0.08f, -0.35f), glm::vec3(1.0f), glm::vec3(0.1f, 0.1f, 0.1f), 
                'E', "Sistema sensorial - Ojos"},

                {"eyeRetna", "eyeRetnaMasks_extraction_points_mesh.obj", 
                glm::vec3(1.1f, 0.08f, -0.36f), glm::vec3(0.9f), glm::vec3(0.8f, 0.2f, 0.2f), 
                'T', "Sistema sensorial - Retina"},

                {"eyeWhite", "eyeWhiteMasks_extraction_points_mesh.obj", 
                glm::vec3(1.1f, 0.08f, -0.34f), glm::vec3(1.05f), glm::vec3(0.9f, 0.9f, 0.9f), 
                'W', "Sistema sensorial - Esclerotica"},

                {"heart", "heartMasks_extraction_points_mesh.obj", 
                glm::vec3(0.2f, 0.13f, -0.15f), glm::vec3(0.7f), glm::vec3(0.7f, 0.1f, 0.1f), 
                'H', "Sistema circulatorio - Corazon"},

                {"ileum", "ileumMasks_extraction_points_mesh.obj", 
                glm::vec3(-0.6f, -0.05f, 0.1f), glm::vec3(0.5f), glm::vec3(0.95f, 0.7f, 0.4f), 
                'I', "Sistema digestivo - Ileon"},

                {"intestine", "intestineMasks_extraction_points_mesh.obj", 
                glm::vec3(-0.4f, 0.0f, 0.09f), glm::vec3(0.7f), glm::vec3(0.85f, 0.6f, 0.3f), 
                'N', "Sistema digestivo - Intestino"},

                {"kidney", "kidneyMasks_extraction_points_mesh.obj", 
                glm::vec3(-0.2f, 0.1f, -0.2f), glm::vec3(1.2f), glm::vec3(0.6f, 0.3f, 0.2f), 
                'K', "Sistema excretor - Rinion"},

                {"liver", "liverMasks_extraction_points_mesh.obj", 
                glm::vec3(0.0f, 0.3f, -0.12f), glm::vec3(0.85f), glm::vec3(0.5f, 0.2f, 0.1f), 
                'L', "Sistema digestivo - Higado"},

                {"lung", "lungMasks_extraction_points_mesh.obj", 
                glm::vec3(0.2f, 0.10f, -0.10f), glm::vec3(1.2f), glm::vec3(0.9f, 0.6f, 0.6f), 
                'U', "Sistema respiratorio - Pulmon"},

                {"muscle", "muscleMasks_extraction_points_mesh.obj", 
                glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.85f), glm::vec3(0.13f, 0.54f, 0.13f), 
                'M', "Sistema muscular - Musculos"},

                {"nerve", "nerveMasks_extraction_points_mesh.obj", 
                glm::vec3(0.0f, 0.15f, -0.05f), glm::vec3(4.3f), glm::vec3(1.0f, 0.9f, 0.3f), 
                'V', "Sistema nervioso - Nervios"},

                {"skeleton", "skeletonMasks_extraction_points_mesh.obj", 
                glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f), glm::vec3(0.9f, 0.9f, 0.8f), 
                'S', "Sistema esqueletico - Huesos"},

                {"spleen", "spleenMasks_extraction_points_mesh.obj", 
                glm::vec3(0.0f, 0.18f, -0.15f), glm::vec3(0.5f), glm::vec3(0.7f, 0.2f, 0.2f), 
                'P', "Sistema linfatico - Bazo"},

                {"stomach", "stomachMasks_extraction_points_mesh.obj", 
                glm::vec3(0.0f, -0.05f, -0.15f), glm::vec3(0.9f), glm::vec3(1.0f, 0.7f, 0.4f), 
                'G', "Sistema digestivo - Estomago"}
            };
        }
        
        bool initialize() {
            if (!glfwInit()) {
                std::cerr << "Error al inicializar GLFW" << std::endl;
                return false;
            }
            
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
            window = glfwCreateWindow(1200, 800, "Anatomía de Rana - Visualizador 3D", NULL, NULL);
            if (!window) {
                std::cerr << "Error al crear ventana" << std::endl;
                glfwTerminate();
                return false;
            }
            
            glfwMakeContextCurrent(window);
            glfwSetWindowUserPointer(window, this);
            glfwSetKeyCallback(window, keyCallback);
            glfwSetScrollCallback(window, scrollCallback);
            
            if (glewInit() != GLEW_OK) {
                std::cerr << "Error al inicializar GLEW" << std::endl;
                return false;
            }
            
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            
            if (!createShaders()) {
                return false;
            }
            
            loadModels();
            
            printInstructions();
            
            return true;
        }
        
        bool createShaders() {
            const char* vertexShaderSource = R"(
                #version 330 core
                layout (location = 0) in vec3 aPos;
                layout (location = 1) in vec3 aNormal;
                
                uniform mat4 model;
                uniform mat4 view;
                uniform mat4 projection;
                
                out vec3 FragPos;
                out vec3 Normal;
                
                void main() {
                    FragPos = vec3(model * vec4(aPos, 1.0));
                    Normal = mat3(transpose(inverse(model))) * aNormal;
                    gl_Position = projection * view * vec4(FragPos, 1.0);
                }
            )";
            
            const char* fragmentShaderSource = R"(
                #version 330 core
                in vec3 FragPos;
                in vec3 Normal;
                
                uniform vec3 color;
                uniform vec3 lightPos;
                uniform vec3 viewPos;
                
                out vec4 FragColor;
                
                void main() {
                    vec3 norm = normalize(Normal);
                    vec3 lightDir = normalize(lightPos - FragPos);
                    
                    // Ambient
                    float ambientStrength = 0.3;
                    vec3 ambient = ambientStrength * color;
                    
                    // Diffuse
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec3 diffuse = diff * color;
                    
                    // Specular
                    float specularStrength = 0.5;
                    vec3 viewDir = normalize(viewPos - FragPos);
                    vec3 reflectDir = reflect(-lightDir, norm);
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
                    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);
                    
                    vec3 result = ambient + diffuse + specular;
                    FragColor = vec4(result, 0.8);
                }
            )";
            
            auto compileShader = [](GLuint type, const char* source) -> GLuint {
                GLuint shader = glCreateShader(type);
                glShaderSource(shader, 1, &source, NULL);
                glCompileShader(shader);
                
                GLint success;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    GLchar infoLog[512];
                    glGetShaderInfoLog(shader, 512, NULL, infoLog);
                    std::cerr << "Error compiled shader: " << infoLog << std::endl;
                    return 0;
                }
                return shader;
            };
            
            GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
            GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
            
            if (!vertexShader || !fragmentShader) {
                return false;
            }
            
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);
            
            GLint success;
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
                std::cerr << "Error linked program: " << infoLog << std::endl;
                return false;
            }
            
            modelLoc = glGetUniformLocation(shaderProgram, "model");
            viewLoc = glGetUniformLocation(shaderProgram, "view");
            projLoc = glGetUniformLocation(shaderProgram, "projection");
            colorLoc = glGetUniformLocation(shaderProgram, "color");
            lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
            viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
            
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            
            return true;
        }
        
        void loadModels() {
            for (const auto& organ : organInfo) {
                if (loadOBJ("mallas/" + organ.filename, organ.name)) {
                    organs[organ.name].color = organ.color;
                    organs[organ.name].position = organ.anatomicalPosition;
                    organs[organ.name].scale = organ.anatomicalScale;
                    std::cout << "Model load: " << organ.name << " - Key: " << organ.key << std::endl;
                } else {
                    std::cout << "Error load: " << organ.name << std::endl;
                }
            }
        }
        
        bool loadOBJ(const std::string& filename, const std::string& name) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
                return false;
            }
            
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<unsigned int> vertexIndices, normalIndices;
            
            std::string line;
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                
                std::istringstream iss(line);
                std::string type;
                iss >> type;
                
                if (type == "v") {
                    glm::vec3 vertex;
                    iss >> vertex.x >> vertex.y >> vertex.z;
                    vertices.push_back(vertex);
                }
                else if (type == "vn") {
                    glm::vec3 normal;
                    iss >> normal.x >> normal.y >> normal.z;
                    normals.push_back(normal);
                }
                else if (type == "f") {
                    std::string vertex1, vertex2, vertex3;
                    iss >> vertex1 >> vertex2 >> vertex3;
                    
                    auto parseIndex = [](const std::string& str) -> std::pair<int, int> {
                        size_t pos1 = str.find('/');
                        if (pos1 == std::string::npos) {
                            return {std::stoi(str) - 1, -1};
                        }
                        
                        size_t pos2 = str.find('/', pos1 + 1);
                        int vertexIndex = std::stoi(str.substr(0, pos1)) - 1;
                        int normalIndex = -1;
                        
                        if (pos2 != std::string::npos && pos2 + 1 < str.length()) {
                            std::string normalStr = str.substr(pos2 + 1);
                            if (!normalStr.empty()) {
                                normalIndex = std::stoi(normalStr) - 1;
                            }
                        }
                        return {vertexIndex, normalIndex};
                    };
                    
                    auto [v1, n1] = parseIndex(vertex1);
                    auto [v2, n2] = parseIndex(vertex2);
                    auto [v3, n3] = parseIndex(vertex3);
                    
                    vertexIndices.push_back(v1);
                    vertexIndices.push_back(v2);
                    vertexIndices.push_back(v3);
                    
                    if (n1 >= 0) {
                        normalIndices.push_back(n1);
                    }
                    if (n2 >= 0) {
                        normalIndices.push_back(n2);
                    }
                    if (n3 >= 0) {
                        normalIndices.push_back(n3);
                    }
                }
            }
            
            file.close();
            
            if (vertices.empty()) {
                std::cerr << "No se encontraron vértices en: " << filename << std::endl;
                return false;
            }
            
            if (vertexIndices.empty()) {
                std::cout << "No se encontraron caras en " << filename << ", creando triángulos automáticamente..." << std::endl;
                
                for (size_t i = 0; i < vertices.size() - 2; i += 3) {
                    vertexIndices.push_back(i);
                    vertexIndices.push_back(i + 1);
                    vertexIndices.push_back(i + 2);
                }
            }
            
            Mesh& mesh = organs[name];
            mesh.vertices.clear();
            mesh.indices.clear();
            
            glm::vec3 minPos = vertices[0];
            glm::vec3 maxPos = vertices[0];
            for (const auto& vertex : vertices) {
                minPos = glm::min(minPos, vertex);
                maxPos = glm::max(maxPos, vertex);
            }
            glm::vec3 center = (minPos + maxPos) * 0.5f;
            float scale = glm::length(maxPos - minPos);
            
            for (auto& vertex : vertices) {
                vertex = (vertex - center) / scale;
            }
            
            if (normalIndices.empty() || normals.empty()) {
                normals.resize(vertices.size(), glm::vec3(0.0f));
                
                for (size_t i = 0; i < vertexIndices.size(); i += 3) {
                    if (vertexIndices[i] >= vertices.size() || 
                        vertexIndices[i + 1] >= vertices.size() || 
                        vertexIndices[i + 2] >= vertices.size()) {
                        continue;
                    }
                    
                    glm::vec3 v0 = vertices[vertexIndices[i]];
                    glm::vec3 v1 = vertices[vertexIndices[i + 1]];
                    glm::vec3 v2 = vertices[vertexIndices[i + 2]];
                    
                    glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
                    
                    normals[vertexIndices[i]] += normal;
                    normals[vertexIndices[i + 1]] += normal;
                    normals[vertexIndices[i + 2]] += normal;
                }
                
                for (auto& normal : normals) {
                    if (glm::length(normal) > 0.0f) {
                        normal = glm::normalize(normal);
                    }
                }
            }
            
            for (size_t i = 0; i < vertexIndices.size(); ++i) {
                if (vertexIndices[i] >= vertices.size()) {
                    continue;
                }
                
                Vertex vertex;
                vertex.position = vertices[vertexIndices[i]];
                vertex.normal = normalIndices.empty() ? 
                            normals[vertexIndices[i]] : 
                            normals[normalIndices[i]];
                mesh.vertices.push_back(vertex);
                mesh.indices.push_back(i);
            }
            
            std::cout << "Loaded " << mesh.vertices.size() << " vertex and " << mesh.indices.size() << " index for " << name << std::endl;
            
            setupMesh(mesh);
            return true;
        }
        
        void setupMesh(Mesh& mesh) {
            glGenVertexArrays(1, &mesh.VAO);
            glGenBuffers(1, &mesh.VBO);
            glGenBuffers(1, &mesh.EBO);
            
            glBindVertexArray(mesh.VAO);
            
            glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
            glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            
            glBindVertexArray(0);
        }
        
        void render() {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            if (wireframeMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(1.0f);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            
            glUseProgram(shaderProgram);
            
            glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 100.0f);
            
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
            glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 2.0f);
            glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
            glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
            
            for (const auto& organ : organInfo) {
                if (organs[organ.name].visible) {
                    renderOrgan(organ.name);
                }
            }
        }
        
        void renderOrgan(const std::string& name) {
            Mesh& mesh = organs[name];
            
            if (mesh.vertices.empty()) return;
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, mesh.position);
            model = glm::scale(model, mesh.scale);
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(colorLoc, 1, glm::value_ptr(mesh.color));
            
            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        
        void updateCamera() {
            cameraPos.x = cameraDistance * sin(cameraAngleY) * cos(cameraAngleX);
            cameraPos.y = cameraDistance * sin(cameraAngleX);
            cameraPos.z = cameraDistance * cos(cameraAngleY) * cos(cameraAngleX);
        }
        
        void processInput() {
            if (keys[GLFW_KEY_ESCAPE]) {
                glfwSetWindowShouldClose(window, true);
            }
            
            if (keys[GLFW_KEY_LEFT]) {
                cameraAngleY -= rotationSpeed;
            }
            if (keys[GLFW_KEY_RIGHT]) {
                cameraAngleY += rotationSpeed;
            }
            if (keys[GLFW_KEY_UP]) {
                cameraAngleX += rotationSpeed;
            }
            if (keys[GLFW_KEY_DOWN]) {
                cameraAngleX -= rotationSpeed;
            }
            
            // Mejorado: Rango de rotación más amplio
            cameraAngleX = glm::clamp(cameraAngleX, -1.57f, 1.57f); // Casi 90 grados
            
            // Normalizar ángulo Y para rotación completa
            if (cameraAngleY > 2.0f * M_PI) {
                cameraAngleY -= 2.0f * M_PI;
            }
            if (cameraAngleY < -2.0f * M_PI) {
                cameraAngleY += 2.0f * M_PI;
            }
            
            if (keys[GLFW_KEY_EQUAL]) cameraDistance = std::max(1.0f, cameraDistance - 0.1f);
            if (keys[GLFW_KEY_MINUS]) cameraDistance = std::min(15.0f, cameraDistance + 0.1f);
            
            if (keys[GLFW_KEY_PAGE_UP] && !keyPressed[GLFW_KEY_PAGE_UP]) {
                rotationSpeed = std::min(0.1f, rotationSpeed + 0.01f);
                std::cout << "Velocidad de rotación: " << rotationSpeed << std::endl;
                keyPressed[GLFW_KEY_PAGE_UP] = true;
            }
            if (keys[GLFW_KEY_PAGE_DOWN] && !keyPressed[GLFW_KEY_PAGE_DOWN]) {
                rotationSpeed = std::max(0.01f, rotationSpeed - 0.01f);
                std::cout << "Velocidad de rotación: " << rotationSpeed << std::endl;
                keyPressed[GLFW_KEY_PAGE_DOWN] = true;
            }
            
            if (keys[GLFW_KEY_A] && !keyPressed[GLFW_KEY_A]) {
                for (auto& pair : organs) {
                    pair.second.visible = true;
                }
                keyPressed[GLFW_KEY_A] = true;
            }
            
            if (keys[GLFW_KEY_C] && !keyPressed[GLFW_KEY_C]) {
                for (auto& pair : organs) {
                    pair.second.visible = false;
                }
                keyPressed[GLFW_KEY_C] = true;
            }
            
            if (keys[GLFW_KEY_F] && !keyPressed[GLFW_KEY_F]) {
                wireframeMode = !wireframeMode;
                std::cout << "Modo wireframe: " << (wireframeMode ? "ON" : "OFF") << std::endl;
                keyPressed[GLFW_KEY_F] = true;
            }
            
            if (!keys[GLFW_KEY_A]) keyPressed[GLFW_KEY_A] = false;
            if (!keys[GLFW_KEY_C]) keyPressed[GLFW_KEY_C] = false;
            if (!keys[GLFW_KEY_F]) keyPressed[GLFW_KEY_F] = false;
            if (!keys[GLFW_KEY_PAGE_UP]) keyPressed[GLFW_KEY_PAGE_UP] = false;
            if (!keys[GLFW_KEY_PAGE_DOWN]) keyPressed[GLFW_KEY_PAGE_DOWN] = false;
            
            for (const auto& organ : organInfo) {
                if (keys[organ.key] && !keyPressed[organ.key]) {
                    organs[organ.name].visible = !organs[organ.name].visible;
                    std::cout << organ.name << " (" << organ.description << "): " 
                              << (organs[organ.name].visible ? "Visible" : "Oculto") << std::endl;
                    keyPressed[organ.key] = true;
                }
                if (!keys[organ.key]) {
                    keyPressed[organ.key] = false;
                }
            }
            
            updateCamera();
        }
        
        void printInstructions() {
            std::cout << "\n=== VISOR DE ANATOMÍA DE RANA ===" << std::endl;
            std::cout << "Controles:" << std::endl;
            std::cout << "  Flechas: Rotar cámara" << std::endl;
            std::cout << "  + / -: Zoom" << std::endl;
            std::cout << "  A: Mostrar todos los órganos" << std::endl;
            std::cout << "  C: Ocultar todos los órganos" << std::endl;
            std::cout << "  F: Alternar modo wireframe" << std::endl;
            std::cout << "  Page Up/Down: Ajustar velocidad de rotación" << std::endl;
            std::cout << "  ESC: Salir" << std::endl;
            std::cout << "\nÓrganos disponibles:" << std::endl;
            for (const auto& organ : organInfo) {
                std::cout << "  " << organ.key << ": " << organ.description << std::endl;
            }
            std::cout << "\nPresiona las teclas correspondientes para mostrar/ocultar órganos." << std::endl;
            std::cout << "===============================\n" << std::endl;
        }
        
        void run() {
            while (!glfwWindowShouldClose(window)) {
                processInput();
                render();
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }
        
        void cleanup() {
            for (auto& pair : organs) {
                Mesh& mesh = pair.second;
                if (mesh.VAO != 0) {
                    glDeleteVertexArrays(1, &mesh.VAO);
                    glDeleteBuffers(1, &mesh.VBO);
                    glDeleteBuffers(1, &mesh.EBO);
                }
            }
            
            if (shaderProgram != 0) {
                glDeleteProgram(shaderProgram);
            }
            
            if (window) {
                glfwDestroyWindow(window);
            }
            glfwTerminate();
        }
        
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            FrogAnatomyViewer* viewer = static_cast<FrogAnatomyViewer*>(glfwGetWindowUserPointer(window));
            if (key >= 0 && key < 1024) {
                if (action == GLFW_PRESS) {
                    viewer->keys[key] = true;
                } else if (action == GLFW_RELEASE) {
                    viewer->keys[key] = false;
                }
            }
        }
        
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
            FrogAnatomyViewer* viewer = static_cast<FrogAnatomyViewer*>(glfwGetWindowUserPointer(window));
            viewer->cameraDistance = std::max(1.0f, std::min(15.0f, viewer->cameraDistance - (float)yoffset * 0.5f));
        }
};

int main() {
    FrogAnatomyViewer viewer;
    
    if (!viewer.initialize()) {
        std::cerr << "Error al inicializar el visor" << std::endl;
        return -1;
    }
    
    viewer.run();
    
    return 0;
}
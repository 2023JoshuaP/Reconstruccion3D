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

// Estructura para vertices
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

// Estructura para mesh
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

// Estructura para órganos
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
    
    // Shaders
    GLuint shaderProgram;
    GLint modelLoc, viewLoc, projLoc, colorLoc, lightPosLoc, viewPosLoc;
    
    // Cámara
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    
    float cameraAngleX = 0.0f;
    float cameraAngleY = 0.0f;
    float cameraDistance = 5.0f;
    
    // Input
    bool keys[1024];
    bool keyPressed[1024]; // Para evitar repetición de teclas
    
public:
    FrogAnatomyViewer() {
        setupOrganInfo();
        // Inicializar arrays
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
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.8f), glm::vec3(0.8f, 0.1f, 0.1f), 
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

            {"eyeRetina", "eyeRetnaMasks_extraction_points_mesh.obj", 
            glm::vec3(1.1f, 0.08f, -0.36f), glm::vec3(0.9f), glm::vec3(0.8f, 0.2f, 0.2f), 
            'T', "Sistema sensorial - Retina"},

            {"eyeWhite", "eyeWhiteMasks_extraction_points_mesh.obj", 
            glm::vec3(1.1f, 0.08f, -0.34f), glm::vec3(1.05f), glm::vec3(0.9f, 0.9f, 0.9f), 
            'W', "Sistema sensorial - Esclerótica"},

            {"heart", "heartMasks_extraction_points_mesh.obj", 
            glm::vec3(0.2f, 0.13f, -0.15f), glm::vec3(0.7f), glm::vec3(0.7f, 0.1f, 0.1f), 
            'H', "Sistema circulatorio - Corazón"},

            {"ileum", "ileumMasks_extraction_points_mesh.obj", 
            glm::vec3(-0.6f, -0.05f, 0.1f), glm::vec3(0.5f), glm::vec3(0.95f, 0.7f, 0.4f), 
            'I', "Sistema digestivo - Íleon"},

            {"intestine", "intestineMasks_extraction_points_mesh.obj", 
            glm::vec3(-0.4f, 0.0f, 0.09f), glm::vec3(0.7f), glm::vec3(0.85f, 0.6f, 0.3f), 
            'N', "Sistema digestivo - Intestino"},

            {"kidney", "kidneyMasks_extraction_points_mesh.obj", 
            glm::vec3(-0.2f, 0.1f, -0.2f), glm::vec3(1.2f), glm::vec3(0.6f, 0.3f, 0.2f), 
            'K', "Sistema excretor - Riñón"},

            {"liver", "liverMasks_extraction_points_mesh.obj", 
            glm::vec3(0.0f, 0.3f, -0.12f), glm::vec3(0.85f), glm::vec3(0.5f, 0.2f, 0.1f), 
            'L', "Sistema digestivo - Hígado"},

            {"lung", "lungMasks_extraction_points_mesh.obj", 
            glm::vec3(0.2f, 0.10f, -0.10f), glm::vec3(1.2f), glm::vec3(0.9f, 0.6f, 0.6f), 
            'U', "Sistema respiratorio - Pulmón"},

            {"muscle", "muscleMasks_extraction_points_mesh.obj", 
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.6f), glm::vec3(0.8f, 0.3f, 0.3f), 
            'M', "Sistema muscular - Músculos"},

            {"nerve", "nerveMasks_extraction_points_mesh.obj", 
            glm::vec3(0.0f, 0.15f, -0.05f), glm::vec3(5.0f), glm::vec3(1.0f, 0.9f, 0.3f), 
            'V', "Sistema nervioso - Nervios"},

            {"skeleton", "skeletonMasks_extraction_points_mesh.obj", 
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f), glm::vec3(0.9f, 0.9f, 0.8f), 
            'S', "Sistema esquelético - Huesos"},

            {"spleen", "spleenMasks_extraction_points_mesh.obj", 
            glm::vec3(0.0f, 0.18f, -0.15f), glm::vec3(0.5f), glm::vec3(0.7f, 0.2f, 0.2f), 
            'P', "Sistema linfático - Bazo"},

            {"stomach", "stomachMasks_extraction_points_mesh.obj", 
            glm::vec3(0.0f, -0.05f, -0.15f), glm::vec3(0.9f), glm::vec3(1.0f, 0.7f, 0.4f), 
            'G', "Sistema digestivo - Estómago"}
        };
    }
    
    bool initialize() {
        // Inicializar GLFW
        if (!glfwInit()) {
            std::cerr << "Error al inicializar GLFW" << std::endl;
            return false;
        }
        
        // Configurar OpenGL context
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        // Crear ventana
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
        
        // Inicializar GLEW
        if (glewInit() != GLEW_OK) {
            std::cerr << "Error al inicializar GLEW" << std::endl;
            return false;
        }
        
        // Configurar OpenGL
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        // Crear shaders
        if (!createShaders()) {
            return false;
        }
        
        // Cargar modelos
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
        
        // Función auxiliar para compilar shaders
        auto compileShader = [](GLuint type, const char* source) -> GLuint {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, NULL);
            glCompileShader(shader);
            
            GLint success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetShaderInfoLog(shader, 512, NULL, infoLog);
                std::cerr << "Error compilando shader: " << infoLog << std::endl;
                return 0;
            }
            return shader;
        };
        
        // Compilar shaders
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        if (!vertexShader || !fragmentShader) {
            return false;
        }
        
        // Crear programa
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        
        // Verificar linkeo
        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cerr << "Error linkeando programa: " << infoLog << std::endl;
            return false;
        }
        
        // Obtener ubicaciones de uniforms
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
                std::cout << "Modelo cargado: " << organ.name << " - Tecla: " << organ.key << std::endl;
            } else {
                std::cout << "Error cargando: " << organ.name << std::endl;
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
            if (line.empty() || line[0] == '#') continue;
            
            std::istringstream iss(line);
            std::string type;
            iss >> type;
            
            if (type == "v") {
                glm::vec3 vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                vertices.push_back(vertex);
            } else if (type == "vn") {
                glm::vec3 normal;
                iss >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            } else if (type == "f") {
                std::string vertex1, vertex2, vertex3;
                iss >> vertex1 >> vertex2 >> vertex3;
                
                // Parsear índices de vértices y normales
                auto parseIndex = [](const std::string& str) -> std::pair<int, int> {
                    size_t pos1 = str.find('/');
                    if (pos1 == std::string::npos) {
                        // Solo índice de vértice
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
                
                if (n1 >= 0) normalIndices.push_back(n1);
                if (n2 >= 0) normalIndices.push_back(n2);
                if (n3 >= 0) normalIndices.push_back(n3);
            }
        }
        
        file.close();
        
        // Verificar que tenemos datos válidos
        if (vertices.empty()) {
            std::cerr << "No se encontraron vértices en: " << filename << std::endl;
            return false;
        }
        
        // Si no hay caras definidas, crear triángulos automáticamente
        if (vertexIndices.empty()) {
            std::cout << "No se encontraron caras en " << filename << ", creando triángulos automáticamente..." << std::endl;
            
            // Crear triángulos secuencialmente con cada 3 vértices
            for (size_t i = 0; i < vertices.size() - 2; i += 3) {
                vertexIndices.push_back(i);
                vertexIndices.push_back(i + 1);
                vertexIndices.push_back(i + 2);
            }
        }
        
        // Crear mesh
        Mesh& mesh = organs[name];
        mesh.vertices.clear();
        mesh.indices.clear();
        
        // Calcular bounding box para normalizar
        glm::vec3 minPos = vertices[0];
        glm::vec3 maxPos = vertices[0];
        for (const auto& vertex : vertices) {
            minPos = glm::min(minPos, vertex);
            maxPos = glm::max(maxPos, vertex);
        }
        glm::vec3 center = (minPos + maxPos) * 0.5f;
        float scale = glm::length(maxPos - minPos);
        
        // Normalizar vertices
        for (auto& vertex : vertices) {
            vertex = (vertex - center) / scale;
        }
        
        // Si no hay normales, calcularlas
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
            
            // Normalizar normales
            for (auto& normal : normals) {
                if (glm::length(normal) > 0.0f) {
                    normal = glm::normalize(normal);
                }
            }
        }
        
        // Crear vértices finales
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
        
        std::cout << "Cargados " << mesh.vertices.size() << " vértices y " 
                  << mesh.indices.size() << " índices para " << name << std::endl;
        
        setupMesh(mesh);
        return true;
    }
    
    void setupMesh(Mesh& mesh) {
        glGenVertexArrays(1, &mesh.VAO);
        glGenBuffers(1, &mesh.VBO);
        glGenBuffers(1, &mesh.EBO);
        
        glBindVertexArray(mesh.VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), 
                     mesh.vertices.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), 
                     mesh.indices.data(), GL_STATIC_DRAW);
        
        // Posición
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        
        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                             (void*)offsetof(Vertex, normal));
        
        glBindVertexArray(0);
    }
    
    void render() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        // Configurar matrices
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                               1200.0f / 800.0f, 0.1f, 100.0f);
        
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Configurar iluminación
        glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 2.0f);
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
        
        // Renderizar órganos visibles
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
        
        // Controles de cámara
        if (keys[GLFW_KEY_LEFT]) cameraAngleY -= 0.02f;
        if (keys[GLFW_KEY_RIGHT]) cameraAngleY += 0.02f;
        if (keys[GLFW_KEY_UP]) cameraAngleX += 0.02f;
        if (keys[GLFW_KEY_DOWN]) cameraAngleX -= 0.02f;
        
        // Limitar ángulo vertical
        cameraAngleX = glm::clamp(cameraAngleX, -1.5f, 1.5f);
        
        // Zoom
        if (keys[GLFW_KEY_EQUAL]) cameraDistance = std::max(1.0f, cameraDistance - 0.1f);
        if (keys[GLFW_KEY_MINUS]) cameraDistance = std::min(15.0f, cameraDistance + 0.1f);
        
        // Mostrar/ocultar todos los órganos
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
        
        // Reset de teclas presionadas
        if (!keys[GLFW_KEY_A]) keyPressed[GLFW_KEY_A] = false;
        if (!keys[GLFW_KEY_C]) keyPressed[GLFW_KEY_C] = false;
        
        updateCamera();
    }
    
    void printInstructions() {
        std::cout << "\n=== VISUALIZADOR DE ANATOMÍA DE RANA ===" << std::endl;
        std::cout << "Controles:" << std::endl;
        std::cout << "- Flechas: Rotar cámara" << std::endl;
        std::cout << "- +/-: Zoom" << std::endl;
        std::cout << "- Rueda del mouse: Zoom" << std::endl;
        std::cout << "- A: Mostrar todos los órganos" << std::endl;
        std::cout << "- C: Ocultar todos los órganos" << std::endl;
        std::cout << "- ESC: Salir" << std::endl;
        std::cout << "\nÓrganos disponibles:" << std::endl;
        
        for (const auto& organ : organInfo) {
            std::cout << "- " << organ.key << ": " << organ.name 
                      << " (" << organ.description << ")" << std::endl;
        }
    }

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        FrogAnatomyViewer* viewer = static_cast<FrogAnatomyViewer*>(glfwGetWindowUserPointer(window));
        if (key >= 0 && key < 1024) {
            if (action == GLFW_PRESS) {
                viewer->keys[key] = true;

                // Alternar visibilidad de órganos
                for (const auto& organ : viewer->organInfo) {
                    if (key == organ.key && !viewer->keyPressed[key]) {
                        viewer->organs[organ.name].visible = !viewer->organs[organ.name].visible;
                        viewer->keyPressed[key] = true;
                        break;
                    }
                }

            } else if (action == GLFW_RELEASE) {
                viewer->keys[key] = false;
                viewer->keyPressed[key] = false;
            }
        }
    }

    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        FrogAnatomyViewer* viewer = static_cast<FrogAnatomyViewer*>(glfwGetWindowUserPointer(window));
        viewer->cameraDistance -= static_cast<float>(yoffset) * 0.5f;
        viewer->cameraDistance = glm::clamp(viewer->cameraDistance, 1.0f, 15.0f);
    }

    void cleanup() {
        for (auto& [name, mesh] : organs) {
            if (mesh.VAO) glDeleteVertexArrays(1, &mesh.VAO);
            if (mesh.VBO) glDeleteBuffers(1, &mesh.VBO);
            if (mesh.EBO) glDeleteBuffers(1, &mesh.EBO);
        }
        if (shaderProgram) glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            processInput();
            render();
            glfwSwapBuffers(window);
        }
    }
};

// Función principal
int main() {
    FrogAnatomyViewer viewer;
    if (!viewer.initialize()) {
        return -1;
    }

    viewer.mainLoop();
    return 0;
}
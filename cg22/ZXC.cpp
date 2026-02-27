#include <iostream>
#include <cmath> // للمكتبة الرياضية
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ==========================================
// Vertex Shader رئيسي - يتحكم بكل شيء
// ==========================================
const char* vertexShaderSource_Main = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform float uTime;\n"
"uniform int uPartType;\n"  // 0:عيون, 1:فم, 2:يدين, 3:ثابت, 4:أرجل, 5:هوائي
"void main()\n"
"{\n"
"   vec3 newPos = aPos;\n"
"   \n"
"   // ===== الحركة الكلية للروبوت (يمين ويسار) =====\n"
"   float walkX = sin(uTime * 1.5) * 0.2;\n"
"   newPos.x = aPos.x + walkX;\n"
"   \n"
"   // ===== الحركات الداخلية حسب نوع الجزء =====\n"
"   \n"
"   // العيون (تغلق وتفتح)\n"
"   if (uPartType == 0) {\n"
"       float blink = abs(sin(uTime * 3.0));\n"
"       blink = pow(blink, 1.5);\n"
"       float centerY = 0.25;\n"
"       \n"
"       if (aPos.y > centerY + 0.01) {\n"
"           newPos.y = centerY + (aPos.y - centerY) * blink;\n"
"       }\n"
"       else if (aPos.y < centerY - 0.01) {\n"
"           newPos.y = centerY - (centerY - aPos.y) * blink;\n"
"       }\n"
"       ourColor = aColor * (0.5 + 0.5 * blink);\n"
"   }\n"
"   // الفم (يتمدد ويتقلص)\n"
"   else if (uPartType == 1) {\n"
"       float smileAmount = sin(uTime * 2.0) * 0.08;\n"
"       if (aPos.x > 0.12 - walkX) {\n"
"           newPos.x = aPos.x + smileAmount;\n"
"       }\n"
"       else if (aPos.x < -0.12 - walkX) {\n"
"           newPos.x = aPos.x - smileAmount;\n"
"       }\n"
"       ourColor = aColor * (0.8 + 0.2 * sin(uTime * 2.0));\n"
"   }\n"
"   // اليدين (تتحرك للأعلى والأسفل)\n"
"   else if (uPartType == 2) {\n"
"       float armWave = sin(uTime * 3.0) * 0.15;\n"
"       if (aPos.x < -0.25 + walkX) {  // أضيق للأيدي الرفيعة\n"
"           newPos.y = aPos.y + armWave;\n"
"       }\n"
"       else if (aPos.x > 0.25 + walkX) {\n"
"           newPos.y = aPos.y - armWave;\n"
"       }\n"
"       ourColor = aColor;\n"
"   }\n"
"   // الأرجل (تتحرك عكس اليدين)\n"
"   else if (uPartType == 4) {\n"
"       float legWave = sin(uTime * 3.0 + 3.14) * 0.1;\n"
"       if (aPos.x < -0.12 + walkX) {\n"
"           newPos.y = aPos.y + legWave;\n"
"       }\n"
"       else if (aPos.x > 0.12 + walkX) {\n"
"           newPos.y = aPos.y - legWave;\n"
"       }\n"
"       ourColor = aColor;\n"
"   }\n"
"   // الهوائي (يتحرك يمين ويسار)\n"
"   else if (uPartType == 5) {\n"
"       float antennaWave = sin(uTime * 4.0) * 0.1;\n"
"       newPos.x = aPos.x + antennaWave;\n"
"       ourColor = aColor;\n"
"   }\n"
"   else {\n"
"       ourColor = aColor;\n"
"   }\n"
"   \n"
"   gl_Position = vec4(newPos, 1.0);\n"
"}\0";

// ==========================================
// Fragment Shader - موحد للكل
// ==========================================
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"uniform float uAlpha;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, uAlpha);\n"
"}\n\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int main()
{
    // --- تهيئة GLFW ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Robot SR-2000 (Long Torso & Thin Arms)", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- تهيئة GLEW ---
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ==========================================
    // بناء برنامج الشيدر الرئيسي
    // ==========================================

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource_Main, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ==========================================
    // بيانات الرأس - روبوت بجسد طويل (صدر + خصر)
    // ==========================================

    float vertices[] = {
        // ===== الرأس (6 رؤوس) - فضي =====
        -0.2f,  0.4f, 0.0f,  0.7f, 0.7f, 0.8f,  // 0
         0.2f,  0.4f, 0.0f,  0.7f, 0.7f, 0.8f,  // 1
        -0.2f,  0.0f, 0.0f,  0.7f, 0.7f, 0.8f,  // 2
         0.2f,  0.0f, 0.0f,  0.7f, 0.7f, 0.8f,  // 3
        -0.2f,  0.0f, 0.0f,  0.7f, 0.7f, 0.8f,  // 4
         0.2f,  0.4f, 0.0f,  0.7f, 0.7f, 0.8f,  // 5

         // ===== الرقبة (6 رؤوس) - رمادي =====
         -0.12f, 0.0f,  0.05f,  0.5f, 0.5f, 0.5f,  // 6
          0.12f, 0.0f,  0.05f,  0.5f, 0.5f, 0.5f,  // 7
         -0.12f, -0.07f, 0.05f,  0.5f, 0.5f, 0.5f,  // 8
          0.12f, -0.07f, 0.05f,  0.5f, 0.5f, 0.5f,  // 9
         -0.12f, -0.07f, 0.05f,  0.5f, 0.5f, 0.5f,  // 10
          0.12f, 0.0f,  0.05f,  0.5f, 0.5f, 0.5f,  // 11

          // ===== الصدر (6 رؤوس) - أزرق فاتح =====
          -0.25f, -0.07f, 0.1f,  0.3f, 0.5f, 0.9f,  // 12 (كتف)
           0.25f, -0.07f, 0.1f,  0.3f, 0.5f, 0.9f,  // 13 (كتف)
          -0.22f, -0.35f, 0.1f,  0.3f, 0.5f, 0.9f,  // 14 (وسط - أضيق قليلاً)
           0.22f, -0.35f, 0.1f,  0.3f, 0.5f, 0.9f,  // 15 (وسط)
          -0.22f, -0.35f, 0.1f,  0.3f, 0.5f, 0.9f,  // 16
           0.25f, -0.07f, 0.1f,  0.3f, 0.5f, 0.9f,  // 17

           // ===== الخصر (6 رؤوس) - أزرق داكن قليلاً =====
           -0.22f, -0.35f, 0.1f,  0.2f, 0.4f, 0.8f,  // 18 (وسط)
            0.22f, -0.35f, 0.1f,  0.2f, 0.4f, 0.8f,  // 19 (وسط)
           -0.2f,  -0.7f,  0.1f,  0.2f, 0.4f, 0.8f,  // 20 (أسفل - أضيق)
            0.2f,  -0.7f,  0.1f,  0.2f, 0.4f, 0.8f,  // 21 (أسفل)
           -0.2f,  -0.7f,  0.1f,  0.2f, 0.4f, 0.8f,  // 22
            0.22f, -0.35f, 0.1f,  0.2f, 0.4f, 0.8f,  // 23

            // ===== الفم (4 رؤوس) - برتقالي =====
            -0.12f, 0.1f, -0.05f,  1.0f, 0.5f, 0.0f,  // 24
            -0.12f, 0.2f, -0.05f,  1.0f, 0.5f, 0.0f,  // 25
             0.12f, 0.2f, -0.05f,  1.0f, 0.5f, 0.0f,  // 26
             0.12f, 0.1f, -0.05f,  1.0f, 0.5f, 0.0f,  // 27

             // ===== العين اليسرى (9 رؤوس) - أحمر =====
             -0.08f, 0.28f, -0.1f,  1.0f, 0.2f, 0.2f,  // 28 (مركز)
             -0.03f, 0.33f, -0.1f,  1.0f, 0.2f, 0.2f,  // 29
             -0.08f, 0.33f, -0.1f,  1.0f, 0.2f, 0.2f,  // 30
             -0.13f, 0.33f, -0.1f,  1.0f, 0.2f, 0.2f,  // 31
             -0.15f, 0.28f, -0.1f,  1.0f, 0.2f, 0.2f,  // 32
             -0.13f, 0.23f, -0.1f,  1.0f, 0.2f, 0.2f,  // 33
             -0.08f, 0.23f, -0.1f,  1.0f, 0.2f, 0.2f,  // 34
             -0.03f, 0.23f, -0.1f,  1.0f, 0.2f, 0.2f,  // 35
              0.0f,  0.28f, -0.1f,  1.0f, 0.2f, 0.2f,  // 36

              // ===== العين اليمنى (9 رؤوس) - أزرق =====
              0.08f, 0.28f, -0.1f,  0.2f, 0.2f, 1.0f,  // 37 (مركز)
              0.13f, 0.33f, -0.1f,  0.2f, 0.2f, 1.0f,  // 38
              0.08f, 0.33f, -0.1f,  0.2f, 0.2f, 1.0f,  // 39
              0.03f, 0.33f, -0.1f,  0.2f, 0.2f, 1.0f,  // 40
              0.0f,  0.28f, -0.1f,  0.2f, 0.2f, 1.0f,  // 41
              0.03f, 0.23f, -0.1f,  0.2f, 0.2f, 1.0f,  // 42
              0.08f, 0.23f, -0.1f,  0.2f, 0.2f, 1.0f,  // 43
              0.13f, 0.23f, -0.1f,  0.2f, 0.2f, 1.0f,  // 44
              0.15f, 0.28f, -0.1f,  0.2f, 0.2f, 1.0f,  // 45

              // ===== اليد اليسرى (4 رؤوس) - أحمر (أطول وأرفع) =====
              -0.3f, -0.15f, 0.15f,  0.9f, 0.2f, 0.2f,  // 46 (أرفع)
              -0.22f, -0.15f, 0.15f,  0.9f, 0.2f, 0.2f,  // 47 (أرفع)
              -0.3f, -0.6f, 0.15f,   0.9f, 0.2f, 0.2f,  // 48 (أطول)
              -0.22f, -0.6f, 0.15f,   0.9f, 0.2f, 0.2f,  // 49 (أطول)

              // ===== اليد اليمنى (4 رؤوس) - أزرق (أطول وأرفع) =====
              0.22f, -0.15f, 0.15f,  0.2f, 0.2f, 0.9f,  // 50 (أرفع)
              0.3f, -0.15f, 0.15f,   0.2f, 0.2f, 0.9f,  // 51 (أرفع)
              0.22f, -0.6f, 0.15f,   0.2f, 0.2f, 0.9f,  // 52 (أطول)
              0.3f, -0.6f, 0.15f,    0.2f, 0.2f, 0.9f,  // 53 (أطول)

              // ===== الرجل اليسرى (4 رؤوس) - بني محمر =====
              -0.18f, -0.7f, 0.2f,  0.8f, 0.3f, 0.2f,  // 54
              -0.05f, -0.7f, 0.2f,  0.8f, 0.3f, 0.2f,  // 55
              -0.18f, -1.0f, 0.2f,  0.8f, 0.3f, 0.2f,  // 56
              -0.05f, -1.0f, 0.2f,  0.8f, 0.3f, 0.2f,  // 57

              // ===== الرجل اليمنى (4 رؤوس) - بني =====
              0.05f, -0.7f, 0.2f,  0.6f, 0.4f, 0.2f,  // 58
              0.18f, -0.7f, 0.2f,  0.6f, 0.4f, 0.2f,  // 59
              0.05f, -1.0f, 0.2f,  0.6f, 0.4f, 0.2f,  // 60
              0.18f, -1.0f, 0.2f,  0.6f, 0.4f, 0.2f,  // 61

              // ===== هوائي (3 رؤوس) - أحمر =====
              -0.05f, 0.45f, -0.2f,  1.0f, 0.0f, 0.0f,  // 62
               0.05f, 0.45f, -0.2f,  1.0f, 0.0f, 0.0f,  // 63
               0.0f,  0.55f, -0.2f,  1.0f, 0.0f, 0.0f   // 64
    };

    // مؤشرات (Indices) لتكوين المثلثات
    unsigned int indices[] = {
        // الرأس
        0, 1, 2,
        3, 4, 5,

        // الرقبة
        6, 7, 8,
        9, 10, 11,

        // الصدر
        12, 13, 14,
        15, 16, 17,

        // الخصر
        18, 19, 20,
        21, 22, 23,

        // الفم
        24, 25, 26,
        24, 26, 27,

        // العين اليسرى (8 مثلثات)
        28, 29, 30,
        28, 30, 31,
        28, 31, 32,
        28, 32, 33,
        28, 33, 34,
        28, 34, 35,
        28, 35, 36,
        28, 36, 29,

        // العين اليمنى (8 مثلثات)
        37, 38, 39,
        37, 39, 40,
        37, 40, 41,
        37, 41, 42,
        37, 42, 43,
        37, 43, 44,
        37, 44, 45,
        37, 45, 38,

        // اليد اليسرى
        46, 47, 48,
        47, 48, 49,

        // اليد اليمنى
        50, 51, 52,
        51, 52, 53,

        // الرجل اليسرى
        54, 55, 56,
        55, 56, 57,

        // الرجل اليمنى
        58, 59, 60,
        59, 60, 61,

        // هوائي
        62, 63, 64
    };

    // أرقام البداية والنهاية لكل جزء
    const int HEAD_INDEX_START = 0;
    const int HEAD_INDEX_COUNT = 6;

    const int NECK_INDEX_START = 6;
    const int NECK_INDEX_COUNT = 6;

    const int CHEST_INDEX_START = 12;
    const int CHEST_INDEX_COUNT = 6;

    const int WAIST_INDEX_START = 18;
    const int WAIST_INDEX_COUNT = 6;

    const int MOUTH_INDEX_START = 24;
    const int MOUTH_INDEX_COUNT = 6;

    const int LEFT_EYE_INDEX_START = 30;
    const int LEFT_EYE_INDEX_COUNT = 24;

    const int RIGHT_EYE_INDEX_START = 54;
    const int RIGHT_EYE_INDEX_COUNT = 24;

    const int LEFT_ARM_INDEX_START = 78;
    const int LEFT_ARM_INDEX_COUNT = 6;

    const int RIGHT_ARM_INDEX_START = 84;
    const int RIGHT_ARM_INDEX_COUNT = 6;

    const int LEFT_LEG_INDEX_START = 90;
    const int LEFT_LEG_INDEX_COUNT = 6;

    const int RIGHT_LEG_INDEX_START = 96;
    const int RIGHT_LEG_INDEX_COUNT = 6;

    const int ANTENNA_INDEX_START = 102;
    const int ANTENNA_INDEX_COUNT = 3;

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- حلقة الرسم ---
    float timeValue = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        timeValue = glfwGetTime();

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);

        // إرسال الوقت
        int timeLoc = glGetUniformLocation(shaderProgram, "uTime");
        glUniform1f(timeLoc, timeValue);

        int alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
        int partTypeLoc = glGetUniformLocation(shaderProgram, "uPartType");

        // ===== الأجزاء الثابتة (uPartType = 3) =====
        glUniform1i(partTypeLoc, 3);
        glUniform1f(alphaLoc, 1.0f);

        glDrawElements(GL_TRIANGLES, HEAD_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(HEAD_INDEX_START * sizeof(unsigned int)));
        glDrawElements(GL_TRIANGLES, NECK_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(NECK_INDEX_START * sizeof(unsigned int)));
        glDrawElements(GL_TRIANGLES, CHEST_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(CHEST_INDEX_START * sizeof(unsigned int)));
        glDrawElements(GL_TRIANGLES, WAIST_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(WAIST_INDEX_START * sizeof(unsigned int)));

        // ===== الهوائي المتحرك (uPartType = 5) =====
        glUniform1i(partTypeLoc, 5);
        glUniform1f(alphaLoc, 1.0f);
        glDrawElements(GL_TRIANGLES, ANTENNA_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(ANTENNA_INDEX_START * sizeof(unsigned int)));

        // ===== العيون (uPartType = 0) =====
        glUniform1i(partTypeLoc, 0);
        glUniform1f(alphaLoc, 0.9f);

        glDrawElements(GL_TRIANGLES, LEFT_EYE_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(LEFT_EYE_INDEX_START * sizeof(unsigned int)));
        glDrawElements(GL_TRIANGLES, RIGHT_EYE_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(RIGHT_EYE_INDEX_START * sizeof(unsigned int)));

        // ===== الفم (uPartType = 1) =====
        glUniform1i(partTypeLoc, 1);
        glUniform1f(alphaLoc, 1.0f);
        glDrawElements(GL_TRIANGLES, MOUTH_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(MOUTH_INDEX_START * sizeof(unsigned int)));

        // ===== اليدين (uPartType = 2) =====
        glUniform1i(partTypeLoc, 2);
        glUniform1f(alphaLoc, 1.0f);

        glDrawElements(GL_TRIANGLES, LEFT_ARM_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(LEFT_ARM_INDEX_START * sizeof(unsigned int)));
        glDrawElements(GL_TRIANGLES, RIGHT_ARM_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(RIGHT_ARM_INDEX_START * sizeof(unsigned int)));

        // ===== الأرجل (uPartType = 4) =====
        glUniform1i(partTypeLoc, 4);
        glUniform1f(alphaLoc, 1.0f);

        glDrawElements(GL_TRIANGLES, LEFT_LEG_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(LEFT_LEG_INDEX_START * sizeof(unsigned int)));
        glDrawElements(GL_TRIANGLES, RIGHT_LEG_INDEX_COUNT, GL_UNSIGNED_INT, (void*)(RIGHT_LEG_INDEX_START * sizeof(unsigned int)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
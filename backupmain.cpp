#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <cctype>
#include <sstream>
#include <array>
#include <dlfcn.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <thread>      // for std::this_thread::sleep_for
#include <chrono>      // for std::chrono::milliseconds
#include <algorithm>   // for std::min, std::max
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "ImGui/FONTS/DEFAULT.h"
#include "ImGui/Font.h"
#include "ImGui/Icon.h"
#include "ImGui/Iconcpp.h"
#include "imgui/imgui.h"
#include "imgui/imgui_additional.h"
#include "imgui/backends/imgui_impl_android.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/android_native_app_glue.h"
#include "Includes/Vector2.h"
#include "Includes/Vector3.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "Menu/Setup.h"
#include <sys/system_properties.h>
#include <Includes/Dobby/dobby.h>
#include "ByNameModding/Tools.h"
#include "ByNameModding/fake_dlfcn.h"
#include "ByNameModding/Il2Cpp.h"
#include "Il2Cpp/il2cpp_dump.h"
#include "Includes/log.h"
#include "Roboto-Regular.h"
#include "pointers.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "foxcheats/include/ScanEngine.hpp"
//Target lib here
#define targetLibName OBFUSCATE("libnba2k20_clean_opt.so")
#include "GG.h"
#include "JNIStuff.h"
#include "Includes/Macros.h"
#include "MonoString.h"
#include "input.h"
#include "patches.h"
#include "game_guardian_code.h"
#include "gg_scanner.h"
using namespace kFox;
int glWidth, glHeight;
bool g_Initialized;
uintptr_t g_IL2Cpp;
void *m_EGL;

int speed;
static bool bShot = false;
static ImVec4 color;
static bool bRls = false;
static bool bRetro = false;
static int setQuarter   = 4;
static float setTime    = 1.0f;
static float setdrillTime =1.0f;
static int setHomeScore = 10;
static int setAwayScore = 100;
static bool show_scanner = false;
MemoryPatch ShotPatch, testpatch,testpatch1, testpatch2, testpatch3, testpatch4, testpatch5, testpatch6;

std::string GetProp(const char* key) {
    char value[PROP_VALUE_MAX];
    __system_property_get(key, value);
    return std::string(value);
}


uintptr_t GetFirstResultAddr() {
    Maps_t* res = kFox::GetResult();
    return (res != nullptr) ? res->start : 0;
}




// @invoke
// invoke-static {p0}, Lcom/android/support/Main;->StartWithoutPermission(Landroid/content/Context;)V

//####################################
//@Hooks
static bool play_back = false;

typedef void (*StartPlayback_t)();
StartPlayback_t orig_StartPlayback;

// Our hooked function
void my_StartPlayback() {
    if (play_back) {
        LOGI("Hooked StartPlayback!");
        if (orig_StartPlayback)
            orig_StartPlayback();
        LOGI("Finished StartPlayback!");
    } else {
        // If toggle is off, just call the original normally
        if (orig_StartPlayback)
            orig_StartPlayback();
    }
}



//####################################

void DrawMenu() {  
    auto il2cpp_handle = dlopen(targetLibName, 4);
    const ImVec2 window_size = ImVec2(700, 600);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Once);

    ImGui::Begin("HANES NBA2K TOOL", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);

    static int activeTab = 0;
    float windowWidth = ImGui::GetContentRegionAvail().x;

    // Tab buttons layout
    float spacing = 8.0f;
    int tabCount = 3;
    float buttonWidth = (windowWidth - spacing * (tabCount - 1)) / tabCount;
    float buttonHeight = 48.0f;

    auto TabButton = [&](const char* label, int id) {
        ImGuiStyle& style = ImGui::GetStyle();
        float oldRounding = style.FrameRounding;
        style.FrameRounding = 0.0f;

        ImVec2 size(buttonWidth, buttonHeight);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        bool pressed = ImGui::Button(label, size);
        ImGui::PopStyleVar();

        if (pressed) activeTab = id;

        style.FrameRounding = oldRounding;
    };

    // Draw tab buttons
    for (int i = 0; i < tabCount; i++) {
        if (i > 0) ImGui::SameLine(0, spacing);
        if (i == 0) TabButton("Visual", 0);
        else if (i == 1) TabButton("PLAYER", 1);
        else if (i == 2) TabButton("SETTINGS & UI", 2);
    }

    ImGui::Separator();
    float spacingChild = 8.0f;

    // ==========================================================
    // VISUAL TAB
    // ==========================================================
    if (activeTab == 0) {
        float childWidth = (windowWidth - spacingChild * 2.0f) / 3.0f;

        // --- ORIGINAL (left) ---
        ImGui::BeginChild("original", ImVec2(childWidth, 0), true);
        ImGui::PushItemWidth(-1);

        const char* items[] = { "BROKEN RIM", "HD COURT", "TEST", "SWEAT", "NO MISSED A SHOT" };
        static int current_item = 0;

        ImGui::Text("InGame");
        if (ImGui::BeginCombo("Select Option", items[current_item])) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                bool is_selected = (current_item == n);
                if (ImGui::Selectable(items[n], is_selected))
                    current_item = n;
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        static bool ankleBreakerValue = false;
        if (ImGui::Checkbox("HD BLACKTOP", &ankleBreakerValue)) {
            HD_player_blacktop();
        }

        static bool teamchems = false;
        if (ImGui::Checkbox("100 Team chemistry", &teamchems)) {
            team_chemistry();
        }

        static bool bxer_shorts = false;
        if (ImGui::Checkbox("BOXER SHORTS", &bxer_shorts)) {
            boxer_shorts();
        }

        if (ImGui::Checkbox("HalfCourt Jumpshot", &bShot)) {
            if (bShot) ShotPatch.Modify();
            else ShotPatch.Restore();
        }

        // Court color picker
        color.x = *(float*)colorR;
        color.y = *(float*)colorG;
        color.z = *(float*)colorB;

        ImGui::Text("Court Color Picker");
        if (ImGui::ColorEdit3("Player Color", (float*)&color)) {
            *(float*)colorR = color.x;
            *(float*)colorG = color.y;
            *(float*)colorB = color.z;
        }

        if (ImGui::Selectable("AUTO BLOCK")) { /* ... */ }
        if (ImGui::Selectable("HIGH STEAL")) { /* ... */ }

        if (ImGui::CollapsingHeader("Advanced Settings")) {
            ImGui::SliderInt("Max Players", &speed, 1, 16);

            static bool patchQuarter = false;
            if (ImGui::Checkbox("Patch Quarter Values", &patchQuarter)) {
                if (patchQuarter) {
                    patchBssOffset(quartertime, "00000000", true);
                    patchBssOffset(quarterminutes, "00000000", true);
                }
            }
        }

        ImGui::PopItemWidth();
        ImGui::EndChild();

        // --- OTHERS (middle) ---
        ImGui::SameLine(0, spacingChild);
        ImGui::BeginChild("others", ImVec2(childWidth, 0), true);
        ImGui::PushItemWidth(-1);

        ImGui::Separator();
        ImGui::TextDisabled("Other");
        ImGui::Text("NBA 2K20");

        static int shoe_current_item = -1; 
        const char* shoes[] = { "shoe_speedy", "shoe_KD", "shoe_jordanv1", "shoe_jordanv2" };

        if (ImGui::BeginCombo("Shoes test", shoes[current_item])) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                bool is_selected = (current_item == n);
                if (ImGui::Selectable(shoes[n], is_selected)) {
                    shoe_current_item = n;
                    if (shoe_current_item == 0) shoe_speedy();
                    else if (shoe_current_item == 1) { /* shoe_KD(); */ }
                    else if (shoe_current_item == 2) { /* shoe_jordanv1(); */ }
                    else if (shoe_current_item == 3) { /* shoe_jordanv2(); */ }
                }
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Selected: %s", shoes[current_item]);

        static bool shoe_blueC = false;
        static bool shoe_cyellow = false;
        static bool shoe = false;

        ImGui::Checkbox("playback", &play_back);

        if (ImGui::Checkbox("shoe_blueTestbuild", &shoe_blueC)) shoe_blue();
        if (ImGui::Checkbox("shoe_yellow", &shoe_cyellow)) shoe_yellow();
        if (ImGui::Checkbox("shoe_RGB", &shoe)) shoe_rgb();

        if (ImGui::Checkbox("Retro Shorts", &bRetro)) {
            if (bRetro) retro_on();
            else retro_off();
        }

        ImGui::PopItemWidth();
        ImGui::EndChild();

        // --- NEW (right) ---
        ImGui::SameLine(0, spacingChild);
        ImGui::BeginChild("new", ImVec2(childWidth, 0), true);
        ImGui::PushItemWidth(-1);

        ImGui::Separator();
        ImGui::TextDisabled("New");

        if (ImGui::Button("GG SCANNER")) {
            show_scanner = !show_scanner;
        }

        if (show_scanner) {
            gg_scanner();
        }

        ImGui::PopItemWidth();
        ImGui::EndChild();
    }

    // ==========================================================
    // MEMORY TAB
    // ==========================================================
    if (activeTab == 1) {
        float childWidth = (windowWidth - spacingChild) / 2.0f;

        // ---- LEFT CHILD ----
        ImGui::BeginChild("memory_tab", ImVec2(childWidth, 0), true);
        ImGui::PushItemWidth(-1);

        ImGui::Separator();
        ImGui::Text("MC CREATION SKIPS");

        if (ImGui::CollapsingHeader("Scoreboard Controls")) {
            ImGui::Text("Quarter/Time");   
            ImGui::InputInt("Quarter", &setQuarter);
            ImGui::InputFloat("Time", &setTime, 0.1f, 1.0f, "%.1f");

            ImGui::Text("HOME/AWAY SCORES");   
            ImGui::InputInt("Home Score", &setHomeScore);
            ImGui::InputInt("Away Score", &setAwayScore);

            ImGui::Text("Drill"); 
            ImGui::InputFloat("Skip Drill", &setdrillTime, 0.1f, 1.0f, "%.1f");

            if (ImGui::Button("Apply Scores / Time")) {
                if (addrQuarter)   *(int*)addrQuarter = setQuarter;
                if (addrTime)      *(float*)addrTime = setTime;
                if (addrScoreHome) *(int*)addrScoreHome = setHomeScore;
                if (addrScoreAway) *(int*)addrScoreAway = setAwayScore;
            }
            if (ImGui::Button("SKIP DRILL")) {
                if (addrDrillClock) *(float*)addrDrillClock = setdrillTime;
            }
        }

        ImGui::PopItemWidth();
        ImGui::EndChild();

        ImGui::SameLine(0, spacingChild);

        // ---- RIGHT CHILD ----
        ImGui::BeginChild("team_selector_tab", ImVec2(childWidth, 0), true);
        ImGui::PushItemWidth(-1);

        ImGui::Text("EDIT PLAYER");

        static const char* teams[] = {
            "76ers","BUCKS","BULLS","CAVALIERS","CELTICS","CLIPPERS","GRIZZLIERS",
            "HAWKS","HEAT","HORNET","JAZZ","KINGS","KNICKS","LAKERS","MAGIC",
            "MAVERICKS","NETS","NUGGETS","PACERS","PELICANS","PISTONS","RAPTORS",
            "ROCKETS","SPURS","SUNS","THUNDER","TIMBERWOLVES","BLAZERS",
            "WARRIORS","WIZARDS","PRE AGENTS"
        };
        static int selectedTeam201 = 0;

        uintptr_t teamid  = getAbsoluteAddress(targetLibName, string2Offset("0x2E6DCA0"));
        uintptr_t option1 = getAbsoluteAddress(targetLibName, string2Offset("0x2E6DC98"));
        uintptr_t option2 = getAbsoluteAddress(targetLibName, string2Offset("0x2E6DC9C"));

        ImGui::Separator();
        ImGui::Text("Team Selector");

        if (ImGui::Combo("Teams", &selectedTeam201, teams, IM_ARRAYSIZE(teams))) {
            PatchTeam(teamid, option1, option2, selectedTeam201, "02000000", "02000000");
        }

        ImGui::PopItemWidth();
        ImGui::EndChild();
    }

    // ==========================================================
    // SETTINGS TAB
    // ==========================================================
    if (activeTab == 2) {
        ImGui::BeginChild("settings_tab", ImVec2(0, 0), true);
        ImGui::PushItemWidth(-1);

        const char* scaleLabels[] = { "Small", "Normal", "Large", "Extra Large" };
        static int currentScale = 1;

        const char* preview_value = (currentScale >= 0 && currentScale < IM_ARRAYSIZE(scaleLabels))
            ? scaleLabels[currentScale]
            : "Unknown";

        ImGui::Text("DISPLAY SIZE & UI Design");
        if (ImGui::BeginCombo("DISPLAY SIZE", preview_value)) {
            for (int i = 0; i < IM_ARRAYSIZE(scaleLabels); i++) {
                bool isSelected = (currentScale == i);
                if (ImGui::Selectable(scaleLabels[i], isSelected)) {
                    currentScale = i;

                    ImGuiIO& io = ImGui::GetIO();
                    ImGuiStyle& style = ImGui::GetStyle();

                    switch (currentScale) {
                        case 0: io.FontGlobalScale = 0.85f; style.ScaleAllSizes(0.85f); break;
                        case 1: io.FontGlobalScale = 1.0f;  style.ScaleAllSizes(1.0f);  break;
                        case 2: io.FontGlobalScale = 1.25f; style.ScaleAllSizes(1.25f); break;
                        case 3: io.FontGlobalScale = 1.5f;  style.ScaleAllSizes(1.5f);  break;
                    }
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("ABOUT DEVICE");
        ImGui::BulletText("Device Name: %s", GetProp("ro.product.device").c_str());
        ImGui::BulletText("Model: %s", GetProp("ro.product.model").c_str());
        ImGui::BulletText("Manufacturer: %s", GetProp("ro.product.manufacturer").c_str());
        ImGui::BulletText("Android Version: %s", GetProp("ro.build.version.release").c_str());
        ImGui::BulletText("SDK Version: %s", GetProp("ro.build.version.sdk").c_str());
        ImGui::BulletText("CPU ABI: %s", GetProp("ro.product.cpu.abi").c_str());

        ImGui::PopItemWidth();
        ImGui::EndChild();
    }

    ImGui::End();  
}
int (*old_getWidth)(ANativeWindow* window);
int hook_getWidth(ANativeWindow* window) {
    return old_getWidth(window);
}
int (*old_getHeight)(ANativeWindow* window);
int hook_getHeight(ANativeWindow* window) {
    return old_getHeight(window);
}
EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    static bool is_setup = false;
    if (!is_setup) {
        // SetupImgui();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2(glWidth, glHeight);
		ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5, 0.5);
		
        ImGui::StyleColorsDark();
        ImGui_ImplOpenGL3_Init("#version 100");
        io.IniFilename = nullptr;

        // Configure font
        ImFontConfig font_cfg;
        font_cfg.SizePixels = 35.0f;

        // Important: add full language ranges
        static const ImWchar ranges[] = {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0100, 0x024F, // Extended Latin
            0x0400, 0x052F, // Cyrillic
            0,
        };

        // Load the font with ranges
        io.Fonts->AddFontFromMemoryTTF(
            Roboto_Regular,
            sizeof(Roboto_Regular),
            35.0f,
            &font_cfg,
            ranges
        );

        // Scale UI
        is_setup = true;
    }

    ImGuiIO &io = ImGui::GetIO();
    // Handle Android soft keyboard

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    DrawMenu();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();

    return old_eglSwapBuffers(dpy, surface);
}








void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));
	DobbyHook((void *) dlsym(dlopen("libandroid.so", RTLD_NOW), "ANativeWindow_getWidth"), (void *) hook_getWidth, (void **) &old_getWidth);
    DobbyHook((void *) dlsym(dlopen("libandroid.so", RTLD_NOW), "ANativeWindow_getHeight"), (void *) hook_getHeight, (void **) &old_getHeight);
    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));
    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);
    __INPUT__(); ///Touch
	DobbyHook((void *) DobbySymbolResolver("/system/lib/libEGL.so", OBFUSCATE("eglSwapBuffers")), (void *) hook_eglSwapBuffers, (void **) &old_eglSwapBuffers);
	
    patchBssOffset(ballhandling2, "000090C1", true);
      patchBssOffset(ballhandling, "0000A0C1", true);
      
//HD PLAYER
      patchBssOffset(hdplayer, floatToReverseHex(-1), true);
      patchBssOffset(hdplayer2, floatToReverseHex(-1), true); 
      patchBssOffset(hdplayer3, floatToReverseHex(-1), true); 
//Muslces
      patchBssOffset(muscles, floatToReverseHex(-0.680), true); 
    ShotPatch = MemoryPatch::createWithHex(
    "libnba2k20_clean_opt.so", 
    0x201361c, 
    "28 6E 6B 4E"
);

	HOOK_LIB(targetLibName, "0xe2eff0", my_StartPlayback, orig_StartPlayback);
	
	/*
	Il2CppAttach(targetLibName);
    sleep(5);
	DobbyHook(Il2CppGetMethodOffset("Assembly-CSharp.dll", "NameSpace", "ClassName", "MethodName", 0), (void*)hook_example, (void**)&orig_hookexample);
	*/
	
	
	/* 
	@il2cpp_dumper_Optional
    auto il2cpp_handle = dlopen(targetLibName, 4);
    il2cpp_dump(il2cpp_handle); 
    */
	
    
    return NULL;
}

jobjectArray EnglishList(JNIEnv *env, jobject context) {
    const char *features[] = {
        OBFUSCATE("RichTextView_CheatCode lgl imgui project"),
    };

    int total = sizeof(features) / sizeof(features[0]);
    jobjectArray ret = env->NewObjectArray(total, env->FindClass(OBFUSCATE("java/lang/String")),
                                           env->NewStringUTF(""));
    for (int i = 0; i < total; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));
    return ret;
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");
    switch (featNum) {
		case 0:
			break;
    }
}

__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
            {OBFUSCATE("IsGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
            {OBFUSCATE("Init"),  OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)V"), reinterpret_cast<void *>(Init)},
            {OBFUSCATE("SettingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(SettingsList)},
    		{OBFUSCATE("EnglishList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(EnglishList)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterPreferences(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Preferences"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"), reinterpret_cast<void *>(CheckOverlayPermission)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Main"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}




extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
     jvm = vm;
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (RegisterMenu(env) != 0)
        return JNI_ERR;
    if (RegisterPreferences(env) != 0)
        return JNI_ERR;
    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}
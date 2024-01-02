#include <windows.h>
#include <iostream>
#include <string>

#include <thread>
#include <chrono>

#include "offsets.h"
#include "memory.h"

#include <fbxsdk.h>

using namespace std;
using namespace fbxsdk;

#define FILE_NAME "file.fbx"
#define FRAME_RATE 60
#define RECORD_DURARION 4.0

// logs
bool LogError(const string& log)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    cerr << "Error: ";

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << log << endl;

    Sleep(3250);
    return false;
}

void LogSuccess(const string& log)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    cout << "Success: ";

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << log << endl;
}

// player class
class Player
{
public:
    float translationX;
    float translationY;
    float translationZ;

    float rotationX;
    float rotationY;
    float rotationZ;
};

int main()
{
    Player player;

    cout << "[insert] Start record" << endl;
    cout << "[delete] Stop record" << endl;

    // get procces
    uintptr_t processID = GetProcessID(L"cs2.exe");
    if (!processID)
        return LogError("cant find process");
    else
        LogSuccess("procces find");

    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!processHandle)
        return LogError("failed to open process");
    else
        LogSuccess("procces opened");

    uintptr_t clientModule = GetModuleBaseAddress(processID, L"client.dll");
    if (!clientModule)
        return LogError("failed to get module base address");
    else
        LogSuccess("module base address readed");

    // scene
    FbxManager* manager = FbxManager::Create();
    if (!manager)
        return LogError("failed to create fbx manager");
    else
        LogSuccess("fbx manager created");

    FbxScene* scene = FbxScene::Create(manager, "scene");
    if (!scene)
        return LogError("failed to create fbx scene");
    else
        LogSuccess("fbx scene created");

    scene->GetGlobalSettings().SetTimeMode(FbxTime::eFrames60);
    scene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);

    // anim stack
    FbxAnimStack* stack = FbxAnimStack::Create(scene, "stack");
    FbxAnimLayer* layer = FbxAnimLayer::Create(scene, "layer");
    stack->AddMember(layer);

    // create camera
    FbxCamera* camera = FbxCamera::Create(scene, "camera");
    FbxNode* cameraNode = FbxNode::Create(scene, "viewmodel");
    scene->GetRootNode()->AddChild(cameraNode);

    cameraNode->SetNodeAttribute(camera);
    camera->FocusDistance.Set(3000.0);      // culing distance

    // Key frames
    FbxTime time;
    int key;

    FbxAnimCurve* translationX = cameraNode->LclTranslation.GetCurve(layer, "X", true);
    FbxAnimCurve* translationY = cameraNode->LclTranslation.GetCurve(layer, "Y", true);
    FbxAnimCurve* translationZ = cameraNode->LclTranslation.GetCurve(layer, "Z", true);

    FbxAnimCurve* rotationX = cameraNode->LclRotation.GetCurve(layer, "X", true);
    FbxAnimCurve* rotationY = cameraNode->LclRotation.GetCurve(layer, "Y", true);
    FbxAnimCurve* rotationZ = cameraNode->LclRotation.GetCurve(layer, "Z", true);

    FbxAnimCurve* scaleX = cameraNode->LclScaling.GetCurve(layer, "X", true);
    FbxAnimCurve* scaleY = cameraNode->LclScaling.GetCurve(layer, "Y", true);
    FbxAnimCurve* scaleZ = cameraNode->LclScaling.GetCurve(layer, "Z", true);

    for (int i = 0; i < static_cast<int>(FRAME_RATE * RECORD_DURARION); ++i)
    {
        // read memory
        ReadProcessMemory(processHandle, (LPCVOID)(clientModule + offsets::viewMatrix + offsets::translationX), &player.translationX, sizeof(float), NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(clientModule + offsets::viewMatrix + offsets::translationY), &player.translationY, sizeof(float), NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(clientModule + offsets::viewMatrix + offsets::translationZ), &player.translationZ, sizeof(float), NULL);

        ReadProcessMemory(processHandle, (LPCVOID)(clientModule + offsets::viewMatrix + offsets::rotationX), &player.rotationX, sizeof(float), NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(clientModule + offsets::viewMatrix + offsets::rotationY), &player.rotationY, sizeof(float), NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(clientModule + offsets::viewMatrix + offsets::rotationZ), &player.rotationZ, sizeof(float), NULL);

        // place keys
        time.SetFrame(i);

        key = translationX->KeyAdd(time);
        translationX->KeySet(key, time, player.translationX, FbxAnimCurveDef::eInterpolationLinear);
        key = translationY->KeyAdd(time);
        translationY->KeySet(key, time, player.translationZ, FbxAnimCurveDef::eInterpolationLinear);    // swap Y and Z
        key = translationZ->KeyAdd(time);
        translationZ->KeySet(key, time, -player.translationY, FbxAnimCurveDef::eInterpolationLinear);   // swap Y and Z

        key = rotationX->KeyAdd(time);
        rotationX->KeySet(key, time, 0, FbxAnimCurveDef::eInterpolationLinear);
        key = rotationY->KeyAdd(time);
        rotationY->KeySet(key, time, 0, FbxAnimCurveDef::eInterpolationLinear);
        key = rotationZ->KeyAdd(time);
        rotationZ->KeySet(key, time, player.rotationX, FbxAnimCurveDef::eInterpolationLinear);

        key = scaleX->KeyAdd(time);
        scaleX->KeySet(key, time, 1, FbxAnimCurveDef::eInterpolationLinear);
        key = scaleY->KeyAdd(time);
        scaleY->KeySet(key, time, 1, FbxAnimCurveDef::eInterpolationLinear);
        key = scaleZ->KeyAdd(time);
        scaleZ->KeySet(key, time, 1, FbxAnimCurveDef::eInterpolationLinear);

        // delay
        this_thread::sleep_for(chrono::milliseconds(static_cast<int>(1000.0 / FRAME_RATE)));
    }

    // Export
    FbxExporter* exporter = FbxExporter::Create(manager, "");
    FbxString exportFileName(FILE_NAME);

    if (!exporter->Initialize(exportFileName.Buffer(), -1, manager->GetIOSettings()))
        return LogError("failed to initialize fbx exporter");
    else
        LogSuccess("initialize fbx done");

    if (!exporter->Export(scene))
        return LogError("failed to export scene to fbx");
    else
        LogSuccess("scene successfully recorded to fbx");

    // clear
    Sleep(5000);

    exporter->Destroy();
    scene->Destroy();
    manager->Destroy();
    CloseHandle(processHandle);

    return 0;
}
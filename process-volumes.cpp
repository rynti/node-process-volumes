#include <string>
#include <memory>
#include <vector>

#include <node.h>

#undef _WIN32_WINNT
#undef WINVER
#include <Windows.h>
#include <ks.h>
#include <ksmedia.h>
#include <devicetopology.h>
#include <mmdeviceapi.h>
#include <tchar.h>
#include <atlstr.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <psapi.h>

namespace process_volumes {

struct ProcessVolume {
  int processId;
  std::string processPath;
  std::string processName;
  float peakVolume;
};

std::string GetProcessNameFromPid(DWORD pid) {
  std::string name;

  HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  if (handle) {
    TCHAR buffer[MAX_PATH];
    GetModuleBaseName(handle, 0, buffer, MAX_PATH);
    CloseHandle(handle);
    name = buffer;
  }

  return std::move(name);
}

float GetPeakVolumeFromAudioSessionControl(IAudioSessionControl* session) {
  IAudioMeterInformation* info = nullptr;
  session->QueryInterface(__uuidof(IAudioMeterInformation), (void **)&info);

  float peakVolume;
  info->GetPeakValue(&peakVolume);

  info->Release();
  info = nullptr;

  return peakVolume;
}

IAudioSessionEnumerator* GetAudioSessionEnumerator() {
  IMMDeviceEnumerator* deviceEnumerator = nullptr;
  CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);

  IMMDevice* device = nullptr;
  deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);

  IAudioSessionManager2* sessionManager = nullptr;
  device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&sessionManager);

  IAudioSessionEnumerator* enumerator = nullptr;
  sessionManager->GetSessionEnumerator(&enumerator);

  deviceEnumerator->Release();
  device->Release();
  sessionManager->Release();

  return enumerator;
}

std::vector<ProcessVolume> GetProcessVolumes() {
  std::vector<ProcessVolume> volumes;

  CoInitialize(NULL);

  IAudioSessionEnumerator* enumerator = GetAudioSessionEnumerator();
  int sessionCount;
  enumerator->GetCount(&sessionCount);
  for (int index = 0; index < sessionCount; index++) {
    IAudioSessionControl* session = nullptr;
    IAudioSessionControl2* session2 = nullptr;
    enumerator->GetSession(index, &session);
    session->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&session2);

    DWORD id = 0;
    session2->GetProcessId(&id);

    std::string processName = GetProcessNameFromPid(id);
    float peakVolume = GetPeakVolumeFromAudioSessionControl(session);

    ProcessVolume data;
    data.processName = processName;
    data.processId = id;
    data.peakVolume = peakVolume;
    volumes.push_back(data);

    session2->Release();
    session->Release();
  }
  enumerator->Release();

  return std::move(volumes);
}

void GetProcessVolumesHelper(const v8::FunctionCallbackInfo<v8::Value>& args) {
  std::vector<ProcessVolume> volumes = GetProcessVolumes();

  v8::Isolate* isolate = args.GetIsolate();

  v8::Local<v8::Array> array = v8::Array::New(isolate, volumes.size());
  for (int i = 0; i < volumes.size(); i++) {
    v8::Local<v8::Object> object = v8::Object::New(isolate);
    object->Set(v8::String::NewFromUtf8(isolate, "processPath"), v8::String::NewFromUtf8(isolate, volumes[i].processPath.c_str()));
    object->Set(v8::String::NewFromUtf8(isolate, "processName"), v8::String::NewFromUtf8(isolate, volumes[i].processName.c_str()));
    object->Set(v8::String::NewFromUtf8(isolate, "processId"), v8::Integer::New(isolate, volumes[i].processId));
    object->Set(v8::String::NewFromUtf8(isolate, "peakVolume"), v8::Number::New(isolate, volumes[i].peakVolume));
    array->Set(i, object);
  }

  args.GetReturnValue().Set(array);
}

void init(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "getProcessVolumes", GetProcessVolumesHelper);
}

NODE_MODULE(process_volumes, init)

}

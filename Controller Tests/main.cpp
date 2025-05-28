#define WIN32_LEAN_AND_MEAN
#include <dinput.h>

#include <iostream>

BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* instance, VOID* context) {
    std::cout << "Dound device: " << instance->tszProductName << std::endl;
    return DIENUM_CONTINUE;
}

void ControllerUpdate()
{
    IDirectInput8* dinput;
    HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
                                   IID_IDirectInput8, (void**)&dinput, NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to create DirectInput8 interface\n";
        return;
    }

    GUID joystickGuid;
    // Enumerate devices, for brevity assume you got joystickGuid from enumeration

    // For demo: just get the first joystick device
    dinput->EnumDevices(DI8DEVCLASS_GAMECTRL, [](const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) -> BOOL {
        GUID* pGuid = (GUID*)pContext;
        *pGuid = pdidInstance->guidInstance;
        std::cout << "Found controller: " << pdidInstance->tszProductName << std::endl;
        return DIENUM_STOP;  // Stop after first device
    }, &joystickGuid, DIEDFL_ATTACHEDONLY);

    IDirectInputDevice8* joystickDevice;
    hr = dinput->CreateDevice(joystickGuid, &joystickDevice, NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to create device\n";
        return;
    }

    joystickDevice->SetDataFormat(&c_dfDIJoystick2);
    // Use NULL HWND and non-exclusive cooperative level for console app:
    joystickDevice->SetCooperativeLevel(NULL, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    joystickDevice->Acquire();

    DIJOYSTATE2 js;

    while (true) {
        hr = joystickDevice->Poll();
        if (FAILED(hr)) {
            // Try to reacquire if lost
            joystickDevice->Acquire();
            continue;
        }

        hr = joystickDevice->GetDeviceState(sizeof(DIJOYSTATE2), &js);
        if (SUCCEEDED(hr)) {
            std::cout << "X: " << js.lX << " Y: " << js.lY << " Buttons: ";
            for (int i = 1; i < 32; i++) {
                std::cout << ((js.rgbButtons[i] & 0x80) ? "1" : "0");
            }
            if (js.rgbButtons[0] & 0x80)
                return;
            std::cout << "\r"; // carriage return to overwrite line
        }
    }

    // Cleanup (never reached in this loop)
    joystickDevice->Unacquire();
    joystickDevice->Release();
    dinput->Release();
}

class InputClass
{
public:
    InputClass();
    InputClass(const InputClass&);
    ~InputClass();

    bool Initialize(HINSTANCE, HWND, int, int);
    void Shutdown();
    bool Frame();

    void GetAxis(int&, int&);
    bool IsButtonPressed();

private:
    bool ReadGamepad();
    void ProcessInput();

private:

    IDirectInput8* m_directInput;
    IDirectInputDevice8* m_gamepad;

    DIJOYSTATE2 m_controllerState;

};

int main()
{
    ControllerUpdate();

}
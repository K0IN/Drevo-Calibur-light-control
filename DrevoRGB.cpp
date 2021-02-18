#include <iostream>
#include <Windows.h>

// this is just a showcase on how to use the DREVO72HWI.dll to control the Light on Drevo Keyboards!

constexpr int data_size = 72 * 3;

typedef void* (__thiscall* ctor_t)(void*);
typedef void  (__thiscall* dtor_t)(void*);
typedef bool  (__thiscall* detectDevice)(void*);
typedef bool  (__thiscall* setKeyboardLightArea)(void*, unsigned int*);
typedef bool  (__thiscall* createDev)(void*);
typedef bool  (__thiscall* CloseDev_t)(void*);

int main(int argc, char* argv[]) {
	
	HMODULE dll = LoadLibrary(L"DREVO72HWI.dll");

	if (!dll) {
		std::cout << "cant finde: DREVO72HWI.dll!" << std::endl;
		return 2;
	}

	ctor_t new_CKB75HWI = (ctor_t)GetProcAddress(dll, "??0CKB75HWI@@QAE@XZ");	
	detectDevice detect = (detectDevice)GetProcAddress(dll, "?DetectDevice@CKB75HWI@@QAE_NXZ");
	setKeyboardLightArea setLight = (setKeyboardLightArea)GetProcAddress(dll, "?setKeyboardLightArea@CKB75HWI@@QAE_NPAH@Z");
	createDev createDevice = (createDev)GetProcAddress(dll, "?__createDev@CKB75HWI@@QAE_NXZ");
	dtor_t delete_CKB75HWI = (dtor_t)GetProcAddress(dll, "??1CKB75HWI@@QAE@XZ");
	CloseDev_t closeDevice = (CloseDev_t)GetProcAddress(dll, "?__closeDev@CKB75HWI@@QAE_NXZ");

	if( !new_CKB75HWI || !detect || !setLight || !createDevice || !delete_CKB75HWI  || !closeDevice) {
		std::cout << "DREVO72HWI.dll can finde dll expoer!" << std::endl;
		FreeLibrary(dll);
		return 3;
	}

	// hacky way to init it on the HEAP 
	void* f = new int[0xff];

	void* CKB75HWI_instance = (void*)new_CKB75HWI(f);

	if (!createDevice(CKB75HWI_instance)) {
		std::cout << "CKB75HWI::createDevice Failed!" << std::endl;
		goto deinit;
		return 4;
	}

	if (!detect(CKB75HWI_instance)) {
		std::cout << "CKB75HWI::detectDevice No device Detected!" << std::endl;
		goto deinit;
		return 5;
	}

	unsigned int color[data_size];
	
	/// write your color here !!!
	// example > memset(color, 0xff, sizeof(int) * data_size); // all white
	// every index is one color chan of a key 
	// index 0 = r of first key 
	// index 1 = g of first key 
	// index 2 = b of first key 
	// index 3 = r of second key 
	// ...
	// the color is int but only the bottom byte is used and the real controller only use 4 bits that are maped to 1 byte  
	
	
	std::cout << "CKB75HWI::setLight:" << setLight(CKB75HWI_instance, color) << std::endl;

	deinit:
		std::cout << "deinit!" << std::endl;
		closeDevice(CKB75HWI_instance);
		delete_CKB75HWI(CKB75HWI_instance);	
		delete[] f;
		FreeLibrary(dll);


	return 0;
}


<div align="center">

# CapWare-SDK-Lib
This Is A Lib Version Of CapWare SDK

Original Code: https://github.com/urfavhazee/capware-cpp-sdk

## Integration Example

```cpp
#include "Globals/CapWare/Include/CapWare.hpp"
#include <iostream>

int main() {
	capware::sdk sdk;
    if (!sdk.initialize()) {
        std::cout << "init failed\n";
        return 1;
	}

    //Example: Using The SDK
    std::cout << std::hex << sdk.player_team() << "\n";

	//Example: GetOffset
	uintptr_t NamePrivate = sdk.get_offset("APlayerState", "PlayerNamePrivate");

	std::cout << std::hex << NamePrivate << "\n";
	system("pause");
	return 0;
}
```

## U Have The Either Include The .hpp From This Code Or Make One In Your C++ App

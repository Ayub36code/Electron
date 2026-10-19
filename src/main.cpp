#include "UI/MainWindow.h"
#include <iostream>

int main() {
    MainWindow app;
    
    if(!app.initialize(1280, 720, "Advanced Electron Simulation")) {
        std::cout << "Failed to initialize application!" << std::endl;
        return -1;
    }
    
    std::cout << "Starting Electron Simulation..." << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - Drag mouse to orbit camera" << std::endl;
    std::cout << "  - Scroll to zoom" << std::endl;
    std::cout << "  - Press ESC to exit" << std::endl;
    std::cout << "  - Use GUI panels for controls" << std::endl;
    
    app.run();
    
    return 0;
}
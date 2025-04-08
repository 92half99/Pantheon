# Pantheon

Pantheon is an experimental game project that explores real-time rendering and networking concepts. The project demonstrates rendering cubes with Vulkan on the client side and implements a working server/client multiplayer framework. Built upon the [Utopia](https://github.com/92half99/UtopiaAppTemplate) application template, Pantheon leverages a lightweight framework designed for seamless integration of Vulkan with a powerful UI library.

## Features

- **Vulkan Rendering:**  
  Modern real-time 3D rendering using Vulkan, with a focus on cube-based visuals.

- **Multiplayer Architecture:**  
  A working server/client architecture supporting multiplayer gameplay concepts.

- **Modular Design:**  
  Separation of concerns with dedicated modules for client, common/shared code, and server functions.

- **Built on Utopia:**  
  Utilizes the lightweight Utopia framework which integrates Vulkan and Dear ImGui, providing a robust platform for developing desktop applications across Windows, macOS, and Linux.

## Getting Started

### Prerequisites

Before building the project, ensure you have the following installed:

- **Visual Studio 2022** (the provided setup scripts are tailored for this IDE)  
- **Vulkan SDK** (use a recent version for compatibility and performance)  
- **Git** (to clone the repository with submodules)

### Cloning the Repository

Clone the repository along with its submodules:

```
git clone --recurse-submodules https://github.com/92half99/Pantheon.git
```

# Counter Strike 2 camera exporter

Counter Strike 2 camera exporter is a written in C++ for exporting camera movements from gameplay to the Autodesk .fbx format.
This tool enables users to record the movement of the in-game camera, which can be imported into various 3d modeling and animation software for further editing and integration.

## Features
- **Real time camera recording**: Records the movement of the in-game camera in real-time during gameplay.
- **Export to fbx**: Exports the recorded camera movements as an file, preserving keyframes for further animation editing.
- **Customizable recording duration**: Allows users to specify the duration of the recording session to capture specific segments of gameplay.
- **Frame control**: Supports customizable frame rates for the exported animation, ensuring compatibility with different software platforms.

## Usage
1. **Start recording**: Press the `[insert]` key to begin recording the camera movement during gameplay.
2. **Stop recording**: Press the `[delete]` key to stop the recording session and export the captured camera movements to an file.
3. **Import into 3d software**: Import the exported file into your preferred 3d modeling or animation software to integrate the camera movements into your projects.

## Dependencies
- **fbx sdk**: Autodesk fbx sdk for creating and manipulating files.

## Installation
1. **Clone repository**: Clone the repository to your local machine using Git.
2. **Compile source code**: Compile the source code using a C++ compiler compatible with x64.
3. **Run executable**: Execute the compiled executable while Counter-Strike 2 is running.

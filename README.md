# Description
This is a **Server Core** project and several example projects using the server core are included. 
Please refer to my blog for more information. 
[blog]https://ddukddaksudal.tistory.com/113

# Environment
* Used Visual Studio 2022


# How to Build
1. Open the solution file with VisualStudio
2. Build ServerCore project
3. Run ./Tools/ProcedureGenerator/MakeExe.bat
4. Move the result (GenProcs.exe) to ./Common/Procedures
5. Run ./Tools/PacketGenerator/MakeExe.bat
6. Move the result (GenPackets.exe) to ./Common/Protobuf/bin
7. Build GameServer & DummyClient project(maybe twice)
8. Run GameServer & DummyClient project

# Usage

# Files
### ServerCore
ServerCore using IOCP. 
### GameServer
Example server using ServerCore
  - Protobuf : generating protocol.
  - 
### DummyClient
Example client using ServerCore
### Tools
Tools for automation.
  - PacketGenerator
  - ProcedureGenerator

    
### Reference

# SiprixUA
Project contains ready to use SIP VoIP Client (UserAgent).
It's console application, written on C++, and could be compiled for Windows/MacOS/Linux.
Application created as example of using Siprix API and can be used as automation test tool.

As SIP engine it uses Siprix SDK, included in binary form for each platform.

Application (Siprix) has ability to:

- Add multiple SIP accounts
- Send/receive multiple calls (Audio and Video)
- Manage calls with:
   - Hold
   - Mute microphone/camera
   - Play sound to call from mp3 file
   - Record received sound to file
   - Send/receive DTMF
   - Transfer
   - ...

Application's UI may not contain all the features, avialable in the SDK, they will be added later.

## Build notes

- **Windows**: 
  - Run `win\cmake_VS2022.bat` - it will generate `build\SiprixUA.sln`.   
  - Open generated solution file in the VS2022, build/run/debug the app.
  
- **MacOS**:
  - Start Terminal, go to folder `mac` of the cloned repo.
  - Enable execute permissions:  `chmod 755 ./cmake_XCode.sh`  
  - Run `./cmake_XCode.sh` - it will generate `build\SiprixUA.xcodeproj`. 
  - Open generated solution file in the XCodeVS2022, build app.
  - Start compiled app from terminal using commands: `cd build/out/SiprixUA.app/Contents/MacOS`, `./SiprixUA`

- **Linux**:
  - Start Terminal, go to folder `linux` of the cloned repo.
  - Enable execute permissions: `cmake_Makefiles.sh`.
  - Run `./cmake_Makefiles.sh` - it will generate make files and build app. 
  - Start compiled app from terminal using commands: `cd build/out`, `./SiprixUA` 	

## Limitations

Siprix doesn't provide VoIP services. For testing app you need an account(s) credentials from a SIP service provider(s). 
Some features may be not supported by all SIP providers.

Attached Siprix SDK works in trial mode and has limited call duration - it drops call after 60sec.
Upgrading to a paid license removes this restriction, enabling calls of any length.

Please contact [sales@siprix-voip.com](mailto:sales@siprix-voip.com) for more details.

## More resources

Product web site: https://siprix-voip.com

Manual: https://docs.siprix-voip.com

## Screeshots

<a href="https://docs.siprix-voip.com/screenshots/SiprixUA-Linux.PNG"  title="Linux screenshot">
<img src="https://docs.siprix-voip.com/screenshots/SiprixUA-Linux_Mini.png" width="50"></a>,<a href="https://docs.siprix-voip.com/screenshots/SiprixUA-MacOS.PNG"  title="MacOS screenshot">
<img src="https://docs.siprix-voip.com/screenshots/SiprixUA-MacOS_Mini.png" width="50"></a>,<a href="https://docs.siprix-voip.com/screenshots/SiprixUA-Windows.PNG"  title="Windows screenshot">
<img src="https://docs.siprix-voip.com/screenshots/SiprixUA-Windows_Mini.png" width="50"></a>



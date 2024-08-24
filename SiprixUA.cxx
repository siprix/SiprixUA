#include <signal.h>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

#ifdef __APPLE__
#include "SiprixCpp.h"
#else
#include "Siprix.h"
#endif

#define NOMINMAX

static void signalHandler(int signo)
{
    std::cerr << "Shutting down" << std::endl;
}

////////////////////////////////////////////////////////////////////////////
//SiprixCliApp

class SiprixCliApp : public Siprix::ISiprixEventHandler
{
public:
    int run();
    
    enum MenuId { eMain, eAccounts, eDevices, eCalls };

protected:
    //Menu    
    void handleCmds();
    bool handleCmdMain(MenuId& menuId, char cmd);
    bool handleCmdAccounts(char cmd);
    bool handleCmdCalls(char cmd);
    bool handleCmdDevices(char cmd);

    //Accounts
    void AddAccount();
    void DelAccount();
    void UnregAccount();
    void RegAccount();
    void UpdSecureMediaAccount();

    //Calls
    void InitiateCall();
    void EndCall();
    void RejectCall();
    void AcceptCall();
    void SendDtmfToCall();
    void TransferCallBlind();
    void TransferCallAttended();
    void PlayFileToCall();
    void RecordCallToFile();
    void MuteMicOfCall();
    void MuteCamOfCall();
    void ToggleHoldCall();
    void SwitchToCall();
    void MakeConfCall();

    //Devices
    void DisplayPlayoutDevices();
    void DisplayRecordDevices();
    void DisplayVideoDevices();
    void SelectDevice();

    //Callbacks
    void OnTrialModeNotified();
    void OnDevicesAudioChanged();

    void OnAccountRegState(Siprix::AccountId accId, Siprix::RegState state, const char* response);
    void OnNetworkState(const char* name, Siprix::NetworkState state);
    void OnPlayerState(Siprix::PlayerId playerId, Siprix::PlayerState state);
    void OnRingerState(bool started) {}
    
    void OnCallIncoming(Siprix::CallId callId, Siprix::AccountId accId, bool withVideo, const char* hdrFrom, const char* hdrTo);
    void OnCallConnected(Siprix::CallId callId, const char* hdrFrom, const char* hdrTo, bool withVideo);
    void OnCallTerminated(Siprix::CallId callId, uint32_t statusCode);
    void OnCallProceeding(Siprix::CallId callId, const char* response);
    void OnCallTransferred(Siprix::CallId callId, uint32_t statusCode);
    void OnCallRedirected(Siprix::CallId origCallId, Siprix::CallId relatedCallId, const char* referTo);
    void OnCallDtmfReceived(Siprix::CallId callId, uint16_t tone);    
    void OnCallHeld(Siprix::CallId callId, Siprix::HoldState state);
    void OnCallSwitched(Siprix::CallId callId);

    //Create and init siprix module
    bool initializeSiprixModule();
    void configureVideo();

protected:    
    Siprix::ISiprixModule* sprxModule_ = nullptr;
};


////////////////////////////////////////////////////////////////////////////
//main

int main(int argc, char** argv)
{
#ifndef _WIN32
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        std::cerr << "Couldn't install signal handler for SIGPIPE" << std::endl;
        exit(-1);
    }
#endif

    if (signal(SIGINT, signalHandler) == SIG_ERR)
    {
        std::cerr << "Couldn't install signal handler for SIGINT" << std::endl;
        exit(-1);
    }

    if (signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        std::cerr << "Couldn't install signal handler for SIGTERM" << std::endl;
        exit(-1);
    }
        
    SiprixCliApp app;
    return app.run();
}


////////////////////////////////////////////////////////////////////////////
//Helpers

void displayAccErr(Siprix::ErrorCode code, Siprix::AccountId accId, const char* success, const char* err)
{
    if (code == Siprix::ErrorCode::EOK)
        std::cout << success << ". AccId:" << accId << std::endl;
    else
        std::cout << err << ". Err: " << code << " " << Siprix::GetErrorText(code) << std::endl;
}


void displayCallErr(Siprix::ErrorCode code, Siprix::CallId callId, const char* success, const char* err)
{
    if (code == Siprix::ErrorCode::EOK)
        std::cout << success << ". CallId: ~~~ " << callId << " ~~~"<< std::endl;
    else
        std::cout << err << ". Err: " << code << " " << Siprix::GetErrorText(code) << std::endl;
}

const char* getAccRegStateStr(Siprix::RegState state)
{
    switch (state)
    {
        case Siprix::RegState::Success: return "Success";    
        case Siprix::RegState::Removed: return "Removed";
        default:                        return "Failed";
    }
}

const char* getNetworkStateStr(Siprix::NetworkState state)
{
    switch (state)
    {
        case Siprix::NetworkState::NetworkRestored: return "Restored";
        case Siprix::NetworkState::NetworkSwitched: return "Switched";
        default:                                    return "Lost";
    }
}

const char* getPlayerStateStr(Siprix::PlayerState state)
{
    switch (state)
    {
        case Siprix::PlayerState::PlayerStarted: return "PlayerStarted";
        case Siprix::PlayerState::PlayerStopped: return "PlayerStopped";
        default:                                 return "PlayerFailed";
    }
}

////////////////////////////////////////////////////////////////////////////
//Accounts

void SiprixCliApp::AddAccount()
{
    std::string server, extension, password;
    std::cout << "Enter server domain name or IP address: "; std::cin >> server;
    std::cout << "Enter extension: ";                        std::cin >> extension;
    std::cout << "Enter password: ";                         std::cin >> password;

    Siprix::AccData* acc = Siprix::Acc_GetDefault();    
    Siprix::Acc_SetSipServer(acc,    server.c_str());
    Siprix::Acc_SetSipExtension(acc, extension.c_str());
    Siprix::Acc_SetSipPassword(acc,  password.c_str());    
    Siprix::Acc_SetTranspProtocol(acc, Siprix::SipTransport::TCP);
    Siprix::Acc_SetExpireTime(acc, 300);
    
    //Siprix::Acc_SetRewriteContactIp(acc, true);
    //Siprix::Acc_SetDisplayName(acc, "%%%");
    //Siprix::Acc_SetUserAgent(acc, "-%-");
    //Siprix::Acc_SetKeepAliveTime(acc, 0);
    
    //Siprix::Acc_ResetAudioCodecs(acc);
    //Siprix::Acc_AddAudioCodec(acc, Siprix::AudioCodec::PCMA);
    //Siprix::Acc_AddAudioCodec(acc, Siprix::AudioCodec::DTMF);
    
    //Siprix::Acc_SetSecureMediaMode(acc, Siprix::SecureMedia::SdesSrtp);
    //Siprix::Acc_SetInstanceId(acc, Siprix::Acc_GenerateInstanceId());
    //Siprix::Acc_AddXContactUriParam(acc, "pn-prid", "ASDFSDFDSFDS1");
    //Siprix::Acc_SetRingToneFile(acc, "ringtone.mp3");
    
    Siprix::AccountId accId=0;
    const Siprix::ErrorCode err = Siprix::Account_Add(sprxModule_, acc, &accId);
    displayAccErr(err, accId, "Accound added", "Can't add account");
}

void SiprixCliApp::DelAccount()
{
    Siprix::AccountId accId = 0;
    std::cout << "Enter accId to delete: ";
    std::cin >> accId;

    const Siprix::ErrorCode err = Siprix::Account_Delete(sprxModule_, accId);
    displayAccErr(err, accId, "Accound deleted successfully", "Can't delete  account");
}

void SiprixCliApp::UnregAccount()
{
    Siprix::AccountId accId = 0;
    std::cout << "Enter accId to unregister: ";
    std::cin >> accId;

    const Siprix::ErrorCode err = Siprix::Account_Unregister(sprxModule_, accId);
    displayAccErr(err, accId, "Unregister request sent", "Can't unregister account");
}

void SiprixCliApp::RegAccount()
{
    int expireSec=300;
    Siprix::AccountId accId = 0;
    std::cout << "Enter accId to update registration: ";     std::cin >> accId;
    std::cout << "Enter expire time (seconds): ";    std::cin >> expireSec;    

    const Siprix::ErrorCode err = Siprix::Account_Register(sprxModule_, accId, expireSec);
    displayAccErr(err, accId, "Register request sent", "Can't register account");
}

void SiprixCliApp::UpdSecureMediaAccount()
{
    int sMedia = 0;
    Siprix::AccountId accId = 0;
    std::cout << "Enter accId to update: ";
    std::cin >> accId;

    std::cout << "Enter secure media setting [0(Disabled), 1(SDES SRTP), 2(DTLS SRTP)]: ";
    std::cin >> sMedia;

    Siprix::AccData* acc = Siprix::Acc_GetDefault();
    Siprix::Acc_SetSecureMediaMode(acc, static_cast<Siprix::SecureMedia>(sMedia));

    const Siprix::ErrorCode err = Siprix::Account_Update(sprxModule_, acc, accId);
    displayAccErr(err, accId, "Account updated", "Can't update account");
}


////////////////////////////////////////////////////////////////////////////
//Calls

void SiprixCliApp::InitiateCall()
{
    //Ask details
    char withVideo=0;
    std::string destExt;
    Siprix::AccountId accId = 0;
    std::cout << "Enter accId where to initiate call: ";   std::cin >> accId;
    std::cout << "Enter destination number (extension): "; std::cin >> destExt;
    std::cout << "Make call with video (y/n): ";           std::cin >> withVideo;

    //Prepare dest
    Siprix::DestData* dest = Siprix::Dest_GetDefault();
    Dest_SetExtension(dest, destExt.c_str());
    Dest_SetAccountId(dest, accId);
    Dest_SetVideoCall(dest, (withVideo=='v')||(withVideo == 'y'));
    //Dest_AddXHeader(dest, "XTest", "invHeaderVal1");
    //Dest_AddXHeader(dest, "XTest", "invHeaderVal2");

    //Start call
    Siprix::CallId callId = 0;
    const Siprix::ErrorCode err = Siprix::Call_Invite(sprxModule_, dest, &callId);
    displayCallErr(err, callId, "Starting...", "Can't initiate call");
}

void SiprixCliApp::EndCall()
{
    Siprix::CallId callId = 0;
    std::cout << "Enter callId to end: ";   std::cin >> callId;
    
    const Siprix::ErrorCode err = Siprix::Call_Bye(sprxModule_, callId);
    displayCallErr(err, callId, "End call request has sent", "Can't end call");
}

void SiprixCliApp::RejectCall()
{
    Siprix::CallId callId = 0;
    std::cout << "Enter callId to reject: ";   std::cin >> callId;

    const Siprix::ErrorCode err = Siprix::Call_Reject(sprxModule_, callId, 486);
    displayCallErr(err, callId, "Call rejected", "Can't reject call");
}

void SiprixCliApp::AcceptCall()
{
    char withVideo = false;
    Siprix::CallId callId = 0;
    std::cout << "Enter callId to accept: ";        std::cin >> callId;
    std::cout << "Accept call with video (y/n): ";  std::cin >> withVideo;

    const Siprix::ErrorCode err = Siprix::Call_Accept(sprxModule_, callId, (withVideo == 'v') || (withVideo == 'y'));
    displayCallErr(err, callId, "Call accepting... ", "Can't accept call");
}


void SiprixCliApp::SendDtmfToCall()
{
    std::string tones;
    Siprix::CallId callId = 0;
    Siprix::DtmfMethod method = Siprix::DtmfMethod::DTMF_RTP;//DTMF_INFO;
    std::cout << "Enter callId where to send tones: ";   std::cin >> callId;
    std::cout << "Enter DTMF tone(s): ";   std::cin >> tones;

    const Siprix::ErrorCode err = Siprix::Call_SendDtmf(sprxModule_, callId, tones.c_str(), 200, 50, method);
    displayCallErr(err, callId, "Sending tones started successfully", "Can't send tones");
}

void SiprixCliApp::TransferCallBlind()
{
    std::string toAddr;
    Siprix::CallId callId = 0;
    std::cout << "Enter callId to transfer: ";  std::cin >> callId;
    std::cout << "Enter destination addr: ";   std::cin >> toAddr;

    const Siprix::ErrorCode err = Siprix::Call_TransferBlind(sprxModule_, callId, toAddr.c_str());
    displayCallErr(err, callId, "Transfer request sent", "Can't transfer");
}

void SiprixCliApp::TransferCallAttended()
{
    Siprix::CallId srcCallId = 0, destCallId = 0;
    std::cout << "Enter callId to transfer: ";  std::cin >> srcCallId;
    std::cout << "Enter destination callId: ";   std::cin >> destCallId;

    const Siprix::ErrorCode err = Siprix::Call_TransferAttended(sprxModule_, srcCallId, destCallId);
    displayCallErr(err, srcCallId, "Transfer request sent", "Can't transfer");
}


void SiprixCliApp::PlayFileToCall()
{
    std::string mp3File;
    Siprix::CallId callId = 0;
    std::cout << "Enter callId where to play mp3 file: ";   std::cin >> callId;
    std::cout << "Enter path(name) of mp3 file: ";         std::cin >> mp3File;
    
    Siprix::PlayerId playerId=0;
    const Siprix::ErrorCode err = Siprix::Call_PlayFile(sprxModule_, callId, mp3File.c_str(), false, &playerId);
    displayCallErr(err, callId, "Play file started successfully", "Can't play file");
}

void SiprixCliApp::RecordCallToFile()
{
    bool start = true;
    Siprix::CallId callId = 0;
    std::cout << "Enter callId to start/stop recording: "; std::cin >> callId;
    std::cout << "Enter 1 to start/0 stop recording: ";    std::cin >> start;

    std::string filePath = std::to_string(callId) + ".wav";
    const Siprix::ErrorCode err = start ? Siprix::Call_RecordFile(sprxModule_, callId, filePath.c_str())
                                        : Siprix::Call_StopRecordFile(sprxModule_, callId);
    displayCallErr(err, callId, "Record file started successfully", "Can't record file");
}

void SiprixCliApp::MuteMicOfCall()
{
    bool mute = true;
    Siprix::CallId callId = 0;
    std::cout << "Enter callId where to mute mic: "; std::cin >> callId;    
    std::cout << "Enter 1 to mute/0 unmute: ";       std::cin >> mute;

    const Siprix::ErrorCode err = Siprix::Call_MuteMic(sprxModule_, callId, mute);
    displayCallErr(err, callId, "Mute state changed successfully", "Can't mute call");
}

void SiprixCliApp::MuteCamOfCall()
{
    bool mute = true;
    Siprix::CallId callId = 0;
    std::cout << "Enter callId where to mute camera: "; std::cin >> callId;
    std::cout << "Enter 1 to mute/0 unmute: ";          std::cin >> mute;

    const Siprix::ErrorCode err = Siprix::Call_MuteCam(sprxModule_, callId, mute);
    displayCallErr(err, callId, "Mute state changed successfully", "Can't mute call");
}

void SiprixCliApp::ToggleHoldCall()
{
    Siprix::CallId callId = 0;
    std::cout << "Enter callId to hold: "; std::cin >> callId;

    const Siprix::ErrorCode err = Siprix::Call_Hold(sprxModule_, callId);
    displayCallErr(err, callId, "Hold request sent", "Can't hold call");
}

void SiprixCliApp::SwitchToCall()
{
    Siprix::CallId callId = 0;
    std::cout << "Enter callId where to switch: ";   std::cin >> callId;

    const Siprix::ErrorCode err = Siprix::Mixer_SwitchToCall(sprxModule_, callId);
    displayCallErr(err, callId, "Switched to call successfully", "Can't switch to call");    
}

void SiprixCliApp::MakeConfCall()
{
    const Siprix::ErrorCode err = Siprix::Mixer_MakeConference(sprxModule_);
    displayCallErr(err, 0, "Calls joined to conference", "Can't make conference");
}


////////////////////////////////////////////////////////////////////////////
//Devices

void SiprixCliApp::DisplayPlayoutDevices()
{
    uint32_t numberOfDevices=0;
    Siprix::ErrorCode err = Dvc_GetPlayoutDevices(sprxModule_, &numberOfDevices);
    if(!numberOfDevices || (err != Siprix::ErrorCode::EOK))
        return;

    char name[50] = "";
    char guid[50] = "";
    std::cout << "Detected "<< numberOfDevices <<" playout audio devices:";
    for (uint32_t i = 0; i < numberOfDevices; ++i) {
        err = Dvc_GetPlayoutDevice(sprxModule_, i, name, sizeof(name), guid, sizeof(guid));
        std::cout << "\n    -" << i << "- " << name << " [" << guid<<"]";
    }
    std::cout << std::endl;
}

void SiprixCliApp::DisplayRecordDevices()
{
    uint32_t numberOfDevices = 0;
    Siprix::ErrorCode err = Dvc_GetRecordingDevices(sprxModule_, &numberOfDevices);
    if (!numberOfDevices || (err != Siprix::ErrorCode::EOK))
        return;

    char name[50] = "";
    char guid[50] = "";
    std::cout << "Detected " << numberOfDevices << " recording audio devices:";
    for (uint32_t i = 0; i < numberOfDevices; ++i) {
        err = Dvc_GetRecordingDevice(sprxModule_, i, name, sizeof(name), guid, sizeof(guid));
        std::cout << "\n    -" << i << "- " << name << " [" << guid << "]";
    }
    std::cout << std::endl;
}

void SiprixCliApp::DisplayVideoDevices()
{
    uint32_t numberOfDevices = 0;
    Siprix::ErrorCode err = Dvc_GetVideoDevices(sprxModule_, &numberOfDevices);
    if (!numberOfDevices || (err != Siprix::ErrorCode::EOK))
        return;

    char name[50] = "";
    char guid[50] = "";
    std::cout << "Detected " << numberOfDevices << " recording audio devices:";
    for (uint32_t i = 0; i < numberOfDevices; ++i) {
        err = Dvc_GetVideoDevice(sprxModule_, i, name, sizeof(name), guid, sizeof(guid));
        std::cout << "\n    -" << i << "- " << name << " [" << guid << "]";
    }
    std::cout << std::endl;
}

void SiprixCliApp::SelectDevice()
{
    char deviceType=0;
    std::cout << "Enter which device to set: p - Playback, r - Recording, v - Video: ";
    std::cin >> deviceType;

    int deviceIndex=0;
    std::cout << "Enter device index: ";
    std::cin >> deviceIndex;

    Siprix::ErrorCode err = Siprix::ErrorCode::EOK;
    switch (deviceType)
    {
        case 'p': err = Dvc_SetPlayoutDevice(sprxModule_, deviceIndex); break;
        case 'r': err = Dvc_SetRecordingDevice(sprxModule_, deviceIndex); break;
        case 'v': err = Dvc_SetVideoDevice(sprxModule_, deviceIndex); break;
        default : std::cout<<"Wrong device type."<<std::endl;
    }

    if(err != Siprix::ErrorCode::EOK)
        std::cout << "Err: " << err << " " << Siprix::GetErrorText(err) << std::endl;
}


////////////////////////////////////////////////////////////////////////////
//Callbacks

void SiprixCliApp::OnAccountRegState(Siprix::AccountId accId, Siprix::RegState state, const char* response)
{
    std::cout << "\n--- OnAccountRegState accId:" << accId 
              << " state:" << getAccRegStateStr(state)
              << " response:" << response << std::endl;
}

void SiprixCliApp::OnNetworkState(const char* name, Siprix::NetworkState state)
{
    std::cout << "\n---!!! OnNetworkState name:" << name << " state:" << getNetworkStateStr(state) << std::endl;
}

void SiprixCliApp::OnPlayerState(Siprix::PlayerId playerId, Siprix::PlayerState state)
{
    std::cout << "\n--- OnPlayerState playerId:" << playerId
        << " state:" << getPlayerStateStr(state) << std::endl;
}

void SiprixCliApp::OnCallProceeding(Siprix::CallId callId, const char* response)
{
    std::cout << "\n--- OnCallProceeding callId:" << callId 
              << " response:" << response << std::endl;
}

void SiprixCliApp::OnCallTerminated(Siprix::CallId callId, uint32_t statusCode)
{
    std::cout << "\n--- OnCallTerminated callId:" << callId 
              << " statusCode:" << statusCode << std::endl;
}

void SiprixCliApp::OnCallConnected(Siprix::CallId callId, const char* hdrFrom, const char* hdrTo, bool withVideo)
{
    std::cout << "\n--- OnCallConnected callId:" << callId 
              << " From:[" << hdrFrom << "] To:[" << hdrTo << "] withVideo: " << withVideo << std::endl;
}

void SiprixCliApp::OnCallIncoming(Siprix::CallId callId, Siprix::AccountId accId, bool withVideo, const char* hdrFrom, const char* hdrTo)
{
    std::cout << "\n--- OnCallIncoming callId:" << callId <<" accId:" 
              << accId <<" withVideo:"<< withVideo
              << " From:[" << hdrFrom << "] To:[" << hdrTo << "]" << std::endl;
}

void SiprixCliApp::OnCallDtmfReceived(Siprix::CallId callId, uint16_t tone)
{
    char ch = (tone == 10) ? '*' : (tone==11 ? '#' : tone+'0');
    std::cout << "\n--- OnCallDtmfReceived callId:" << callId 
              << " tone:" << ch << std::endl;
}

void SiprixCliApp::OnCallSwitched(Siprix::CallId callId)
{
    std::cout << "\n--- OnCallSwitched callId:" << callId << std::endl;
}

void SiprixCliApp::OnCallHeld(Siprix::CallId callId, Siprix::HoldState state)
{
    std::cout << "\n--- OnCallHeld callId:" << callId 
              << " holdState: " << (int)state << std::endl;
}

void SiprixCliApp::OnCallTransferred(Siprix::CallId callId, uint32_t statusCode)
{
    std::cout << "\n--- OnCallTransferred callId:" << callId
        << " statusCode: " << statusCode << std::endl;
}

void SiprixCliApp::OnCallRedirected(Siprix::CallId origCallId, Siprix::CallId relatedCallId, const char* referTo)
{
    std::cout << "\n--- OnCallRedirected origCallId:" << origCallId 
              <<" relatedCallId:" << relatedCallId << " referTo: " << referTo << std::endl;
}

void SiprixCliApp::OnDevicesAudioChanged()
{
    std::cout << "\n--- OnDevicesAudioChanged" << std::endl;
}

void SiprixCliApp::OnTrialModeNotified()
{
    std::cout << "\n--- SIPRIX SDK is working in TRIAL mode ---" << std::endl;
}





////////////////////////////////////////////////////////////////////////////
//Main

bool SiprixCliApp::handleCmdAccounts(char cmd)
{
    switch (cmd)
    {
        case 'a': AddAccount();     return false;
        case 'd': DelAccount();     return false;
        case 'u': UnregAccount();   return false;
        case 'r': RegAccount();     return false;
        case 's': UpdSecureMediaAccount();     return false;
        case '-': return true;//!!!
    }

    std::cout << "  a  Add account\n";
    std::cout << "  d  Delete account\n";
    std::cout << "  u  Unregister account\n";
    std::cout << "  r  Refresh account registration\n";
    std::cout << "  s  Update secure media settings\n";
    std::cout << "  -  -> Back to main menu\n";
    return false;
}

bool SiprixCliApp::handleCmdCalls(char cmd)
{
    switch (cmd)
    {
        case 'i': InitiateCall();   return false;
        case 'a': AcceptCall();     return false;
        case 'j': RejectCall();     return false;        
        case 'e': EndCall();        return false;

        case 'd': SendDtmfToCall(); return false;
        case 'p': PlayFileToCall(); return false;
        case 'r': RecordCallToFile(); return false;
        case 'm': MuteMicOfCall();  return false;
        case 'v': MuteCamOfCall();  return false;
        case 'h': ToggleHoldCall(); return false;
        case 't': TransferCallBlind();   return false;
        case 'x': TransferCallAttended();   return false;

        case 's': SwitchToCall();   return false;
        case 'c': MakeConfCall();   return false;

        case '-': return true;//!!!
    }

    std::cout << "  i  Initiate new call\n";
    std::cout << "  a  Accept incoming call\n";
    std::cout << "  j  Reject incoming call\n";
    std::cout << "  e  End/Cancel call\n\n";

    std::cout << "  d  Send DTMF tones to call\n";
    std::cout << "  p  Play file to call\n";
    std::cout << "  r  Record call to file\n";
    std::cout << "  m  Mute mic of call\n";
    std::cout << "  v  Mute camera of call\n";
    std::cout << "  h  Hold/unhold call\n";
    std::cout << "  t  Transfer call (blind)\n";
    std::cout << "  x  Transfert call (attended)\n\n";
    
    std::cout << "  s  Switch to call (start hear/speak it)\n";
    std::cout << "  c  Make conference call\n";

    std::cout << "  -  -> Back to main menu\n";
    return false;
}

bool SiprixCliApp::handleCmdDevices(char cmd)
{
    switch (cmd)
    {
        case 'p': DisplayPlayoutDevices(); return false;
        case 'r': DisplayRecordDevices();  return false;
        case 'v': DisplayVideoDevices();   return false;
        case 's': SelectDevice();          return false;
        case '-': return true;//!!!
    }

    std::cout << "  p  Display list of Playout devices\n";
    std::cout << "  r  Display list of Recording devices\n";
    std::cout << "  v  Display list of Video devices\n";
    std::cout << "  s  Switch device\n";    
    std::cout << "  -  -> Back to main menu\n";
    return false;
}


bool SiprixCliApp::handleCmdMain(MenuId& menuId, char cmd)
{   
    switch (cmd)
    {
        case 'A': menuId = eAccounts; handleCmdAccounts(cmd);  return false;
        case 'C': menuId = eCalls;    handleCmdCalls(cmd);     return false;
        case 'D': menuId = eDevices;  handleCmdDevices(cmd);   return false;
        case 'Q': case 'q': return true;//!!!
    }

    std::cout << " A  Accounts menu\n";
    std::cout << " C  Calls menu\n";
    std::cout << " D  Devices menu\n";
    std::cout << " Q  => Quit\n";
    return false;
}


void SiprixCliApp::handleCmds()
{
    //Run commands loop    
    MenuId curMenu = MenuId::eMain;
    handleCmdMain(curMenu, 0);

    while (true)
    {
        std::cout << "\n>>> Enter command: ";
        char cmd = '\0';
        if (!(std::cin >> cmd))
        {
            std::cout << "Failed to read!\n";
            std::cin.clear(); // Reset stream
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        bool menuExit = false;
        switch (curMenu)
        {
            case MenuId::eMain:     menuExit = handleCmdMain(curMenu, cmd); break;
            case MenuId::eAccounts: menuExit = handleCmdAccounts(cmd); break;
            case MenuId::eDevices:  menuExit = handleCmdDevices(cmd); break;
            case MenuId::eCalls:    menuExit = handleCmdCalls(cmd); break;
        }//switch

        if (menuExit)
        {
            if (curMenu == MenuId::eMain)
            {
                break;
            }
            else {
                curMenu = MenuId::eMain;
                handleCmdMain(curMenu, 0);//print main menu
            }
        }
    }//while

    //UnInitialize
    Module_UnInitialize(sprxModule_);
}

bool SiprixCliApp::initializeSiprixModule()
{
    //Create module
    sprxModule_ = Siprix::Module_Create();
    if (!sprxModule_)
    {
        std::cout << "Can't create siprix module." << std::endl;
        return false;
    }

    //Initialize
    Siprix::IniData* ini = Siprix::Ini_GetDefault();
    //Ini_SetHomeFolder(ini, "SiprixUA");
    Ini_SetLicense(ini, "...license-credentials...");
    Ini_SetLogLevelFile(ini, Siprix::LogLevel::Debug);
    Ini_SetLogLevelIde(ini, Siprix::LogLevel::NoLog);
    Ini_SetTlsVerifyServer(ini, false);

    const Siprix::ErrorCode err = Siprix::Module_Initialize(sprxModule_, ini);
    if (err != Siprix::ErrorCode::EOK)
    {
        std::cout << "Can't initialize siprix module. Err: "
                  << err << " " << Siprix::GetErrorText(err) << std::endl;
        return false;
    }
    else{
        std::cout << "Siprix module successfully initialized.\nVersion: "
                  << Siprix::Module_Version(sprxModule_) <<std::endl;

        configureVideo();
        
        //Set callbacks
        Callback_SetEventHandler(sprxModule_, this);
        return true;
    }
}

void SiprixCliApp::configureVideo()
{
    //Siprix::VideoData* vdoData = Siprix::Vdo_GetDefault();
    //Siprix::Vdo_SetBitrate(vdoData, 600);//600kbps
    //Siprix::Vdo_SetFramerate(vdoData, 5);//5fps
    //Siprix::Vdo_SetHeight(vdoData, 480); 
    //Siprix::Vdo_SetWidth(vdoData,  640); //640x480
    //Siprix::Vdo_SetNoCameraImgPath(vdoData, "logo.JPG");

    //Siprix::Dvc_SetVideoDevice(sprxModule_, 555);//force to use NoCameraImg
    //Siprix::Dvc_SetVideoParams(sprxModule_, vdoData);
}


int SiprixCliApp::run()
{
    if (initializeSiprixModule())
    {
        handleCmds();
        return 0;
    }

    return 1;
}

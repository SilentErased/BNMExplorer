#pragma once

#include "BNMResolve.hpp"
#include <vector>
#include <cstdint>

struct NetPlayer;
struct Player;
struct Hashtable;
struct PhotonNetwork;
struct ServerSettings;
struct ScriptableObject;
struct AppSettings;
struct RaiseEventOptions;
struct SendOptions;
struct PhotonView;
struct RoomInfo;
struct Room;
struct AuthenticationValues;
struct TypedLobby;
struct RoomOptions;



enum RpcTarget {
    All,
    Others,
    MasterClient,
    AllBuffered,
    OthersBuffered,
    AllViaServer,
    AllBufferedViaServer
};

enum PunLogLevel {
    ErrorsOnly,
    Informational,
    Full
};

enum ConnectionProtocol : uint8_t {
    Udp,
    Tcp,
    WebSocket = 4,
    WebSocketSecure
};

enum AuthModeOption {
    Auth,
    AuthOnce,
    AuthOnceWss
};

enum DebugLevel : uint8_t {
    OFF,
    ERROR,
    WARNING,
    INFO,
    ALL = 5
};

enum CustomAuthenticationType : uint8_t {
    Custom,
    Steam,
    Facebook,
    Oculus,
    PlayStation4,
    PlayStation = 4,
    Xbox,
    Viveport = 10,
    NintendoSwitch,
    PlayStation5,
    Playstation5 = 12,
    Epic,
    FacebookGaming = 15,
    None = 255
};

enum MatchmakingMode : uint8_t
{
    FillRoom,
    SerialMatching,
    RandomMatching
};

enum LobbyType : uint8_t
{
    Default,
    SqlLobby = 2,
    AsyncRandomLobby
};


struct RoomOptions : IL2CPP::Il2CppObject {
    static Class GetClass(){
        static Class cached = Class("Photon.Realtime", "RoomOptions");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    bool GetIsVisible() {
        static Method<bool> getisVisible = GetClass().GetMethod("get_IsVisible", 0);
        return getisVisible[this]();
    }

    void SetIsVisible(bool visible) {
        static Method<void> setisVisible = GetClass().GetMethod("set_IsVisible", 1);
        setisVisible[this](visible);
    }


    bool GetIsOpen() {
        static Method<bool> getIsOpen = GetClass().GetMethod("get_IsOpen", 0);
        return getIsOpen[this]();
    }

    void SetIsOpen(bool open) {
        static Method<void> setIsOpen = GetClass().GetMethod("set_IsOpen", 1);
        setIsOpen[this](open);
    }


    bool GetCleanupCacheOnLeave() {
        static Method<bool> getCleanupCacheOnLeave = GetClass().GetMethod("get_CleanupCacheOnLeave", 0);
        return getCleanupCacheOnLeave[this]();
    }

    void SetCleanupCacheOnLeave(bool value) {
        static Method<void> setCleanupCacheOnLeave = GetClass().GetMethod("set_CleanupCacheOnLeave", 1);
        setCleanupCacheOnLeave[this](value);
    }


    bool GetSuppressRoomEvents() {
        static Method<bool> getSuppressRoomEvents = GetClass().GetMethod("get_SuppressRoomEvents", 0);
        return getSuppressRoomEvents[this]();
    }

    void SetSuppressRoomEvents(bool value) {
        static Method<void> setSuppressRoomEvents = GetClass().GetMethod("set_SuppressRoomEvents", 1);
        setSuppressRoomEvents[this](value);
    }


    bool GetSuppressPlayerInfo() {
        static Method<bool> getSuppressPlayerInfo = GetClass().GetMethod("get_SuppressPlayerInfo", 0);
        return getSuppressPlayerInfo[this]();
    }

    void SetSuppressPlayerInfo(bool value) {
        static Method<void> setSuppressPlayerInfo = GetClass().GetMethod("set_SuppressPlayerInfo", 1);
        setSuppressPlayerInfo[this](value);
    }


    bool GetPublishUserId() {
        static Method<bool> getPublishUserId = GetClass().GetMethod("get_PublishUserId", 0);
        return getPublishUserId[this]();
    }

    void SetPublishUserId(bool value) {
        static Method<void> setPublishUserId = GetClass().GetMethod("set_PublishUserId", 1);
        setPublishUserId[this](value);
    }


    bool GetDeleteNullProperties() {
        static Method<bool> getDeleteNullProperties = GetClass().GetMethod("get_DeleteNullProperties", 0);
        return getDeleteNullProperties[this]();
    }

    void SetDeleteNullProperties(bool value) {
        static Method<void> setDeleteNullProperties = GetClass().GetMethod("set_DeleteNullProperties", 1);
        setDeleteNullProperties[this](value);
    }


    bool GetBroadcastPropsChangeToAll() {
        static Method<bool> getBroadcastPropsChangeToAll = GetClass().GetMethod("get_BroadcastPropsChangeToAll", 0);
        return getBroadcastPropsChangeToAll[this]();
    }

    void SetBroadcastPropsChangeToAll(bool value) {
        static Method<void> setBroadcastPropsChangeToAll = GetClass().GetMethod("set_BroadcastPropsChangeToAll", 1);
        setBroadcastPropsChangeToAll[this](value);
    }


    uint8_t GetMaxPlayers(){
        static Field<uint8_t> maxPlayers = GetClass().GetField("MaxPlayers");
        maxPlayers.SetInstance(this);
        return maxPlayers.Get();
    }

    void SetMaxPlayers(uint8_t value){
        static Field<uint8_t> maxPlayers = GetClass().GetField("MaxPlayers");
        maxPlayers.SetInstance(this);
        maxPlayers.Set(value);
    }


    int GetPlayerTtl(){
        static Field<int> maxPlayerTtl = GetClass().GetField("PlayerTtl");
        maxPlayerTtl.SetInstance(this);
        return maxPlayerTtl.Get();
    }

    void SetPlayerTtl(int value){
        static Field<int> maxPlayerTtl = GetClass().GetField("PlayerTtl");
        maxPlayerTtl.SetInstance(this);
        maxPlayerTtl.Set(value);
    }


    int GetEmptyRoomTtl(){
        static Field<int> emptyRoomTtl = GetClass().GetField("EmptyRoomTtl");
        emptyRoomTtl.SetInstance(this);
        return emptyRoomTtl.Get();
    }

    void SetEmptyRoomTtl(int value){
        static Field<int> emptyRoomTtl = GetClass().GetField("EmptyRoomTtl");
        emptyRoomTtl.SetInstance(this);
        emptyRoomTtl.Set(value);
    }


    Hashtable* GetCustomRoomProperties(){
        static Field<Hashtable*> customRoomProperties = GetClass().GetField("CustomRoomProperties");
        customRoomProperties.SetInstance(this);
        return customRoomProperties.Get();
    }

    void SetCustomRoomProperties(Hashtable* value){
        static Field<Hashtable*> customRoomProperties = GetClass().GetField("CustomRoomProperties");
        customRoomProperties.SetInstance(this);
        customRoomProperties.Set(value);
    }


    std::vector<String*> GetCustomRoomPropertiesForLobby() {
        static Field<Array<String*>*> customRoomPropertiesForLobby = GetClass().GetField("CustomRoomPropertiesForLobby");
        customRoomPropertiesForLobby.SetInstance(this);
        return customRoomPropertiesForLobby.Get()->ToVector();
    }

    void SetCustomRoomPropertiesForLobby(Array<String*>* properties) {
        static Field<Array<String*>*> customRoomPropertiesForLobby = GetClass().GetField("CustomRoomPropertiesForLobby");
        customRoomPropertiesForLobby.SetInstance(this);
        customRoomPropertiesForLobby.Set(properties);
    }


    std::vector<String*> GetPlugins() {
        static Field<Array<String*>*> plugins = GetClass().GetField("Plugins");
        plugins.SetInstance(this);
        return plugins.Get()->ToVector();
    }

    void SetPlugins(Array<String*>* properties) {
        static Field<Array<String*>*> plugins = GetClass().GetField("Plugins");
        plugins.SetInstance(this);
        plugins.Set(properties);
    }
};

struct RoomInfo : IL2CPP::Il2CppObject {
    static Class GetClass(){
        static Class cached = Class("Photon.Realtime", "RoomInfo");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }



};

struct Room : RoomInfo {
    static Class GetClass(){
        static Class cached = Class("Photon.Realtime", "Room");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }


    std::string GetName() {
        static Method<String*> getName = GetClass().GetMethod("get_Name", 0);
        return getName[this]()->str();
    }

    uint8_t GetPlayerCount() {
        static Method<uint8_t> getPlayerCount = GetClass().GetMethod("get_PlayerCount", 0);
        return getPlayerCount[this]();
    }

    void SetPlayerCount(uint8_t count) {
        static Method<void> setPlayerCount = GetClass().GetMethod("set_PlayerCount", 1);
        setPlayerCount[this](count);
    }

    uint8_t GetMaxPlayers() {
        static Method<uint8_t> getMaxPlayers = GetClass().GetMethod("get_MaxPlayers", 0);
        return getMaxPlayers[this]();
    }

    bool GetIsOpen() {
        static Method<bool> getIsOpen = GetClass().GetMethod("get_IsOpen", 0);
        return getIsOpen[this]();
    }

    void SetIsOpen(bool is) {
        static Method<void> setIsOpen = GetClass().GetMethod("set_IsOpen", 1);
        setIsOpen[this](is);
    }

    bool GetIsVisible() {
        static Method<bool> getIsVisible = GetClass().GetMethod("get_IsVisible", 0);
        return getIsVisible[this]();
    }

    void SetIsVisible(bool is) {
        static Method<void> setIsVisible = GetClass().GetMethod("set_IsVisible", 1);
        setIsVisible[this](is);
    }

    std::string ToStringFull() {
        static Method<String*> toStringFull = GetClass().GetMethod("ToStringFull", 0);
        return toStringFull[this]()->str();
    }
};

struct PhotonView : Behaviour {
    static Class GetClass(){
        static Class cached = Class("Photon.Pun", "PhotonView");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    bool GetIsMine() {
        static Method<bool> isMine = GetClass().GetMethod("get_IsMine", 0);
        return isMine[this]();
    }

    void SetIsMine(bool value) {
        static Method<void> isMine = GetClass().GetMethod("set_IsMine", 1);
        isMine[this](value);
    }

    int GetViewID() {
        static Method<int> getViewId = GetClass().GetMethod("get_ViewID", 0);
        return getViewId[this]();
    }

    void SetViewID(int value) {
        static Method<void> setViewId = GetClass().GetMethod("set_ViewID", 1);
        setViewId[this](value);
    }

    Player* GetOwner() {
        static Method<Player*> getOwner = GetClass().GetMethod("get_Owner", 0);
        return getOwner[this]();
    }

    void SetOwner(Player* player) {
        static Method<Player*> setOwner = GetClass().GetMethod("set_Owner", 1);
        setOwner[this](player);
    }

    static PhotonView* Get(GameObject* gameObj) {
        static Method<PhotonView*> getView = GetClass().GetMethod("Get", { "gameObj" });
        return getView(gameObj);
    }

    static PhotonView* Get(Component* component) {
        static Method<PhotonView*> getView = GetClass().GetMethod("Get", { "component" });
        return getView(component);
    }

    void RPC(std::string methodName, RpcTarget target, Array<IL2CPP::Il2CppObject*>* parameters) {
        static Method<void> rpc = GetClass().GetMethod("RPC", {"methodName", "target", "parameters"});
        rpc[this](CreateMonoString(methodName), target, parameters);
    }

    void RPC(std::string methodName, Player* targetPlayer, Array<IL2CPP::Il2CppObject*>* parameters) {
        static Method<void> rpc = GetClass().GetMethod("RPC", {"methodName", "targetPlayer", "parameters"});
        rpc[this](CreateMonoString(methodName), targetPlayer, parameters);
    }
};

struct RaiseEventOptions : IL2CPP::Il2CppObject {

    static Class GetClass(){
        static Class cached = Class("Photon.Realtime", "RaiseEventOptions");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    static RaiseEventOptions* GetDefault(){
        static Field<RaiseEventOptions*> defaultF = GetClass().GetField("Default");
        return defaultF.Get();
    }

};

enum DeliveryMode
{
    Unreliable,
    Reliable,
    UnreliableUnsequenced,
    ReliableUnsequenced
};


struct SendOptions
{
    int DeliveryMode;
    bool Encrypt;
    byte Channel;

    void SetReliability(bool reliability) {
        this->DeliveryMode = (reliability ? DeliveryMode::Reliable : DeliveryMode::Unreliable);
    }
    bool GetReliability() {
        return this->DeliveryMode == DeliveryMode::Reliable;
    }

    inline static SendOptions sendReliable() {
        SendOptions opts {};
        opts.SetReliability(true);
        return opts;
    }
    inline static SendOptions sendUnreliable() {
        SendOptions opts {};
        opts.SetReliability(false);
        return opts;
    }
};


struct ScriptableObject : Object {
    static Class GetClass(){
        static Class cached = Class("UnityEngine", "ScriptableObject");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }
};

struct AppSettings : IL2CPP::Il2CppObject {
    static Class GetClass(){
        static Class cached = Class("Photon.Realtime", "AppSettings");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    bool GetIsMasterServerAdress() {
        static Method<bool> getIsMasterServerAdressM = GetClass().GetMethod("get_IsMasterServerAddress", 0);
        return getIsMasterServerAdressM[this]();
    }

    bool GetIsBestRegion() {
        static Method<bool> getIsBestRegionM = GetClass().GetMethod("get_IsBestRegion", 0);
        return getIsBestRegionM[this]();
    }

    bool GetIsDefaultNameServer() {
        static Method<bool> getIsDefaultNameServerM = GetClass().GetMethod("get_IsDefaultNameServer", 0);
        return getIsDefaultNameServerM[this]();
    }

    bool GetIsDefaultPort() {
        static Method<bool> getIsDefaultPortM = GetClass().GetMethod("get_IsDefaultPort", 0);
        return getIsDefaultPortM[this]();
    }

    std::string ToStringFull(){
        static Method<String*> toStringFullM = GetClass().GetMethod("ToStringFull", 0);
        return toStringFullM[this]()->str();
    }

    static bool IsAppId(std::string val){
        static Method<bool> isAppIdM = GetClass().GetMethod("IsAppId", 1);
        return isAppIdM(CreateMonoString(val));
    }

    std::string HideAppId(std::string appId) {
        static Method<String*> hideAppIdM = GetClass().GetMethod("HideAppId", 1);
        return hideAppIdM(CreateMonoString(appId))->str();
    }


    std::string GetAppIdRealtime() {
        static Field<String*> appIdRealtimeF = GetClass().GetField("AppIdRealtime");
        return appIdRealtimeF[this].Get()->str();
    }

    void SetAppIdRealtime(std::string value) {
        static Field<String*> appIdRealtimeF = GetClass().GetField("AppIdRealtime");
        appIdRealtimeF[this].Set(CreateMonoString(value));
    }


    std::string GetAppIdFusion() {
        static Field<String*> appIdRealtimeF = GetClass().GetField("AppIdFusion");
        return appIdRealtimeF[this].Get()->str();
    }

    void SetAppIdFusion(std::string value) {
        static Field<String*> AppIdFusionF = GetClass().GetField("AppIdFusion");
        AppIdFusionF[this].Set(CreateMonoString(value));
    }

    std::string GetAppIdChat() {
        static Field<String*> AppIdChatF = GetClass().GetField("AppIdChat");
        return AppIdChatF[this].Get()->str();
    }

    void SetAppIdChat(std::string value) {
        static Field<String*> AppIdChatF = GetClass().GetField("AppIdChat");
        AppIdChatF[this].Set(CreateMonoString(value));
    }


    std::string GetAppIdVoice() {
        static Field<String*> AppIdVoiceF = GetClass().GetField("AppIdVoice");
        return AppIdVoiceF[this].Get()->str();
    }

    void SetAppIdVoice(std::string value) {
        static Field<String*> AppIdVoiceF = GetClass().GetField("AppIdVoice");
        AppIdVoiceF[this].Set(CreateMonoString(value));
    }


    std::string GetAppVersion() {
        static Field<String*> AppVersionF = GetClass().GetField("AppVersion");
        return AppVersionF[this].Get()->str();
    }

    void SetAppVersion(std::string value) {
        static Field<String*> AppVersionF = GetClass().GetField("AppVersion");
        AppVersionF[this].Set(CreateMonoString(value));
    }

    void SetUseNameServer(bool value) {
        static Field<bool> UseNameServerF = GetClass().GetField("UseNameServer");
        UseNameServerF.SetInstance( this);
        UseNameServerF.Set(value);
    }

    bool GetUseNameServer() {
        static Field<bool> UseNameServerF = GetClass().GetField("UseNameServer");
        UseNameServerF.SetInstance(this);
        return UseNameServerF.Get();
    }

    std::string GetFixedRegion() {
        static Field<String*> FixedRegionF = GetClass().GetField("FixedRegion");
        return FixedRegionF[this].Get()->str();
    }

    void SetFixedRegion(std::string value) {
        static Field<String*> FixedRegionF = GetClass().GetField("FixedRegion");
        FixedRegionF[this].Set(CreateMonoString(value));
    }



    std::string GetBestRegionSummaryFromStorage() {
        static Field<String*> BestRegionSummaryFromStorageF = GetClass().GetField("BestRegionSummaryFromStorage");
        return BestRegionSummaryFromStorageF[this].Get()->str();
    }

    void SetBestRegionSummaryFromStorage(std::string value) {
        static Field<String*> BestRegionSummaryFromStorageF = GetClass().GetField("BestRegionSummaryFromStorage");
        BestRegionSummaryFromStorageF[this].Set(CreateMonoString(value));
    }


    std::string GetServer() {
        static Field<String*> ServerF = GetClass().GetField("Server");
        return ServerF[this].Get()->str();
    }

    void SetServer(std::string value) {
        static Field<String*> ServerF = GetClass().GetField("Server");
        ServerF[this].Set(CreateMonoString(value));
    }


    int GetPort() {
        static Field<int> portF = GetClass().GetField("Port");
        portF.SetInstance(this);
        return portF.Get();
    }

    void SetServer(int value) {
        static Field<int> portF = GetClass().GetField("Port");
        portF.SetInstance(this);
        portF.Set(value);
    }


    std::string GetProxyServer() {
        static Field<String*> ProxyServerF = GetClass().GetField("ProxyServer");
        return ProxyServerF[this].Get()->str();
    }

    void SetProxyServer(std::string value) {
        static Field<String*> ProxyServerF = GetClass().GetField("ProxyServer");
        ProxyServerF[this].Set(CreateMonoString(value));
    }


    ConnectionProtocol* GetProtocol() {
        static Field<ConnectionProtocol*> ProtocolF = GetClass().GetField("Protocol");
        return ProtocolF[this].Get();
    }

    void SetProtocol(ConnectionProtocol* value) {
        static Field<ConnectionProtocol*> ProtocolF = GetClass().GetField("Protocol");
        ProtocolF[this].Set(value);
    }


    void SetEnableProtocolFallback(bool value) {
        static Field<bool> EnableProtocolFallbackF = GetClass().GetField("EnableProtocolFallback");
        EnableProtocolFallbackF.SetInstance( this);
        EnableProtocolFallbackF.Set(value);
    }

    bool GetEnableProtocolFallback() {
        static Field<bool> EnableProtocolFallbackF = GetClass().GetField("EnableProtocolFallback");
        EnableProtocolFallbackF.SetInstance(this);
        return EnableProtocolFallbackF.Get();
    }


    AuthModeOption* GetAuthMode() {
        static Field<AuthModeOption*> AuthModeF = GetClass().GetField("AuthMode");
        return AuthModeF[this].Get();
    }

    void SetAuthMode(AuthModeOption* value) {
        static Field<AuthModeOption*> AuthModeF = GetClass().GetField("AuthMode");
        AuthModeF[this].Set(value);
    }



    void SetEnableLobbyStatistics(bool value) {
        static Field<bool> EnableLobbyStatisticsF = GetClass().GetField("EnableLobbyStatistics");
        EnableLobbyStatisticsF.SetInstance( this);
        EnableLobbyStatisticsF.Set(value);
    }

    bool GetEnableLobbyStatistics() {
        static Field<bool> EnableLobbyStatisticsF = GetClass().GetField("EnableLobbyStatistics");
        EnableLobbyStatisticsF.SetInstance(this);
        return EnableLobbyStatisticsF.Get();
    }

    DebugLevel* GetNetworkLogging() {
        static Field<DebugLevel*> NetworkLoggingF = GetClass().GetField("NetworkLogging");
        return NetworkLoggingF[this].Get();
    }

    void SetNetworkLogging(DebugLevel* value) {
        static Field<DebugLevel*> NetworkLoggingF = GetClass().GetField("NetworkLogging");
        NetworkLoggingF[this].Set(value);
    }
};



struct ServerSettings : ScriptableObject {
    static Class GetClass(){
        static Class cached = Class("Photon.Pun", "ServerSettings");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    void UseCloud(std::string cloudAppId, std::string code) {
        static Method<void> useCloudM = GetClass().GetMethod("UseCloud", 2);
        return useCloudM[this](CreateMonoString(cloudAppId), CreateMonoString(code));
    }

    static bool IsAppId(std::string val) {
        static Method<bool> isAppIdM = GetClass().GetMethod("IsAppId", 1);
        return isAppIdM(CreateMonoString(val));
    }

    static std::string BestRegionSummaryInPreferences() {
        static Method<String*> bestRegionSummaryM = GetClass().GetMethod("get_BestRegionSummaryInPreferences", 0);
        return bestRegionSummaryM()->str();
    }

    static void ResetBestRegionCodeInPreferences() {
        static Method<void> bestRegionSummaryM = GetClass().GetMethod("ResetBestRegionCodeInPreferences", 0);
        return bestRegionSummaryM();
    }

    AppSettings* GetAppSettings(){
        static Field<AppSettings*> appSettingsF = GetClass().GetField("AppSettings");
        return appSettingsF[this].Get();
    }

    void SetAppSettings(AppSettings* settings){
        static Field<AppSettings*> appSettingsF = GetClass().GetField("AppSettings");
        return appSettingsF[this].Set(settings);
    }

    void SetDevRegion(std::string devRegion) {
        static Field<String*> devRegionF = GetClass().GetField("DevRegion");
        devRegionF[this].Set(CreateMonoString(devRegion));
    }

    std::string GetDevRegion() {
        static Field<String*> devRegionF = GetClass().GetField("DevRegion");
        return devRegionF[this].Get()->str();
    }

    PunLogLevel* GetPunLogLevel() {
        static Field<PunLogLevel*> PunLogLevelF = GetClass().GetField("PunLogging");
        return PunLogLevelF[this].Get();
    }

    void SetPunLogLevel(PunLogLevel* level) {
        static Field<PunLogLevel*> PunLogLevelF = GetClass().GetField("PunLogging");
        PunLogLevelF[this].Set(level);
    }

    bool GetEnableSupportLogger() {
        static Field<bool> enableSupportLoggerF = GetClass().GetField("EnableSupportLogger");
        enableSupportLoggerF.SetInstance(this);
        return enableSupportLoggerF.Get();
    }

    void SetEnableSupportLogger(bool value) {
        static Field<bool> enableSupportLoggerF = GetClass().GetField("EnableSupportLogger");
        enableSupportLoggerF.SetInstance(this);
        enableSupportLoggerF.Set(value);
    }

    bool GetRunInBackground() {
        static Field<bool> runInBackgroundF = GetClass().GetField("RunInBackground");
        runInBackgroundF.SetInstance(this);
        return runInBackgroundF.Get();
    }

    void SetRunInBackground(bool value) {
        static Field<bool> runInBackgroundF = GetClass().GetField("RunInBackground");
        runInBackgroundF.SetInstance(this);
        runInBackgroundF.Set(value);
    }

    void SetStartInOfflineMode(bool value) {
        static Field<bool> startInOfflineModeF = GetClass().GetField("StartInOfflineMode");
        startInOfflineModeF.SetInstance(this);
        startInOfflineModeF.Set(value);
    }

    bool GetStartInOfflineMode() {
        static Field<bool> startInOfflineModeF = GetClass().GetField("StartInOfflineMode");
        startInOfflineModeF.SetInstance(this);
        return startInOfflineModeF.Get();
    }

    std::vector<String*> GetRPCList() {
        static Field<List<String*>*> rpcListF = GetClass().GetField("RpcList");
        rpcListF.SetInstance(this);
        return rpcListF.Get()->ToVector();
    }
};

struct Hashtable : Structures::Mono::Dictionary<BNM::IL2CPP::Il2CppObject*, BNM::IL2CPP::Il2CppObject*> {
    static Class GetClass(){
        static Class cached = Class("ExitGames.Client.Photon", "Hashtable");
        return cached;
    }

    void Add(uint8_t k, BNM::IL2CPP::Il2CppObject* v) {
        static Method<void> Add = GetClass().GetMethod("Add");
        return Add[this](k,v);
    }

    void Set(IL2CPP::Il2CppObject* key, IL2CPP::Il2CppObject* value) {
        static Method<void> setItem = GetClass().GetMethod("set_Item", {"key", "value"});
        setItem[this](key, value);
    }

    IL2CPP::Il2CppObject* Get(IL2CPP::Il2CppObject* key) {
        static Method<IL2CPP::Il2CppObject*> getItem = GetClass().GetMethod("get_Item", {"key", "value"});
        return getItem[this](key);
    }

    bool ContainsKey(uint8_t key){
        static Method<bool> ContainsKey = GetClass().GetMethod("ContainsKey");
        return ContainsKey[this](key);
    }

    void Remove(uint8_t k){
        static Method<void> Remove = GetClass().GetMethod("Remove");
        return Remove[this](k);
    }

    std::string ToString(){
        static Method<String*> toString = GetClass().GetMethod("ToString");
        return toString[this]()->str();
    }

};

struct NetPlayer : IL2CPP::Il2CppObject {
    static Class GetClass(){
        static Class cached = Class("", "NetPlayer");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }
};

struct Player : IL2CPP::Il2CppObject {
    static Class GetClass(){
        static Class cached = Class("Photon.Realtime", "Player", Image("PhotonRealtime.dll"));
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    int GetActorNumber() {
        static Method<int> actorNumberM = GetClass().GetMethod("get_ActorNumber", 0);
        return actorNumberM[this]();
    }

    bool GetHasRejoined() {
        static Method<bool> hasRejoinedM = GetClass().GetMethod("get_HasRejoined", 0);
        return hasRejoinedM[this]();
    }

    void SetHasRejoined(bool value) {
        static Method<void> hasRejoinedM = GetClass().GetMethod("set_HasRejoined", 1);
        hasRejoinedM[this](value);
    }

    std::string GetNickname() {
        static Method<String*> getNicknameM = GetClass().GetMethod("get_NickName", 0);
        return getNicknameM[this]()->str();
    }

    void SetNickname(std::string nickname) {
        static Method<void> setNicknameM = GetClass().GetMethod("set_NickName", 1);
        setNicknameM[this](CreateMonoString(nickname));
    }

    std::string GetDefaultName() {
        static Field<String*> defaultName = GetClass().GetField("defaultName");
        return defaultName[this].Get()->str();
    }

    void SetDefaultName(std::string name) {
        static Field<String*> defaultName = GetClass().GetField("defaultName");
        defaultName[this].Set(CreateMonoString(name));
    }

    std::string GetUserId(){
        static Method<String*> getUserIdM = GetClass().GetMethod("get_UserId", 0);
        return getUserIdM[this]()->str();
    }

    void SetUserId(std::string userId){
        static Method<void> setUserIdM = GetClass().GetMethod("set_UserId", 1);
        setUserIdM[this](CreateMonoString(userId));
    }

    bool GetIsMasterClient() {
        static Method<bool> getIsMasterM = GetClass().GetMethod("get_IsMasterClient", 0);
        return getIsMasterM[this]();
    }

    bool GetIsInactive() {
        static Method<bool> getIsInactiveM = GetClass().GetMethod("get_IsInactive", 0);
        return getIsInactiveM[this]();
    }

    void SetIsInactive(bool is) {
        static Method<void> setIsInactiveM = GetClass().GetMethod("set_IsInactive", 1);
        setIsInactiveM[this](is);
    }

    Player* Get(int id) {
        static Method<Player*> getM = GetClass().GetMethod("Get", 1);
        return getM[this](id);
    }

    Player* GetNext() {
        static Method<Player*> getM = GetClass().GetMethod("GetNext", 0);
        return getM[this]();
    }

    Player* GetNextFor(Player* currentPlayer) {
        static Method<Player*> getM = GetClass().GetMethod("GetNextFor", 1);
        return getM[this](currentPlayer);
    }

    Player* GetNextFor(int currentPlayerId) {
        static Method<Player*> getM = GetClass().GetMethod("GetNextFor", 1);
        return getM[this](currentPlayerId);
    }

    bool SetCustomProperties(Hashtable* propertiesToSet) {
        static Method<bool> setCustomPropertiesM = GetClass().GetMethod("SetCustomProperties", 3);
        return setCustomPropertiesM[this](propertiesToSet, nullptr, nullptr);
    }
};

struct AuthenticationValues : IL2CPP::Il2CppObject {
    static Class GetClass(){
        static Class cached = Class("Photon.Realtime", "AuthenticationValues");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    CustomAuthenticationType GetAuthType() {
        static Method<CustomAuthenticationType> getAuthType = GetClass().GetMethod("get_AuthType", 0);
        return getAuthType[this]();
    }

    void SetAuthType(CustomAuthenticationType type) {
        static Method<void> setAuthType = GetClass().GetMethod("set_AuthType", 1);
        return setAuthType[this](type);
    }

    std::string GetAuthGetParameters() {
        static Method<String*> getAuthGetParameters = GetClass().GetMethod("get_AuthGetParameters", 0);
        return getAuthGetParameters[this]()->str();
    }

    void SetAuthGetParameters(std::string parameters) {
        static Method<void> setAuthGetParameters = GetClass().GetMethod("set_AuthGetParameters", 1);
        setAuthGetParameters[this](CreateMonoString(parameters));
    }

    IL2CPP::Il2CppObject* GetAuthPostData() {
        static Method<IL2CPP::Il2CppObject*> getAuthPostData = GetClass().GetMethod("get_AuthPostData", 0);
        return getAuthPostData[this]();
    }

    IL2CPP::Il2CppObject* GetToken() {
        static Method<IL2CPP::Il2CppObject*> getToken = GetClass().GetMethod("get_Token", 0);
        return getToken[this]();
    }

    std::string GetUserId() {
        static Method<String*> getUserId = GetClass().GetMethod("get_UserId", 0);
        return getUserId[this]()->str();
    }

    void SetUserId(std::string userid){
        static Method<void> setUserId = GetClass().GetMethod("set_UserId", 1);
        setUserId[this](CreateMonoString(userid));
    }

    void SetAuthPostData(std::string stringData) {
        static Method<void> setAuthPostData = GetClass().GetMethod("SetAuthPostData", {"stringData"});
        setAuthPostData[this](CreateMonoString(stringData));
    }

    void SetAuthPostData(Array<uint8_t>* byteData) {
        static Method<void> setAuthPostData = GetClass().GetMethod("SetAuthPostData", {"byteData"});
        setAuthPostData[this](byteData);
    }

    void SetAuthPostData(Dictionary<String*, IL2CPP::Il2CppObject*>* dictData) {
        static Method<void> setAuthPostData = GetClass().GetMethod("SetAuthPostData", {"dictData"});
        setAuthPostData[this](dictData);
    }

    void AddAuthParameter(std::string key, std::string value) {
        static Method<void> addAuthParameter = GetClass().GetMethod("AddAuthParameter", 2);
        addAuthParameter[this](CreateMonoString(key), CreateMonoString(value));
    }

    std::string ToString() {
        static Method<String*> toString = GetClass().GetMethod("ToString", 0);
        return toString[this]()->str();
    }

};

struct TypedLobby : IL2CPP::Il2CppObject {
    static Class GetClass(){
        static Class cached = Class("Photon.Realtime", "TypedLobby");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    std::string GetName() {
        static Field<String*> nameF = GetClass().GetField("Name");
        return nameF[this].Get()->str();
    }

    void SetName(std::string name) {
        static Field<String*> nameF = GetClass().GetField("Name");
        nameF[this].Set(CreateMonoString(name));
    }

    LobbyType GetLobbyType() {
        static Field<LobbyType> lType = GetClass().GetField("Type");
        lType.SetInstance(this);
        return lType.Get(); // i dont know why but when i do [this] it errors out :cry:
    }

    void SetLobbyType(LobbyType type) {
        static Field<LobbyType> lType = GetClass().GetField("Type");
        lType.SetInstance(this);
        lType.Set(type); // i dont know why but when i do [this] it errors out :cry:
    }

    static TypedLobby* GetDefault() {
        static Field<TypedLobby*> def = GetClass().GetField("Default");
        return def.Get();
    }

    std::string ToString() {
        static Method<String*> toString = GetClass().GetMethod("ToString", 0);
        return toString[this]()->str();
    }
};

struct PhotonNetwork : IL2CPP::Il2CppObject {
    static Class GetClass(){
        static Class cached = Class("Photon.Pun", "PhotonNetwork");
        return cached;
    }

    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    static ServerSettings* GetPhotonServerSettings() {
        static Method<ServerSettings*> settings = GetClass().GetMethod("get_PhotonServerSettings", 0);
        return settings();
    }

    static bool ConnectUsingSettings() {
        static Method<bool> connect = GetClass().GetMethod("ConnectUsingSettings", 0);
        return connect();
    }

    static bool ConnectUsingSettings(AppSettings* appSettings, bool startInOfflineMode = false) {
        static Method<bool> connect = GetClass().GetMethod("ConnectUsingSettings", {"appSettings", "startInOfflineMode"});
        return connect();
    }

    static bool GetInRoom() {
        static Method<bool> inRoom = GetClass().GetMethod("get_InRoom", 0);
        return inRoom();
    }

    static bool ConnectToRegion(std::string region) {
        static Method<bool> Cregion = GetClass().GetMethod("ConnectToRegion", {"region"});
        return Cregion(CreateMonoString(region));
    }

    static void Disconnect() {
        static Method<void> disconnect = GetClass().GetMethod("Disconnect", 0);
        disconnect();
    }

    static void Reconnect() {
        static Method<void> reconnect = GetClass().GetMethod("Reconnect", 0);
        reconnect();
    }

    static int GetPing() {
        static Method<int> getPing = GetClass().GetMethod("GetPing", 0);
        return getPing();
    }

    static int GetCountOfPlayersOnMaster() {
        static Method<int> countOf = GetClass().GetMethod("get_CountOfPlayersOnMaster", 0);
        return countOf();
    }

    static int GetCountOfPlayersInRooms() {
        static Method<int> countOf = GetClass().GetMethod("get_CountOfPlayersInRooms", 0);
        return countOf();
    }

    static int GetCountOfPlayers() {
        static Method<int> countOf = GetClass().GetMethod("get_CountOfPlayers", 0);
        return countOf();
    }

    static int GetCountOfRooms() {
        static Method<int> countOf = GetClass().GetMethod("get_CountOfRooms", 0);
        return countOf();
    }

    static void OpRemoveCompleteCache() {
        static Method<void> remove = GetClass().GetMethod("OpRemoveCompleteCache", 0);
        remove();
    }

    static void OpRemoveCompleteCacheOfPlayer(int actorNumber) {
        static Method<void> remove = GetClass().GetMethod("OpRemoveCompleteCacheOfPlayer", 1);
        remove(actorNumber);
    }

    static void OpCleanActorRpcBuffer(int actorNumber) {
        static Method<void> clean = GetClass().GetMethod("OpCleanActorRpcBuffer", 1);
        clean(actorNumber);
    }


    static void SetSerializationRate(int rate) {
        static Method<void> serializationRate = GetClass().GetMethod("set_SerializationRate", 1);
        serializationRate(rate);
    }

    static int GetSerializationRate() {
        static Method<int> serializationRate = GetClass().GetMethod("get_SerializationRate", 0);
        return serializationRate();
    }

    static void SetSendRate(int rate) {
        static Method<void> serializationRate = GetClass().GetMethod("set_SendRate", 1);
        serializationRate(rate);
    }

    static int GetSendRate() {
        static Method<int> serializationRate = GetClass().GetMethod("get_SendRate", 0);
        return serializationRate();
    }

    static int GetServerTimestamp() {
        static Method<int> serverTimestamp = GetClass().GetMethod("get_ServerTimestamp", 0);
        return serverTimestamp();
    }

    static bool GetInLobby() {
        static Method<bool> inLobby = GetClass().GetMethod("get_InLobby", 0);
        return inLobby();
    }

    static AuthenticationValues* GetAuthValues(){
        static Method<AuthenticationValues*> getAuthValues = GetClass().GetMethod("get_AuthValues", 0);
        return getAuthValues();
    }

    static void SetAuthValues(AuthenticationValues* value){
        static Method<void> setAuthValues = GetClass().GetMethod("set_AuthValues", 1);
        setAuthValues(value);
    }

    static std::string GetGameVersion() {
        static Method<String*> getGameVersionM = GetClass().GetMethod("get_GameVersion", 0);
        return getGameVersionM()->str();
    }

    static void SetGameVersion(std::string gameVersion) {
        static Method<void> setGameVersionM = GetClass().GetMethod("set_GameVersion", 1);
        return setGameVersionM(CreateMonoString(gameVersion));
    }

    static std::string GetAppVersion() {
        static Method<String*> getAppVersionM = GetClass().GetMethod("get_AppVersion", 0);
        return getAppVersionM()->str();
    }

    static bool SetMasterClient(Player* player){
        static Method<bool> setMasterClientM = GetClass().GetMethod("SetMasterClient", 1);
        return setMasterClientM(player);
    }

    static bool CloseConnection(Player* player) {
        static Method<bool> closeConnectionM = GetClass().GetMethod("CloseConnection", 1);
        return closeConnectionM(player);
    }

    static bool JoinRandomRoom(){
        static Method<bool> joinRandomRoomM = GetClass().GetMethod("JoinRandomRoom", 0);
        return joinRandomRoomM();
    }

    static bool JoinRandomRoom(Hashtable* expectedCustomRoomProperties, uint8_t expectedMaxPlayers) {
        static Method<bool> joinRandomRoomM = GetClass().GetMethod("JoinRandomRoom", {"expectedCustomRoomProperties", "expectedMaxPlayers"});
        return joinRandomRoomM(expectedCustomRoomProperties, expectedMaxPlayers);
    }

    static bool JoinRandomRoom(Hashtable* expectedCustomRoomProperties, uint8_t expectedMaxPlayers, MatchmakingMode matchingType, TypedLobby* typedLobby, std::string sqlLobbyFilter, Array<String*>* expectedUsers = nullptr) {
        static Method<bool> joinRandomRoomM = GetClass().GetMethod("JoinRandomRoom", {"expectedCustomRoomProperties", "expectedMaxPlayers", "matchingType", "typedLobby", "sqlLobbyFilter", "expectedUsers"});
        return joinRandomRoomM(expectedCustomRoomProperties, expectedMaxPlayers, matchingType, typedLobby,
                               CreateMonoString(sqlLobbyFilter), expectedUsers);
    }

    static bool JoinRandomOrCreateRoom(Hashtable* expectedCustomRoomProperties = nullptr, uint8_t expectedMaxPlayers = 0, MatchmakingMode matchingType = MatchmakingMode::FillRoom, TypedLobby* typedLobby = nullptr, std::string sqlLobbyFilter = nullptr, std::string roomName = nullptr, RoomOptions* roomOptions = nullptr, Array<String*>* expectedUsers = nullptr) {
        static Method<bool> joinRandomRoomM = GetClass().GetMethod("JoinRandomOrCreateRoom", {"expectedCustomRoomProperties", "expectedMaxPlayers", "matchingType", "typedLobby", "sqlLobbyFilter", "roomName", "roomOptions", "expectedUsers"});
        return joinRandomRoomM(expectedCustomRoomProperties, expectedMaxPlayers, matchingType, typedLobby,
                               CreateMonoString(sqlLobbyFilter), CreateMonoString(roomName),
                               expectedUsers);
    }

    static bool CreateRoom(std::string roomName, RoomOptions* roomOptions = nullptr, TypedLobby* typedLobby = nullptr, Array<String*>* expectedUsers = nullptr) {
        static Method<bool> createRoomM = GetClass().GetMethod("CreateRoom", {"roomName", "roomOptions", "typedLobby", "expectedUsers"});
        return createRoomM(CreateMonoString(roomName), roomOptions, typedLobby, expectedUsers);
    }

    static bool JoinOrCreateRoom(std::string roomName, RoomOptions* roomOptions, TypedLobby* typedLobby, Array<String*>* expectedUsers = nullptr) {
        static Method<bool> joinOrCreateRoomM = GetClass().GetMethod("JoinOrCreateRoom", {"roomName", "roomOptions", "typedLobby", "expectedUsers"});
        return joinOrCreateRoomM(CreateMonoString(roomName), roomOptions, typedLobby, expectedUsers);
    }

    static bool JoinRoom(std::string roomName, Array<String*>* expectedUsers = nullptr) {
        static Method<bool> joinRoomM = GetClass().GetMethod("JoinRoom", {"roomName", "expectedUsers"});
        return joinRoomM(CreateMonoString(roomName), expectedUsers);
    }

    static bool RejoinRoom(std::string roomName) {
        static Method<bool> RejoinRoomM = GetClass().GetMethod("RejoinRoom", {"roomName"});
        return RejoinRoomM(CreateMonoString(roomName));
    }

    static bool JoinLobby() {
        static Method<bool> joinLobby = GetClass().GetMethod("JoinLobby", 0);
        return joinLobby();
    }

    static bool JoinLobby(TypedLobby* typedLobby) {
        static Method<bool> joinLobby = GetClass().GetMethod("JoinLobby", {"typedLobby"});
        return joinLobby(typedLobby);
    }

    static bool LeaveLobby() {
        static Method<bool> leaveLobby = GetClass().GetMethod("LeaveLobby", 0);
        return leaveLobby();
    }

    static bool ReconnectAndRejoin() {
        static Method<bool> reconnectAndRejoin = GetClass().GetMethod("ReconnectAndRejoin", 0);
        return reconnectAndRejoin();
    }

    static bool LeaveRoom(bool becomeInactive = true){
        static Method<bool> leaveRoom = GetClass().GetMethod("LeaveRoom", {"becomeInactive"});
        return leaveRoom(becomeInactive);
    }

    static bool FindFriends(Structures::Mono::List<String*> friends){
        static Method<bool> findFriendsM = GetClass().GetMethod("FindFriends", 1);
        return findFriendsM(friends);
    }

    static void LoadLevel(int levelNumber) {
        static Method<void> loadLevelM = GetClass().GetMethod("LoadLevel", 1);
        loadLevelM(levelNumber);
    }

    static void LoadLevel(std::string levelName) {
        static Method<void> loadLevelM = GetClass().GetMethod("LoadLevel", 1);
        loadLevelM(CreateMonoString(levelName));
    }

    static Player* GetLocalPlayer(){
        static Method<Player*> getLocalPlayerM = GetClass().GetMethod("get_LocalPlayer", 0);
        return getLocalPlayerM();
    }

    static std::vector<Player*> GetPlayerList(){
        static Method<Array<Player*>*> getPlayerListM = GetClass().GetMethod("get_PlayerList", 0);
        return getPlayerListM()->ToVector();
    }

    static std::vector<Player*> GetPlayerListOthers(){
        static Method<Array<Player*>*> getPlayerListOthersM = GetClass().GetMethod("get_PlayerListOthers", 0);
        return getPlayerListOthersM()->ToVector();
    }

    static void DestroyAll(){
        static Method<void> destroyAll = GetClass().GetMethod("DestroyAll", 0);
        destroyAll();
    }

    static void DestroyPlayerObjects(Player* player){
        static Method<void> destroyPlayerObjects = GetClass().GetMethod("DestroyPlayerObjects", {"targetPlayer"});
        destroyPlayerObjects(player);
    }

    static void DestroyPlayerObjects(int targetPlayerId) {
        static Method<void> destroyPlayerObjects = GetClass().GetMethod("DestroyPlayerObjects", {"targetPlayerId"});
        destroyPlayerObjects(targetPlayerId);
    }

    static void RemoveRPCs(Player* targetPlayer) {
        static Method<void> removeRpcs = GetClass().GetMethod("RemoveRPCs", {"targetPlayer"});
        removeRpcs(targetPlayer);
    }

    static void RemoveRPCs(PhotonView* targetPhotonView) {
        static Method<void> removeRpcs = GetClass().GetMethod("RemoveRPCs", {"targetPhotonView"});
        removeRpcs(targetPhotonView);
    }

    static bool RaiseEvent(uint8_t eventCode, IL2CPP::Il2CppObject* eventContent, RaiseEventOptions* raiseEventOptions, SendOptions sendOptions){
        static Method<bool> raiseEvent = GetClass().GetMethod("RaiseEvent", {"eventCode", "eventContent", "raiseEventOptions", "sendOptions"});
        return raiseEvent(eventCode, eventContent, raiseEventOptions, sendOptions);
    }

    static std::string GetPunVersion() {
        static Field<String*> punVersion = GetClass().GetField("PunVersion");
        return punVersion.Get()->str();
    }

    static bool GetCustomRoomList(TypedLobby* typedLobby, std::string sqlLobbyFilter) {
        static Method<bool> getCustomRoomList = GetClass().GetMethod("GetCustomRoomList", {"typedLobby", "sqlLobbyFilter"});
        return getCustomRoomList(typedLobby, CreateMonoString(sqlLobbyFilter));
    }

    static Room* GetCurrentRoom() {
        static Method<Room*> getCurrentRoom = GetClass().GetMethod("get_CurrentRoom", 0);
        return getCurrentRoom();
    }

    static Player* GetMasterClient() {
        static Method<Player*> getM = GetClass().GetMethod("get_MasterClient", 0);
        return getM();
    }

    static GameObject* Instantiate(std::string prefabName, Vector3 position, Quaternion rotation, uint8_t group = 0, Array<IL2CPP::Il2CppObject*>* data = nullptr){
        static Method<GameObject*> instantiate = GetClass().GetMethod("Instantiate");
        return instantiate(CreateMonoString(prefabName), position, rotation, group, data);
    }

    static GameObject* InstantiateRoomObject(std::string prefabName, Vector3 position, Quaternion rotation, uint8_t group = 0, Array<IL2CPP::Il2CppObject*>* data = nullptr){
        static Method<GameObject*> instantiate = GetClass().GetMethod("InstantiateRoomObject");
        return instantiate(CreateMonoString(prefabName), position, rotation, group, data);
    }

    static GameObject* InstantiateSceneObject(std::string prefabName, Vector3 position, Quaternion rotation, uint8_t group = 0, Array<IL2CPP::Il2CppObject*>* data = nullptr){
        static Method<GameObject*> instantiate = GetClass().GetMethod("InstantiateSceneObject");
        return instantiate(CreateMonoString(prefabName), position, rotation, group, data);
    }

    static void Destroy(GameObject* targetGo) {
        static Method<void> destroy = GetClass().GetMethod("Destroy", {"targetGo"});
        destroy(targetGo);
    }

    static void Destroy(PhotonView* targetView) {
        static Method<void> destroy = GetClass().GetMethod("Destroy", {"targetView"});
        destroy(targetView);
    }
};
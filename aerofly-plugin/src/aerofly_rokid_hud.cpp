// Read-only bridge; network heartbeat is independent of simulation time.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include "tm_external_message.h"
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

namespace {
constexpr char hello[] = "AEROFLY_ROKID_HELLO_V1";
constexpr unsigned short hudPort = 49002, discoveryPort = 49003;
struct Field { tm_external_message message; const char* key; double scale; bool fraction; };
Field fields[] = {
#define VALUE(name, key, unit, scale, fraction) {tm_external_message(name, tm_msg_data_type::Double, tm_msg_flag::Value, tm_msg_access::Read, unit), key, scale, fraction}
  VALUE("Aircraft.IndicatedAirspeed", "ias", tm_msg_unit::MeterPerSecond, 1.9438444924406, false),
  VALUE("Aircraft.Altitude", "alt", tm_msg_unit::Meter, 3.2808398950131, false),
  VALUE("Aircraft.Pitch", "pitch", tm_msg_unit::Radiant, 57.29577951308232, false),
  VALUE("Aircraft.Bank", "bank", tm_msg_unit::Radiant, 57.29577951308232, false),
  VALUE("Aircraft.MagneticHeading", "hdg", tm_msg_unit::Radiant, 57.29577951308232, false),
  VALUE("Aircraft.Throttle", "thr", tm_msg_unit::None, 1, true),
  VALUE("Aircraft.Flaps", "flaps", tm_msg_unit::None, 1, true),
  VALUE("Aircraft.AirBrake", "spoilers", tm_msg_unit::None, 1, true),
  VALUE("Aircraft.VerticalSpeed", "vs", tm_msg_unit::MeterPerSecond, 196.85039370079, false),
  VALUE("Aircraft.Gear", "gear", tm_msg_unit::None, 1, true),
#undef VALUE
#define WARNING(name, key) {tm_external_message(name, tm_msg_data_type::Double, tm_msg_flag::Event, tm_msg_access::Read, tm_msg_unit::None), key, 1, true}
  WARNING("Warnings.MasterWarning", "master"), WARNING("Warnings.MasterCaution", "caution"),
  WARNING("Warnings.EngineFire", "fire"), WARNING("Warnings.LowOilPressure", "oil"),
  WARNING("Warnings.LowFuelPressure", "fuel"), WARNING("Warnings.LowHydraulicPressure", "hyd"),
  WARNING("Warnings.AltitudeAlert", "altAlert")
#undef WARNING
};
constexpr size_t fieldCount = sizeof(fields) / sizeof(fields[0]);
struct Snapshot {
  std::array<double, fieldCount> values{};
  std::array<ULONGLONG, fieldCount> updated{};
};
Snapshot snapshot;
std::mutex dataMutex;
std::atomic<bool> running{false};
std::thread worker;
SOCKET udp = INVALID_SOCKET;
bool wsaStarted = false;
sockaddr_in broadcast{}, peer{};
ULONGLONG lastHello = 0, session = 0, sequence = 0;

void refreshBroadcast() {
  broadcast = {};
  broadcast.sin_family = AF_INET;
  broadcast.sin_port = htons(hudPort);
  broadcast.sin_addr.s_addr = INADDR_BROADCAST;
  sockaddr_in probe{};
  probe.sin_family = AF_INET;
  InetPtonA(AF_INET, "8.8.8.8", &probe.sin_addr);
  DWORD index = 0;
  if (GetBestInterfaceEx(reinterpret_cast<sockaddr*>(&probe), &index) != NO_ERROR) return;
  ULONG size = 0;
  if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, nullptr, &size) != ERROR_BUFFER_OVERFLOW) return;
  std::vector<unsigned char> buffer(size);
  auto* adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
  if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapters, &size) != NO_ERROR) return;
  for (auto* a = adapters; a; a = a->Next) {
    if (a->IfIndex != index || a->OperStatus != IfOperStatusUp) continue;
    for (auto* u = a->FirstUnicastAddress; u; u = u->Next) {
      if (!u->Address.lpSockaddr || u->Address.lpSockaddr->sa_family != AF_INET) continue;
      const ULONG prefix = u->OnLinkPrefixLength;
      if (prefix == 0 || prefix > 30) continue;
      auto* local = reinterpret_cast<sockaddr_in*>(u->Address.lpSockaddr);
      broadcast.sin_addr.s_addr = htonl(ntohl(local->sin_addr.s_addr) | ~(0xFFFFFFFFu << (32 - prefix)));
      return;
    }
  }
}
bool openSocket() {
  udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (udp == INVALID_SOCKET) return false;
  BOOL enabled = TRUE;
  u_long nonBlocking = 1;
  sockaddr_in local{};
  local.sin_family = AF_INET;
  local.sin_port = htons(discoveryPort);
  if (setsockopt(udp, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&enabled), sizeof(enabled)) ||
      ioctlsocket(udp, FIONBIO, &nonBlocking) ||
      bind(udp, reinterpret_cast<sockaddr*>(&local), sizeof(local))) {
    closesocket(udp); udp = INVALID_SOCKET;
    return false;
  }
  return true;
}
void registrations() {
  for (int i = 0; i < 32; ++i) {
    char buffer[128];
    sockaddr_in sender{};
    int size = sizeof(sender);
    int n = recvfrom(udp, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&sender), &size);
    if (n == SOCKET_ERROR) {
      if (WSAGetLastError() == WSAEMSGSIZE) continue;
      break;
    }
    if (n == sizeof(hello) - 1 && std::memcmp(buffer, hello, n) == 0 && sender.sin_port == htons(hudPort)) {
      peer = sender; lastHello = GetTickCount64();
    }
  }
}
void networkLoop() {
  ULONGLONG lastRefresh = 0, lastBeacon = 0, lastOpen = 0;
  while (running.load()) {
    auto now = GetTickCount64();
    if (!lastRefresh || now - lastRefresh >= 3000) {
      refreshBroadcast(); lastRefresh = now;
    }
    if (udp == INVALID_SOCKET && (!lastOpen || now - lastOpen >= 1000)) {
      openSocket(); lastOpen = now;
    }
    if (udp != INVALID_SOCKET) {
      registrations();
      Snapshot data;
      { std::lock_guard<std::mutex> lock(dataMutex); data = snapshot; }
      bool flight = true;
      ULONGLONG age = 0;
      for (size_t i = 0; i < 4; ++i) {
        if (!data.updated[i]) { flight = false; age = 999999; }
        else {
          auto elapsed = now >= data.updated[i] ? now - data.updated[i] : 0;
          if (elapsed > age) age = elapsed;
          if (elapsed > 2500) flight = false;
        }
      }
      bool unicast = lastHello && now >= lastHello && now - lastHello < 6000;
      char json[1200]{};
      int length = _snprintf_s(json, sizeof(json), _TRUNCATE,
        "{\"v\":2,\"session\":%llu,\"seq\":%llu,\"flight\":%s,\"flightAge\":%llu,\"mode\":\"%s\"",
        session, ++sequence, flight ? "true" : "false", age, unicast ? "unicast" : "broadcast");
      for (size_t i = 0; i < fieldCount && length > 0; ++i) {
        bool valid = data.updated[i] && (i >= 10 || now < data.updated[i] || now - data.updated[i] <= 2500);
        double value = data.values[i] * fields[i].scale;
        if (fields[i].fraction) value = (std::max)(0.0, (std::min)(1.0, value));
        if (i == 4) value = std::fmod(value + 360.0, 360.0);
        int added = valid
          ? _snprintf_s(json + length, sizeof(json) - length, _TRUNCATE, ",\"%s\":%.3f", fields[i].key, value)
          : _snprintf_s(json + length, sizeof(json) - length, _TRUNCATE, ",\"%s\":null", fields[i].key);
        if (added < 0) { length = -1; break; }
        length += added;
      }
      if (length > 0 && length + 1 < sizeof(json)) {
        json[length++] = '}';
        bool failed = false;
        if (unicast)
          failed = sendto(udp, json, length, 0, reinterpret_cast<sockaddr*>(&peer), sizeof(peer)) == SOCKET_ERROR;
        if (!unicast || now - lastBeacon >= 2000) {
          sendto(udp, json, length, 0, reinterpret_cast<sockaddr*>(&broadcast), sizeof(broadcast));
          lastBeacon = now;
        }
        if (failed) { closesocket(udp); udp = INVALID_SOCKET; }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}
void shutdown() {
  running.store(false);
  if (worker.joinable()) worker.join();
  if (udp != INVALID_SOCKET) { closesocket(udp); udp = INVALID_SOCKET; }
  if (wsaStarted) { WSACleanup(); wsaStarted = false; }
}
}
extern "C" {
__declspec(dllexport) int Aerofly_FS_4_External_DLL_GetInterfaceVersion() { return TM_DLL_INTERFACE_VERSION; }
__declspec(dllexport) bool Aerofly_FS_4_External_DLL_Init(const HINSTANCE) {
  if (running.load()) return true;
  WSADATA info{};
  if (WSAStartup(MAKEWORD(2, 2), &info)) return false;
  wsaStarted = true;
  { std::lock_guard<std::mutex> lock(dataMutex); snapshot = {}; }
  lastHello = 0; sequence = 0;
  session = (GetTickCount64() << 16) ^ GetCurrentProcessId();
  if (!openSocket()) { shutdown(); return false; }
  running.store(true);
  try { worker = std::thread(networkLoop); }
  catch (...) { shutdown(); return false; }
  return true;
}
__declspec(dllexport) void Aerofly_FS_4_External_DLL_Shutdown() { shutdown(); }
__declspec(dllexport) void Aerofly_FS_4_External_DLL_Update(
  const tm_double, const tm_uint8* const bytes, const tm_uint32 size,
  const tm_uint32 count, tm_uint8*, tm_uint32& outputSize, tm_uint32& outputCount, const tm_uint32) {
  outputSize = 0; outputCount = 0;
  if (!bytes || !running.load()) return;
  std::lock_guard<std::mutex> lock(dataMutex);
  const auto now = GetTickCount64();
  // A flight reload must not inherit old warning events or unavailable fields.
  if (snapshot.updated[0] && now - snapshot.updated[0] > 2500) snapshot = {};
  tm_uint32 offset = 0;
  for (tm_uint32 n = 0; n < count; ++n) {
    if (offset > size || size - offset < sizeof(tm_msg_header)) break;
    tm_msg_header header{};
    std::memcpy(&header, bytes + offset, sizeof(header));
    if (header.MessageSize < sizeof(header) || header.MessageSize > size - offset) break;
    auto next = offset + header.MessageSize;
    if (header.DataType == tm_msg_data_type::Double &&
        header.MessageSize >= sizeof(header) + sizeof(double) &&
        header.MessageSize <= sizeof(header) + tm_external_message::GetMaxDataSize()) {
      const auto message = tm_external_message::GetFromByteStream(bytes, offset);
      double value = message.GetDouble();
      if (std::isfinite(value)) {
        for (size_t i = 0; i < fieldCount; ++i) {
          if (fields[i].message.GetID() == message.GetID()) {
            snapshot.values[i] = value; snapshot.updated[i] = now; break;
          }
        }
      }
    }
    offset = next;
  }
}
}

#include "pvpserver/core/config.h"

#include <cstdlib>
#include <stdexcept>

namespace {
constexpr double kDefaultTickRate = 60.0;
constexpr std::uint16_t kDefaultPort = 8080;
constexpr std::uint16_t kDefaultMetricsPort = 9090;
constexpr const char* kDefaultDsn = "postgresql://localhost:5432/pvpserver";

double ParseDoubleOrDefault(const char* value, double fallback) {
    if (!value) {
        return fallback;
    }
    try {
        return std::stod(value);
    } catch (const std::exception&) {
        return fallback;
    }
}

std::uint16_t ParsePortOrDefault(const char* value, std::uint16_t fallback) {
    if (!value) {
        return fallback;
    }
    try {
        const long parsed = std::stol(value);
        if (parsed < 0 || parsed > 65535) {
            return fallback;
        }
        return static_cast<std::uint16_t>(parsed);
    } catch (const std::exception&) {
        return fallback;
    }
}
}  // namespace

namespace pvpserver {

GameConfig::GameConfig(std::uint16_t port, std::uint16_t metrics_port, double tick_rate,
                       std::string database_dsn)
    : port_(port),
      metrics_port_(metrics_port),
      tick_rate_(tick_rate),
      database_dsn_(std::move(database_dsn)) {}

GameConfig GameConfig::FromEnv() {
    const char* env_port = std::getenv("PVPSERVER_PORT");
    const char* env_metrics_port = std::getenv("PVPSERVER_METRICS_PORT");
    const char* env_tick = std::getenv("PVPSERVER_TICK_RATE");
    const char* env_dsn = std::getenv("PVPSERVER_DATABASE_DSN");

    const auto port = ParsePortOrDefault(env_port, kDefaultPort);
    const auto metrics_port = ParsePortOrDefault(env_metrics_port, kDefaultMetricsPort);
    const auto tick_rate = ParseDoubleOrDefault(env_tick, kDefaultTickRate);
    const std::string dsn = env_dsn ? env_dsn : kDefaultDsn;

    return GameConfig{port, metrics_port, tick_rate, dsn};
}

}  // namespace pvpserver

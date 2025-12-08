#include <gtest/gtest.h>

#include "pvpserver/storage/postgres_storage.h"

TEST(PostgresStorageTest, ConnectionFailureIsHandled) {
    pvpserver::PostgresStorage storage("postgresql://localhost:1/pvpserver");
    EXPECT_FALSE(storage.IsConnected());
    EXPECT_FALSE(storage.Connect());
    EXPECT_FALSE(storage.IsConnected());
}

TEST(PostgresStorageTest, RecordSessionEventFailsWithoutConnection) {
    pvpserver::PostgresStorage storage("postgresql://localhost:1/pvpserver");
    EXPECT_FALSE(storage.RecordSessionEvent("player", "start"));
    EXPECT_DOUBLE_EQ(0.0, storage.LastQueryDurationSeconds());
    const auto snapshot = storage.MetricsSnapshot();
    EXPECT_NE(snapshot.find("database_query_duration_seconds"), std::string::npos);
}

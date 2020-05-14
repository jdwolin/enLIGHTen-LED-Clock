// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonArray::size()") {
  DynamicJsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  SECTION("InitialSizeIsZero") {
    REQUIRE(0U == array.size());
  }

  SECTION("increases after add()") {
    array.add("hello");
    REQUIRE(1U == array.size());

    array.add("world");
    REQUIRE(2U == array.size());
  }

  SECTION("remains the same after set()") {
    array.add("hello");
    REQUIRE(1U == array.size());

    array.set(0, "hello");
    REQUIRE(1U == array.size());
  }

  SECTION("remains the same after assigment") {
    array.add("hello");
    REQUIRE(1U == array.size());

    array[0] = "hello";
    REQUIRE(1U == array.size());
  }
}

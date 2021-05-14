#include "core/Ptr.hh"

#include <gtest/gtest.h>

TEST(Ptr, Default) {
  freeisle::core::Ptr<int> ptr;
  ASSERT_EQ(ptr, nullptr);
}

TEST(Ptr, InitializePtr) {
  int val = 3;
  freeisle::core::Ptr<int> ptr(&val);
  ASSERT_EQ(ptr, &val);
}

TEST(Ptr, InitializeConstPtr) {
  int val = 3;
  freeisle::core::Ptr<const int> ptr(&val);
  ASSERT_EQ(ptr, &val);
}

TEST(Ptr, InitializeConstPtrFromConst) {
  const int val = 3;
  // freeisle::core::Ptr<int> ptr(&val); // fails
  freeisle::core::Ptr<const int> ptr(&val);
  ASSERT_EQ(ptr, &val);
}

TEST(Ptr, Assign) {
  int val = 3;
  freeisle::core::Ptr<int> ptr;
  ptr = &val;
  ASSERT_EQ(ptr, &val);
}

TEST(Ptr, AssignToConst) {
  int val = 3;
  freeisle::core::Ptr<const int> ptr;
  ptr = &val;
  ASSERT_EQ(ptr, &val);
}

TEST(Ptr, AssignConstToConst) {
  const int val = 3;
  freeisle::core::Ptr<const int> ptr;
  ptr = &val;
  ASSERT_EQ(ptr, &val);
}

TEST(Ptr, ConstructPtrFromPtr) {
  int val = 3;
  freeisle::core::Ptr<int> ptr(&val);
  freeisle::core::Ptr<int> ptr2(ptr);
  ASSERT_EQ(ptr, &val);
  ASSERT_EQ(ptr2, &val);
}

TEST(Ptr, ConstructConstPtrFromPtr) {
  int val = 3;
  freeisle::core::Ptr<int> ptr(&val);
  freeisle::core::Ptr<const int> ptr2(ptr);
  ASSERT_EQ(ptr, &val);
  ASSERT_EQ(ptr2, &val);
}

TEST(Ptr, ConstructConstPtrFromConstPtr) {
  int val = 3;
  freeisle::core::Ptr<const int> ptr(&val);
  // freeisle::core::Ptr<int> ptr2(ptr); // fails
  freeisle::core::Ptr<const int> ptr2(ptr);
  ASSERT_EQ(ptr, &val);
  ASSERT_EQ(ptr2, &val);
}

TEST(Ptr, AssignPtr) {
  int val = 3;
  freeisle::core::Ptr<int> ptr(&val);
  freeisle::core::Ptr<int> ptr2;
  ptr2 = ptr;
  ASSERT_EQ(ptr, &val);
  ASSERT_EQ(ptr2, &val);
}

TEST(Ptr, AssignPtrToConst) {
  int val = 3;
  freeisle::core::Ptr<int> ptr(&val);
  freeisle::core::Ptr<const int> ptr2;
  ptr2 = ptr;
  ASSERT_EQ(ptr, &val);
  ASSERT_EQ(ptr2, &val);
}

TEST(Ptr, AssignConstPtrToConst) {
  const int val = 3;
  freeisle::core::Ptr<const int> ptr(&val);
  freeisle::core::Ptr<const int> ptr2;
  ptr2 = ptr;
  ASSERT_EQ(ptr, &val);
  ASSERT_EQ(ptr2, &val);
}

TEST(Ptr, AutomaticUpcastInit) {
  struct A {};
  struct B : A {};

  B b;
  freeisle::core::Ptr<A> ptr(&b);
}

TEST(Ptr, AutomaticUpcastInitPtr) {
  struct A {};
  struct B : A {};

  B b;
  freeisle::core::Ptr<B> ptr(&b);
  freeisle::core::Ptr<A> ptr2(ptr);
}

TEST(Ptr, AutomaticUpcastAssign) {
  struct A {};
  struct B : A {};

  B b;
  freeisle::core::Ptr<A> ptr;
  ptr = &b;
}

TEST(Ptr, AutomaticUpcastAssignPtr) {
  struct A {};
  struct B : A {};

  B b;
  freeisle::core::Ptr<B> ptr(&b);
  freeisle::core::Ptr<A> ptr2;
  ptr2 = ptr;
}

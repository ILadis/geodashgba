
#include <prefab.h>
#include "test.h"

struct Stub {
  long value1;
  char value2;
  int ttl;
};

test(NewInstance_ShouldReturnNewObjectInstanceForEachCall) {
  // arrange
  Prefab *prefabs = Prefab_Create(struct Stub, 3);
  Prefab_ResetAll(prefabs);

  // act
  struct Stub *stub1 = Prefab_NewInstance(prefabs);
  struct Stub *stub2 = Prefab_NewInstance(prefabs);
  struct Stub *stub3 = Prefab_NewInstance(prefabs);
  struct Stub *stub4 = Prefab_NewInstance(prefabs);
  struct Stub *stub5 = Prefab_NewInstance(prefabs);

  // assert
  assert(stub1 != NULL);
  assert(stub2 != NULL);
  assert(stub3 != NULL);

  assert(stub4 == NULL);
  assert(stub5 == NULL);

  assert(stub1 != stub2);
  assert(stub2 != stub3);
  assert(stub3 != stub1);
}

test(ResetAll_ShouldMakeAllUsedPrefabsAvailableAgain) {
  // arrange
  Prefab *prefabs = Prefab_Create(struct Stub, 2);
  Prefab_ResetAll(prefabs);

  struct Stub *stub1 = Prefab_NewInstance(prefabs);
  struct Stub *stub2 = Prefab_NewInstance(prefabs);

  // act
  Prefab_ResetAll(prefabs);

  struct Stub *stub3 = Prefab_NewInstance(prefabs);
  struct Stub *stub4 = Prefab_NewInstance(prefabs);

  // assert
  assert(stub1 != NULL);
  assert(stub2 != NULL);

  assert(stub1 == stub3);
  assert(stub2 == stub4);
}

static bool
Action_DecreaseTTL(struct Stub *stub) {
  int ttl = stub->ttl--;
  return ttl > 0;
}

test(ForEach_ShouldInvokeActionForEachAlivePrefab) {
  // arrange
  Prefab *prefabs = Prefab_Create(struct Stub, 3);
  Prefab_ResetAll(prefabs);

  struct Stub *stub1 = Prefab_NewInstance(prefabs);
  stub1->ttl = 3;

  struct Stub *stub2 = Prefab_NewInstance(prefabs);
  stub2->ttl = 5;

  // act
  Prefab_ForEach(prefabs, (Action) Action_DecreaseTTL);

  // assert
  assert(stub1->ttl == 2);
  assert(stub2->ttl == 4);
}

test(ForEach_ShouldMakePrefabsAvailableAgainIfActionReturnsFalse) {
  // arrange
  Prefab *prefabs = Prefab_Create(struct Stub, 2);
  Prefab_ResetAll(prefabs);

  struct Stub *stub1 = Prefab_NewInstance(prefabs);
  stub1->ttl = 0;

  struct Stub *stub2 = Prefab_NewInstance(prefabs);
  stub2->ttl = 1;

  // act
  Prefab_ForEach(prefabs, (Action) Action_DecreaseTTL);

  struct Stub *stub3 = Prefab_NewInstance(prefabs);
  struct Stub *stub4 = Prefab_NewInstance(prefabs);

  // assert
  assert(stub3 == stub1);
  assert(stub4 == NULL);
}

suite(
  NewInstance_ShouldReturnNewObjectInstanceForEachCall,
  ResetAll_ShouldMakeAllUsedPrefabsAvailableAgain,
  ForEach_ShouldInvokeActionForEachAlivePrefab,
  ForEach_ShouldMakePrefabsAvailableAgainIfActionReturnsFalse);

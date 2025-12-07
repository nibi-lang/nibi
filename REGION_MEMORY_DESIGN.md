# Region-Based Memory Management for Nibi


I chatted with a claude about my runtime rsults after I got the various benchmarks back on nibi. I dont have time to do this idea right now but before this branch
gets merged in I want to do, at least conceptually, what the clanker laid out below. It boils down to: 

1) make the interpreter aware of/ able to interact-with the allocator and pool
2) make 2 new constructs to define memory regions and how to lift cells out of them

I made a memory pool that made large allocations happen a LOT faster but it hits hard when lots of small ops kill cells.
I figure with the ref pointer we have in dedicated regions we could mitigate memory "bloat" at the language level by designating
high memory use regions and then indicating when its safe to free those batches

Everything works as-is now (tests fine locally) but valgrind and ci have not run so we shall see. 


## Executive Summary

This document outlines a design for adding explicit region-based memory management to Nibi through new language constructs `region` and `lift`. This approach combines the performance benefits of arena allocation with the safety of reference counting, while giving users explicit control over memory regions.

## Motivation

Current cell pool implementation shows:
- Excellent performance on allocation-heavy workloads (matrixops: 3.16x faster, bubblesort: 4.74x faster)
- Regressions on lightweight workloads with many short-lived allocations (primality: 1.77x slower, mandelbrot: 6.83x slower)

Root cause: Per-cell deallocation overhead (queue operations) adds up when cells are very short-lived.

**Region-based allocation solves this by:**
1. Batch allocating cells in a contiguous memory region
2. Using bump-pointer allocation (extremely fast, just increment offset)
3. Batch freeing entire regions at once (single operation vs thousands)
4. Eliminating per-cell deallocation overhead entirely

## Core Concepts

### What is a Region?

A region is a contiguous block of pre-allocated memory where cells can be allocated sequentially. Think of it as a scratch space for temporary computations.

**Key properties:**
- Linear allocation (bump pointer, ~1-2ns overhead)
- All cells in the region die together when region exits
- Explicit escape mechanism (`lift`) to promote cells to parent region
- No fragmentation within a region
- Cache-friendly due to sequential allocation

### Language-Level Control

Users explicitly define regions in their Nibi code

This makes memory lifetime explicit and predictable.

## Implementation Strategy

### Phase 1: Core Infrastructure

#### 1.1 Region Data Structure

Add to interpreter state:

**Region representation:**
- Memory block pointer (aligned, pre-allocated)
- Current offset (bump pointer)
- Capacity (size of block)
- List of all cells allocated in this region (for cleanup)
- Parent region pointer (for nesting)

**Region stack:**
- Stack of active regions
- Top of stack = current allocation target
- Global region at bottom (default, never destroyed)

#### 1.2 Modified Allocation Path

Current: `allocate_cell(args...) -> new cell_c(args...)`

New path:
1. Check if interpreter has active region
2. If yes: allocate from region's bump pointer
3. If no: fall back to existing pool allocator
4. Use placement new to construct cell in pre-allocated memory
5. Track cell in region's cell list

Key insight: `allocate_cell` needs access to interpreter context to check region stack.

#### 1.3 Region Lifecycle

**Region creation (`region` builtin):**
1. Allocate large memory block (configurable size, e.g., 64KB)
2. Push new region onto stack
3. Execute body expressions
4. Handle region exit

**Region exit:**
1. Process lifted cells (see Phase 2)
2. Call destructors for all non-lifted cells in region
3. Free entire memory block with single deallocation
4. Pop region from stack

### Phase 2: Escape Mechanism (`lift`)

#### 2.1 Lift Semantics

`lift` takes a cell allocated in current region and promotes it to parent region.

**Deep vs shallow copy decision:**
- Shallow: Just move pointer, update tracking (fast but complex)
- Deep: Clone cell into parent region (safer, clearer semantics)

Recommendation: Start with deep copy for safety, optimize later if needed.

#### 2.2 Lift Implementation Approach

**For each lifted cell:**
1. Recursively clone the cell and all its referenced cells
2. Allocate clones in parent region (not current)
3. Update reference counts appropriately
4. Mark original as "lifted" so it's not batch-freed
5. Return the cloned version to parent context

**Special cases to handle:**
- Lists: clone all elements
- Dicts: clone all key-value pairs
- Functions with closures: clone captured environment
- Symbols: resolve and clone the value they reference

#### 2.3 Implicit Lifting

Return values from functions should be implicitly lifted:

```
(; some fn
  (region
    (let result (big-computation))
    result))  ; Implicitly lifted to caller's region
```

This requires:
- Detecting return context
- Auto-lifting the return value
- User can override with explicit `(lift ...)`

### Phase 3: Safety and Validation

#### 3.1 Cross-Region Reference Prevention

**Problem:** Cell in region A references cell in region B. Region B exits. Dangling pointer!

**Solution:** Enforce that cells can only reference cells in:
- Same region
- Ancestor regions (parent, grandparent, etc.)
- Global region

**Implementation:**
- Tag each cell with its region ID
- On assignment/reference creation, validate regions
- Throw error if child region tries to capture parent region cell without lift

#### 3.2 Runtime Checks

Add assertions during development:
- Verify bump pointer doesn't exceed capacity
- Check that lifted cells are valid
- Validate region stack integrity
- Ensure no dangling references after region exit

Can be disabled in release builds for performance.

### Phase 4: Integration Points

#### 4.1 Builtin Functions

**New builtins to add:**

1. `region` - Creates and manages a new region
   - Takes body expressions
   - Returns result of last expression (auto-lifted)
   - Cleans up region on exit

2. `lift` - Explicitly lifts a value to parent region
   - Takes one argument (the cell to lift)
   - Returns the lifted copy
   - Can be called multiple times for different values

#### 4.2 Interpreter State Changes

**Extend `interpreter_c`:**
- Add region stack member
- Add methods: `push_region()`, `pop_region()`, `current_region()`
- Add method: `allocate_in_region(size) -> void*`
- Add method: `lift_cell(cell_ptr) -> cell_ptr`

**Modify `allocate_cell` lambda:**
- Check interpreter state for active region
- Conditionally route to region allocator or pool allocator
- Requires passing interpreter context (might need refactoring)

#### 4.3 Reference Counting Interaction

**Key insight:** Reference counting still works within regions!

- Cells reference each other normally
- Ref counts track lifetime as usual
- Difference: At region exit, ignore ref counts and batch-free everything

**Only exception:** Lifted cells maintain their ref counts across region boundary.

### Phase 5: Optimization Opportunities

#### 5.1 Region Size Strategy

**Options:**
- Fixed size (e.g., 64KB per region)
- Adaptive sizing based on allocation patterns
- Nested regions share same memory block when possible

**Start simple:** Fixed 64KB, allocate new block if exhausted.

#### 5.2 Region Reuse

Don't constantly malloc/free region memory blocks:
- Keep a pool of freed region blocks
- Reuse them for new regions
- Only free back to system on shutdown

#### 5.3 Batch Size Tuning

Different workloads need different region sizes:
- Small regions: Less wasted memory, more overhead
- Large regions: More wasted memory, less overhead

Could expose as parameter: `(region :size 4096 ...)`

### Phase 6: Migration Strategy

#### 6.1 Backward Compatibility

Regions are opt-in. Existing code continues to work:
- No `region` keyword = uses existing pool allocator
- Global region acts as default for non-region code
- Gradual adoption possible

#### 6.2 Benchmarking Strategy

Test regions in performance-critical areas first:
- Wrap inner loops in mandelbrot with `region`
- Wrap primality test iterations with `region`
- Measure if regressions disappear

Expected results:
- Mandelbrot: Should approach baseline (0.05s)
- Primality: Should approach baseline (0.43s)
- Matrixops/bubblesort: Should maintain or improve gains

#### 6.3 Standard Library Integration

Identify stdlib functions that benefit from regions:
- List operations (map, filter, reduce)
- String manipulation
- Recursive algorithms

Could add auto-region wrappers for these.

## Edge Cases and Gotchas

### Circular References in Regions

**Problem:** Cell A references cell B, B references A, both in same region.

**Solution:** No problem! Both freed together at region exit. Ref counting prevents premature destruction during region lifetime.

### Lifting Circular Structures

**Problem:** Lift a cell that's part of a cycle.

**Solution:** Deep clone breaks the cycle or preserves it in parent region. Need to track visited cells during clone to handle cycles correctly.

### Large Allocations

**Problem:** Single cell larger than region capacity.

**Solution:** 
- Fall back to pool allocator for oversized allocations
- Or: Allocate a special single-cell region
- Mark as "oversized" so it's individually freed

### Exceptions During Region

**Problem:** Exception thrown mid-region, region cleanup skipped.

**Solution:** Use RAII wrapper or ensure region cleanup happens in unwind path. Nibi's `try` mechanism should handle this.

### Region Nesting Depth

**Problem:** Deep recursion creates many nested regions.

**Solution:**
- Limit nesting depth (e.g., 1000 levels)
- Throw error if exceeded
- Consider: Only create regions at certain recursion depths

## Performance Expectations

### Before Regions (Current)

- Pool overhead: ~6-11ns per allocation/deallocation
- Queue operations: push/pop/front
- Good for long-lived cells, bad for short-lived

### After Regions

- Region allocation: ~1-2ns (bump pointer increment)
- Region exit: Single free operation (not N operations)
- Lift overhead: Deep copy cost (only for escaped values)

**Expected improvements:**
- Mandelbrot: 6.83x slower → should approach 1x (baseline)
- Primality: 1.77x slower → should approach 1x (baseline)
- Total time: ~39s → potentially <10s on benchmarks

### Break-Even Analysis

Regions help when:
- Most cells are temporary (high death rate in region)
- Few cells need to escape (low lift rate)
- Allocation frequency is high

Regions hurt when:
- Most cells need to escape (high lift cost)
- Allocation frequency is low (region overhead wasted)

## Implementation Checklist

### Prerequisites
- [ ] Understand current allocate_cell flow
- [ ] Understand ref counting in ref.hpp
- [ ] Understand interpreter execution model
- [ ] Review how builtins are implemented

### Core Implementation
- [ ] Add region_s struct to interpreter
- [ ] Add region stack to interpreter_c
- [ ] Implement push_region/pop_region methods
- [ ] Implement allocate_in_region method
- [ ] Modify allocate_cell to check for active region
- [ ] Add region builtin function
- [ ] Add lift builtin function
- [ ] Implement deep clone for lift
- [ ] Handle region cleanup/destruction
- [ ] Add cross-region reference validation

### Testing
- [ ] Test simple region creation/destruction
- [ ] Test allocation within region
- [ ] Test lift with primitives
- [ ] Test lift with complex types (lists, dicts)
- [ ] Test nested regions
- [ ] Test implicit return lifting
- [ ] Test cross-region reference errors
- [ ] Test exception handling during region
- [ ] Run existing test suite (ensure no breakage)
- [ ] Benchmark mandelbrot with regions
- [ ] Benchmark primality with regions

### Documentation
- [ ] Add region/lift to language docs
- [ ] Add examples showing proper usage
- [ ] Document performance characteristics
- [ ] Add migration guide for existing code

## Future Enhancements

### Advanced Features
- Region annotations for automatic region insertion
- Static analysis to suggest where regions would help
- Region pools for reuse
- Adaptive region sizing based on profiling
- Region statistics/telemetry

### Language Extensions
- `with-region` macro for common patterns
- `auto-region` for automatic region boundaries
- Region size hints: `(region :size 1024 ...)`
- `lift-all` to lift multiple values at once

## Conclusion

Region-based memory management in Nibi offers:
1. **Performance:** Eliminate per-cell deallocation overhead
2. **Control:** Users explicitly manage memory regions
3. **Safety:** Ref counting + validation prevents dangling pointers
4. **Compatibility:** Opt-in feature, doesn't break existing code
5. **Simplicity:** Clear semantics, predictable behavior

By making regions explicit in the language, we sidestep complex escape analysis while giving users powerful control over memory allocation patterns. This should eliminate the regressions in lightweight benchmarks while maintaining the massive gains in allocation-heavy workloads.

The key insight: Let users guide the allocator with explicit regions, rather than trying to guess optimal allocation strategies automatically.

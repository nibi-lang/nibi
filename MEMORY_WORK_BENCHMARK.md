# Memory Pool Implementation Benchmarks

## Complete Results Table

| Test | Baseline (No Pool) | Single Pool (Locked) | Multi-Pool (4, Locked) | Multi-Pool (4, Lock-Free) | Single Pool (Lock-Free) |
|------|-------------------|---------------------|----------------------|--------------------------|------------------------|
| primality.nibi | 0.4284s | 0.9123s | 0.9785s | 0.8232s | 0.7587s |
| matrixops.nibi | 82.1973s | 29.5978s | 29.3571s | 28.067s | 26.0163s |
| leibniz.nibi | 0.0504s | 0.0531s | 0.0518s | 0.0447s | 0.0434s |
| bubblesort.nibi | 53.3845s | 12.5813s | 12.467s | 11.8163s | 11.2578s |
| mandelbrot.nibi | 0.0494s | 0.3799s | 0.3568s | 0.3884s | 0.3374s |
| 4sieve.nibi | 0.4441s | 0.4361s | 0.4458s | 0.3784s | 0.3766s |
| **Total** | **6827.71s** | **43.96s** | **43.66s** | **41.52s** | **38.79s** |

## Speedup vs Baseline

| Test | Single Pool (Locked) | Multi-Pool (4, Locked) | Multi-Pool (4, Lock-Free) | Single Pool (Lock-Free) |
|------|---------------------|----------------------|--------------------------|------------------------|
| primality.nibi | 0.47x | 0.44x | 0.52x | 0.56x |
| matrixops.nibi | 2.78x | 2.80x | 2.93x | 3.16x |
| leibniz.nibi | 0.95x | 0.97x | 1.13x | 1.16x |
| bubblesort.nibi | 4.24x | 4.28x | 4.52x | 4.74x |
| mandelbrot.nibi | 0.13x | 0.14x | 0.13x | 0.15x |
| 4sieve.nibi | 1.02x | 1.00x | 1.17x | 1.18x |
| **Total** | **155.3x** | **156.4x** | **164.4x** | **176.0x** |

## Configuration Details

| Implementation | Synchronization | Pool Count | Batch Size | Overhead per Operation |
|---------------|----------------|------------|------------|----------------------|
| Baseline | N/A | N/A | N/A | System allocator |
| Single Pool (Locked) | Mutex | 1 | 1024 | ~30-70ns |
| Multi-Pool (4, Locked) | Mutex + Atomics | 4 | 1024 | ~30-70ns + round-robin |
| Multi-Pool (4, Lock-Free) | None | 4 | 1024 | ~7-14ns + round-robin |
| Single Pool (Lock-Free) | None | 1 | 64 | ~6-11ns |

## Key Findings

| Metric | Best Implementation | Value |
|--------|-------------------|-------|
| Overall Performance | Single Pool (Lock-Free) | 38.79s total |
| Allocation-Heavy (matrixops) | Single Pool (Lock-Free) | 3.16x faster |
| Allocation-Heavy (bubblesort) | Single Pool (Lock-Free) | 4.74x faster |
| Lightweight (primality) | Single Pool (Lock-Free) | 1.77x slower |
| Lightweight (mandelbrot) | Single Pool (Lock-Free) | 6.83x slower |

## Progression Summary

| Version | Total Time | Change from Previous | Change from Baseline |
|---------|-----------|---------------------|---------------------|
| Baseline | 6827.71s | - | - |
| Single Pool (Locked) | 43.96s | -6783.75s | -99.4% |
| Multi-Pool (4, Locked) | 43.66s | -0.30s | -99.4% |
| Multi-Pool (4, Lock-Free) | 41.52s | -2.14s | -99.4% |
| Single Pool (Lock-Free) | 38.79s | -2.73s | -99.4% |

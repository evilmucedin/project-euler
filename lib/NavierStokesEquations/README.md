# NavierStokesEquations

A small C++17 library that solves the incompressible Navier-Stokes equations in
two dimensions,

```
du/dt + (u . grad) u = -grad p + nu laplacian(u) + f,     div u = 0,
```

on a rectangle, together with four worked examples that check it against exact
solutions and published benchmark data.

The library has no dependencies beyond the standard library.

## Contents

| File | What it holds |
| --- | --- |
| `field.h` / `.cpp` | `Field2D`, a rectangular array of doubles with a ghost-cell halo |
| `grid.h` | `StaggeredGrid`, the geometry and the sample positions of each field |
| `boundary.h` / `.cpp` | boundary kinds and the pass that fills boundary faces and ghost cells |
| `poisson.h` / `.cpp` | `PressurePoisson`, the Neumann Poisson solve at the heart of the projection |
| `solver.h` / `.cpp` | `NavierStokesSolver`, the time stepping |
| `diagnostics.h` / `.cpp` | divergence, vorticity, cell-centred velocity, kinetic energy |
| `obstacle.h` / `.cpp` | builders for the solid-cell masks that immerse an obstacle in the flow |
| `io.h` / `.cpp` | CSV and PPM output, for `examples/plot.py` or for viewing directly |
| `navierStokes.h` | umbrella header |
| `tests/` | the unit tests |
| `examples/` | the four validation cases and a plotting script |

## Discretisation

**Staggered grid.** Pressure lives at the cell centres, `u` on the vertical
faces and `v` on the horizontal ones (an Arakawa C, or MAC, layout):

```
      v(i, j+1)
   +------^------+
   |             |
u(i,j)  p(i,j)  u(i+1,j)
-->  |     x     |  -->
   |             |
   +------^------+
      v(i, j)
```

Each velocity component sits on the face it crosses, so the divergence of a
cell is a difference of its four faces and the pressure gradient on a face is a
difference of the two cells sharing it. Those two operators are exact
transposes, which is the reason the projection below removes the divergence
down to the residual of the pressure solve rather than to some larger
discretisation error. A ghost-cell halo carries the boundary conditions, so
every interior stencil is evaluated without special cases.

**Time stepping** is Chorin's fractional step (projection) method: each step
advances momentum explicitly while ignoring the pressure, then projects the
result back onto the divergence-free fields.

1. `u* = u + dt (-advection + nu laplacian(u) + f)`
2. solve `laplacian(p) = div(u*) / dt`
3. `u = u* - dt grad p`

That is second-order accurate in space and first-order in time. Advection is
available in conservative central form (second order, the default) or as
first-order upwind, which is more diffusive but stays well behaved at cell
Reynolds numbers where the central scheme rings.

**The pressure problem** has homogeneous Neumann conditions on every side,
because every boundary either prescribes its normal velocity or is periodic,
and the projection therefore must not change those faces. The same applies to
the faces of solid cells: dropping their flux terms from the stencil imposes
`dp/dn = 0` on the obstacle and keeps the operator symmetric, so conjugate
gradients remain legitimate. An all-Neumann problem is singular — constants are
in its null space — and solvable only when the right-hand side has zero mean,
which is why `PressurePoisson` projects the mean out of both the right-hand
side and the solution, and why an outflow boundary rescales its faces so that
the net flux through the domain boundary vanishes. The default solver is
unpreconditioned conjugate gradients; SOR is available as a slower reference
(on the 64x64 cavity below: 239 CG iterations against 1420 SOR sweeps for the
same tolerance).

**Boundary conditions**, per side: `NoSlip` (with an optional wall velocity, as
for a moving lid), `FreeSlip`, `Periodic`, `Inflow` with a prescribed normal
velocity, and zero-gradient `Outflow`. One detail worth knowing if you extend
the library: the outflow faces are re-derived from the interior only *before*
the momentum and pressure stages of a step. Re-deriving them after the
projection would put the divergence the projection just removed straight back
into the last row of cells, which is what `OutflowUpdate::Keep` exists to
prevent.

**Obstacles** are immersed by marking whole cells solid; a shape is therefore
resolved as a staircase, which is first-order accurate at the surface. That is
enough for a wake to be shed at the right frequency, but not for an accurate
drag coefficient.

## Usage

```cpp
#include "lib/NavierStokesEquations/navierStokes.h"

using namespace navierStokes;

const StaggeredGrid grid(64, 64, 1.0, 1.0);   // 64 x 64 cells on a unit square

SolverOptions options;
options.viscosity = 0.01;                     // Re = U L / nu = 100
options.poisson.tolerance = 1e-10;

NavierStokesSolver solver(grid, BoundaryConditions::cavity(1.0), options);

while (solver.time() < 30.0) {
    // stepAdaptive caps the step at the CFL, viscous and body-force limits.
    const StepReport report = solver.stepAdaptive(0.05);
    if (!report.poisson.converged) {
        return 1;
    }
}

writeFieldCsv("vorticity.csv", vorticityField(grid, solver.u(), solver.v()));
```

Setting up a flow by hand: `setVelocity` samples a pair of functions at the
face positions and `setUniformVelocity` fills a constant; both then apply the
boundary conditions, which override the faces a wall owns. Neither field is
divergence free in general, so follow either with `projectVelocity()`. To
immerse an obstacle, pass a mask from `obstacle.h` to `setSolid` before setting
the velocity.

`StepReport` carries the step that was taken, the largest velocity and
divergence, and the report of the pressure solve. `maxDivergence` is exactly
`dt` times the residual the pressure solve left behind, so it tightens with
`PoissonOptions::tolerance`: at `1e-12` it sits at a few times `1e-15`.

## Building and running

Buck2 is the primary build system, and Ninja files are generated from the
`BUCK` files by `scripts/generate_ninja.py`.

```sh
# Buck2: the library and the examples
buck2 build //lib/NavierStokesEquations: //lib/NavierStokesEquations/examples/...
buck2 run //lib/NavierStokesEquations/examples:lidDrivenCavity

# Ninja, which is also how the tests are built and run
python3 scripts/generate_ninja.py                    # after editing any BUCK file
ninja lib/NavierStokesEquations/tests/solverTest
./build-ninja/bin/lib/NavierStokesEquations/tests/solverTest
ninja lib/NavierStokesEquations/examples/taylorGreen
./build-ninja/bin/lib/NavierStokesEquations/examples/taylorGreen

# Bazel, for the library targets
./bBazel.sh //lib/NavierStokesEquations:all
```

`buck2 build //lib/NavierStokesEquations/...` with the trailing wildcard also
picks up the `cxx_test` targets in `tests/`, and those need a `toolchains//:test`
target that this repository does not define — the same is true of the existing
tests under `lib/stat/tests`. Build and run the test binaries through Ninja, as
above.

The repository's Ninja flags do not define `NDEBUG`, so the `assert`s that
check field shapes and index bounds stay active in those builds. They are what
catches a mis-sized field passed to the solver, and they cost enough that a
timing measurement should use a build with `-DNDEBUG`.

The examples write their output into the working directory. Plot it with

```sh
python3 lib/NavierStokesEquations/examples/plot.py cavity    # needs numpy, matplotlib
```

or view the `.ppm` images directly — they are plain binary P6 and need no
library at all.

## Validation

Every number below was measured by the examples in `examples/`.

**Taylor-Green vortex** (`taylorGreen.cpp`) is an exact solution on a doubly
periodic domain, so the error can be measured directly. With the time step
proportional to `dx^2`, the second-order spatial convergence is clean:

| cells | rms error | order | max error | order | max divergence |
| --- | --- | --- | --- | --- | --- |
| 16 | 3.038e-04 | | 5.791e-04 | | 1.2e-14 |
| 32 | 7.515e-05 | 2.015 | 1.474e-04 | 1.974 | 3.9e-15 |
| 64 | 1.866e-05 | 2.009 | 3.700e-05 | 1.993 | 2.3e-15 |
| 128 | 4.650e-06 | 2.005 | 9.260e-06 | 1.998 | 3.4e-15 |

**Plane Poiseuille flow** (`channelFlow.cpp`) has the steady parabola
`u = G y (h - y) / 2 nu`. The peak velocity comes out exactly right at every
resolution, and the profile error falls by exactly four per refinement:

| cells across | max error | order | peak velocity (exact 0.125) |
| --- | --- | --- | --- |
| 8 | 1.953e-03 | | 0.125000 |
| 16 | 4.883e-04 | 2.000 | 0.125000 |
| 32 | 1.221e-04 | 2.000 | 0.125000 |
| 64 | 3.052e-05 | 2.000 | 0.125000 |

The divergence is exactly zero here, to the last bit: the flow is
unidirectional, so the projection has nothing to do.

**Lid-driven cavity** (`lidDrivenCavity.cpp`) is compared with the tabulated
profiles of Ghia, Ghia & Shin (J. Comput. Phys. 48, 1982). On a 64 x 64 grid at
Re = 100 the computed `u` along the vertical centreline agrees with the
reference table to within 0.0083 everywhere, and the minimum of the profile is

| grid | minimum of u | at y |
| --- | --- | --- |
| 32 x 32 | -0.21363 | 0.4531 |
| 64 x 64 | -0.21540 | 0.4609 |
| reference | -0.21090 | 0.4531 |

At Re = 400 the 64 x 64 grid gives -0.32175 at y = 0.2891 against the reference
-0.32726 at y = 0.2813, about 2% out — consistent with a second-order scheme on
a grid this coarse. The run takes about 9800 steps to reach steady state at
t = 30, the viscous stability limit rather than the CFL condition setting the
step, and the kinetic energy settles to 0.0339489.

**Karman vortex street** (`cylinderWake.cpp`) sheds vortices from a cylinder at
Re = 100 and recovers the shedding frequency from a probe two diameters
downstream. Over 15 cycles the period is 1.236, giving a Strouhal number
`f D / U` of 0.1618 against the expected value of about 0.16. The run also
writes 40 frames of the vorticity field as PPM images.

## Possible extensions

- A multigrid or preconditioned pressure solve. Unpreconditioned conjugate
  gradients need iterations proportional to the grid size, which is what
  dominates the cost of the finer runs above.
- Second-order time stepping (Adams-Bashforth for advection, Crank-Nicolson
  for diffusion), which would remove the `dt ~ dx^2` restriction that the
  convergence study needs.
- A cut-cell or immersed-boundary treatment of obstacles, for surface
  quantities such as drag.
- A transported scalar (temperature, dye) on the same grid, which the
  staggered layout and the existing advection routines already support.

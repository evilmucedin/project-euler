#pragma once

// Umbrella header for the two-dimensional incompressible Navier-Stokes solver.
// See lib/NavierStokesEquations/README.md for the discretisation and for the
// validation cases under examples/.

#include "lib/NavierStokesEquations/boundary.h"
#include "lib/NavierStokesEquations/diagnostics.h"
#include "lib/NavierStokesEquations/field.h"
#include "lib/NavierStokesEquations/grid.h"
#include "lib/NavierStokesEquations/io.h"
#include "lib/NavierStokesEquations/obstacle.h"
#include "lib/NavierStokesEquations/poisson.h"
#include "lib/NavierStokesEquations/solver.h"
